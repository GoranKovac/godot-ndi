#include "ndi_find.h"

void NDIFind::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_show_local_sources", "show_local_sources"), &NDIFind::set_show_local_sources);
    ClassDB::bind_method(D_METHOD("get_show_local_sources"), &NDIFind::get_show_local_sources);

    ClassDB::bind_method(D_METHOD("set_groups", "groups"), &NDIFind::set_groups);
    ClassDB::bind_method(D_METHOD("get_groups"), &NDIFind::get_groups);

    ClassDB::bind_method(D_METHOD("set_extra_ips", "extra_ips"), &NDIFind::set_extra_ips);
    ClassDB::bind_method(D_METHOD("get_extra_ips"), &NDIFind::get_extra_ips);

    ClassDB::bind_method(D_METHOD("get_sources"), &NDIFind::get_sources);

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "show_local_sources"), "set_show_local_sources", "get_show_local_sources");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "groups"), "set_groups", "get_groups");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "extra_ips"), "set_extra_ips", "get_extra_ips");
}

NDIFind::NDIFind() {
	find = lib->find_create_v2(&find_desc);
}

NDIFind::~NDIFind() {
	lib->find_destroy(find);
}

void NDIFind::set_show_local_sources(const bool state) {
	find_desc.show_local_sources = state;

	lib->find_destroy(find);
	find = lib->find_create_v2(&find_desc);
}

bool NDIFind::get_show_local_sources() const {
	return find_desc.show_local_sources;
}

void NDIFind::set_groups(const PackedStringArray _groups) {
	groups = _groups;

	find_desc.p_groups = NULL;
	if (groups.size() > 0) {
		find_desc.p_groups = String(",").join(groups).utf8();
	}

	lib->find_destroy(find);
	find = lib->find_create_v2(&find_desc);
}

PackedStringArray NDIFind::get_groups() const {
	if (find_desc.p_groups == NULL) {
		return PackedStringArray();
	}

	return String::utf8(find_desc.p_groups).split(",", false);
}

void NDIFind::set_extra_ips(const PackedStringArray _extra_ips) {
	extra_ips = _extra_ips;
	
	find_desc.p_extra_ips = NULL;
	if (extra_ips.size() > 0) {
		find_desc.p_extra_ips = String(",").join(extra_ips).utf8();
	}

	lib->find_destroy(find);
	find = lib->find_create_v2(&find_desc);
}

PackedStringArray NDIFind::get_extra_ips() const {
	if (find_desc.p_extra_ips == NULL) {
		return PackedStringArray();
	}

	return String::utf8(find_desc.p_extra_ips).split(",", false);
}

TypedArray<VideoStreamNDI> NDIFind::get_sources() const {
	TypedArray<VideoStreamNDI> sources;

	uint32_t num_sources = 0;
	const NDIlib_source_t *sources_pointer = lib->find_get_current_sources(find, &num_sources);

	for (int i = 0; i < num_sources; i++) {
		VideoStreamNDI* source = memnew(VideoStreamNDI(sources_pointer[i]));
		sources.push_back(source);
	}

	return sources;
}