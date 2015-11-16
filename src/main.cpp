#include "cMachine.h"

int main(int argc, const char* argv[])
{
    WAP64::Game Game;
    for(int i=1; i<argc; i++)
        Game.ProcessCommand(argv[i]);
    Game.Start();
    while(Game.Run());
    return Game.GetState();
}
