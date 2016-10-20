#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <random>
#include <ctime>
#include<SDL2/SDL.h>
#include "bitmap_image.hpp"
#include "barners-hut.hpp"

using namespace std;

const int width = 500;
const int height = 500;
const int saveInterval = 100;
void draw(vector<planet> pl, string id) {
    bitmap_image output(width,height);
    output.set_all_channels(0,0,0);
    image_drawer draw(output);

    draw.pen_color(255,255,255);

    for(int i = 0;i < pl.size();i ++) {
		if(pl[i].pos > vec(0,0) && pl[i].pos < vec(width, height)) {
			output.set_pixel((int)pl[i].pos.x, (int)pl[i].pos.y, 255, 255, 255);
		}
    }

    string name = "output/frame" + id + ".bmp";
    output.save_image(name);
}

void gravity(planet &obj, quadtree* tree) {
    if((tree->leaf && !(tree->pl.pos == obj.pos) ) || tree->size / (obj.pos - tree->center).dist() < 0.5) {
        obj.addGravity(planet(tree->center,tree->size,tree->mass));
        return;
    } else {
        if(tree->qu1) gravity(obj, tree->q1);
        if(tree->qu2) gravity(obj, tree->q2);
        if(tree->qu3) gravity(obj, tree->q3);
        if(tree->qu4) gravity(obj, tree->q4);
        return;
    }
}

int ( int argc, char* args[] ){
    srand(time(0));

    SDL_Window* window = NULL;

    SDL_Surface* screenSurface = NULL;

    if(SDL_Init( SDL_INIT_VIDEO) < 0)   cerr << "SDL could not initialize!";

    vector<planet> planets;

    for(int i = 0;i < 100;i ++) {
        planets.push_back(planet(vec(rand()%width, rand()%height), 1, 3.14));
    }

    for(int step = 0;step <10000;step ++) {
        quadtree q(planets,0,0,width);
        for(int i = 0; i < planets.size(); i++) {
            gravity(planets[i],&q);
        }
        for(int i = 0; i < planets.size(); i++) {
            planets[i].updateVelocity();

            if(planets[i].pos.x < 0 || planets[i].pos.x > width) {
                planets[i].vel.x *= -1;
            }
            if(planets[i].pos.y < 0 || planets[i].pos.y >height) {
                planets[i].vel.y *= -1;
            }
        }

        if(step % saveInterval == 0) {
			string name(10, '0');
			int cp = step;
			for(int len = 0;cp > 0;len ++) {
				name[10 - len] = '0' + cp % 10;
				cp /= 10;
			}
            draw(planets, name);
            cout << name <<endl;
        }
    }
}
