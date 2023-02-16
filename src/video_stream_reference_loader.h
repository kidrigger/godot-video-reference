#include <godot_cpp/classes/resource_format_loader.hpp>
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/video_stream_playback.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "video_stream_reference.h"

namespace godot {

class VideoStreamReferenceLoader : public ResourceFormatLoader {
	GDCLASS(VideoStreamReferenceLoader, ResourceFormatLoader);

protected:
	static void _bind_methods() {}

public:
	virtual bool _handles_type(const StringName &type) const override;
	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual String _get_resource_type(const String &path) const override;
	virtual Variant _load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const override;
};

} // namespace godot