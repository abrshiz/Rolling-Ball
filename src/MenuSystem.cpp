#include "MenuSystem.h"
#include "RenderUtils.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void Button::Draw() {
    if (isHovered) {
        RenderUtils::DrawRect(x, y, w, h, 0.4f, 0.6f, 0.9f); // light blue
    } else {
        RenderUtils::DrawRect(x, y, w, h, 0.2f, 0.4f, 0.8f); // normal blue
    }
    // inner shadow/border
    RenderUtils::DrawRect(x+2, y+2, w-4, h-4, 0.1f, 0.2f, 0.4f);

    float tw = RenderUtils::GetTextWidth(text, GLUT_BITMAP_HELVETICA_18);
    float tx = x + w/2 - tw/2; // center text
    float ty = y + h/2 + 6;
    RenderUtils::DrawTextStr(tx, ty, text, GLUT_BITMAP_HELVETICA_18, 1, 1, 1);
}

bool Button::CheckHover(float mx, float my) {
    // simple bounding box check
    isHovered = (mx >= x && mx <= x+w && my >= y && my <= y+h);
    return isHovered;
}

void MenuSystem::Init(GameState state, int w, int h) {
    buttons.clear(); // clear old buttons
    float bx = w/2 - 100; // center buttons horizontally
    
    if (state == GameState::MAIN_MENU) {
        float by = h/2 - 100;
        buttons.push_back({bx, by, 200, 40, "New Game", false, onNewGame});
        if (hasSavedGame) {
            buttons.push_back({bx, by + 50, 200, 40, "Continue", false, onContinue});
        }
        buttons.push_back({bx, by + 100, 200, 40, "High Score", false, onHighScore});
        buttons.push_back({bx, by + 150, 200, 40, "Rules", false, onRules});
        buttons.push_back({bx, by + 200, 200, 40, "About", false, onAbout});
        buttons.push_back({bx, by + 250, 200, 40, "Exit", false, onExit});
    }
    else if (state == GameState::PAUSED) {
        float by = h/2 - 50;
        buttons.push_back({bx, by, 200, 40, "Resume", false, onResume});
        buttons.push_back({bx, by + 50, 200, 40, "Main Menu", false, onBackToMenu});
    }
    else if (state == GameState::GAME_OVER) {
        float by = h/2 + 50;
        buttons.push_back({bx, by, 200, 40, "Play Again", false, onNewGame});
        buttons.push_back({bx, by + 50, 200, 40, "Main Menu", false, onGameOverToMenu});
    }
    else if (state == GameState::EXIT_CONFIRM) {
        float by = h/2 + 20;
        buttons.push_back({bx - 110, by, 200, 40, "Yes", false, onConfirmExit});
        buttons.push_back({bx + 110, by, 200, 40, "No", false, onCancelExit});
    }
    else if (state == GameState::ABOUT || state == GameState::RULES || state == GameState::HIGH_SCORE) {
        // back button for all these screens
        buttons.push_back({bx, h - 80.0f, 200, 40, "Back", false, onBackToMenu});
    }
}

void MenuSystem::Update(float dt) {
    // nothing to update for UI yet
}

