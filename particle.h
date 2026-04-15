#ifndef PARTICLE_H
#define PARTICLE_H

struct Particle {
    double x, y, z;
    double vx, vy, vz;
    double particleMass;
    double density;  // Measured local density
    double pressure; // Calculated from density
};

#endif // PARTICLE_H
