
#include "video_stream_reference_loader.h"
#include "video_stream_reference.h"

godot::PackedStringArray godot::VideoStreamReferenceLoader::_get_recognized_extensions() const {
	PackedStringArray psa;
	psa.append("webm");
	return psa;
}

godot::String godot::VideoStreamReferenceLoader::_get_resource_type(const godot::String &p_path) const {
	String el = p_path.get_extension().to_lower();
	if (el == "webm") {
		return "VideoStreamReference";
	}
	return "";
}