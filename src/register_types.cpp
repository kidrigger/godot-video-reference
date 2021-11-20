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

void register_types() {
	ClassDB::register_class<VideoStreamPlaybackReference>();
	ClassDB::register_class<VideoStreamReference>();

	VideoDecoderServer* p_server = VideoDecoderServer::get_singleton();
	ERR_FAIL_NULL(p_server);

	interface_ref.instantiate();
	p_server->add_interface(interface_ref);
}

void unregister_types() {
	if (interface_ref.is_valid()) {
		VideoDecoderServer* p_server = VideoDecoderServer::get_singleton();
		ERR_FAIL_NULL(p_server);

		p_server->remove_interface(interface_ref);
		interface_ref.unref();
	}
}
