
#include "video_stream_reference.h"
#include "video_stream_playback_reference.h"

void godot::VideoStreamReference::_bind_methods() {
}

godot::Object *godot::VideoStreamReference::_instantiate_playback() {
	VideoStreamPlaybackReference *ref = memnew(VideoStreamPlaybackReference());
	return static_cast<Object *>(ref);
}

godot::PackedStringArray godot::VideoStreamReference::_get_supported_extensions() const {
	PackedStringArray psa;
	psa.append("webm");
	return psa;
}

void godot::VideoStreamReference::_initialize() {}
void godot::VideoStreamReference::_cleanup() {}

godot::String godot::VideoStreamReference::_get_plugin_name() const {
	return "Godot GDExtension VideoDecoder Reference";
}

godot::VideoStreamReference::VideoStreamReference() {}
godot::VideoStreamReference::~VideoStreamReference() {}