void MenuSystem::Draw(GameState state, int w, int h, int cs, int hs) {
    if (state == GameState::PAUSED || state == GameState::GAME_OVER || state == GameState::EXIT_CONFIRM) {
        // semi transparent black overlay to darken the game screen
        RenderUtils::DrawRect(0, 0, w, h, 0, 0, 0, 0.7f);
    }

    if (state == GameState::MAIN_MENU) {
        std::string title = "ROLLING BALL";
        float tw = RenderUtils::GetTextWidth(title, GLUT_BITMAP_TIMES_ROMAN_24);
        RenderUtils::DrawTextStr(w/2 - tw/2, h/4, title, GLUT_BITMAP_TIMES_ROMAN_24, 1, 0.8f, 0.2f);
    }
    else if (state == GameState::PAUSED) {
        std::string title = "PAUSED";
        float tw = RenderUtils::GetTextWidth(title, GLUT_BITMAP_TIMES_ROMAN_24);
        RenderUtils::DrawTextStr(w/2 - tw/2, h/3, title, GLUT_BITMAP_TIMES_ROMAN_24, 1, 1, 1);
    }
    else if (state == GameState::GAME_OVER) {
        std::string title = "GAME OVER";
        float tw = RenderUtils::GetTextWidth(title, GLUT_BITMAP_TIMES_ROMAN_24);
        RenderUtils::DrawTextStr(w/2 - tw/2, h/3 - 40, title, GLUT_BITMAP_TIMES_ROMAN_24, 1, 0.2f, 0.2f);
        
        // show scores
        std::string s1 = "Score: " + std::to_string(cs);
        std::string s2 = "High Score: " + std::to_string(hs);
        RenderUtils::DrawTextStr(w/2 - RenderUtils::GetTextWidth(s1, GLUT_BITMAP_HELVETICA_18)/2, h/3, s1, GLUT_BITMAP_HELVETICA_18, 1,1,1);
        RenderUtils::DrawTextStr(w/2 - RenderUtils::GetTextWidth(s2, GLUT_BITMAP_HELVETICA_18)/2, h/3 + 30, s2, GLUT_BITMAP_HELVETICA_18, 1,1,0);
    }
    else if (state == GameState::EXIT_CONFIRM) {
        std::string title = "Are you sure you want to exit?";
        float tw = RenderUtils::GetTextWidth(title, GLUT_BITMAP_HELVETICA_18);
        RenderUtils::DrawTextStr(w/2 - tw/2, h/2 - 30, title, GLUT_BITMAP_HELVETICA_18, 1, 1, 1);
    }
    else if (state == GameState::ABOUT) {
        std::string title = "ABOUT";
        float tw = RenderUtils::GetTextWidth(title, GLUT_BITMAP_TIMES_ROMAN_24);
        RenderUtils::DrawTextStr(w/2 - tw/2, h/4, title, GLUT_BITMAP_TIMES_ROMAN_24, 1, 1, 1);
        std::string text = "Rolling Ball Arcade Game\nCreated in C++ and OpenGL / freeglut.\n\n © abrshiz 2026";
        RenderUtils::DrawTextStr(w/2 - 150, h/2, text, GLUT_BITMAP_HELVETICA_18, 0.8f, 0.8f, 0.8f);
    }
    else if (state == GameState::RULES) {
        std::string title = "RULES";
        float tw = RenderUtils::GetTextWidth(title, GLUT_BITMAP_TIMES_ROMAN_24);
        RenderUtils::DrawTextStr(w/2 - tw/2, h/4, title, GLUT_BITMAP_TIMES_ROMAN_24, 1, 1, 1);
        std::string text = "Use LEFT/RIGHT ARROWS or A/D to steer.\nFall onto the safe GREEN platforms.\nAvoid RED spiky platforms.\nDon't hit the top or bottom of the screen!";
        RenderUtils::DrawTextStr(w/2 - 180, h/2, text, GLUT_BITMAP_HELVETICA_18, 0.8f, 0.8f, 0.8f);
    }
    else if (state == GameState::HIGH_SCORE) {
        std::string title = "HIGH SCORE";
        float tw = RenderUtils::GetTextWidth(title, GLUT_BITMAP_TIMES_ROMAN_24);
        RenderUtils::DrawTextStr(w/2 - tw/2, h/4, title, GLUT_BITMAP_TIMES_ROMAN_24, 1, 1, 1);
        std::string text = std::to_string(hs);
        RenderUtils::DrawTextStr(w/2 - RenderUtils::GetTextWidth(text, GLUT_BITMAP_TIMES_ROMAN_24)/2, h/2, text, GLUT_BITMAP_TIMES_ROMAN_24, 1, 1, 0); // yellow score
    }

    // draw all the UI buttons
    for (auto& b : buttons) {
        b.Draw();
    }
}

void MenuSystem::OnMouseMove(float mx, float my) {
    for (auto& b : buttons) {
        b.CheckHover(mx, my);
    }
}

void MenuSystem::OnMouseClick(float mx, float my) {
    for (auto& b : buttons) {
        if (b.CheckHover(mx, my)) {
            if (b.onClick) b.onClick(); // fire callback
        }
    }
}
