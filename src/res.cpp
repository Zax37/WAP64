#include "res.h"
#include <fstream>
#include <assert.h>
#include <dirent.h>
#include "common.h"

using namespace std;

wwd_map::wwd_map(){
    wwd = wap_wwd_create();
}

bool wwd_map::load(const char* filename){

    if(wap_wwd_open(wwd, filename)) return false;
    //assert(wap_wwd_get_properties(wwd)->flags & WAP_WWD_FLAG_COMPRESS);

    auto map_properties = wap_wwd_get_properties(wwd);

    base_level = getFirstNumber(map_properties->level_name);

    if(!base_level) return false;

    string path = getLevelDir();
    path += "\\PALETTES\\MAIN.PAL";
    ifstream pal;
    pal.open(path.c_str(),ios::binary|ios::in);
    char c[3];
    for(int i=0; i<256; i++)
    {
        pal.read(c, 3);
        palette[i] = sf::Color(c[0], c[1], c[2], 255);
    }

    spawn_x = map_properties->start_x;
    spawn_y = map_properties->start_y;

    for(uint32_t i=0; i<wap_wwd_get_plane_count(wwd); i++)
    {
        auto plane = wap_wwd_get_plane(wwd, i);
        planes.emplace_back(this, plane);

        if(wap_plane_get_properties(plane)->flags & WAP_PLANE_FLAG_MAIN_PLANE)
            main_plane = plane;
        //auto obj_c = wap_plane_get_object_count(plane);
        //cout << i << ": " << wap_plane_get_properties(plane)->name << "(" << wap_plane_get_image_set(plane, 0) << ")" << endl;
    }
    for(auto it = planes.begin(); it!=planes.end(); it++ )
    {
        it->texture.loadFromImage(it->tileset->texture);
        it->tile_sprite.setTexture(it->texture);
        if(it->plane_w*it->plane_h<4000)
            it->preRender();
        else{
            for(uint32_t y = 0; y < it->plane_h; y++)
                for(uint32_t x = 0; x < it->plane_w; x++)
                    it->tiles.emplace_back(wap_plane_get_tile(it->plane, x, y), it->wwd_map_ptr);
        }
    }

}

wwd_map_tile::wwd_map_tile(uint32_t v, wwd_map* ptr)
{
    value = v;
    properties = wap_wwd_get_tile_description(ptr->wwd, v);
}

wwd_map::~wwd_map(){
    wap_wwd_free(wwd);
}

const char* wwd_map::getLevelName(){ return wap_wwd_get_properties(wwd)->level_name; }

const char* wwd_map::getLevelDir()
{
    string path = "DATA\\LEVEL";
    path += to_string(base_level);
    return path.c_str();
}

wwd_resource* wwd_map::loadResource(const char* p, const char* as)
{
    string path = p;

    DIR * assets = opendir(p);
    struct dirent *d;
    if(assets)
    {
        while((d = readdir(assets)) != NULL)
            if (strcmp(d->d_name, ".") && strcmp(d->d_name, ".."))
            {
                DIR * dir = opendir((path+d->d_name).c_str());
                if (dir != NULL)
                {
                   closedir(dir);
                   string npath = path+d->d_name+'\\';
                   string nas = as;
                   nas += '_';
                   nas += d->d_name;
                   loadResource(npath.c_str(), nas.c_str());
                   continue;
                }

                unsigned short id = getFirstNumber(d->d_name);
                sf::Image img;
                assert(img.loadFromFile(path+d->d_name));
                resources[as].set(id, img);
            }
    }
    return &resources[as];
}

void wwd_map_plane::setTileImage(unsigned short id){
    tile_sprite.setTextureRect((*tileset)[id]);
}

uint32_t wwd_map_plane::getTile(uint32_t x, uint32_t y)
{
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
    //tile.setSize(sf::Vector2f(TILE_W, TILE_H));
    fill_color = wwd_map_ptr->palette[wap_plane_get_properties(plane)->fill_color];

    const char * imgset = wap_plane_get_image_set(plane, 0);
    tileset = wwd_map_ptr->getResource(imgset);
    if(!tileset)
    {
        //cerr << "not loaded" << endl;
        string path = wwd_map_ptr->getLevelDir();
        path += "\\TILES\\";
        path += imgset;
        path += '\\';
        tileset = wwd_map_ptr->loadResource( path.c_str(), imgset );
    }
    wap_plane_get_map_dimensions(plane,&plane_w,&plane_h);
}

