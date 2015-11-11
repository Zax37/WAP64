#include "res.h"
#include <assert.h>
#include <string.h>

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

    for(int i=0; i<wap_wwd_get_plane_count(wwd); i++)
    {
        wwd_map_plane plane(this, wap_wwd_get_plane(wwd, i));

        if(wap_plane_get_properties(plane.plane)->flags & WAP_PLANE_FLAG_MAIN_PLANE)
            main_plane = &*planes.end();

        planes.push_back(plane);
        //auto obj_c = wap_plane_get_object_count(plane);
        //cout << i << ": " << wap_plane_get_properties(plane)->name << "(" << wap_plane_get_image_set(plane, 0) << ")" << endl;
    }

}

wwd_map::~wwd_map(){
    wap_wwd_free(wwd);
}

const char* wwd_map::getLevelName(){ return wap_wwd_get_properties(wwd)->level_name; }

wwd_map_plane* wwd_map::getMainPlane(){
    wwd_map_plane * main_plane = 0;

    for(auto i=planes.begin(); i!=planes.end(); i++)
    {
        if(wap_plane_get_properties(i->plane)->flags & WAP_PLANE_FLAG_MAIN_PLANE)
            main_plane = &*i;
        //auto obj_c = wap_plane_get_object_count(plane);
        //cout << i << ": " << wap_plane_get_properties(plane)->name << "(" << wap_plane_get_image_set(plane, 0) << ")" << endl;
    }
    return main_plane;
}

void wwd_map_plane::loadTileset(const char* tileset)
{
    string path = "DATA\\LEVEL";
    path += to_string(wwd_map_ptr->base_level);
    path += "\\TILES\\";
    path += tileset;
    path += '\\';

    DIR * assets = opendir(path.c_str());
    struct dirent *d;
    if(assets)
    {
        while((d = readdir(assets)) != NULL)
            if (strcmp(d->d_name, ".") && strcmp(d->d_name, ".."))
            {
                unsigned short id = atoi(d->d_name);
                if(id)
                {
                    /*std::map<unsigned short, unsigned short>::iterator it = imageset.find(id);
                    if (it != imageset.end())
                    {
                        cerr << "Error loading tile " << d->d_name << "." << endl;
                        continue;
                    }*/
                    sf::Image img = texture.copyToImage();
                    sf::Image nimg;
                    unsigned int width = img.getSize().x;
                    nimg.create(width+TILE_W, TILE_H);
                    nimg.copy(img, 0, 0);
                    assert(img.loadFromFile(path+d->d_name));
                    nimg.copy(img, width, 0);
                    texture.loadFromImage(nimg);
                    imageset[id] = width/TILE_W;
                    //cout << id << ": " << width/TILE_W << endl;
                }
                else cerr << "Error loading tile " << d->d_name << "." << endl;
            }
    }

}

void wwd_map_plane::setTileImage(unsigned short id){
    std::map<unsigned short, unsigned short>::iterator it = imageset.find(id);
    if (it != imageset.end())
    {
        tile.setTextureRect(sf::IntRect( (*it).second*TILE_W, 0, TILE_W, TILE_H ));
    }
    else
    {
        cerr << "tile " << id << " not found" << endl;
        tile.setTextureRect(sf::IntRect(0, 0, TILE_W, TILE_H) );
    }
}

uint32_t wwd_map_plane::getTile(uint32_t x, uint32_t y)
{
    uint32_t plane_w, plane_h;
    wap_plane_get_map_dimensions(plane, &plane_w, &plane_h);
    if(x>=plane_w)
    {
        if(wap_plane_get_properties(plane)->flags & WAP_PLANE_FLAG_X_WRAPPING)
            x= x % plane_w;
        else return 0;
    }
    if(y>=plane_h)
    {
        if(wap_plane_get_properties(plane)->flags & WAP_PLANE_FLAG_Y_WRAPPING)
            y= y % plane_h;
        else return 0;
    }
    if(x<0 || y<0) return 0;
    return wap_plane_get_tile(plane, x, y);
}

wwd_map_plane::wwd_map_plane(wwd_map * wp, wap_plane* p)
{
    wwd_map_ptr = wp;
    plane = p;
    TILE_W = wap_plane_get_properties(plane)->tile_width;
    TILE_H = wap_plane_get_properties(plane)->tile_height;
    tile.setSize(sf::Vector2f(TILE_W, TILE_H));
    texture.create(TILE_W, TILE_H);
    loadTileset(wap_plane_get_image_set(plane, 0));
}

void wwd_map::draw(sf::RenderTarget& target, sf::IntRect rect)
{
    for(auto p = planes.begin(); p!=planes.end(); p++)
        p->draw( target, rect);
}

void wwd_map_plane::draw(sf::RenderTarget& target, sf::IntRect rect )
{
    int32_t m_x = wap_plane_get_properties(plane)->movement_x_percent;
    int32_t m_y = wap_plane_get_properties(plane)->movement_y_percent;

    rect.left = (rect.left * m_x) / 100;
    rect.top = (rect.top * m_y) / 100;

    tile.setTexture(&texture);

    int x0 = rect.left/TILE_W;
    int y0 = rect.top/TILE_H;
    int xmax = (rect.left+rect.width)/TILE_W+1;
    int ymax = (rect.top+rect.height)/TILE_H+1;

    int px=0, py=0;

    if(m_x<100)
        px = rect.left%TILE_W;
    else if(m_x>100)
        px = -(rect.left%TILE_W)/4;
    if(m_y<100)
        py = rect.top%TILE_H;
    else if(m_y>100)
        py = -(rect.top%TILE_H)/4;

    for(int y=y0; y<=ymax; y++ )
        for(int x=x0; x<=xmax; x++ )
            if(auto tile_id = getTile(x, y))
            {
                if(tile_id>1000) continue;
                tile.setPosition((x0*TILE_W*100)/m_x+(x-x0)*TILE_W+px, (y0*TILE_H*100)/m_y+(y-y0)*TILE_H+py);
                setTileImage(tile_id);
                target.draw(tile);
            }
}
