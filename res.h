#include <wap32.h>
#include <iostream>
#include <SFML/Graphics.hpp>

#include <dirent.h>

class wwd_map_plane;

class wwd_map {
friend class wwd_map_plane;
protected:
    unsigned short base_level = 0;
    wap_wwd* wwd;
    std::vector<wwd_map_plane> planes;
    wwd_map_plane* main_plane;
public:
    unsigned int spawn_x=0, spawn_y=0;
    wwd_map(std::string filename);
    ~wwd_map();
    const char* getLevelName();
    const char* getLevelDir();
    wwd_map_plane* getMainPlane();
    void draw( sf::RenderTarget& target, sf::IntRect rect );
};

class wwd_map_plane {
friend class wwd_map;
protected:
    sf::RectangleShape tile;
    wap_plane * plane;
    wwd_map * wwd_map_ptr;
    sf::Color palette[256];

    std::map<unsigned short, unsigned short> imageset;
    sf::Texture texture; bool any_loaded = false;
    void setTileImage(unsigned short id);
    uint32_t getTile(uint32_t x, uint32_t y);
    void loadTileset(const char* imageset);
    void loadPalette();
public:
    unsigned short TILE_W, TILE_H;
    wwd_map_plane(wwd_map * wp, wap_plane* p);
    void draw(sf::RenderTarget& target, sf::IntRect rect );
};
