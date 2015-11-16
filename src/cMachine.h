#include "res.h"

namespace WAP64
{
    enum {
        WAP64_ERR = -1,
        WAP64_CLOSED_OK=0,
        WAP64_INIT,
        WAP64_MAINMENU,
        WAP64_PLAY
    } wState;

    class Game
    {
    private:
        int _state;
        sf::RenderWindow window;
        sf::VideoMode vidmode;
        wwd_map level;
    public:
        Game();
        void Start();
        void ProcessCommand(const char* command);
        void HandleEvent(sf::Event& event);
        int GetState();
        bool Run();
        void LoadLevel(const char* file);
        void MainMenuLoop();
        void PlayLevelLoop();
    };
}
