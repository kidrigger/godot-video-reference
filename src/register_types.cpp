#include "register_types.h"

#include <godot/gdnative_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "video_stream_playback_reference.h"
#include "video_stream_reference.h"
#include <godot_cpp/classes/video_decoder_server.hpp>

using namespace godot;

Ref<VideoStreamReference> interface_ref;

void register_types(godot::ModuleInitializationLevel init_level) {
	if (init_level != godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SERVERS) return;

	ClassDB::register_class<VideoStreamPlaybackReference>();
	ClassDB::register_class<VideoStreamReference>();

	interface_ref.instantiate();

	VideoDecoderServer::add_interface(interface_ref);
}

void unregister_types(godot::ModuleInitializationLevel init_level) {
	if (init_level != godot::ModuleInitializationLevel::MODULE_INITIALIZATION_LEVEL_SERVERS) return;

	if (interface_ref.is_valid()) {

		VideoDecoderServer::remove_interface(interface_ref);
		interface_ref.unref();
	}
}
