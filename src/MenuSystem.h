#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#include <string>
#include <vector>
#include <functional>

enum class GameState {
    SPLASH, MAIN_MENU, PLAYING, PAUSED, GAME_OVER, ABOUT, RULES, HIGH_SCORE, EXIT_CONFIRM
};

struct Button {
    float x, y, w, h;
    std::string text;
    bool isHovered;
    std::function<void()> onClick;

    void Draw();
    bool CheckHover(float mx, float my);
};

class MenuSystem {
public:
    std::vector<Button> buttons;
    bool hasSavedGame;

    std::function<void()> onNewGame;
    std::function<void()> onContinue;
    std::function<void()> onResume;
    std::function<void()> onPause;
    std::function<void()> onExit;
    std::function<void()> onConfirmExit;
    std::function<void()> onCancelExit;
    std::function<void()> onAbout;
    std::function<void()> onRules;
    std::function<void()> onHighScore;
    std::function<void()> onBackToMenu;
    std::function<void()> onGameOverToMenu;

    MenuSystem() : hasSavedGame(false) {}

    void Init(GameState state, int w, int h);
    void Update(float dt);
    void Draw(GameState state, int windowWidth, int windowHeight, int currentScore, int highScore);
    void OnMouseMove(float mx, float my);
    void OnMouseClick(float mx, float my);
};

#endif
