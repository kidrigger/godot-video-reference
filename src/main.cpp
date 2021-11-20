
#include "register_types.h"

#include <godot/gdnative_interface.h>
#include <godot_cpp/core/class_db.hpp>

extern "C" {

// Initialization.

GDNativeBool GDN_EXPORT godot_video_reference_library_init(const GDNativeInterface *p_interface, const GDNativeExtensionClassLibraryPtr p_library, GDNativeInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

	init_obj.register_driver_initializer(register_types);
	init_obj.register_driver_terminator(unregister_types);

	return init_obj.init();
}
	
}
