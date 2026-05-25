#ifndef RENDERUTILS_H
#define RENDERUTILS_H

#include <string>

class RenderUtils {
public:
    static void DrawRect(float x, float y, float w, float h, float r, float g, float b, float a = 1.0f);
    static void DrawGradientRect(float x, float y, float w, float h, 
                                 float r1, float g1, float b1, 
                                 float r2, float g2, float b2);
    static void DrawCircle(float cx, float cy, float radius, float r, float g, float b, int segments = 32);
    static void DrawTextStr(float x, float y, const std::string& text, void* font, float r, float g, float b);
    static float GetTextWidth(const std::string& text, void* font);
};

#endif
