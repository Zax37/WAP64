#include <wap32.h>
#include <iostream>
#include <SFML/Graphics.hpp>

#define TILE_W 64

class wwd_map{
protected:
    unsigned short base_level = 0;
    wap_wwd* wwd;
    std::map<unsigned short, unsigned short> action;
    sf::Texture action_tex; bool any_loaded = false;
public:
    unsigned int spawn_x=0, spawn_y=0;
    wwd_map(std::string filename);
    ~wwd_map();
    const char* getLevelName();
    wap_plane* getMainPlane();
    void setTileImage(sf::RectangleShape &tile, unsigned short id);
    sf::Texture* _getT(){ return &action_tex; }
    uint32_t getTile(wap_plane* plane, uint32_t x, uint32_t y);
};
