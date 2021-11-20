#ifndef VIDEO_STREAM_WEBM_H
#define VIDEO_STREAM_WEBM_H

#include <godot_cpp/classes/video_stream_extension.hpp>
#include <godot_cpp/classes/video_stream_playback_extension.hpp>

namespace godot {

class VideoStreamReference : public VideoStreamExtension {
	GDCLASS(VideoStreamReference, VideoStreamExtension);
	
protected:
	static void _bind_methods();
public:
	virtual Ref<VideoStreamPlaybackExtension> _instance_playback() override;
	virtual PackedStringArray _get_supported_extensions() const override;

	virtual void _initialize() override;
	virtual void _cleanup() override;
	
	String get_plugin_name();

	VideoStreamReference();
	~VideoStreamReference();
};
}

#endif // VIDEO_STREAM_WEBM_H