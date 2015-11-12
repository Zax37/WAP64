#include "res.h"
#include <assert.h>
#include <string.h>

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

using namespace std;
int main2(sf::RenderWindow * window, const char * l_name);

int main()
{
    sfg::SFGUI sfgui;
    vector<shared_ptr<sfg::Label>> levels;

    sf::RenderWindow window(sf::VideoMode(640, 480), "WAP64");
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

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
                    levels.back()->GetSignal( sfg::Widget::OnLeftClick ).Connect( [&window, label](){ main2(&window, label->GetText().toAnsiString().c_str()); } );
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

int main2(sf::RenderWindow* window, const char * l_name)
{
    wwd_map level(l_name);

    sf::Font font;
    if (!font.loadFromFile("cambriab.ttf"))
        return 1;

    sf::Text text;
    text.setFont(font);
    text.setString(to_string(level.spawn_x)+"x"+to_string(level.spawn_y));
    text.setCharacterSize(24);
    text.setColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);
    text.setPosition(sf::Vector2f(level.spawn_x, level.spawn_y));

    sf::Sprite claw;
    wwd_resource * clawres = level.getResource("CLAW");
    sf::Texture c_tex;
    c_tex.loadFromImage(clawres->texture);
    claw.setTexture(c_tex);
    claw.setTextureRect(clawres->get(2));
    claw.setPosition(level.spawn_x, level.spawn_y);

    sf::View view(sf::FloatRect(0, 0, 640, 480));

    view.setCenter(sf::Vector2f(level.spawn_x, level.spawn_y));
    window->setView(view);

    sf::Vector2u s = window->getSize();

    sf::Vector2i touch_st_pos; sf::Vector2f touch_center;
    bool touch_moving = false; unsigned int touch_finger=0;

    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            switch(event.type)
            {
                case sf::Event::Closed:
                    window->close();
                break;
                case sf::Event::Resized:
                    view.setSize(event.size.width, event.size.height);
                    window->setView(sf::View(view));
                    s = window->getSize();
                break;
                case sf::Event::TouchBegan:
                    if(!touch_moving)
                    {
                        touch_finger = event.touch.finger;
                        touch_st_pos.x = event.touch.x;
                        touch_st_pos.y = event.touch.y;
                        touch_center = view.getCenter();
                        touch_moving = true;
                    }
                break;
                case sf::Event::TouchEnded:
                    if(event.touch.finger==touch_finger)
                        touch_moving = false;
                break;
                case sf::Event::KeyPressed:
                    if(event.key.code == sf::Keyboard::Escape)
                        return 0;
                break;
                default:
                break;
            }


        }

        if(touch_moving && sf::Touch::isDown(touch_finger))
        {
            sf::Vector2i touch_cur_pos = sf::Touch::getPosition(touch_finger, *window);
            view.setCenter(touch_center);
            sf::Vector2f diff = (sf::Vector2f)(touch_st_pos-touch_cur_pos);
            view.move(diff);
        }


        #define step 9
        //sf::Touch::getPosition()
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            view.move(-step,0);
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            view.move(step,0);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            view.move(0,-step);
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            view.move(0,step);

        window->setView(view);

        window->clear();

        sf::Vector2f c = view.getCenter();
        level.draw( window, sf::IntRect( c.x-s.x/2, c.y-s.y/2, s.x, s.y ) );
        window->draw(claw);
        window->draw(text);
        window->display();
    }

return 0;

}
