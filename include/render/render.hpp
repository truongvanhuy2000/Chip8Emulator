#pragma once

#include <GL/glut.h>
#include <chip8/chip8.hpp>

class applicationRender
{
    int screenWidth = 640;
    int screenHeight = 320;
    int displayMode = GLUT_DOUBLE | GLUT_RGBA;

    const int colorDepth = 3;
    //OpenGL uses a column-major ordering for its matrices and arrays
    uint8_t screenData[32][64][3];

    static chip *application;
    static applicationRender *renderApp;
    void setupTexture();
    void updateTexture();

    static void displayCallback();
    static void keyboardUpCallback(unsigned char key, int xCoord, int yCoord);
    static void keyboardDownCallback(unsigned char key, int xCoord, int yCoord);
    static void reshapeCallback(int xCoord, int yCoord);

public:
    applicationRender(chip *app, int width, int height)
        : screenWidth(width),
          screenHeight(height)
    {
        application = app;
        renderApp = this;
        // screenData = new uint8_t[pixel * colorDepth];
    }
    ~applicationRender()
    {
        // delete[] screenData;
    }
    void debugRender();
    void setupOpenGL(int *argc, char **argv);
    void startRender();
};
