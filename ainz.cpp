#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <algorithm>

const int WIDTH = 800;
const int HEIGHT = 600;
const int SEGMENTS = 50;
const float SEGMENT_LENGTH = 0.05f;
bool heatLampOn = true;

struct Segment {
    float x, y;
};

struct Color {
    float r, g, b;
};

std::vector<Segment> snake;
std::vector<Color> segmentColors;
float timeElapsed = 0.0f;
double mouseX = 0.0, mouseY = 0.0;
float modeLerp = 1.0f;        // 0 = slither, 1 = ball
float targetLerp = 1.0f;

float clamp(float value, float minVal, float maxVal) {
    return std::max(minVal, std::min(maxVal, value));
}

double clampd(double value, double minVal, double maxVal) {
    return std::max(minVal, std::min(maxVal, value));
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    mouseX = clampd((xpos / WIDTH) * 4.0 - 2.0, -1.45, 1.45);
    mouseY = clampd(-((ypos / HEIGHT) * 3.0 - 1.5), -1.35, 1.35);    
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_S) {
            targetLerp = 0.0f; // transition to slither
        } else if (key == GLFW_KEY_B) {
            targetLerp = 1.0f; // transition to ball
        } else if (key == GLFW_KEY_H) {
            heatLampOn = !heatLampOn; // toggle heat lamp
        }
    }
}

