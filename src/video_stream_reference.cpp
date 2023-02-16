
#include "video_stream_reference.h"
#include "godot_cpp/core/error_macros.hpp"
#include "video_stream_playback_reference.h"

void godot::VideoStreamReference::_bind_methods() {
}

godot::Ref<godot::VideoStreamPlayback> godot::VideoStreamReference::_instantiate_playback() {
	VideoStreamPlaybackReference *ref = memnew(VideoStreamPlaybackReference());
	ref->_set_audio_track(audio_track);
	if (ref->open_file(get_file())) {
		return ref;
	}
	return nullptr;
}

godot::VideoStreamReference::VideoStreamReference() {}
godot::VideoStreamReference::~VideoStreamReference() {}
