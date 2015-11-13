#include <wap32.h>
#include <iostream>
#include <SFML/Graphics.hpp>

#include <dirent.h>

#define TILE_EMPTY 4294967295
#define TILE_FILL 4008636142

class wwd_map_plane;
class wwd_resource;

class wwd_map {
friend class wwd_map_plane;
friend class wwd_map_tile;
protected:
    unsigned short base_level = 0;
    sf::Color palette[256];
    wap_wwd* wwd;
    std::vector<wwd_map_plane> planes;
    std::map<std::string, wwd_resource> resources;
    wap_plane* main_plane;
public:
    unsigned int spawn_x=0, spawn_y=0;
    wwd_map(std::string filename);
    ~wwd_map();
    const char* getLevelName();
    const char* getLevelDir();
    void draw( sf::RenderTarget& target, sf::IntRect rect );
    wwd_resource* loadResource(const char* path, const char* as);
    wwd_resource* getResource(const char* name);
    wap_plane* getMainPlane(){ return main_plane; }
};

class wwd_map_plane {
friend class wwd_map;
protected:
    sf::Sprite tile_sprite;
    wap_plane * plane;
    wwd_map * wwd_map_ptr;
    wwd_resource * tileset;
    sf::Color fill_color;
    sf::Texture texture;
    bool preRendered = false;
    //sf::RenderTexture preRendTex;
    uint32_t plane_w, plane_h;
    std::vector<class wwd_map_tile> tiles;

    void setTileImage(unsigned short id);
    uint32_t getTile(uint32_t x, uint32_t y);
public:
    unsigned short TILE_W, TILE_H;
    wwd_map_plane(wwd_map * wp, wap_plane* p);
    void draw(sf::RenderTarget& target, sf::IntRect rect );
    void preRender();
};

class wwd_map_tile
{
    uint32_t value;
    wap_tile_description* properties;
    std::vector<wap_object*> objects;
public:
    wwd_map_tile(uint32_t v, wwd_map* ptr);
};

class wwd_resource
{
    std::map<unsigned short, sf::IntRect> imageset;
public:
    sf::Image texture;
    sf::IntRect operator[](unsigned short id);
    sf::IntRect get(unsigned short id);
    void set(unsigned short id, sf::Image &img);
};
