
#include "video_stream_reference.h";
#include <godot_cpp/classes/video_stream_loader.hpp>

namespace godot {

class VideoStreamReferenceLoader : public VideoStreamLoader {
	GDCLASS(VideoStreamReferenceLoader, VideoStreamLoader);

protected:
	static void _bind_methods() {}

public:
	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual String _get_resource_type(const String &path) const override;
	virtual Object *_instantiate_video_stream() override {
		return memnew(VideoStreamReference());
	}
};

} // namespace godot