#include "register_types.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "video_stream_playback_reference.h"
#include "video_stream_reference.h"
#include "video_stream_reference_loader.h"

#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;

Ref<VideoStreamReferenceLoader> loader;

void register_types(godot::ModuleInitializationLevel init_level) {
	if (init_level != godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) return;

	GDREGISTER_CLASS(VideoStreamReferenceLoader);
	GDREGISTER_CLASS(VideoStreamReference);
	GDREGISTER_CLASS(VideoStreamPlaybackReference);

	loader.instantiate();
	ResourceLoader::get_singleton()->add_resource_format_loader(loader, true);
}

void unregister_types(godot::ModuleInitializationLevel init_level) {
	if (init_level != godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SCENE) return;

	ResourceLoader::get_singleton()->remove_resource_format_loader(loader);
	loader.unref();
}
