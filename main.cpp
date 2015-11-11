#include "res.h"

using namespace std;

int main(int argc, const char **argv)
{
    for(int i = 1; i < argc; ++i) {
        wwd_map level(argv[i]);

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

        sf::RenderWindow window(sf::VideoMode(800, 600), level.getLevelName());
        window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(60);
        sf::View view(sf::FloatRect(0, 0, 800, 600));

        view.setCenter(sf::Vector2f(level.spawn_x, level.spawn_y));
        window.setView(view);

        auto main_plane = level.getMainPlane();

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();

                if (event.type == sf::Event::Resized)
                {
                    // update the view to the new size of the window
                    view.setSize(event.size.width, event.size.height);
                    window.setView(sf::View(view));

                }

            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                view.move(-8,0);
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                view.move(8,0);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                view.move(0,-8);
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                view.move(0,8);

            window.setView(view);

            window.clear();

            sf::Vector2f c = view.getCenter();
            sf::Vector2u s = window.getSize();
            level.draw( window, sf::IntRect( c.x-s.x/2, c.y-s.y/2, s.x, s.y ) );
            window.draw(text);
            window.display();
        }

    }

    return 0;

}
