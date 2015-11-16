#include "cMachine.h"
#include <assert.h>
#include <iostream>

using namespace std;

namespace WAP64
{
    Game::Game()
    {

    }

    void Game::Start()
    {
        _state = WAP64_INIT;
    }

    void Game::ProcessCommand(const char* command)
    {
        //PlayLevel(window, command);
    }

    int Game::GetState()
    {
        return _state;
    }

    void Game::HandleEvent(sf::Event& event)
    {
        switch(event.type)
        {
            case sf::Event::Closed:
                window.close();
                _state = WAP64_CLOSED_OK;
            break;
        }
    }

    void Game::LoadLevel(const char* file)
    {
        cout << file << endl;
        if(level.load(file))
            _state = WAP64_PLAY;
        else _state = WAP64_ERR;
    }

    bool Game::Run()
    {
        switch(_state)
        {
        case WAP64_INIT:
            //load resources
            vidmode = sf::VideoMode(800, 600);
            window.create(vidmode, "WAP64");
            window.setVerticalSyncEnabled(true);
            window.setFramerateLimit(30);
            _state = WAP64_MAINMENU;
        break;
        case WAP64_MAINMENU:
            window.setView(window.getDefaultView());
            MainMenuLoop();
        break;
        case WAP64_PLAY:
            PlayLevelLoop();
        break;
        }
        return _state>0;
    }
}
