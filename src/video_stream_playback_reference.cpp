
#include "video_stream_playback_reference.h"
#include "video_stream_reference.h"

#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <yuv2rgb.h>

// libsimplewebm
#include <OpusVorbisDecoder.hpp>
#include <VPXDecoder.hpp>

// libvpx
#include <vpx/vpx_image.h>

// libwebm
#include <mkvparser/mkvparser.h>

#include <godot_cpp/classes/audio_server.hpp>

class MkvReader : public mkvparser::IMkvReader {
	godot::VideoStreamPlaybackReference *playback;

public:
	MkvReader(godot::VideoStreamPlaybackReference *pb) :
			playback(pb) {
	}

	virtual ~MkvReader() {
	}

	int Read(long long pos, long len, unsigned char *buf) override {
		if (playback) {
			if (playback->file_pos() != (uint64_t)pos) {
				playback->file_seek(pos);
			}

			const auto data_buffer = playback->file_read(len);
			if (data_buffer.size() == len) {
				memmove(buf, data_buffer.ptr(), len);
				return 0;
			}
		}
		return -1;
	}

	int Length(long long *total, long long *available) override {
		if (playback) {
			const uint64_t len = playback->file_len();
			if (total) {
				*total = len;
			}
			if (available) {
				*available = len;
			}
			return 0;
		}
		return -1;
	}
};

void godot::VideoStreamPlaybackReference::_bind_methods() {}

void godot::VideoStreamPlaybackReference::_stop() {
	if (playing) {
		delete_pointers();

		audio_frame = nullptr;
		video_frames = nullptr;

		video = nullptr;
		audio = nullptr;

		file_seek(0); // Should not fail here...

		video_frames_capacity = video_frames_pos = 0;
		num_decoded_samples = 0;
		samples_offset = -1;
		video_frame_delay = video_pos = 0.0;
	}
	time = 0.0;
	playing = false;
}

void godot::VideoStreamPlaybackReference::_play() {
	_stop();

	delay_compensation = ProjectSettings::get_singleton()->get("audio/video/video_delay_compensation_ms");
	delay_compensation /= 1000.0;

	playing = true;
}

bool godot::VideoStreamPlaybackReference::_is_playing() const {
	return playing;
}

void godot::VideoStreamPlaybackReference::_set_paused(bool p_paused) {
	this->paused = p_paused;
}

bool godot::VideoStreamPlaybackReference::_is_paused() const {
	return paused;
}

void godot::VideoStreamPlaybackReference::_set_loop(bool enable) {
}

bool godot::VideoStreamPlaybackReference::_has_loop() const {
	return false;
}

double godot::VideoStreamPlaybackReference::_get_length() const {
	if (webm) {
		return webm->getLength();
	}
	return 0.0;
}

double godot::VideoStreamPlaybackReference::_get_playback_position() const {
	return 0.0;
}

void godot::VideoStreamPlaybackReference::_seek(double time) {
}

void godot::VideoStreamPlaybackReference::_set_audio_track(int64_t idx) {
}

godot::Ref<godot::Texture2D> godot::VideoStreamPlaybackReference::_get_texture() const {
	return texture;
}

bool godot::VideoStreamPlaybackReference::should_process(WebMFrame &video_frame) {
	// FIXME: AudioServer output latency was fixed in af9bb0e, previously it used to
	// systematically return 0. Now that it gives a proper latency, it broke this
	// code where the delay compensation likely never really worked.
	// const double audio_delay = AudioServer::get_singleton()->get_output_latency();
	return video_frame.time >= time + /* audio_delay + */ delay_compensation;
}

