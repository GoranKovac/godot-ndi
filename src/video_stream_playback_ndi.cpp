/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "video_stream_playback_ndi.hpp"

VideoStreamPlaybackNDI::VideoStreamPlaybackNDI() {
    texture.instantiate();

    // Fall back to 1080p, source can be something else, but won't scale to that until UI redraw (see issue #1)
    texture->set_image(Image::create_empty(1920, 1080, false, Image::FORMAT_RGBA8));
}

VideoStreamPlaybackNDI::~VideoStreamPlaybackNDI() {
    texture.unref();
}

void VideoStreamPlaybackNDI::setup(const NDIlib_recv_create_v3_t *_recv_desc) {
    recv_desc = _recv_desc;

    if (!start_receiving()) {
        return;
    }

    if (ndi->recv_capture_v3(recv, &video_frame, NULL, NULL, 5000) == NDIlib_frame_type_video) {
        texture->set_image(Image::create_empty(video_frame.xres, video_frame.yres, false, Image::FORMAT_RGBA8));
        ndi->recv_free_video_v2(recv, &video_frame);
    } else {
        UtilityFunctions::push_warning("NDI Source not found");
    }
}

void VideoStreamPlaybackNDI::_play() {
    if (!start_receiving()) {
        return;
    }

    if (!start_syncing()) {
        return;
    }
    
    playing = true;
}

void VideoStreamPlaybackNDI::_stop() {
    playing = false;
    stop_syncing();
    stop_receiving();
}

bool VideoStreamPlaybackNDI::_is_playing() const {
	return playing;
}

void VideoStreamPlaybackNDI::_set_paused(bool p_paused) {
    paused = p_paused;
}

bool VideoStreamPlaybackNDI::_is_paused() const {
	return paused;
}

double VideoStreamPlaybackNDI::_get_length() const {
	return 0.0;
}

double VideoStreamPlaybackNDI::_get_playback_position() const {
	return 0.0;
}

void VideoStreamPlaybackNDI::_seek(double p_time) { }

void VideoStreamPlaybackNDI::_set_audio_track(int32_t p_idx) { }

int32_t VideoStreamPlaybackNDI::_get_channels() const {
    return 2;
}

int32_t VideoStreamPlaybackNDI::_get_mix_rate() const {
    return ProjectSettings::get_singleton()->get_setting("audio/driver/mix_rate", 41400);
}

Ref<Texture2D> VideoStreamPlaybackNDI::_get_texture() const {
	return texture;
}

void VideoStreamPlaybackNDI::_update(double p_delta) {
    ERR_FAIL_COND_MSG(receiving == false || syncing == false, "VideoStreamPlaybackNDI wasn't setup properly. Call setup() before passing to VideoStreamPlayer!");
    render_video();
    render_audio(p_delta);
}

void VideoStreamPlaybackNDI::_bind_methods() {}

bool VideoStreamPlaybackNDI::start_receiving(bool force) {
    if (force || !receiving) {
        ERR_FAIL_NULL_V_MSG(recv_desc, false, "Can't create a NDI Receiver without description");
        stop_receiving();
        recv = ndi->recv_create_v3(recv_desc);
        receiving = true;
    }

    return true;
}

void VideoStreamPlaybackNDI::stop_receiving() {
    if (receiving) {
        stop_syncing();
        ndi->recv_destroy(recv);
        receiving = false;
    }
}

bool VideoStreamPlaybackNDI::start_syncing(bool force) {
    if (force || !syncing) {
        if (!receiving) {
            ERR_FAIL_COND_V_MSG(!start_receiving(), false, "Can't create a NDI Framesync without a NDI Receiver");
        }

        stop_syncing();
        sync = ndi->framesync_create(recv);
        syncing = true;
    }

    return true;
}

void VideoStreamPlaybackNDI::stop_syncing() {
    if (syncing) {
        ndi->framesync_destroy(sync);
        syncing = false;
    }
}

void VideoStreamPlaybackNDI::render_video() {
    ndi->framesync_capture_video(sync, &video_frame, NDIlib_frame_format_type_progressive);

    if (video_frame.p_data != NULL && (video_frame.FourCC == NDIlib_FourCC_type_RGBA || video_frame.FourCC == NDIlib_FourCC_type_RGBX))
    {
        video_buffer.resize(video_frame.line_stride_in_bytes * video_frame.yres);
        memcpy(video_buffer.ptrw(), video_frame.p_data, video_buffer.size());
        
        Ref<Image> img = Image::create_from_data(video_frame.xres, video_frame.yres, false, Image::Format::FORMAT_RGBA8, video_buffer);
        texture->set_image(img);
        img.unref();
    }

    ndi->framesync_free_video(sync, &video_frame);
}

void VideoStreamPlaybackNDI::render_audio(double p_delta) {

    // This is clipped in an attempt to fix issue #3.
    // FIXME: Should be clipping no_samples to buffer size because that's what causes the error.
    //        But no clue what buffer is meant. The one from Project Settings or the one of VideoStreamPlayer (Buffer msec)?

    p_delta = UtilityFunctions::min(p_delta, 0.5);

    int no_samples = (double)_get_mix_rate() * p_delta;
    ndi->framesync_capture_audio_v2(sync, &audio_frame, _get_mix_rate(), _get_channels(), no_samples);

    if (audio_frame.p_data != NULL)
    {
        audio_buffer_planar.resize(audio_frame.no_channels * audio_frame.no_samples);
        audio_buffer_interleaved.resize(audio_buffer_planar.size());

        memcpy(audio_buffer_planar.ptrw(), audio_frame.p_data, audio_buffer_planar.size()*4);
        
        for (int64_t i = 0; i < audio_buffer_interleaved.size(); i++)
        {
            int channel = i % audio_frame.no_channels;
            int stride_index = channel * audio_frame.no_samples;
            int stride_offset = i / audio_frame.no_channels;
            
            audio_buffer_interleaved.set(i, audio_buffer_planar[stride_index + stride_offset]);
        }

        mix_audio(audio_frame.no_samples, audio_buffer_interleaved, 0);
    }

    ndi->framesync_free_audio_v2(sync, &audio_frame);
}