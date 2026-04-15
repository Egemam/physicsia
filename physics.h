#ifndef PHYSICS_H
#define PHYSICS_H

#include "particle.h"
#include "constants.h"
#include <vector>
#include <cmath>

void updatePressures(std::vector<Particle>& particles) {
    // Not needed for simple repulsion model
    (void)particles;
}

// Simple repulsion - particles push apart when close
void applyPressureForces(std::vector<Particle>& particles) {
    const double h = std::sqrt(INFLUENCE_RADIUS_SQ);
    const size_t n = particles.size();

    for (size_t i = 0; i < n; ++i) {
        auto& p = particles[i];

        // Particle-particle repulsion
        for (size_t j = i + 1; j < n; ++j) {
            auto& q = particles[j];

            double dx = p.x - q.x;
            double dy = p.y - q.y;
            double r2 = dx*dx + dy*dy;

            if (r2 < INFLUENCE_RADIUS_SQ && r2 > 1e-10) {
                double r = std::sqrt(r2);

                // Normalize
                double nx = dx / r;
                double ny = dy / r;

                // Repulsion force - quadratic falloff
                double overlap = 1.0 - r/h;
                double forceMag = REPULSION_STRENGTH * overlap * overlap;

                p.vx += forceMag * nx;
                p.vy += forceMag * ny;
                q.vx -= forceMag * nx;
                q.vy -= forceMag * ny;
            }
        }

        // Soft boundary forces (top/bottom only)
        if (p.y > BOX_Y_MAX) {
            p.vy -= BOUNDARY_PUSH * (p.y - BOX_Y_MAX);
        }
        if (p.y < BOX_Y_MIN) {
            p.vy += BOUNDARY_PUSH * (BOX_Y_MIN - p.y);
        }
    }
}

void updateParticles(std::vector<Particle>& particles) {
    for (auto& p : particles) {
        p.vx *= (1.0 - DRAG);
        p.vy *= (1.0 - DRAG);

        p.x += p.vx;
        p.y += p.vy;
    }
}

#endif // PHYSICS_H