void godot::VideoStreamPlaybackReference::_update(double p_delta) {
	if ((!playing || paused) || !video) {
		return;
	}

	time += p_delta;

	if (time < video_pos) {
		return;
	}

	bool audio_buffer_full = false;

	if (samples_offset > -1) {
		// Mix remaining samples
		const int to_read = num_decoded_samples - samples_offset;
		const int mixed = mix_audio(to_read, pcm, samples_offset * webm->getChannels());
		if (mixed != to_read) {
			samples_offset += mixed;
			audio_buffer_full = true;
		} else {
			samples_offset = -1;
		}
	}

	const bool hasAudio = audio && (mix_audio(-1) != -1);
	while ((hasAudio && !audio_buffer_full && !has_enough_video_frames()) ||
			(!hasAudio && video_frames_pos == 0)) {
		if (hasAudio && !audio_buffer_full && audio_frame->isValid() &&
				audio->getPCMF(*audio_frame, pcm.ptrw(), num_decoded_samples) && num_decoded_samples > 0) {
			const int mixed = mix_audio(num_decoded_samples, pcm);

			if (mixed != num_decoded_samples) {
				samples_offset = mixed;
				audio_buffer_full = true;
			}
		}

		if (video_frames_pos >= video_frames_capacity) {
			WebMFrame **video_frames_new = static_cast<WebMFrame **>(memrealloc(video_frames, ++video_frames_capacity * sizeof(void *)));
			ERR_FAIL_COND(!video_frames_new); // Out of memory
			(video_frames = video_frames_new)[video_frames_capacity - 1] = memnew(WebMFrame);
		}
		WebMFrame *video_frame = video_frames[video_frames_pos];

		if (!webm->readFrame(video_frame, audio_frame)) { // This will invalidate frames
			break; // Can't demux, EOS?
		}

		if (video_frame->isValid()) {
			++video_frames_pos;
		}
	};

	bool video_frame_done = false;
	while (video_frames_pos > 0 && !video_frame_done) {
		WebMFrame *video_frame = video_frames[0];

		// It seems VPXDecoder::decode has to be executed even though we might skip this frame
		if (video->decode(*video_frame)) {
			VPXDecoder::IMAGE_ERROR err;
			VPXDecoder::Image image;

			if (should_process(*video_frame)) {
				if ((err = video->getImage(image)) != VPXDecoder::NO_FRAME) {
					if (err == VPXDecoder::NO_ERROR && image.w == webm->getWidth() && image.h == webm->getHeight()) {
						uint8_t *w = frame_data.ptrw();
						bool converted = false;

						if (image.chromaShiftW == 0 && image.chromaShiftH == 0 && image.cs == VPX_CS_SRGB) {
							uint8_t *wp = w;
							unsigned char *rRow = image.planes[2];
							unsigned char *gRow = image.planes[0];
							unsigned char *bRow = image.planes[1];
							for (int i = 0; i < image.h; i++) {
								for (int j = 0; j < image.w; j++) {
									*wp++ = rRow[j];
									*wp++ = gRow[j];
									*wp++ = bRow[j];
									*wp++ = 255;
								}
								rRow += image.linesize[2];
								gRow += image.linesize[0];
								bRow += image.linesize[1];
							}
							converted = true;
						} else if (image.chromaShiftW == 1 && image.chromaShiftH == 1) {
							yuv420_2_rgb8888(w, image.planes[0], image.planes[1], image.planes[2], image.w, image.h, image.linesize[0], image.linesize[1], image.w << 2);
							// libyuv::I420ToARGB(image.planes[0], image.linesize[0], image.planes[2], image.linesize[2], image.planes[1], image.linesize[1], w.ptr(), image.w << 2, image.w, image.h);
							converted = true;
						} else if (image.chromaShiftW == 1 && image.chromaShiftH == 0) {
							yuv422_2_rgb8888(w, image.planes[0], image.planes[1], image.planes[2], image.w, image.h, image.linesize[0], image.linesize[1], image.w << 2);
							// libyuv::I422ToARGB(image.planes[0], image.linesize[0], image.planes[2], image.linesize[2], image.planes[1], image.linesize[1], w.ptr(), image.w << 2, image.w, image.h);
							converted = true;
						} else if (image.chromaShiftW == 0 && image.chromaShiftH == 0) {
							yuv444_2_rgb8888(w, image.planes[0], image.planes[1], image.planes[2], image.w, image.h, image.linesize[0], image.linesize[1], image.w << 2);
							// libyuv::I444ToARGB(image.planes[0], image.linesize[0], image.planes[2], image.linesize[2], image.planes[1], image.linesize[1], w.ptr(), image.w << 2, image.w, image.h);
							converted = true;
						} else if (image.chromaShiftW == 2 && image.chromaShiftH == 0) {
							// libyuv::I411ToARGB(image.planes[0], image.linesize[0], image.planes[2], image.linesize[2] image.planes[1], image.linesize[1], w.ptr(), image.w << 2, image.w, image.h);
							// converted = true;
						}

						if (converted) {
							Ref<Image> img = memnew(Image());
							img->create_from_data(image.w, image.h, 0, Image::FORMAT_RGBA8, frame_data);
							texture->update(img); // Zero copy send to rendering server
							video_frame_done = true;
						}
					}
				}
			}
		}

		video_pos = video_frame->time;
		memmove(video_frames, video_frames + 1, (--video_frames_pos) * sizeof(void *));
		video_frames[video_frames_pos] = video_frame;
	}

	if (video_frames_pos == 0 && webm->isEOS()) {
		_stop();
	}
}

