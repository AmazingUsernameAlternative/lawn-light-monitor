#include "conversion.h"

namespace lawn_light {

float lux_to_ppfd(float lux, float k) {
    return lux * k;
}

}