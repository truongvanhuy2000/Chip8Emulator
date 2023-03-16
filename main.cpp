#include <iostream>
#include "render.hpp"
#include "chip8.hpp"
int main(int argc, char **argv)
{
    chip *myChip8 = new chip8();
    applicationRender openGLRender(myChip8, 640, 320);
    if (myChip8->loadApplication("pong2.c8"))
    {
        openGLRender.setupOpenGL(&argc, argv);
        openGLRender.startRender();
    }
    return 0;
}