int64_t godot::VideoStreamPlaybackReference::_get_channels() const {
	if (audio) {
		return webm->getChannels();
	}
	return 0;
}

int64_t godot::VideoStreamPlaybackReference::_get_mix_rate() const {
	return static_cast<int64_t>(get_sample_rate());
}

double godot::VideoStreamPlaybackReference::get_sample_rate() const {
	if (audio) {
		return webm->getSampleRate();
	}
	return 0.0;
}

inline bool godot::VideoStreamPlaybackReference::has_enough_video_frames() const {
	if (video_frames_pos > 0) {
		// FIXME: AudioServer output latency was fixed in af9bb0e, previously it used to
		// systematically return 0. Now that it gives a proper latency, it broke this
		// code where the delay compensation likely never really worked.
		// const double audio_delay = AudioServer::get_singleton()->get_output_latency();
		const double video_time = video_frames[video_frames_pos - 1]->time;
		return video_time >= time + /* audio_delay + */ delay_compensation;
	}
	return false;
}

void godot::VideoStreamPlaybackReference::delete_pointers() {

	if (audio_frame) {
		memdelete(audio_frame);
	}
	if (video_frames) {
		for (int i = 0; i < video_frames_capacity; ++i) {
			memdelete(video_frames[i]);
		}
		memfree(video_frames);
	}

	if (video) {
		memdelete(video);
	}
	if (audio) {
		memdelete(audio);
	}

	if (webm) {
		memdelete(webm);
	}
}

void godot::VideoStreamPlaybackReference::_initialize() {
	if (!initialized) {
		texture = Ref<ImageTexture>(memnew(ImageTexture));

		initialized = true;
	}
}

void godot::VideoStreamPlaybackReference::_cleanup() {
	if (initialized) {
		delete_pointers();
		texture.unref();
		initialized = false;
	}
}

bool godot::VideoStreamPlaybackReference::_file_opened() {
	webm = memnew(WebMDemuxer(new MkvReader(this), 0, audio_track));
	if (webm->isOpen()) {
		video = memnew(VPXDecoder(*webm, OS::get_singleton()->get_processor_count()));
		if (video->isOpen()) {
			audio = memnew(OpusVorbisDecoder(*webm));
			if (audio->isOpen()) {
				audio_frame = memnew(WebMFrame);
				pcm.resize(1i64 * audio->getBufferSamples() * webm->getChannels());

			} else {
				memdelete(audio);
				audio = nullptr;
			}

			frame_data.resize(4i64 * webm->getWidth() * webm->getHeight());
			Ref<Image> img;
			img.instantiate();
			img->create(webm->getWidth(), webm->getHeight(), false, Image::FORMAT_RGBA8);
			texture->set_image(img);

			return true;
		}
		memdelete(video);
		video = nullptr;
	}
	memdelete(webm);
	webm = nullptr;
	return false;
}

godot::VideoStreamPlaybackReference::VideoStreamPlaybackReference() {
	VideoStreamPlaybackReference::_initialize();
}

godot::VideoStreamPlaybackReference::~VideoStreamPlaybackReference() {
	VideoStreamPlaybackReference::_cleanup();
}
