#include "render.hpp"
#include <cstring>
#include <iostream>

chip *applicationRender::application = nullptr;
applicationRender *applicationRender::renderApp = nullptr;

void applicationRender::setupOpenGL(int *argc, char **argv)
{
    // Set up openGL
    glutInit(argc, argv);
    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(320, 320);
    glutInitDisplayMode(displayMode);
    // Create the window
    glutCreateWindow("chip8 by Huy");

    // glutDisplayFunc sets the display callback for the current window.
    // When GLUT determines that the normal plane for the window needs to be redisplayed,
    // the display callback for the window is called
    glutDisplayFunc(displayCallback);
    glutKeyboardFunc(keyboardDownCallback);
    glutKeyboardUpFunc(keyboardUpCallback);
    glutReshapeFunc(reshapeCallback);
    glutIdleFunc(displayCallback);

    setupTexture();
}

void applicationRender::setupTexture()
{
    int height = application->getDisplayHeight();
    int width = application->getDisplayWidth();
    memset(screenData, 0, sizeof(screenData));
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *)screenData);

    // Set up the texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // Enable textures
    glEnable(GL_TEXTURE_2D);
}
void applicationRender::startRender()
{
    glutMainLoop();
}
void applicationRender::displayCallback()
{
    application->emulateCycle();
    if (application->isDrawn())
    {
        // Clear framebuffer
        glClear(GL_COLOR_BUFFER_BIT);
        renderApp->updateTexture();
        // Swap buffers!
        glutSwapBuffers();
        application->setDrawnFlag(0);
    }
}
void applicationRender::reshapeCallback(int width, int height)
{
    glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, width, height);
    // Resize quad
    renderApp->screenWidth = width;
    renderApp->screenHeight = height;
}
void applicationRender::updateTexture()
{
    int height = application->getDisplayHeight();
    int width = application->getDisplayWidth();
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            for (int k = 0; k < colorDepth; k++)
            {
                if (application->getDisplayData(i * 64 + j) == 0)
                {
                    screenData[i][j][k] = 0;
                }
                else
                {
                    screenData[i][j][k] = 255;
                }
            }
        }
    }
    // debugRender();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *)screenData);

    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0);
    glVertex2d(0.0, 0.0);
    glTexCoord2d(1.0, 0.0);
    glVertex2d(screenWidth, 0.0);
    glTexCoord2d(1.0, 1.0);
    glVertex2d(screenWidth, screenHeight);
    glTexCoord2d(0.0, 1.0);
    glVertex2d(0.0, screenHeight);
    glEnd();
}
void applicationRender::keyboardUpCallback(unsigned char key, int xCoord, int yCoord)
{
    if (key == 27)
    { // esc
        exit(0);
    }
    int pos = -1; // Initialize with an invalid value
    switch (key)
    {
    case '1':
        pos = 0x01;
        break;
    case '2':
        pos = 0x02;
        break;
    case '3':
        pos = 0x03;
        break;
    case '4':
        pos = 0xC;
        break;
    case 'q':
        pos = 0x4;
        break;
    case 'w':
        pos = 0x5;
        break;
    case 'e':
        pos = 0x6;
        break;
    case 'r':
        pos = 0xD;
        break;
    case 'a':
        pos = 0x7;
        break;
    case 's':
        pos = 0x8;
        break;
    case 'd':
        pos = 0x9;
        break;
    case 'f':
        pos = 0xE;
        break;
    case 'z':
        pos = 0xA;
        break;
    case 'x':
        pos = 0x0;
        break;
    case 'c':
        pos = 0xB;
        break;
    case 'v':
        pos = 0xF;
        break;
    }
    if (pos != -1)
    { // Only update the position if a valid key was pressed
        application->setKey(pos, 1);
    }
    std::cout << "Press key: " << key << std::endl;
}

void applicationRender::keyboardDownCallback(unsigned char key, int xCoord, int yCoord)
{
    int pos = -1; // Initialize with an invalid value
    switch (key)
    {
    case '1':
        pos = 0x01;
        break;
    case '2':
        pos = 0x02;
        break;
    case '3':
        pos = 0x03;
        break;
    case '4':
        pos = 0xC;
        break;
    case 'q':
        pos = 0x4;
        break;
    case 'w':
        pos = 0x5;
        break;
    case 'e':
        pos = 0x6;
        break;
    case 'r':
        pos = 0xD;
        break;
    case 'a':
        pos = 0x7;
        break;
    case 's':
        pos = 0x8;
        break;
    case 'd':
        pos = 0x9;
        break;
    case 'f':
        pos = 0xE;
        break;
    case 'z':
        pos = 0xA;
        break;
    case 'x':
        pos = 0x0;
        break;
    case 'c':
        pos = 0xB;
        break;
    case 'v':
        pos = 0xF;
        break;
    }
    if (pos != -1)
    { // Only update the position if a valid key was pressed
        application->setKey(pos, 0);
    }
}
void applicationRender::debugRender()
{
    // Draw
    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            if (screenData[y][x][0] == 0)
                printf(" ");
            else
                printf("0");
        }
        printf("\n");
    }
    printf("\n");
}