void drawGlassEnclosure() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Full interior of tank (terracotta sand tone)
    glColor4f(0.58f, 0.45f, 0.33f, 1.0f); // deeper earthy tone
    glBegin(GL_QUADS);
    glVertex2f(-1.6f, -1.4f);
    glVertex2f(1.6f, -1.4f);
    glVertex2f(1.6f, 1.4f);
    glVertex2f(-1.6f, 1.4f);
    glEnd();

    // Enhanced dirt texture with more grains
    srand(42); // fixed seed for consistency
    float lampX = 1.5f;
    float lampY = 1.3f;

    for (int i = 0; i < 600; ++i) {
        float x = -1.6f + static_cast<float>(rand()) / RAND_MAX * 3.2f;
        float y = -1.4f + static_cast<float>(rand()) / RAND_MAX * 2.8f;
        float r = 0.0015f + static_cast<float>(rand()) / RAND_MAX * 0.0045f;

        float brownTint = 0.35f + static_cast<float>(rand()) / RAND_MAX * 0.25f;
        float rVal = brownTint, gVal = brownTint * 0.85f, bVal = brownTint * 0.6f;
        float alpha = 0.22f + static_cast<float>(rand()) / RAND_MAX * 0.1f;

        if (heatLampOn) {
            float dx = x - lampX;
            float dy = y - lampY;
            float dist = sqrt(dx * dx + dy * dy);
            float intensity = clamp(1.0f - dist / 1.5f, 0.0f, 1.0f);
            rVal += 1.5f * intensity;
            gVal += 1.2f * intensity;
            bVal += 0.05f * intensity;
        }

        glColor4f(rVal, gVal, bVal, alpha);



        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for (int j = 0; j <= 10; ++j) {
            float theta = 2.0f * M_PI * j / 10;
            glVertex2f(x + cos(theta) * r, y + sin(theta) * r);
        }
        glEnd();
    }

    // Draw water bowl like a Venn diagram shape
    float bowlX1 = 0.85f;
    float bowlX2 = 1.15f;
    float bowlY = -1.0f;
    float outerR = 0.26f;
    float innerR = 0.22f;

    // Bowl base (dark rim - left lobe)
    glColor4f(0.4f, 0.3f, 0.2f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(bowlX1, bowlY);
    for (int i = 0; i <= 30; ++i) {
        float theta = 2.0f * M_PI * i / 30;
        glVertex2f(bowlX1 + cos(theta) * outerR, bowlY + sin(theta) * outerR);
    }
    glEnd();

    // Bowl base (dark rim - right lobe)
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(bowlX2, bowlY);
    for (int i = 0; i <= 30; ++i) {
        float theta = 2.0f * M_PI * i / 30;
        glVertex2f(bowlX2 + cos(theta) * outerR, bowlY + sin(theta) * outerR);
    }
    glEnd();

    // Water surface (two lobes with directional light)
    float baseR = 0.3f, baseG = 0.5f, baseB = 0.6f;
    float waterAlpha = 0.85f;

    for (int side = 0; side < 2; ++side) {
        float bx = (side == 0) ? bowlX1 : bowlX2;

        float dx = bx - lampX;
        float dy = bowlY - lampY;
        float dist = sqrt(dx * dx + dy * dy);
        float intensity = heatLampOn ? clamp(1.0f - dist / 1.5f, 0.0f, 1.0f) : 1.0f;

        float r = baseR, g = baseG, b = baseB;
        if (heatLampOn) {
            r += 0.3f * intensity;
            g += 0.2f * intensity;
            b += 0.1f * intensity;
        } else {
            r *= 0.85f;
            g *= 0.85f;
            b *= 1.1f;
        }

        glColor4f(r, g, b, waterAlpha);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(bx, bowlY);
        for (int i = 0; i <= 30; ++i) {
            float theta = 2.0f * M_PI * i / 30;
            glVertex2f(bx + cos(theta) * innerR, bowlY + sin(theta) * innerR);
        }
        glEnd();
    }


    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(bowlX1, bowlY);
    for (int i = 0; i <= 30; ++i) {
        float theta = 2.0f * M_PI * i / 30;
        glVertex2f(bowlX1 + cos(theta) * innerR, bowlY + sin(theta) * innerR);
    }
    glEnd();

    // Glass walls (border only, now thicker)
    glColor4f(0.75f, 0.85f, 0.95f, 0.14f);
    float thickness = 0.12f;

    // Left wall
    glBegin(GL_QUADS);
    glVertex2f(-1.6f - thickness, -1.4f);
    glVertex2f(-1.6f, -1.4f);
    glVertex2f(-1.6f, 1.4f);
    glVertex2f(-1.6f - thickness, 1.4f);
    glEnd();

    // Right wall
    glBegin(GL_QUADS);
    glVertex2f(1.6f, -1.4f);
    glVertex2f(1.6f + thickness, -1.4f);
    glVertex2f(1.6f + thickness, 1.4f);
    glVertex2f(1.6f, 1.4f);
    glEnd();

    // Top wall
    glBegin(GL_QUADS);
    glVertex2f(-1.6f, 1.4f);
    glVertex2f(1.6f, 1.4f);
    glVertex2f(1.6f, 1.4f + thickness);
    glVertex2f(-1.6f, 1.4f + thickness);
    glEnd();

    // Bottom wall
    glBegin(GL_QUADS);
    glVertex2f(-1.6f, -1.4f - thickness);
    glVertex2f(1.6f, -1.4f - thickness);
    glVertex2f(1.6f, -1.4f);
    glVertex2f(-1.6f, -1.4f);
    glEnd();

    // Simulated vertical reflection streaks on glass
    for (int i = 0; i < 3; ++i) {
        float x = -1.6f + 0.2f * i;
        float alpha = 0.04f + 0.02f * (i % 2);

        glColor4f(0.9f, 0.9f, 1.0f, alpha);
        glBegin(GL_QUAD_STRIP);
        for (float y = -1.4f; y <= 1.4f; y += 0.05f) {
            float offset = sin(y * 10.0f + i) * 0.005f;
            glVertex2f(x + offset, y);
            glVertex2f(x + offset + 0.01f, y);
        }
        glEnd();
    }

    // Mirror on right side (flipped horizontally)
    for (int i = 0; i < 3; ++i) {
        float x = 1.6f - 0.2f * i;
        float alpha = 0.04f + 0.02f * (i % 2);

        glColor4f(0.9f, 0.9f, 1.0f, alpha);
        glBegin(GL_QUAD_STRIP);
        for (float y = -1.4f; y <= 1.4f; y += 0.05f) {
            float offset = sin(y * 10.0f + i) * 0.005f;
            glVertex2f(x - offset, y);
            glVertex2f(x - offset - 0.01f, y);
        }
        glEnd();
    }


    // Optional outline
    glColor4f(0.6f, 0.7f, 0.9f, 0.2f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-1.6f, -1.4f);
    glVertex2f(1.6f, -1.4f);
    glVertex2f(1.6f, 1.4f);
    glVertex2f(-1.6f, 1.4f);
    glEnd();

    glDisable(GL_BLEND);
}

