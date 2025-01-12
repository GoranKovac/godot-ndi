#include <godot_cpp/core/class_db.hpp>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <e131.h>

#include "e131_packet.h"


void E131Packet::_bind_methods() {
    BIND_ENUM_CONSTANT(E131_OPT_TERMINATED);
    BIND_ENUM_CONSTANT(E131_OPT_PREVIEW);

    ClassDB::bind_method(D_METHOD("set_option", "option", "state"), &E131Packet::set_option);
    ClassDB::bind_method(D_METHOD("get_option", "option"), &E131Packet::get_option);

    ClassDB::bind_method(D_METHOD("set_source_name", "name"), &E131Packet::set_source_name);
    ClassDB::bind_method(D_METHOD("get_source_name"), &E131Packet::get_source_name);

    ClassDB::bind_method(D_METHOD("set_priority", "priority"), &E131Packet::set_priority);
    ClassDB::bind_method(D_METHOD("get_priority"), &E131Packet::get_priority);

    ClassDB::bind_method(D_METHOD("set_data", "data"), &E131Packet::set_data);
    ClassDB::bind_method(D_METHOD("get_data"), &E131Packet::get_data);

    ClassDB::bind_method(D_METHOD("set_universe", "universe"), &E131Packet::set_universe);
    ClassDB::bind_method(D_METHOD("get_universe"), &E131Packet::get_universe);
    
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "source_name"), "set_source_name", "get_source_name");

    ADD_PROPERTY(PropertyInfo(Variant::INT, "priority", PROPERTY_HINT_RANGE, "0,200"), "set_priority", "get_priority");
    
    ADD_PROPERTY(PropertyInfo(Variant::INT, "universe", PROPERTY_HINT_RANGE, "1,63999"), "set_universe", "get_universe");
}

E131Packet::E131Packet() {
	e131_pkt_init(&packet, 1, 1);
}

void E131Packet::set_option(const e131_option_t option, const bool state) {
	e131_set_option(&packet, option, state);
}

bool E131Packet::get_option(const e131_option_t option) const {
	return e131_get_option(&packet, option);
}

void E131Packet::set_source_name(const String name) {
	strncpy((char *)packet.frame.source_name, name.utf8().ptr(), sizeof(packet.frame.source_name));
}

String E131Packet::get_source_name() const {
	return String((char *)packet.frame.source_name);
}

void E131Packet::set_priority(const uint8_t priority) {
    ERR_FAIL_COND_MSG(priority < 0 || priority > 200, "Priority has to be between 0 and 200 (inclusive)");
	packet.frame.priority = htons(priority);
}

uint8_t E131Packet::get_priority() const {
	return ntohs(packet.frame.priority);
}

void E131Packet::set_data(const PackedByteArray data) {
    ERR_FAIL_COND_MSG(data.size() < 0,   "Frame can't be empty");
    ERR_FAIL_COND_MSG(data.size() > 512, "Frame can't have more than 512 values");

    packet.dmp.prop_val_cnt = data.size() + 1;
    memcpy(&packet.dmp.prop_val[1], data.ptr(), data.size());
}

PackedByteArray E131Packet::get_data() const {
    PackedByteArray data = PackedByteArray();
    int64_t num_slots = Math::clamp<int64_t>(packet.dmp.prop_val_cnt - 1, 0, 512);

    data.resize(num_slots);
    memcpy(data.ptrw(), &packet.dmp.prop_val[1], num_slots);

    return data;
}

void E131Packet::set_universe(const uint16_t universe) {
    ERR_FAIL_COND_MSG(universe < 1 || universe > 63999, "Universe has to be greater than 0 and less than 64000");
    packet.frame.universe = htons(universe);
}

uint16_t E131Packet::get_universe() const {
	return ntohs(packet.frame.universe);
}
