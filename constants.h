#ifndef CONSTANTS_H
#define CONSTANTS_H

// -------- Simulation Parameters --------
const float Y_RANGE = 0.3f;
const float PARTICLE_AMOUNT = 1000.0f;

// -------- Box Boundaries --------
const float BOX_X_MIN = -0.5f;
const float BOX_Y_MIN = -Y_RANGE;
const float BOX_Y_MAX = Y_RANGE;

// -------- Window Settings --------
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const char* WINDOW_TITLE = "Wing Simulation";

// -------- Projection Settings --------
const float PROJ_LEFT = -0.6f;
const float PROJ_RIGHT = 1.2f;
const float PROJ_BOTTOM = -0.5f;
const float PROJ_TOP = 0.5f;

// -------- Wing Parameters --------
const float WING_CHORD = 1.0f;
const int WING_POINTS = 2500;
const float WING_THICKNESS_RATIO = 0.12f;
const float WING_SPAN = 5.0f;
const float ANGLE_OF_ATTACK = 0.15f;  // radians (~8.5 degrees nose up)

// -------- Particle Physics --------
const double AIRFLOW_SPEED = 0.005;
const double PARTICLE_MASS = 1.0;
const double INFLUENCE_RADIUS_SQ = 0.002;   // Interaction radius squared
const double REPULSION_STRENGTH = 0.00002;  // How hard particles push apart
const double BOUNDARY_PUSH = 0.005;         // How hard boundaries push back
const double DRAG = 0.0005;                 // Velocity damping (low so particles keep moving)
const float PENETRATION_EPS = 0.002f;

// -------- Particle Generation --------
const double PARTICLE_X_MIN = -0.9;
const double PARTICLE_X_MAX_FACTOR = 0.0;

// -------- Boundary Forces --------
const double BOUNDARY_FORCE = 0.0005;
const double BOUNDARY_FORCE_NORM = 1000.0;

// -------- Particle Reset --------
const float RESET_X_THRESHOLD = 1.2f;
const float RESET_Y_MARGIN = 0.1f;
const float RESET_X_POSITION = -0.5f;

// -------- Rendering --------
const float POINT_SIZE = 2.0f;
const float PARTICLE_COLOR_R = 0.6f;
const float PARTICLE_COLOR_G = 0.6f;
const float PRESSURE_COLOR_DIVISOR = 0.000000000001f;
const float WING_COLOR_R = 1.0f;
const float WING_COLOR_G = 1.0f;
const float WING_COLOR_B = 1.0f;
const float COLLISION_COLOR_R = 1.0f;
const float COLLISION_COLOR_G = 0.0f;
const float COLLISION_COLOR_B = 0.0f;

// -------- Airfoil Thickness Coefficients --------
const float AIRFOIL_COEF_1 = 0.2969f;
const float AIRFOIL_COEF_2 = 0.1260f;
const float AIRFOIL_COEF_3 = 0.3516f;
const float AIRFOIL_COEF_4 = 0.2843f;
const float AIRFOIL_COEF_5 = 0.1015f;
const float AIRFOIL_SCALE = 5.0f;
const float AIRFOIL_BOTTOM_DIVISOR = 2.0f;

#endif // CONSTANTS_H
