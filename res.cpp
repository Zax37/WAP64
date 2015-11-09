#include "res.h"
#include <assert.h>

using namespace std;

wwd_map::wwd_map(string filename){
    wwd = wap_wwd_create();

    assert(!wap_wwd_open(wwd, filename.c_str()));
    //assert(wap_wwd_get_properties(wwd)->flags & WAP_WWD_FLAG_COMPRESS);

    auto map_properties = wap_wwd_get_properties(wwd);

    string level_name = map_properties->level_name;

    for(string::iterator c = level_name.begin(); c!=level_name.end(); c++)
        if(*c >= '0' && *c <= '9') { base_level = atoi(&*c); break; }

    assert(base_level);

    spawn_x = map_properties->start_x;
    spawn_y = map_properties->start_y;
}

wwd_map::~wwd_map(){
    wap_wwd_free(wwd);
}

const char* wwd_map::getLevelName(){ return wap_wwd_get_properties(wwd)->level_name; }
wap_plane* wwd_map::getMainPlane(){
    wap_plane * main_plane = 0;

    for(int i=0; i<wap_wwd_get_plane_count(wwd); i++)
    {
        auto plane = wap_wwd_get_plane(wwd, i);
        if(wap_plane_get_properties(plane)->flags & WAP_PLANE_FLAG_MAIN_PLANE)
            main_plane = plane;
        //auto obj_c = wap_plane_get_object_count(plane);
        //cout << i << ": " << wap_plane_get_properties(plane)->name << "(" << wap_plane_get_image_set(plane, 0) << ")" << endl;
    }
    return main_plane;
}

void wwd_map::setTileImage(sf::RectangleShape &tile, unsigned short id){
    std::map<unsigned short, unsigned short>::iterator it = action.find(id);
    if (it != action.end())
    {
        tile.setTextureRect(sf::IntRect( (*it).second*TILE_W, 0, TILE_W, TILE_W ));
    }
    else
    {
        string file = "DATA\\LEVEL";
        file += to_string(base_level);
        file += "\\TILES\\ACTION\\";
        file += to_string(id);
        file += ".png";

        if(any_loaded)
        {
            sf::Image img = action_tex.copyToImage();
            sf::Image nimg;
            unsigned int width = img.getSize().x;
            nimg.create(width+TILE_W, TILE_W);
            nimg.copy(img, 0, 0);
            assert(img.loadFromFile(file));
            nimg.copy(img, width, 0);
            action_tex.loadFromImage(nimg);
            action[id] = width/TILE_W;
            tile.setTextureRect(sf::IntRect( width, 0, TILE_W, TILE_W ));
        }
        else
        {
            assert(action_tex.loadFromFile(file));
            action[id] = 0;
            tile.setTextureRect(sf::IntRect( 0, 0, TILE_W, TILE_W ));
            any_loaded = true;
        }


    }
}

uint32_t wwd_map::getTile(wap_plane* plane, uint32_t x, uint32_t y){
    uint32_t plane_w, plane_h;
    wap_plane_get_map_dimensions(plane, &plane_w, &plane_h);
    if(x<0 || y<0 || x>=plane_w || y>=plane_h) return 0;
    return wap_plane_get_tile(plane, x, y);
}
