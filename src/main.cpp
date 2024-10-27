#include "Game.hpp"

int main()
{
    CGame game;
    game.Init();
    game.Run();
    game.Shutdown();

    return 0;
}