void initSnake() {
    Color base = {30 / 255.0f, 20 / 255.0f, 15 / 255.0f};
    Color blotch = {140 / 255.0f, 90 / 255.0f, 40 / 255.0f};

    snake.clear();
    segmentColors.clear();

    for (int i = 0; i < SEGMENTS; ++i) {
        float x = -1.5f + i * SEGMENT_LENGTH;
        snake.push_back({x, 0.0f});
        segmentColors.push_back((i / 4) % 2 == 0 ? base : blotch);
    }
}

void updateSnake(float t) {
    modeLerp += (targetLerp - modeLerp) * 0.05f;

    float angleStep = 5.5f * M_PI / SEGMENTS;
    float baseRadius = 0.4f + 0.01f * sinf(t * 2.0f); // breathing effect

    for (int i = 0; i < SEGMENTS; ++i) {
        // Slither position
        float x1 = -1.5f + i * SEGMENT_LENGTH;
        float offset = 2 * x1 * M_PI + t * 2.0f;
        float y1 = 0.2f * sinf(offset) + static_cast<float>(mouseY);

        // Ball position
        float angle = i * angleStep;
        float radius = baseRadius * (1.0f - static_cast<float>(i) / SEGMENTS);
        float x2 = radius * cos(angle);
        float y2 = radius * sin(angle);

        if (i == SEGMENTS - 1) {
            x2 += 0.05f * cos(angle);
            y2 += 0.05f * sin(angle);
        }

        // Interpolate
        float x = (1 - modeLerp) * x1 + modeLerp * x2;
        float y = (1 - modeLerp) * y1 + modeLerp * y2;

        // Clamp final position to remain inside the enclosure
        snake[i].x = clamp(x, -1.45f, 1.45f);
        snake[i].y = clamp(y, -1.35f, 1.35f);
    }
}

void drawHeatLamp() {
    float lampX = 1.5f;
    float lampY = 1.3f;

    // Bulb is always visible
    if (heatLampOn) {
        glColor3f(1.0f, 0.7f, 0.4f); // warm whitish-orange (ON)
    } else {
        glColor3f(1.0f, 0.3f, 0.1f); // dim reddish-orange (OFF)
    }

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(lampX, lampY);
    for (int i = 0; i <= 20; ++i) {
        float theta = 2.0f * M_PI * i / 20;
        glVertex2f(lampX + cos(theta) * 0.04f, lampY + sin(theta) * 0.04f);
    }
    glEnd();
} 