void wwd_map_plane::preRender()
{
    preRendered = true;
    sf::RenderTexture preRendTex;
    preRendTex.create(plane_w*TILE_W, plane_h*TILE_H);
    for(uint32_t y=0; y<plane_h; y++ )
        for(uint32_t x=0; x<plane_w; x++ )
            if(auto tile_id = getTile(x, y))
            {
                if(tile_id == TILE_EMPTY) continue;
                if(tile_id == TILE_FILL)
                {
                    sf::RectangleShape t_fill(sf::Vector2f(TILE_W, TILE_H));
                    t_fill.setPosition((x*TILE_W), (y*TILE_H));
                    t_fill.setFillColor(fill_color);
                    preRendTex.draw(t_fill);
                }
                else
                {
                    tile_sprite.setPosition((x*TILE_W), (y*TILE_H));
                    setTileImage(tile_id);
                    preRendTex.draw(tile_sprite);
                }
            }
    preRendTex.display();
    cached_tex = preRendTex.getTexture();
    cached_tex.setRepeated(true);
}

void wwd_map::draw(sf::RenderTarget& target, sf::IntRect rect)
{
    for(auto p = planes.begin(); p!=planes.end(); p++)
    {
        p->draw( target, rect, p->plane == main_plane);
    }
}

void wwd_map_plane::draw(sf::RenderTarget& target, sf::IntRect rect, bool rects )
{

    int32_t m_x = wap_plane_get_properties(plane)->movement_x_percent;
    int32_t m_y = wap_plane_get_properties(plane)->movement_y_percent;

    //if(m_x==150) m_x = 50;
    //if(m_y==125) m_y = 125;

    float percent_x = m_x * 0.01;
    float percent_y = m_y * 0.01;

    if(preRendered)
    {
        sf::Sprite prPlane;
        prPlane.setTexture(cached_tex);
        int hw = rect.width/2, hh = rect.height/2;
        int px = (rect.left+hw)*percent_x, py = (rect.top+hh)*percent_y;
        prPlane.setOrigin(-hw, -hh);
        prPlane.setTextureRect(sf::IntRect(px%(plane_w*TILE_W)-hw, py%(plane_h*TILE_H)-hh, rect.width, rect.height));
        prPlane.setPosition(rect.left-hw, rect.top-hh);
        target.draw(prPlane);
        return;
    }

    rect.left *= percent_x;
    rect.top *= percent_y;

    int x0 = rect.left/TILE_W;
    int y0 = rect.top/TILE_H;
    int xmax = (rect.left+rect.width)/TILE_W+1/percent_x;
    int ymax = (rect.top+rect.height)/TILE_H+1/percent_y;

    //int nx = (rect.width)/TILE_W;
    //int ny = (rect.height)/TILE_H;

    int px=0, py=0;

    /*if(m_x<100)
    {
        if(m_x==75)
            px = (rect.left%TILE_W)/3;
        else
            px = rect.left%TILE_W;
    }
    else if(m_x>100)
    {
        if(m_x==150)
            px = -(rect.left%TILE_W)/3;
        else
            px = -(rect.left%TILE_W)*percent_x;
    }
    if(m_y<100)
    {
        if(m_y==75)
            py = rect.top%TILE_H/3;
        else
            py = rect.top%TILE_H;
    }
    else if(m_y>100)
    {
        if(m_y==125)
            py = -(rect.top%TILE_H)/5;
        else
            py = -(rect.top%TILE_H)/3;
    }*/

    for(int y=y0; y<=ymax; y++ )
        for(int x=x0; x<=xmax; x++ )
            if(auto tile_id = getTile(x, y))
            {
                if(tile_id == TILE_EMPTY) continue;
                tile_sprite.setPosition((x0*TILE_W)/percent_x+(x-x0)*TILE_W+px, (y0*TILE_H)/percent_y+(y-y0)*TILE_H+py);
                if(tile_id == TILE_FILL)
                {
                    tile_sprite.setTextureRect(sf::IntRect(0, 0, TILE_W, TILE_H));
                    //tile.setFillColor(fill_color);
                    target.draw(tile_sprite);
                    //tile.setFillColor(sf::Color::White);
                }
                else
                {
                    setTileImage(tile_id);
                    target.draw(tile_sprite);
                }
                wap_tile_description * desc = wap_wwd_get_tile_description(wwd_map_ptr->wwd, tile_id);
                if(desc->type==WAP_TILE_TYPE_SINGLE)
                {
                    if(auto type = desc->inside_attrib)
                    {
                        sf::RectangleShape inside(sf::Vector2f(desc->width, desc->height));
                        switch(type)
                        {
                            case 1: inside.setFillColor(sf::Color(0, 0, 255, 125)); break;
                            case 2: inside.setFillColor(sf::Color(0, 255, 0, 125)); break;
                            case 3: inside.setFillColor(sf::Color(255, 242, 0, 125)); break;
                            case 4: inside.setFillColor(sf::Color(255, 0, 0, 125)); break;
                        }
                        inside.setPosition(tile_sprite.getPosition());
                        target.draw(inside);
                    }
                }
                else if(desc->type==WAP_TILE_TYPE_DOUBLE)
                {
                    auto r = desc->rect;
                    r.right += 1;
                    r.bottom += 1;
                    if(auto type = desc->inside_attrib)
                    {
                        sf::RectangleShape inside(sf::Vector2f(r.right-r.left, r.bottom-r.top));
                        switch(type)
                        {
                            case 1: inside.setFillColor(sf::Color(0, 0, 255, 125)); break;
                            case 2: inside.setFillColor(sf::Color(0, 255, 0, 125)); break;
                            case 3: inside.setFillColor(sf::Color(255, 242, 0, 125)); break;
                            case 4: inside.setFillColor(sf::Color(255, 0, 0, 125)); break;
                        }
                        inside.setPosition(tile_sprite.getPosition().x+r.left, tile_sprite.getPosition().y+r.top);
                        target.draw(inside);
                    }
                    if(auto type = desc->outside_attrib)
                    {
                        sf::RectangleShape outside(sf::Vector2f(r.left, desc->height));
                        switch(type)
                        {
                            case 1: outside.setFillColor(sf::Color(0, 0, 255, 125)); break;
                            case 2: outside.setFillColor(sf::Color(0, 255, 0, 125)); break;
                            case 3: outside.setFillColor(sf::Color(255, 242, 0, 125)); break;
                            case 4: outside.setFillColor(sf::Color(255, 0, 0, 125)); break;
                        }
                        outside.setPosition(tile_sprite.getPosition());
                        target.draw(outside);
                        outside.setSize(sf::Vector2f(desc->width-r.left, r.top));
                        outside.setPosition(tile_sprite.getPosition().x+r.left, tile_sprite.getPosition().y);
                        target.draw(outside);
                        outside.setSize(sf::Vector2f(desc->width-r.left, desc->height-r.bottom));
                        outside.setPosition(tile_sprite.getPosition().x+r.left, tile_sprite.getPosition().y+r.bottom);
                        target.draw(outside);
                        outside.setSize(sf::Vector2f(desc->width-r.right, r.bottom-r.top));
                        outside.setPosition(tile_sprite.getPosition().x+r.right, tile_sprite.getPosition().y+r.top);
                        target.draw(outside);
                    }
                }

            }

    /*vector<wwd_map_tile>::iterator it = tiles.begin()+y0*plane_w+x0;
    for(int y=0; y<ny; y++ )
    {
        for(int x=0; x<nx; x++ )
        {
            auto tile_id = it->value;
            if(tile_id == TILE_EMPTY) continue;
            tile_sprite.setPosition((x0*TILE_W)/percent_x+x*TILE_W, (y0*TILE_H)/percent_y+y*TILE_H);
            if(tile_id == TILE_FILL)
            {
                tile_sprite.setTextureRect(sf::IntRect(0, 0, TILE_W, TILE_H));
                //tile.setFillColor(fill_color);
                target.draw(tile_sprite);
                //tile.setFillColor(sf::Color::White);
            }
            else
            {
                setTileImage(tile_id);
                target.draw(tile_sprite);
            }
            it++;
        }
        it += plane_w - nx;
    }*/
}

sf::IntRect wwd_resource::operator[](unsigned short id)
{
    return imageset[id];
}

sf::IntRect wwd_resource::get(unsigned short id)
{
    return imageset[id];
}

void wwd_resource::set(unsigned short id, sf::Image &img)
{
    sf::IntRect place = imageset[id];
    if(!place.width)
    {
        place = sf::IntRect( texture.getSize().x, 0, img.getSize().x, img.getSize().y );
        sf::Image cp = texture;
        texture.create(texture.getSize().x+img.getSize().x, max(texture.getSize().y,img.getSize().y), sf::Color::Transparent );
        texture.copy(cp, 0, 0);
        imageset[id] = place;
    }
    texture.copy(img, place.left, place.top);
}

wwd_resource* wwd_map::getResource(const char* name)
{
    auto it = resources.find(name);
    if (it != resources.end())
        return &it->second;
    return 0;
}
