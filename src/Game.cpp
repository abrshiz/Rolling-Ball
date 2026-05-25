#include "Game.h"
#include "RenderUtils.h"
#include "SaveData.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cstdlib>
#include <iostream>
#include <algorithm>

Game g_Game; // global game object

Game::Game() {
    state = GameState::SPLASH;
    windowWidth = 800;
    windowHeight = 600;
    currentScore = 0;
    highScore = 0;
    difficulty = 1.0f;
    obstacleSpawnTimer = 0.0f;
    splashTimer = 3.0f; // show logo for 3 secs
    fadeAlpha = 1.0f;
    isFading = true;
    nextState = GameState::MAIN_MENU;
    
    for(int i=0; i<256; ++i) { 
        keys[i] = false; 
        specialKeys[i] = false; 
    }
    moveLeft = false;
    moveRight = false;
}

void Game::Init() {
    // setup button callbacks for the menu
    menu.onNewGame = [this]() { this->ChangeState(GameState::PLAYING); };
    menu.onContinue = [this]() {
        this->ChangeState(GameState::PLAYING);
        this->Load(); // load from binary file
    };
    menu.onResume = [this]() { this->ChangeState(GameState::PLAYING); };
    menu.onPause = [this]() { this->ChangeState(GameState::PAUSED); };
    menu.onExit = [this]() { this->ChangeState(GameState::EXIT_CONFIRM); };
    menu.onConfirmExit = [this]() { exit(0); }; // just quit
    menu.onCancelExit = [this]() { this->ChangeState(GameState::MAIN_MENU); };
    menu.onAbout = [this]() { this->ChangeState(GameState::ABOUT); };
    menu.onRules = [this]() { this->ChangeState(GameState::RULES); };
    menu.onHighScore = [this]() { this->ChangeState(GameState::HIGH_SCORE); };
    menu.onBackToMenu = [this]() { this->ChangeState(GameState::MAIN_MENU); };
    menu.onGameOverToMenu = [this]() { this->ChangeState(GameState::MAIN_MENU); };

    menu.Init(state, windowWidth, windowHeight);
    
    int hs, cs;
    bool hasSaved;
    float diff, px, py, pvel;
    std::vector<ObstacleData> obs;
    
    // load highscore on startup
    if(SaveSystem::Load(hs, hasSaved, cs, diff, px, py, pvel, obs)) {
        highScore = hs;
        menu.hasSavedGame = hasSaved;
    }
}

void Game::ResetGame() {
    player.Reset();
    player.x = windowWidth * 0.5f; // middle
    player.y = windowHeight * 0.2f; // near the top so they drop in
    obstacles.clear();
    particles = ParticleSystem();
    currentScore = 0;
    difficulty = 1.0f;
    obstacleSpawnTimer = 0.0f;

    // give the player a safe platform right under them to start
    obstacles.push_back(Obstacle(windowWidth * 0.5f - 100, windowHeight * 0.5f, 200, 20, false));
}

void Game::Save() {
    // Teacher wanted binary save system, so I made this struct copy
    std::vector<ObstacleData> obsData;
    for(auto& o : obstacles) {
        obsData.push_back({o.x, o.y, o.width, o.height, o.isDanger});
    }
    SaveSystem::Save(highScore, currentScore, difficulty, player.x, player.y, player.velocity, obsData);
    menu.hasSavedGame = true;
}

void Game::Load() {
    int hs, cs;
    bool hasSaved;
    float diff, px, py, pvel;
    std::vector<ObstacleData> obsData;
    
    if(SaveSystem::Load(hs, hasSaved, cs, diff, px, py, pvel, obsData)) {
        highScore = hs;
        // only load game state if they clicked continue
        if(hasSaved && state == GameState::PLAYING) {
            currentScore = cs;
            difficulty = diff;
            player.x = px;
            player.y = py;
            player.velocity = pvel;
            obstacles.clear();
            for(auto& o : obsData) {
                obstacles.push_back(Obstacle(o.x, o.y, o.width, o.height, o.isDanger));
            }
        }
    }
}

void Game::ChangeState(GameState newState) {
    // restart if coming from menu or game over
    if (newState == GameState::PLAYING && (state == GameState::MAIN_MENU || state == GameState::GAME_OVER)) {
        ResetGame();
    }
    // autosave when pause
    if (state == GameState::PLAYING && newState == GameState::PAUSED) {
        Save();
    }
    state = newState;
    menu.Init(state, windowWidth, windowHeight);
    
    // cool fade effect I added
    isFading = true;
    fadeAlpha = 1.0f;
}

void Game::SpawnObstacle() {
    float h = 20.0f;
    float startY = windowHeight + h; // Spawns from the bottom
    
    // platforms get smaller as game gets harder
    float w = 200.0f - (difficulty * 10.0f);
    if (w < 80.0f) w = 80.0f; // don't make it impossible

    float startX = rand() % (int)(windowWidth - w);
    
    // 25% chance it's a spiky red danger platform
    bool danger = (rand() % 100) < 25;

    obstacles.push_back(Obstacle(startX, startY, w, h, danger));
}

