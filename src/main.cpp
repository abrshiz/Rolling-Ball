#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include "Game.h"

// keeping track of time
int lastTime = 0; 

void display() {
    g_Game.Draw();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, h, 0); // origin is top left because it's easier for 2D
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    g_Game.HandleResize(w, h);
}

void update(int value) {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float dt = (currentTime - lastTime) / 1000.0f; // dt in seconds
    lastTime = currentTime;
    
    // clamp it so physics don't explode if window is dragged
    if (dt > 0.1f) dt = 0.1f;

    g_Game.Update(dt);
    
    glutPostRedisplay(); // draw again
    glutTimerFunc(16, update, 0); // roughly 60 fps
}

void keyboard(unsigned char key, int x, int y) {
    g_Game.HandleKeyDown(key);
}

void keyboardUp(unsigned char key, int x, int y) {
    g_Game.HandleKeyUp(key);
}

void specialKeys(int key, int x, int y) {
    g_Game.HandleSpecialKeyDown(key);
}

void specialKeysUp(int key, int x, int y) {
    g_Game.HandleSpecialKeyUp(key);
}

void mouse(int button, int state, int x, int y) {
    // only passing clicks down
    g_Game.HandleMouseClick(button, state, x, y);
}

void passiveMotion(int x, int y) {
    g_Game.HandleMouseMove(x, y);
}

int main(int argc, char** argv) {
    // init glut and window stuff
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Rolling Ball (Hot Air Balloon Style)");

    // enable some nice rendering
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // standard alpha blending

    g_Game.Init(); // start the game logic

    lastTime = glutGet(GLUT_ELAPSED_TIME);

    // register all my callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(passiveMotion);
    
    // start the loop
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    
    return 0;
}
