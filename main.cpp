#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <random>

// Include all modular headers
#include "constants.h"
#include "particle.h"
#include "wing.h"
#include "airfoil.h"
#include "particle_generator.h"
#include "collision.h"
#include "physics.h"
#include "rendering.h"

// -------- Random Number Generation --------
std::random_device rd;
std::mt19937 gen(rd());

// Generate the camber line for the wing
std::vector<float> generateCamberLine(float chord, int points, float m, float p) {
    std::vector<float> camber(points);

    for (int i = 0; i < points; i++) {
        float x = static_cast<float>(i) / static_cast<float>(points - 1);  // normalized 0..1

        float yc;
        if (x < p) {
            yc = (m / (p * p)) * (2 * p * x - x * x);
        } else {
            yc = (m / ((1 - p) * (1 - p))) * ((1 - 2 * p) + 2 * p * x - x * x);
        }

        camber[i] = yc * chord;
    }

    return camber;
}


// Helper function for particle boundary handling
void handleParticleBoundaries(Particle& p) {
    // Reset particle if it goes too far in X
    if (p.x > RESET_X_THRESHOLD || p.x < -RESET_X_THRESHOLD) {
        p.x = RESET_X_POSITION;
        std::uniform_real_distribution<double> y_dist(BOX_Y_MIN, BOX_Y_MAX);
        p.y = y_dist(gen);
        p.vx = AIRFLOW_SPEED;
        p.vy = 0;
        p.vz = 0;
    }

    // Hard clamp Y position and bounce
    if (p.y < BOX_Y_MIN) {
        p.y = BOX_Y_MIN;
        p.vy = std::abs(p.vy) * 0.8;
    }
    if (p.y > BOX_Y_MAX) {
        p.y = BOX_Y_MAX;
        p.vy = -std::abs(p.vy) * 0.8;
    }
}

// Helper function for wing collision
void handleWingCollision(Particle& p, const Wing& wing, const std::vector<float>& wingThickness, const std::vector<float>& camberLine, double& drag, double& lift) {
    // Check if particle is in wing's x range
    if (p.x < wing.x0 || p.x > wing.x0 + wing.chord) return;

    // Map particle x to wing index
    int n = static_cast<int>(wingThickness.size());
    float normalizedX = static_cast<float>((p.x - wing.x0) / wing.chord);
    int idx = static_cast<int>(normalizedX * (n - 1));
    if (idx < 0) idx = 0;
    if (idx >= n - 1) idx = n - 2;

    // Get local camber and thickness
    float yc = camberLine[idx];
    float yt = wingThickness[idx];

    // AoA offset at this x position
    float aoaOffset = static_cast<float>(p.x - wing.x0) * std::tan(ANGLE_OF_ATTACK);

    // Wing surface positions
    float upperY = static_cast<float>(wing.y0) + yc + yt - aoaOffset;
    float lowerY = static_cast<float>(wing.y0) + yc - yt - aoaOffset;

    // Check if particle is inside wing
    if (p.y < lowerY || p.y > upperY) return;

    // Particle is inside the wing - need to push it out
    // Save old velocity for impulse calculation
    double old_vx = p.vx;
    double old_vy = p.vy;

    // Determine closest surface
    float distToUpper = upperY - static_cast<float>(p.y);
    float distToLower = static_cast<float>(p.y) - lowerY;
    bool pushUp = distToUpper < distToLower;

    // Surface normal based on AoA
    float nx, ny;
    if (pushUp) {
        nx = std::sin(ANGLE_OF_ATTACK);
        ny = std::cos(ANGLE_OF_ATTACK);
    } else {
        nx = -std::sin(ANGLE_OF_ATTACK);
        ny = -std::cos(ANGLE_OF_ATTACK);
    }

    // Reflect velocity
    double v_dot_n = p.vx * nx + p.vy * ny;
    if (v_dot_n < 0) {
        p.vx -= 2.0 * v_dot_n * nx;
        p.vy -= 2.0 * v_dot_n * ny;
    }

    // Push particle out - use larger epsilon to prevent re-entry
    float pushDist = 0.005f;
    if (pushUp) {
        p.y = upperY + pushDist;
    } else {
        p.y = lowerY - pushDist;
    }

    // Energy loss
    p.vx *= 0.85;
    p.vy *= 0.85;

    // Calculate impulse
    double dvx = p.vx - old_vx;
    double dvy = p.vy - old_vy;
    drag -= p.particleMass * dvx;
    lift -= p.particleMass * dvy;
}

// -------- Main Simulation Loop --------
int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    // Precompute wing thickness
    std::vector<float> wingThickness(WING_POINTS + 1);
    for (int i = 0; i <= WING_POINTS; ++i) {
        float x = WING_CHORD * static_cast<float>(i) / WING_POINTS;
        wingThickness[i] = airfoilThickness(x);
    }

    float m = 0.06f;   // 6% camber (increased for more lift)
    float p = 0.4f;    // max camber at 40% chord

    std::vector<float> camberLine = generateCamberLine(WING_CHORD, WING_POINTS + 1, m, p);

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    // Setup OpenGL projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(PROJ_LEFT, PROJ_RIGHT, PROJ_BOTTOM, PROJ_TOP, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    // Create wing (slightly below center to simulate angle of attack effect)
    Wing main_wing = {
        0, -0.02, 0,
        WING_CHORD,
        WING_SPAN,
        WING_THICKNESS_RATIO
    };

    // Generate particles
    std::vector<Particle> particles = generateParticles(
        static_cast<int>(PARTICLE_AMOUNT),
        main_wing.chord,
        AIRFLOW_SPEED,
        PARTICLE_MASS
    );

    std::cout << "Simulation started with " << particles.size() << " particles\n";

    int frameCount = 0;
    double totalLift = 0;
    double totalDrag = 0;

    // Main simulation loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        // -------- Physics Simulation --------
        updatePressures(particles);
        applyPressureForces(particles);

        // -------- Handle collisions and boundaries --------
        double frameLift = 0;
        double frameDrag = 0;
        for (auto& particle : particles) {
            handleWingCollision(particle, main_wing, wingThickness, camberLine, frameDrag, frameLift);
            handleParticleBoundaries(particle);
        }

        // Accumulate for averaging
        totalDrag += frameDrag;
        totalLift += frameLift;
        frameCount++;

        const int updateFrames = 1000; // Print every 60 frames

        if (frameCount % updateFrames == 0) {
            std::cout << "Avg Lift: " << totalLift / updateFrames << " | Avg Drag: " << totalDrag / updateFrames
                      << " | L/D: " << (totalDrag != 0 ? totalLift/totalDrag : 0) << std::endl;
            totalLift = 0;
            totalDrag = 0;
        }

        // -------- Update particle positions --------
        updateParticles(particles);

        // -------- Rendering --------
        drawWing(main_wing, wingThickness, camberLine);
        drawParticles(particles, main_wing, wingThickness);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