void drawSnake() {
    const float maxRadius = 0.045f;
    glBegin(GL_TRIANGLE_STRIP);
    for (size_t i = 0; i < snake.size(); ++i) {
        Segment s = snake[i];
        Color c = segmentColors[i];

        if (heatLampOn) {
            // Fake light source from top-left corner
            float dx = s.x - 1.5f;
            float dy = s.y - 1.3f;
            
            float dist = sqrt(dx * dx + dy * dy);
            float lightIntensity = clamp(1.0f - dist / 2.5f, 0.2f, 1.0f);
            glColor3f(c.r * lightIntensity, c.g * lightIntensity, c.b * lightIntensity);
        } else {
            glColor3f(c.r, c.g, c.b);
        }

        float nx = 0.0f, ny = 0.0f;
        if (i < snake.size() - 1) {
            float dx = snake[i + 1].x - s.x;
            float dy = snake[i + 1].y - s.y;
            float len = sqrt(dx * dx + dy * dy);
            nx = -dy / len;
            ny = dx / len;
        } else if (i > 0) {
            float dx = s.x - snake[i - 1].x;
            float dy = s.y - snake[i - 1].y;
            float len = sqrt(dx * dx + dy * dy);
            nx = -dy / len;
            ny = dx / len;
        }

        // Taper tail and neck
        float taper = 1.0f;

        // Tail taper (left side, first 25%)
        int tailEnd = static_cast<int>(snake.size() * 0.25f);
        if (i < tailEnd) {
            float localTaper = static_cast<float>(i) / static_cast<float>(tailEnd - 1);
            taper = 0.2f + 0.8f * localTaper;
        }

        // Neck taper (right side, last 10%)
        int neckStart = static_cast<int>(snake.size() * 0.90f);
        if (i >= neckStart) {
            float localNeckTaper = static_cast<float>(snake.size() - 1 - i) / static_cast<float>(snake.size() - 1 - neckStart);
            taper = 0.7f + 0.3f * localNeckTaper;  // taper from 0.7 → 1.0
        }

        float radius = maxRadius * taper;

        glVertex2f(s.x + nx * radius, s.y + ny * radius);
        glVertex2f(s.x - nx * radius, s.y - ny * radius);
    }
    glEnd();

    // Draw head lobes
    Segment back = snake[snake.size() - 2];
    Segment front = snake[snake.size() - 1];
    Color headColor = segmentColors.back();

    float backRadius = maxRadius * 1.4f;  // slightly bigger than body
    float frontRadius = maxRadius * 1.0f; // slightly smaller

    if (heatLampOn) {
        float dx = back.x - 1.5f;
        float dy = back.y - 1.3f;

        float dist = sqrt(dx * dx + dy * dy);
        float lightIntensity = clamp(1.0f - dist / 2.5f, 0.2f, 1.0f);
        glColor3f(headColor.r * lightIntensity, headColor.g * lightIntensity, headColor.b * lightIntensity);
    } else {
        glColor3f(headColor.r, headColor.g, headColor.b);
    }

    // Back of head
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(back.x, back.y);
    for (int j = 0; j <= 20; ++j) {
        float theta = 2.0f * M_PI * j / 20;
        glVertex2f(back.x + cos(theta) * backRadius, back.y + sin(theta) * backRadius);
    }
    glEnd();

    // Front of head (snout)
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(front.x, front.y);
    for (int j = 0; j <= 20; ++j) {
        float theta = 2.0f * M_PI * j / 20;
        glVertex2f(front.x + cos(theta) * frontRadius, front.y + sin(theta) * frontRadius);
    }
    glEnd();
}

void drawScales() {
    for (size_t i = 2; i < snake.size() - 2; ++i) {
        Segment s = snake[i];
        Segment next = snake[i + 1];
        Segment prev = snake[i - 1];

        float dx = next.x - prev.x;
        float dy = next.y - prev.y;
        float len = sqrt(dx * dx + dy * dy);
        float nx = -dy / len;
        float ny = dx / len;

        float spacing = 0.004f;
        float scaleLength = 0.006f;
        float scaleWidth = 0.003f;

        Color base = segmentColors[i];
        float brightness = (base.r + base.g + base.b) / 3.0f;
        float offset = (brightness > 0.4f) ? -0.2f : 0.3f;

        Color contrast = {
            clamp(base.r + offset, 0.0f, 1.0f),
            clamp(base.g + offset, 0.0f, 1.0f),
            clamp(base.b + offset, 0.0f, 1.0f)
        };

        glColor3f(contrast.r, contrast.g, contrast.b);

        // Stagger pattern: even = center + sides, odd = just sides
        bool even = (i % 2 == 0);
        int pattern[3] = {-1, 0, 1};

        for (int j = 0; j < 3; ++j) {
            int shiftIndex = pattern[j];
        
            if (even && shiftIndex != 0) continue;     // even segments → only center
            if (!even && shiftIndex == 0) continue;    // odd segments → only sides
        
            float baseOffset = (even ? 0.004f : 0.0065f); // more spacing for odd (side) scales
            float shift = shiftIndex * baseOffset;

            float cx = s.x + nx * shift;
            float cy = s.y + ny * shift;
        
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cx, cy);
            glVertex2f(cx + nx * scaleWidth, cy + ny * scaleWidth);
            glVertex2f(cx + dx / len * scaleLength, cy + dy / len * scaleLength);
            glVertex2f(cx - nx * scaleWidth, cy - ny * scaleWidth);
            glVertex2f(cx - dx / len * scaleLength, cy - dy / len * scaleLength);
            glEnd();
        }
        
    }
}

