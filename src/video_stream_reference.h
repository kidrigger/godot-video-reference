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
	virtual Ref<VideoStreamPlayback> _instantiate_playback() override;

	VideoStreamReference();
	~VideoStreamReference();
};
} // namespace godot

#endif // VIDEO_STREAM_WEBM_H