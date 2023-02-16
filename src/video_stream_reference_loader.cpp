
#include "video_stream_reference_loader.h"
#include <godot_cpp/core/class_db.hpp>
#include "gdextension_interface.h"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/godot.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "video_stream_playback_reference.h"
#include "video_stream_reference.h"
#include <godot_cpp/classes/file_access.hpp>

bool godot::VideoStreamReferenceLoader::_handles_type(const StringName &type) const {
	return type.to_lower().contains("videostream");
}

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

godot::Variant godot::VideoStreamReferenceLoader::_load(const godot::String &path, const godot::String &original_path, bool use_sub_threads, int32_t cache_mode) const {
	Ref<FileAccess> f = FileAccess::open(path, FileAccess::READ);
	if (f.is_null()) {
		return Ref<Resource>();
	}

	Ref<VideoStreamReference> stream;
	stream.instantiate();
	stream->set_file(path);

	return stream;
}
