#ifndef WING_H
#define WING_H

struct Wing {
    double x0;          // leading edge x position
    double y0;          // vertical bottom
    double z0;          // start of span
    double chord;       // x
    double span;        // z
    double thickness_ratio;  // thickness ratio (e.g. 0.12)
};

#endif // WING_H
