#ifndef COLLISION_H
#define COLLISION_H

#include "particle.h"
#include "wing.h"
#include "constants.h"
#include <vector>
#include <algorithm>
#include <cmath>

bool hitsWing(const Particle &p,
              const Wing &w,
              const std::vector<float>& wingThickness,
              const std::vector<float>& camberLine)
{
    if (p.x < w.x0 || p.x > w.x0 + w.chord) return false;
    if (p.z < w.z0 || p.z > w.z0 + w.span) return false;

    int n = (int)wingThickness.size();
    if (n < 2 || camberLine.size() != wingThickness.size()) return false;

    float normalizedX = (p.x - w.x0) / w.chord;
    int idx = (int)(normalizedX * (n - 1));
    idx = std::clamp(idx, 0, n - 1);

    float yt = wingThickness[idx];   // thickness distribution
    float yc = camberLine[idx];      // camberline

    // Apply angle of attack: wing tilts up at trailing edge
    // At x=0 (leading edge): no offset
    // At x=chord (trailing edge): offset = chord * sin(AoA) ≈ chord * AoA
    float aoaOffset = (p.x - w.x0) * std::tan(ANGLE_OF_ATTACK);

    float upperY = w.y0 + yc + yt - aoaOffset;
    float lowerY = w.y0 + yc - yt - aoaOffset;

    return (p.y >= lowerY && p.y <= upperY);
}

#endif // COLLISION_H
