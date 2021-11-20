
#include "video_stream_playback_reference.h"

#include "video_stream_reference.h"

#include <godot_cpp/core/class_db.hpp>

void godot::VideoStreamPlaybackReference::_bind_methods() {
}

void godot::VideoStreamPlaybackReference::_stop() {
	playing = false;
}

void godot::VideoStreamPlaybackReference::_play() {
	playing = true;
}

bool godot::VideoStreamPlaybackReference::_is_playing() const {
	return playing;
}

void godot::VideoStreamPlaybackReference::_set_paused(bool paused) {
	this->paused = paused;
}

bool godot::VideoStreamPlaybackReference::_is_paused() const {
	return paused;
}

void godot::VideoStreamPlaybackReference::_set_loop(bool enable) {
}

bool godot::VideoStreamPlaybackReference::_has_loop() const {
	return false;
}

double godot::VideoStreamPlaybackReference::_get_length() const {
	return 0.0;
}

double godot::VideoStreamPlaybackReference::_get_playback_position() const {
	return 0.0;
}

void godot::VideoStreamPlaybackReference::_seek(double time) {
}

void godot::VideoStreamPlaybackReference::_set_audio_track(int64_t idx) {
}

godot::Ref<godot::Texture2D> godot::VideoStreamPlaybackReference::_get_texture() const {
	return nullptr;
}

void godot::VideoStreamPlaybackReference::_update(double delta) {
}

int64_t godot::VideoStreamPlaybackReference::_get_channels() const {
	return 0;
}

int64_t godot::VideoStreamPlaybackReference::_get_mix_rate() const {
	return 0;
}

void godot::VideoStreamPlaybackReference::_initialize() {
	if (!initialized) {
		texture.instantiate();
		initialized = true;
	}
}

void godot::VideoStreamPlaybackReference::_cleanup() {
	if (initialized) {
		texture.unref();
		initialized = false;
	}
}

godot::VideoStreamPlaybackReference::~VideoStreamPlaybackReference() {
}
