#pragma once

#include <godot_cpp/classes/video_stream.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "ndi.h"
#include "video_stream_playback_ndi.h"

using namespace godot;

class VideoStreamNDI : public VideoStream {
    GDCLASS(VideoStreamNDI, VideoStream)

    NDIlib_recv_create_v3_t recv_desc;
    
    CharString name;
    CharString url;

    protected:
        static void _bind_methods();

    public:
        VideoStreamNDI();
        VideoStreamNDI(NDIlib_source_t source);
        ~VideoStreamNDI();
        void set_name(const String _name);
        String get_name() const;
        void set_url(const String _url);
        String get_url() const;
        void set_bandwidth(const NDIlib_recv_bandwidth_e bandwidth);
        NDIlib_recv_bandwidth_e get_bandwidth() const;
        Ref<VideoStreamPlayback> _instantiate_playback() override;
};

VARIANT_ENUM_CAST(NDIlib_recv_bandwidth_e);
