#include "onoff_key.h"
namespace engine {

/* onoff_key::on_key_down */
bool onoff_key::on_key_down( key dik ) {
    if( dik == onOffDik ) {
        isActive = !isActive;
        return true;
    }
    return false;
}

} /* namespace engine */