#ifndef VIDEO_STREAM_WEBM_H
#define VIDEO_STREAM_WEBM_H

#include <godot_cpp/classes/video_stream.hpp>
#include <godot_cpp/classes/video_stream_playback.hpp>

namespace godot {

class VideoStreamReference : public VideoStream {
	GDCLASS(VideoStreamReference, VideoStream);

	String file;
	int audio_track = 0;

protected:
	static void _bind_methods();

public:
	virtual Object *_instantiate_playback() override;
	virtual PackedStringArray _get_supported_extensions() const override;

	virtual void _initialize() override;
	virtual void _cleanup() override;

	virtual String _get_plugin_name() const override;

	VideoStreamReference();
	~VideoStreamReference();
};
} // namespace godot

#endif // VIDEO_STREAM_WEBM_H