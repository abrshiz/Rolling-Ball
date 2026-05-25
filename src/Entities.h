#ifndef ENTITIES_H
#define ENTITIES_H

#include <vector>

class Player {
public:
    float x, y;
    float width, height;
    float speed;
    float velocity;
    float gravity;

    Player();
    void Reset();
    void Update(float dt, bool moveLeft, bool moveRight, float screenWidth);
    void Draw();
};

class Obstacle {
public:
    float x, y;
    float width, height;
    bool isDanger;
    bool passed;

    Obstacle(float startX, float startY, float w, float h, bool danger);
    void Update(float dt, float speed);
    void Draw();
};

struct Particle {
    float x, y;
    float vx, vy;
    float life;
    float maxLife;
    float r, g, b;
};

class ParticleSystem {
    std::vector<Particle> particles;
public:
    void Emit(float x, float y, float r, float g, float b, int count);
    void Update(float dt);
    void Draw();
};

#endif
