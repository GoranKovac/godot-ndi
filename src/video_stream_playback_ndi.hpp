/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <godot_cpp/classes/video_stream_playback.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include "ndi.hpp"

using namespace godot;

class VideoStreamPlaybackNDI : public VideoStreamPlayback {
    GDCLASS(VideoStreamPlaybackNDI, VideoStreamPlayback)

    NDIlib_recv_create_v3_t recv_desc;
    NDIlib_recv_instance_t recv;
    NDIlib_framesync_instance_t sync;

    NDIlib_video_frame_v2_t video_frame;
    PackedByteArray video_buffer;
    Ref<ImageTexture> texture;
    Vector2i resolution;

    NDIlib_audio_frame_v3_t audio_frame;
    PackedFloat32Array audio_buffer_planar;
    PackedFloat32Array audio_buffer_interleaved;

    bool playing;
    bool paused;

    protected:
        static void _bind_methods();

    public:
        VideoStreamPlaybackNDI();
        VideoStreamPlaybackNDI(NDIlib_recv_create_v3_t recv_desc);
        ~VideoStreamPlaybackNDI();
        void _stop() override;
        void _play() override;
        bool _is_playing() const override;
        void _update(double p_delta) override;
        void _set_paused(bool p_paused) override;
        bool _is_paused() const override;
        double _get_length() const override;
        double _get_playback_position() const override;
        void _seek(double p_time) override;
        void _set_audio_track(int32_t p_idx) override;
        Ref<Texture2D> _get_texture() const override;
        int32_t _get_channels() const override;
        int32_t _get_mix_rate() const override;
        void render_video();
        void render_audio(double p_delta);
};

