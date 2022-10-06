#ifndef VIDEO_STREAM_PLAYBACK_REFERENCE_H
#define VIDEO_STREAM_PLAYBACK_REFERENCE_H

#include <deque>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/video_stream_playback.hpp>

class WebMFrame;
class WebMDemuxer;
class VPXDecoder;
class OpusVorbisDecoder;

namespace godot {

class VideoStreamPlaybackReference : public VideoStreamPlayback {
	GDCLASS(VideoStreamPlaybackReference, VideoStreamPlayback)

	String file_name;
	int audio_track = 0;

	WebMDemuxer *webm = nullptr;
	VPXDecoder *video = nullptr;
	OpusVorbisDecoder *audio = nullptr;

	WebMFrame **video_frames = nullptr;
	WebMFrame *audio_frame = nullptr;
	int video_frames_pos = 0, video_frames_capacity = 0;

	int num_decoded_samples = 0, samples_offset = -1;

	bool playing = false, paused = false;
	double delay_compensation = 0.0;
	double time = 0.0, video_frame_delay = 0.0, video_pos = 0.0;

	PackedByteArray frame_data;
	Ref<ImageTexture> texture;

	PackedFloat32Array pcm;

protected:
	static void _bind_methods();

	friend class VideoStreamReference;

	bool initialized = false;

public:
	void delete_pointers();
	virtual void _stop() override;
	virtual void _play() override;
	virtual bool _is_playing() const override;
	virtual void _set_paused(bool paused) override;
	virtual bool _is_paused() const override;
	virtual void _set_loop(bool enable) override;
	virtual bool _has_loop() const override;
	virtual double _get_length() const override;
	virtual double _get_playback_position() const override;
	virtual void _seek(double time) override;
	virtual void _set_audio_track(int64_t idx) override;
	virtual Ref<Texture2D> _get_texture() const override;
	bool should_process(WebMFrame &video_frame);
	virtual void _update(double delta) override;
	virtual int64_t _get_channels() const override;
	virtual int64_t _get_mix_rate() const override;
	double get_sample_rate() const;
	bool has_enough_video_frames() const;
	virtual void _initialize() override;
	virtual void _cleanup() override;

	virtual bool _file_opened() override;

	VideoStreamPlaybackReference();
	~VideoStreamPlaybackReference() override;
};
} // namespace godot

#endif // VIDEO_STREAM_PLAYBACK_H