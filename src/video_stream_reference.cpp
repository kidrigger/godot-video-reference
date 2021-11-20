
#include "video_stream_reference.h"
#include "video_stream_playback_reference.h"

void godot::VideoStreamReference::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_plugin_name"), &VideoStreamReference::get_plugin_name);
}

godot::Ref<godot::VideoStreamPlaybackExtension> godot::VideoStreamReference::_instance_playback() {
	Ref<VideoStreamPlaybackReference> ref;
	ref.instantiate();
	return ref;
}

godot::PackedStringArray godot::VideoStreamReference::_get_supported_extensions() const {
	PackedStringArray psa;
	return psa;
}

void godot::VideoStreamReference::_initialize() {}
void godot::VideoStreamReference::_cleanup() {}

godot::String godot::VideoStreamReference::get_plugin_name() {
	return "Godot GDExtension VideoDecoder Reference";
}

godot::VideoStreamReference::VideoStreamReference() {}
godot::VideoStreamReference::~VideoStreamReference() {}
