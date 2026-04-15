#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H

#include "particle.h"
#include "constants.h"
#include <vector>
#include <random>

extern std::mt19937 gen;

std::vector<Particle> generateParticles(int n, double span, double airflowSpeed, double particleMass) {
    std::vector<Particle> particles;
    particles.reserve(n);

    std::uniform_real_distribution<double> y_dist(BOX_Y_MIN, BOX_Y_MAX);
    std::uniform_real_distribution<double> x_dist(PARTICLE_X_MIN, PARTICLE_X_MAX_FACTOR * span);

    for (int i = 0; i < n; ++i) {
        double y = y_dist(gen);
        double x = x_dist(gen);

        particles.push_back({x, y, 0.0, airflowSpeed, 0.0, 0.0, particleMass, 0.0});
    }

    return particles;
}

#endif // PARTICLE_GENERATOR_H