void Game::Update(float dt) {
    // update fade
    if (isFading) {
        fadeAlpha -= dt * 2.0f;
        if (fadeAlpha <= 0.0f) {
            fadeAlpha = 0.0f;
            isFading = false;
        }
    }

    if (state == GameState::SPLASH) {
        splashTimer -= dt;
        if (splashTimer <= 0.0f) {
            ChangeState(GameState::MAIN_MENU);
        }
        return; // don't update rest of game
    }

    menu.Update(dt);

    if (state == GameState::PLAYING) {
        // inputs
        moveLeft = keys['a'] || keys['A'] || specialKeys[GLUT_KEY_LEFT];
        moveRight = keys['d'] || keys['D'] || specialKeys[GLUT_KEY_RIGHT];
        player.Update(dt, moveLeft, moveRight, windowWidth);

        // particles emit when falling fast, looks nice
        if (player.velocity > 100.0f) {
            particles.Emit(player.x, player.y - player.height/2, 1.0f, 0.8f, 0.8f, 1);
        }
        particles.Update(dt);

        // spawn timer logic
        obstacleSpawnTimer -= dt * difficulty;
        if (obstacleSpawnTimer <= 0.0f) {
            SpawnObstacle();
            obstacleSpawnTimer = 0.8f;
            difficulty += 0.02f; // slowly get harder
        }

        bool onPlatform = false;
        float platformSpeed = 200.0f * difficulty;

        for (auto& obs : obstacles) {
            obs.Update(dt, platformSpeed);
            
            // Score points when a safe platform passes the top of the screen
            if (!obs.passed && obs.y < 0) {
                obs.passed = true;
                if (!obs.isDanger) {
                    currentScore += 10;
                    if (currentScore > highScore) highScore = currentScore; // new high score!
                }
            }

            float playerBottom = player.y + player.height/2;
            
            // collision code 
            // only check if player is falling down
            if (player.velocity >= 0.0f) {
                // check vertical overlap roughly
                if (playerBottom >= obs.y && playerBottom <= obs.y + obs.height + 15.0f) {
                    // check horizontal overlap
                    if (player.x + player.width/2 > obs.x && player.x - player.width/2 < obs.x + obs.width) {
                        if (obs.isDanger) {
                            ChangeState(GameState::GAME_OVER);
                            Save();
                        } else {
                            // land on the platform
                            player.y = obs.y - player.height/2;
                            player.velocity = 0;
                            onPlatform = true;
                            // follow the platform up
                            player.y -= platformSpeed * dt;
                        }
                    }
                }
            }
        }

        if (!onPlatform) {
            // apply gravity if we are in the air
            player.velocity += player.gravity * dt;
        }
        player.y += player.velocity * dt;

        // check top border death
        if (player.y - player.height/2 < 0) {
            ChangeState(GameState::GAME_OVER);
            Save();
        }

        // check bottom border death
        if (player.y - player.height/2 > windowHeight) {
            ChangeState(GameState::GAME_OVER);
            Save();
        }

        // remove platforms that are way off the top of the screen
        obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(), 
            [](const Obstacle& o) { return o.y + o.height < -50; }), obstacles.end());
    }
}

void Game::Draw() {
    // clear screen
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Background gradient
    RenderUtils::DrawGradientRect(0, 0, windowWidth, windowHeight, 
                                  0.05f, 0.1f, 0.2f, 
                                  0.2f, 0.4f, 0.6f);

    if (state == GameState::SPLASH) {
        std::string txt = "abrshiz Studio";
        float tw = RenderUtils::GetTextWidth(txt, GLUT_BITMAP_TIMES_ROMAN_24);
        RenderUtils::DrawTextStr(windowWidth/2 - tw/2, windowHeight/2, txt, GLUT_BITMAP_TIMES_ROMAN_24, 1, 1, 1);
    }
    else {
        if (state == GameState::PLAYING || state == GameState::PAUSED || state == GameState::GAME_OVER) {
            particles.Draw();
            
            for (auto& obs : obstacles) {
                obs.Draw();
            }
            player.Draw();
            
            // HUD text
            std::string scoreTxt = "Score: " + std::to_string(currentScore);
            RenderUtils::DrawTextStr(20, 30, scoreTxt, GLUT_BITMAP_HELVETICA_18, 1, 1, 1);
            std::string hsTxt = "High Score: " + std::to_string(highScore);
            RenderUtils::DrawTextStr(20, 60, hsTxt, GLUT_BITMAP_HELVETICA_18, 1, 1, 0.5f); // yellow for high score
        }

        if (state != GameState::PLAYING) {
            menu.Draw(state, windowWidth, windowHeight, currentScore, highScore);
        }
    }

    // fade overlay
    if (fadeAlpha > 0.0f) {
        RenderUtils::DrawRect(0, 0, windowWidth, windowHeight, 0, 0, 0, fadeAlpha);
    }
}

void Game::HandleKeyDown(unsigned char key) {
    keys[key] = true;
    if (key == 27) { // ESC key pressed
        if (state == GameState::PLAYING) ChangeState(GameState::PAUSED);
        else if (state == GameState::PAUSED) ChangeState(GameState::PLAYING);
        else if (state == GameState::MAIN_MENU) ChangeState(GameState::EXIT_CONFIRM);
        else if (state == GameState::ABOUT || state == GameState::RULES || state == GameState::HIGH_SCORE) ChangeState(GameState::MAIN_MENU);
    }
}

void Game::HandleKeyUp(unsigned char key) {
    keys[key] = false;
}

void Game::HandleSpecialKeyDown(int key) {
    specialKeys[key] = true;
}

void Game::HandleSpecialKeyUp(int key) {
    specialKeys[key] = false;
}

void Game::HandleMouseClick(int button, int s, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && s == GLUT_DOWN) {
        menu.OnMouseClick(x, y);
    }
}

void Game::HandleMouseMove(int x, int y) {
    menu.OnMouseMove(x, y); // for button hovers
}

void Game::HandleResize(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    menu.Init(state, w, h);
}
