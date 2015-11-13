#include "res.h"
#include <assert.h>
#include <string.h>

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

#include "PlayLevel.h"

using namespace std;

int main()
{
    sfg::SFGUI sfgui;
    vector<shared_ptr<sfg::Label>> levels;

    sf::RenderWindow window(sf::VideoMode(640, 480), "WAP64");
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(30);

    sfg::Desktop desktop;
	desktop.SetProperty("Label", "Color", sf::Color( 135, 135, 135 ));
	desktop.SetProperty("Label.hover", "Color", sf::Color( 235, 235, 235 ));

    sf::Image background;
    assert(background.loadFromFile("DATA\\STATES\\MENU\\SCREENS\\MENU.JPG"));

    auto bgd = sfg::Image::Create(background);
    auto bgd_window = sfg::Window::Create();
    bgd_window->Add(bgd);
    bgd_window->SetStyle(0);
    bgd_window->SetPosition(sf::Vector2f(-11, -22));

    desktop.Add( bgd_window );

    auto sfg_window = sfg::Window::Create();
	sfg_window->SetTitle( "Select level" );

	auto box = sfg::Box::Create( sfg::Box::Orientation::VERTICAL, 5.0f );
	sfg_window->Add( box );
	desktop.Add( sfg_window );

    DIR * files = opendir("DATA\\..");
    struct dirent *d;
    if(files)
    {
        while((d = readdir(files)) != NULL)
        {
            string n = d->d_name;
            if(n.length()>4)
            {
                std::transform(n.begin(), n.end(), n.begin(), ::tolower);
                if (n.substr(n.length()-4, 4) == ".wwd")
                {
                    levels.emplace_back( sfg::Label::Create( n.c_str() ) );
                    shared_ptr<sfg::Label> label = levels.back();
                    box->Pack( label );
                    levels.back()->GetSignal( sfg::Widget::OnLeftClick ).Connect( [&window, label](){ PlayLevel(&window, label->GetText().toAnsiString().c_str()); } );
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

    while (window.isOpen())
        {
            while (window.pollEvent(event))
            {
                desktop.HandleEvent( event );
                switch(event.type)
                {
                    case sf::Event::Closed:
                        window.close();
                    break;
                }
            }

            desktop.Update( clock.restart().asSeconds() );

            window.clear();

            sfgui.Display( window );
            window.display();
        }

    return 0;
}
