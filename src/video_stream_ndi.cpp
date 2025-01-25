/*
https://github.com/unvermuthet/godot-ndi
		(C) 2025 Henry Muth - unvermuthet

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this
file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "video_stream_ndi.hpp"

// The only persistent parts of this resource are the name and bandwidth fields.
// I've decided to not expose/bind the url field of the NDI_source_t struct.
// Its only set when obtaining a VideoStreamNDI from NDIFind.
// When creating this resource by hand or loading it from disk the url field stays NULL.
// The SDK will spin up a find instance itself and obtain the url that way.
// Read more in ndi/Processing.NDI.structs.h at line 182

void VideoStreamNDI::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_name", "name"), &VideoStreamNDI::set_name);
    ClassDB::bind_method(D_METHOD("get_name"), &VideoStreamNDI::get_name);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");

    // ClassDB::bind_method(D_METHOD("set_url", "url"), &VideoStreamNDI::set_url);
    // ClassDB::bind_method(D_METHOD("get_url"), &VideoStreamNDI::get_url);
    // ADD_PROPERTY(PropertyInfo(Variant::STRING, "url", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE), "set_url", "get_url");

    ClassDB::bind_method(D_METHOD("set_bandwidth", "bandwidth"), &VideoStreamNDI::set_bandwidth);
    ClassDB::bind_method(D_METHOD("get_bandwidth"), &VideoStreamNDI::get_bandwidth);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "bandwidth", PROPERTY_HINT_ENUM, "Metadata only:-10,Audio only:10,Lowest:0,Highest:100"), "set_bandwidth", "get_bandwidth");

    BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_metadata_only);
    BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_audio_only);
    BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_lowest);
    BIND_ENUM_CONSTANT(NDIlib_recv_bandwidth_highest);
}

VideoStreamNDI::VideoStreamNDI() {
    recv_desc.allow_video_fields = false;
    recv_desc.color_format = NDIlib_recv_color_format_RGBX_RGBA;
    recv_desc.bandwidth = NDIlib_recv_bandwidth_highest;
    recv_desc.p_ndi_recv_name = NULL;
}

VideoStreamNDI::VideoStreamNDI(NDIlib_source_t source) : VideoStreamNDI::VideoStreamNDI() {
    recv_desc.source_to_connect_to = source;
}

VideoStreamNDI::~VideoStreamNDI() { }

void VideoStreamNDI::set_name(const String _name) {
    name = _name.utf8();
    recv_desc.source_to_connect_to.p_ndi_name = name;
}

String VideoStreamNDI::get_name() const {
	if (recv_desc.source_to_connect_to.p_ndi_name == NULL) {
        return String();
    }

    return String::utf8(recv_desc.source_to_connect_to.p_ndi_name);
}

// void VideoStreamNDI::set_url(const String _url) {
//     if (_url.is_empty()) {
//         recv_desc.source_to_connect_to.p_url_address = NULL;
//         return;
//     }
//     url = _url.utf8();
//     recv_desc.source_to_connect_to.p_url_address = url;
// }

// String VideoStreamNDI::get_url() const {
// 	if (recv_desc.source_to_connect_to.p_url_address == NULL) {
//         return String();
//     }

//     return String::utf8(recv_desc.source_to_connect_to.p_url_address);
// }

void VideoStreamNDI::set_bandwidth(const NDIlib_recv_bandwidth_e bandwidth) {
    recv_desc.bandwidth = bandwidth;    
}

NDIlib_recv_bandwidth_e VideoStreamNDI::get_bandwidth() const {
    return recv_desc.bandwidth;
}

Ref<VideoStreamPlayback> VideoStreamNDI::_instantiate_playback() {
    Ref<VideoStreamPlaybackNDI> p = memnew(VideoStreamPlaybackNDI);
    p->setup(&recv_desc);
    return p;
}
