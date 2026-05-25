#include "Entities.h"
#include "RenderUtils.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Player stuff
Player::Player() {
    width = 30;
    height = 30;
    Reset();
}

void Player::Reset() {
    speed = 450.0f;
    velocity = 0.0f;
    gravity = 1200.0f; // falling fast
}

void Player::Update(float dt, bool moveLeft, bool moveRight, float screenWidth) {
    if (moveLeft) x -= speed * dt;
    if (moveRight) x += speed * dt;

    // keep player inside the screen bounds
    if (x - width/2 < 0) x = width/2;
    if (x + width/2 > screenWidth) x = screenWidth - width/2;
}

void Player::Draw() {
    // draw main ball body
    RenderUtils::DrawCircle(x, y, width/2, 0.9f, 0.2f, 0.2f); // red color
    // draw a tiny highlight to make it look 3D-ish
    RenderUtils::DrawCircle(x - width/6, y - height/6, width/8, 1.0f, 0.6f, 0.6f);
}

// Obstacle (Platforms) stuff
Obstacle::Obstacle(float startX, float startY, float w, float h, bool danger) {
    x = startX;
    y = startY;
    width = w;
    height = h;
    isDanger = danger;
    passed = false;
}

void Obstacle::Update(float dt, float speed) {
    y -= speed * dt; // Platforms always move UP
}

void Obstacle::Draw() {
    if (isDanger) {
        // Danger platform (red)
        RenderUtils::DrawRect(x, y, width, height, 0.8f, 0.1f, 0.1f);
        
        // draw some triangle spikes on top
        glBegin(GL_TRIANGLES);
        glColor3f(0.9f, 0.2f, 0.2f);
        for(int i=0; i < width/10; ++i) {
            glVertex2f(x + i*10, y);
            glVertex2f(x + i*10 + 10, y);
            glVertex2f(x + i*10 + 5, y - 10);
        }
        glEnd();
    } else {
        // Safe platform (green)
        RenderUtils::DrawRect(x, y, width, height, 0.2f, 0.8f, 0.2f);
        // inner lighter green border
        RenderUtils::DrawRect(x+5, y+5, width-10 > 0 ? width-10 : 0, height-10, 0.1f, 0.6f, 0.1f);
    }
}

// Particle System
void ParticleSystem::Emit(float x, float y, float r, float g, float b, int count) {
    for(int i=0; i<count; ++i) {
        Particle p;
        p.x = x;
        p.y = y;
        p.vx = (rand()%100 - 50) * 0.5f;
        p.vy = (rand()%100 - 50) * 1.0f; 
        p.maxLife = p.life = (rand()%100) / 100.0f * 0.5f + 0.2f;
        p.r = r; p.g = g; p.b = b;
        particles.push_back(p);
    }
}

void ParticleSystem::Update(float dt) {
    for(auto& p : particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.life -= dt;
    }
    // delete dead particles
    particles.erase(std::remove_if(particles.begin(), particles.end(), 
        [](const Particle& p) { return p.life <= 0; }), particles.end());
}

void ParticleSystem::Draw() {
    glBegin(GL_QUADS);
    for(auto& p : particles) {
        // fade out based on life
        float alpha = p.life / p.maxLife;
        glColor4f(p.r, p.g, p.b, alpha);
        float s = 3.0f; // size
        glVertex2f(p.x - s, p.y - s);
        glVertex2f(p.x + s, p.y - s);
        glVertex2f(p.x + s, p.y + s);
        glVertex2f(p.x - s, p.y + s);
    }
    glEnd();
}
