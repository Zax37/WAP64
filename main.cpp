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

        sf::RenderWindow window(sf::VideoMode(640, 480), level.getLevelName());
        window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(60);
        sf::View view(sf::FloatRect(0, 0, 640, 480));

        view.setCenter(sf::Vector2f(level.spawn_x, level.spawn_y));
        window.setView(view);

        sf::RectangleShape klocek(sf::Vector2f(64, 64));
        klocek.setTexture(level._getT());

        auto main_plane = level.getMainPlane();
        cout << wap_plane_get_image_set(main_plane, 0) << endl;

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
                view.move(-TILE_W,0);
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                view.move(TILE_W,0);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                view.move(0,-TILE_W);
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                view.move(0,TILE_W);

            window.setView(view);

            window.clear();

            for(int y=max((view.getCenter().y-window.getSize().y/2)/TILE_W, 0.f); y<=(view.getCenter().y+window.getSize().y/2)/TILE_W; y++ )
                for(int x=max((view.getCenter().x-window.getSize().x/2)/TILE_W, 0.f); x<=(view.getCenter().x+window.getSize().x/2)/TILE_W; x++ )
                if(auto tile_id = level.getTile(main_plane, x, y))
                {
                    if(tile_id>1000) continue;
                    klocek.setPosition(x*TILE_W, y*TILE_W);
                    level.setTileImage(klocek, tile_id);
                    window.draw(klocek);
                }
            window.draw(text);
            window.display();
        }

    }

    return 0;

}
