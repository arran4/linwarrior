#include "rChatMember.h"

#include "de/hackcraft/world/World.h"

#include "de/hackcraft/psi3d/Console.h"
#include "de/hackcraft/psi3d/GLS.h"

#include <iostream>
using std::cout;
using std::endl;

#include <sstream>
using std::stringstream;

#include <cassert>

std::string rChatMember::cname = "COMCOM";
unsigned int rChatMember::cid = 5007;

rChatMember::rChatMember(Entity* obj) {
    object = obj;
    
    id = (OID) this;

    int w = 20;
    int h = 10;
    mConsole = Console::Console_new(w * h, w);
    assert(mConsole != NULL);
    Console* console = (Console*) mConsole;
    //Console_printf(mConsole, "* Welcome to LinWarrior3D\n");
    Console::Console_print(console, "ComCOM(1) - MENU\n\n");
    Console::Console_print(console, " [1] Formation\n");
    Console::Console_print(console, " [2] Engage\n");
    Console::Console_print(console, " [3] Report\n");
    Console::Console_print(console, " [4] Objectives\n");
    Console::Console_print(console, "\n\n > ");
    mLastMessage = 0;
}

void rChatMember::animate(float spf) {
    rChatMessage* message = NULL; //World::getInstance()->recvMessage(0, mLastMessage);
    if (message != NULL) {
        mLastMessage = message->getTimestamp();
    }
}

void rChatMember::drawHUD() {
    if (!active) return;

    //glColor4f(0,0.1,0,0.2);
    glBegin(GL_QUADS);
    glVertex3f(1, 1, 0);
    glVertex3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);
    glEnd();
    glPushMatrix();
    {
        glColor4f(0, 1, 0, 0.9);
        //glColor4f(0.99, 0.99, 0.19, 1);
        Console* console = (Console*) mConsole;
        float w = console->width;
        float h = console->size / console->width;
        glTranslatef(0, 1.0f + (1.0f / h), 0);
        glScalef(1.0f / w, 1.0f / h, 1.0f);
        Console::Console_draw(console);
    }
    glPopMatrix();
}

void rChatMember::recvMessage(rChatMessage* msg) {
    
    Console* console = (Console*) mConsole;
    Console::Console_print(console, msg->getText().c_str());
}
