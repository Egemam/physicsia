#ifndef AIRFOIL_H
#define AIRFOIL_H

#include "constants.h"
#include <cmath>

float airfoilThickness(float x, float t = WING_THICKNESS_RATIO, float chord = WING_CHORD) {
    float xe = x / chord;
    return AIRFOIL_SCALE * t * (
        AIRFOIL_COEF_1 * sqrtf(xe)
      - AIRFOIL_COEF_2 * xe
      - AIRFOIL_COEF_3 * xe * xe
      + AIRFOIL_COEF_4 * xe * xe * xe
      - AIRFOIL_COEF_5 * xe * xe * xe * xe
    );
}

#endif // AIRFOIL_H
