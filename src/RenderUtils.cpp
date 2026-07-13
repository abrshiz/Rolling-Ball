#include "RenderUtils.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>
void RenderUtils::DrawRect(float x, float y, float w, float h, float r, float g, float b, float a) {
    glColor4f(r, g, b, a);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x+w, y);
    glVertex2f(x+w, y+h);
    glVertex2f(x, y+h);
    glEnd();
}
void RenderUtils::DrawGradientRect(float x, float y, float w, float h, 
                                   float r1, float g1, float b1, 
                                   float r2, float g2, float b2) {
    glBegin(GL_QUADS);
    glColor3f(r1, g1, b1); // top color
    glVertex2f(x, y);
    glVertex2f(x+w, y);
    glColor3f(r2, g2, b2); // bottom color
    glVertex2f(x+w, y+h);
    glVertex2f(x, y+h);
    glEnd();
}

void RenderUtils::DrawCircle(float cx, float cy, float radius, float r, float g, float b, int segments) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    // basic circle math
    for(int i=0; i<segments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(segments);
        float px = radius * cosf(theta);
        float py = radius * sinf(theta);
        glVertex2f(cx + px, cy + py);
    }
    glEnd();
}

void RenderUtils::DrawTextStr(float x, float y, const std::string& text, void* font, float r, float g, float b) {
    glColor3f(r, g, b);
    
    // basic newline handling for glut text
    float startX = x;
    for(char c : text) {
        if(c == '\n') {
            y += 24; // assume 24px line height roughly
            x = startX;
        } else {
            glRasterPos2f(x, y);
            glutBitmapCharacter(font, c);
            x += glutBitmapWidth(font, c);
        }
    }
}

float RenderUtils::GetTextWidth(const std::string& text, void* font) {
    float maxW = 0;
    float w = 0;
    for(char c : text) {
        if(c == '\n') {
            if(w > maxW) maxW = w;
            w = 0;
        } else {
            w += glutBitmapWidth(font, c);
        }
    }
    if(w > maxW) maxW = w; // catch the last line
    return maxW;
}
