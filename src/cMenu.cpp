#include "cMachine.h"
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <dirent.h>
#include <assert.h>

namespace WAP64{

    void Game::MainMenuLoop(){
        sfg::SFGUI sfgui;
        std::vector<std::shared_ptr<sfg::Label>> levels;

        sfg::Desktop desktop;
        desktop.SetProperty("Label", "Color", sf::Color( 135, 135, 135 ));
        desktop.SetProperty("Label.hover", "Color", sf::Color( 235, 235, 235 ));

        auto sfg_window = sfg::Window::Create();
        sfg_window->SetTitle( "Select level" );

        auto box = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 5.0f );
        sfg_window->Add( box );
        desktop.Add( sfg_window );

        sf::Texture bgd; assert(bgd.loadFromFile("DATA\\STATES\\MENU\\SCREENS\\MENU.JPG"));
        sf::RectangleShape screen((sf::Vector2f)window.getSize());
        screen.setTexture(&bgd);

        wap_wwd_properties wwdp;

        DIR * files = opendir(".");
        struct dirent *d;
        if(files)
        {
            while((d = readdir(files)) != NULL)
            {
                std::string n = d->d_name;
                if(n.length()>4)
                {
                    std::transform(n.begin(), n.end(), n.begin(), ::tolower);
                    if (n.substr(n.length()-4, 4) == ".wwd")
                    {
                        //cerr << wap_wwd_getheader(wwdp, n.c_str()) << endl;
                        levels.emplace_back( sfg::Label::Create( n.c_str() ) );
                        std::shared_ptr<sfg::Label> label = levels.back();
                        box->Pack( label );
                        levels.back()->GetSignal( sfg::Widget::OnLeftClick ).Connect( [this, label](){ LoadLevel(label->GetText().toAnsiString().c_str()); } );
                        levels.back()->GetSignal( sfg::Widget::OnMouseEnter ).Connect( [label](){ label->SetClass("hover"); } );
                        levels.back()->GetSignal( sfg::Widget::OnMouseLeave ).Connect( [label](){ label->SetClass(" ");  } );
                    }
                }
            }
        }

        sfg_window->SetPosition(sf::Vector2f(window.getSize().x/2-sfg_window->GetRequisition().x/2, window.getSize().y/2-sfg_window->GetRequisition().y/2));

        sf::Event event;
        sf::Clock clock;

        window.resetGLStates();

        while (window.isOpen() && _state == WAP64_MAINMENU)
            {
                while (window.pollEvent(event))
                {
                    desktop.HandleEvent( event );
                    HandleEvent( event );
                }

                desktop.Update( clock.restart().asSeconds() );

                window.draw(screen);

                sfgui.Display( window );
                window.display();
            }

    }
}
