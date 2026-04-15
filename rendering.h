#ifndef RENDERING_H
#define RENDERING_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "particle.h"
#include "wing.h"
#include "constants.h"
#include <vector>
#include <cmath>

void drawWing(const Wing& wing,
              const std::vector<float>& wingThickness,
              const std::vector<float>& camberLine)
{
    glColor3f(WING_COLOR_R, WING_COLOR_G, WING_COLOR_B);

    int n = (int)wingThickness.size();
    if (n < 2 || camberLine.size() != wingThickness.size()) return;

    glBegin(GL_LINE_STRIP);

    // upper surface (with AoA)
    for (int i = 0; i < n; ++i) {
        float x = wing.x0 + wing.chord * i / (n - 1);
        float aoaOffset = (x - wing.x0) * std::tan(ANGLE_OF_ATTACK);
        float y = wing.y0 + camberLine[i] + wingThickness[i] - aoaOffset;
        glVertex2f(x, y);
    }

    // lower surface (with AoA)
    for (int i = n - 1; i >= 0; --i) {
        float x = wing.x0 + wing.chord * i / (n - 1);
        float aoaOffset = (x - wing.x0) * std::tan(ANGLE_OF_ATTACK);
        float y = wing.y0 + camberLine[i] - wingThickness[i] - aoaOffset;
        glVertex2f(x, y);
    }

    glEnd();

    // camber line (with AoA)
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < n; ++i) {
        float x = wing.x0 + wing.chord * i / (n - 1);
        float aoaOffset = (x - wing.x0) * std::tan(ANGLE_OF_ATTACK);
        float y = wing.y0 + camberLine[i] - aoaOffset;
        glVertex2f(x, y);
    }
    glEnd();
}


void drawParticles(const std::vector<Particle>& particles, const Wing& wing, const std::vector<float>& wingThickness) {
    glPointSize(POINT_SIZE);
    glBegin(GL_POINTS);

    for (const auto& p : particles) {
        // Determine color based on collision status
        if (p.x >= wing.x0 && p.x <= wing.x0 + wing.chord) {
            int idx = static_cast<int>((p.x - wing.x0) / wing.chord * WING_POINTS);
            if (idx < 0) idx = 0;
            if (idx >= WING_POINTS) idx = WING_POINTS - 1;

            float t = wingThickness[idx];
            bool colliding = (p.y >= wing.y0 - t / AIRFOIL_BOTTOM_DIVISOR) &&
                           (p.y <= wing.y0 + t);

            if (colliding) {
                glColor3f(COLLISION_COLOR_R, COLLISION_COLOR_G, COLLISION_COLOR_B);
            } else {
                glColor3f(PARTICLE_COLOR_R, PARTICLE_COLOR_G,
                         (p.vx*p.vx + p.vy*p.vy + p.vz*p.vz)*(p.vx*p.vx + p.vy*p.vy + p.vz*p.vz) / PRESSURE_COLOR_DIVISOR / 1000);
            }
        } else {
            glColor3f(PARTICLE_COLOR_R, PARTICLE_COLOR_G,
                     (p.vx*p.vx + p.vy*p.vy + p.vz*p.vz)*(p.vx*p.vx + p.vy*p.vy + p.vz*p.vz) / PRESSURE_COLOR_DIVISOR / 1000);
        }

        glVertex2f(p.x, p.y);
    }

    glEnd();
}

#endif // RENDERING_H
