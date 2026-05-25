#ifndef GAME_H
#define GAME_H

#include "Entities.h"
#include "MenuSystem.h"

class Game {
public:
    GameState state;
    Player player;
    std::vector<Obstacle> obstacles;
    ParticleSystem particles;
    MenuSystem menu;
    
    int windowWidth, windowHeight;
    bool keys[256];
    bool specialKeys[256];
    bool moveLeft;
    bool moveRight;

    int currentScore;
    int highScore;
    float difficulty;
    float obstacleSpawnTimer;

    float splashTimer;
    float fadeAlpha;
    GameState nextState;
    bool isFading;

    Game();
    void Init();
    void ChangeState(GameState newState);
    
    void Update(float dt);
    void Draw();

    void SpawnObstacle();
    void ResetGame();
    
    void HandleKeyDown(unsigned char key);
    void HandleKeyUp(unsigned char key);
    void HandleSpecialKeyDown(int key);
    void HandleSpecialKeyUp(int key);
    void HandleMouseClick(int button, int state, int x, int y);
    void HandleMouseMove(int x, int y);
    void HandleResize(int w, int h);
    
    void Save();
    void Load();
};

extern Game g_Game;

#endif
