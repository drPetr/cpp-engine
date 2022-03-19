#pragma once
#include <core/input.hpp>
namespace engine {

class onoff_key : public core::input::raw_input {
public:
                    onoff_key( key dik ) : onOffDik{dik} {}

    virtual bool    on_key_down( key dik ) override;
    bool            is_active();
private:
    key             onOffDik;
    bool            isActive{false};
};

/* onoff_key::is_active */
inline bool onoff_key::is_active() {
    return isActive;
}

} /* namespace engine */