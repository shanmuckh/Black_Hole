#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using glm::vec2;
using glm::vec3;
using std::cerr;
using std::cout;
using std::endl;
using std::vector;

static const double C_LIGHT = 299792458.0;
static const double G_NEWTON = 6.67430e-11;

struct Ray;
static void rk4Step(Ray& ray, double dLambda, double rs);

// ---------------- Engine ----------------
struct Engine {
    GLFWwindow* window = nullptr;

    int WIDTH = 800;
    int HEIGHT = 600;

    // Viewport half-size (meters)
    double halfWidth = 1.0e11;
    double halfHeight = 7.5e10;

    // Camera controls (world units)
    double offsetX = 0.0;
    double offsetY = 0.0;
    double zoom = 1.0; // > 0

    Engine() {
        if (!glfwInit()) {
            cerr << "Failed to initialize GLFW\n";
            std::exit(EXIT_FAILURE);
        }

        // Old fixed-pipeline calls (glBegin/glMatrixMode) require a compat profile.
        window = glfwCreateWindow(WIDTH, HEIGHT, "Black Hole Simulation (2D)", nullptr, nullptr);
        if (!window) {
            cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            std::exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(window);

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            cerr << "Failed to initialize GLEW\n";
            glfwDestroyWindow(window);
            glfwTerminate();
            std::exit(EXIT_FAILURE);
        }

        glViewport(0, 0, WIDTH, HEIGHT);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.f, 0.f, 1.f);
    }

    void updateControls(double dt) {
        // Pan speed scales with visible size
        double pan = (halfWidth / zoom) * dt * 0.8;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) offsetY += pan;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) offsetY -= pan;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) offsetX -= pan;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) offsetX += pan;

        // Zoom
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) zoom *= (1.0 + dt);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) zoom *= (1.0 - dt);
        zoom = std::clamp(zoom, 0.05, 50.0);
    }

    void beginFrame() {
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        const double hw = halfWidth / zoom;
        const double hh = halfHeight / zoom;

        const double left = -hw + offsetX;
        const double right = hw + offsetX;
        const double bottom = -hh + offsetY;
        const double top = hh + offsetY;

        glOrtho(left, right, bottom, top, -1.0, 1.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
};

static Engine engine;

// ---------------- Black Hole ----------------
struct BlackHole {
    vec3 position;
    double mass;
    double r_s; // Schwarzschild radius

    BlackHole(const vec3& pos, double m) : position(pos), mass(m) {
        r_s = 2.0 * G_NEWTON * mass / (C_LIGHT * C_LIGHT);
    }

    void draw() const {
        glPushMatrix();
        glTranslated(position.x, position.y, 0.0);

        glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(0.0f, 0.0f);

        const int segments = 120;
        for (int i = 0; i <= segments; i++) {
            double angle = 2.0 * M_PI * double(i) / double(segments);
            double x = r_s * std::cos(angle);
            double y = r_s * std::sin(angle);
            glVertex2f((float)x, (float)y);
        }
        glEnd();

        glPopMatrix();
    }
};

// Sagittarius A* mass ~ 8.54e36 kg (as in your file)
static BlackHole SagA(vec3(0.0f, 0.0f, 0.0f), 8.54e36);

// ---------------- Rays / Geodesics ----------------
struct Ray {
    // Cartesian
    double x = 0.0;
    double y = 0.0;

    // Polar (centered at BH origin)
    double r = 0.0;
    double phi = 0.0;

    // Derivatives wrt affine parameter lambda
    double dr = 0.0;
    double dphi = 0.0;

    // Conserved quantities (simple setup)
    double E = 0.0;
    double L = 0.0;

    vector<vec2> trail;

    Ray(const vec2& pos, const vec2& dir) {
        x = pos.x;
        y = pos.y;

        r = std::sqrt(x * x + y * y);
        if (r < 1e-9) r = 1e-9;
        phi = std::atan2(y, x);

        // Convert initial Cartesian direction to polar rates
        // (Treat dir as velocity-like; only direction matters for the curve)
        const double vx = dir.x;
        const double vy = dir.y;

        dr = vx * std::cos(phi) + vy * std::sin(phi);
        dphi = (-vx * std::sin(phi) + vy * std::cos(phi)) / r;

        // Store conserved quantities (approx)
        L = r * r * dphi;

        const double f = 1.0 - SagA.r_s / r;
        const double dt_dLambda = std::sqrt((dr * dr) / (f * f) + (r * r * dphi * dphi) / f);
        E = f * dt_dLambda;

        trail.reserve(2000);
        trail.push_back(vec2((float)x, (float)y));
    }

    void step(double dLambda, double rs) {
        if (r <= rs) return; // inside horizon => stop
        rk4Step(*this, dLambda, rs);

        // Back to Cartesian
        x = r * std::cos(phi);
        y = r * std::sin(phi);

        trail.push_back(vec2((float)x, (float)y));
        if (trail.size() > 4000) {
            trail.erase(trail.begin(), trail.begin() + 2000);
        }
    }
};

// also this is an improved work of the original work done by the https://github.com/kavan010
// i learned a lot from the work of the kavan010 and improved upon it for my r&d project.
static void geodesicRHS(const Ray& ray, double rhs[4], double rs) {
    const double r = ray.r;
    const double dr = ray.dr;
    const double dphi = ray.dphi;
    const double E = ray.E;

    const double f = 1.0 - rs / r;

    // y = [r, phi, dr, dphi]
    rhs[0] = dr;    // dr/dLambda
    rhs[1] = dphi;  // dphi/dLambda

    // d2r/dLambda^2 (from your form)
    const double dt_dLambda = E / f;

    rhs[2] =
        -(rs / (2.0 * r * r)) * f * (dt_dLambda * dt_dLambda) +
        (rs / (2.0 * r * r * f)) * (dr * dr) +
        (r - rs) * (dphi * dphi);

    // d2phi/dLambda^2
    rhs[3] = -2.0 * dr * dphi / r;
}

static void addState(const double a[4], const double b[4], double factor, double out[4]) {
    for (int i = 0; i < 4; i++) out[i] = a[i] + b[i] * factor;
}

static void rk4Step(Ray& ray, double dLambda, double rs) {
    const double y0[4] = { ray.r, ray.phi, ray.dr, ray.dphi };
    double k1[4], k2[4], k3[4], k4[4], temp[4];

    geodesicRHS(ray, k1, rs);

    addState(y0, k1, dLambda / 2.0, temp);
    Ray r2 = ray; r2.r = temp[0]; r2.phi = temp[1]; r2.dr = temp[2]; r2.dphi = temp[3];
    geodesicRHS(r2, k2, rs);

    addState(y0, k2, dLambda / 2.0, temp);
    Ray r3 = ray; r3.r = temp[0]; r3.phi = temp[1]; r3.dr = temp[2]; r3.dphi = temp[3];
    geodesicRHS(r3, k3, rs);

    addState(y0, k3, dLambda, temp);
    Ray r4 = ray; r4.r = temp[0]; r4.phi = temp[1]; r4.dr = temp[2]; r4.dphi = temp[3];
    geodesicRHS(r4, k4, rs);

    ray.r    += (dLambda / 6.0) * (k1[0] + 2.0 * k2[0] + 2.0 * k3[0] + k4[0]);
    ray.phi  += (dLambda / 6.0) * (k1[1] + 2.0 * k2[1] + 2.0 * k3[1] + k4[1]);
    ray.dr   += (dLambda / 6.0) * (k1[2] + 2.0 * k2[2] + 2.0 * k3[2] + k4[2]);
    ray.dphi += (dLambda / 6.0) * (k1[3] + 2.0 * k2[3] + 2.0 * k3[3] + k4[3]);
}

// Draw all rays (points + fading trails)
static void drawRays(const vector<Ray>& rays) {
    // Current ray points
    glPointSize(2.0f);
    glColor3f(1.0f, 0.8f, 0.2f);
    glBegin(GL_POINTS);
    for (const auto& ray : rays) {
        glVertex2f((float)ray.x, (float)ray.y);
    }
    glEnd();

    // Trails
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0f);

    for (const auto& ray : rays) {
        const size_t N = ray.trail.size();
        if (N < 2) continue;

        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < N; ++i) {
            float alpha = (N <= 1) ? 1.0f : float(i) / float(N - 1);
            alpha = std::max(alpha, 0.05f);
            glColor4f(1.0f, 1.0f, 1.0f, alpha);
            glVertex2f(ray.trail[i].x, ray.trail[i].y);
        }
        glEnd();
    }

    glDisable(GL_BLEND);
}

// Initialize a "beam" of rays
static vector<Ray> makeRaysBeam(int count) {
    vector<Ray> out;
    out.reserve((size_t)count);

    const double startX = -0.9 * engine.halfWidth;
    const double yMin = -engine.halfHeight * 0.9;
    const double yMax = engine.halfHeight * 0.9;

    for (int i = 0; i < count; i++) {
        double t = (count <= 1) ? 0.0 : double(i) / double(count - 1);
        double y = yMin + (yMax - yMin) * t;

        vec2 pos((float)startX, (float)y);
        vec2 dir((float)C_LIGHT, 0.0f); // to the +x direction

        out.emplace_back(pos, dir);
    }

    return out;
}

int main() {
    vector<Ray> rays = makeRaysBeam(250);

    double last = glfwGetTime();

    while (!glfwWindowShouldClose(engine.window)) {
        double now = glfwGetTime();
        double dt = now - last;
        last = now;

        engine.updateControls(dt);
        engine.beginFrame();

        SagA.draw();

        // Smaller step => smoother curves
        const double dLambda = 0.02;

        for (auto& ray : rays) {
            ray.step(dLambda, SagA.r_s);
        }

        drawRays(rays);

        glfwSwapBuffers(engine.window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