void drawShadowSnake() {
    float baseAlpha = heatLampOn ? 0.12f : 0.25f;
    float baseScale = heatLampOn ? 1.2f : 1.0f;
    glColor4f(0.1f, 0.1f, 0.1f, baseAlpha);


    glBegin(GL_TRIANGLE_STRIP);
    for (size_t i = 0; i < snake.size(); ++i) {
        Segment s = snake[i];

        float nx = 0.0f, ny = 0.0f;
        if (i < snake.size() - 1) {
            float dx = snake[i + 1].x - s.x;
            float dy = snake[i + 1].y - s.y;
            float len = sqrt(dx * dx + dy * dy);
            nx = -dy / len;
            ny = dx / len;
        } else if (i > 0) {
            float dx = s.x - snake[i - 1].x;
            float dy = s.y - snake[i - 1].y;
            float len = sqrt(dx * dx + dy * dy);
            nx = -dy / len;
            ny = dx / len;
        }

        // Shadow should follow radius, like body
        const float maxRadius = 0.045f * baseScale;
        float taper = 1.0f;
        int taperEnd = static_cast<int>(snake.size() * 0.25f);
        if (i < taperEnd) {
            float localTaper = static_cast<float>(i) / static_cast<float>(taperEnd - 1);
            taper = 0.2f + 0.8f * localTaper;
        }
        float radius = maxRadius * taper;

        // Drop shadow more downward
        float shadowOffset = -0.10f;

        glVertex2f(s.x + nx * radius, s.y + ny * radius + shadowOffset);
        glVertex2f(s.x - nx * radius, s.y - ny * radius + shadowOffset);
    }
    glEnd();

    // Add shadow for the head (back and front circles)
    Segment back = snake[snake.size() - 2];
    Segment front = snake[snake.size() - 1];

    float backRadius = 0.045f * 1.4f * baseScale;
    float frontRadius = 0.045f * 1.0f * baseScale;
    float shadowOffset = -0.08f;

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(back.x, back.y + shadowOffset);
    for (int j = 0; j <= 20; ++j) {
        float theta = 2.0f * M_PI * j / 20;
        glVertex2f(back.x + cos(theta) * backRadius, back.y + sin(theta) * backRadius + shadowOffset);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(front.x, front.y + shadowOffset);
    for (int j = 0; j <= 20; ++j) {
        float theta = 2.0f * M_PI * j / 20;
        glVertex2f(front.x + cos(theta) * frontRadius, front.y + sin(theta) * frontRadius + shadowOffset);
    }
    glEnd();
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ainz in OpenGl", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback); // <-- Added key callback
    glOrtho(-2, 2, -1.5, 1.5, -1, 1); // 2D view

    initSnake();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // near-black background


    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        timeElapsed += 0.016f; // approx 60fps
        updateSnake(timeElapsed);
        drawGlassEnclosure();
        drawHeatLamp();
        drawShadowSnake();
        drawSnake();
        drawScales();


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
