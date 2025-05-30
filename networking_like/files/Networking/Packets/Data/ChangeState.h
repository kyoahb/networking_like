#pragma once
#include "Utils/Imports/common.h"
#include "Networking/Packets/PacketData.h"

enum ChangeStateType : uint8_t {
    CHANGE_STATE_ANY = 255,
    CHANGE_STATE_TO = 1,
};

// Server -> Client
// Subtype 1 (CHANGE_STATE_TO)
class ChangeStateTo : public PacketData {
public:
    std::string state_name;

    template <typename Archive>
    void serialize(Archive& archive) {
        archive(state_name);
    }
};