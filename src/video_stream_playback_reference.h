#ifndef VIDEO_STREAM_PLAYBACK_REFERENCE_H
#define VIDEO_STREAM_PLAYBACK_REFERENCE_H

#include <godot_cpp/classes/video_stream_playback_extension.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <deque>

namespace godot {

class VideoStreamPlaybackReference : public VideoStreamPlaybackExtension {
	GDCLASS(VideoStreamPlaybackReference, VideoStreamPlaybackExtension)

protected:
	static void _bind_methods();

	friend class VideoStreamReference;

	bool initialized = false;
	bool playing = false;
	bool paused = false;
	Ref<ImageTexture> texture;
public:
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
	virtual void _update(double delta) override;
	virtual int64_t _get_channels() const override;
	virtual int64_t _get_mix_rate() const override;
	virtual void _initialize() override;
	virtual void _cleanup() override;

	VideoStreamPlaybackReference() = default;
	~VideoStreamPlaybackReference() override;
};
}

#endif // VIDEO_STREAM_PLAYBACK_H