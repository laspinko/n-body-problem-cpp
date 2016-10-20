#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <random>
#include <ctime>
#include<SDL2/SDL.h>
#include "bitmap_image.hpp"
#include "barners-hut.hpp"




const int width = 500;
const int height = 500;
int stepsPerCall = 1;

SDL_Window *window = NULL;

SDL_Renderer *renderer = NULL;

std::vector<planet> planets;

void gravity(planet &obj, quadtree* tree);
void draw();
void update();

void draw() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear( renderer );
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    for(int i = 0; i < planets.size(); i ++) {
        SDL_Rect fillRect = {planets[i].pos.x - planets[i].size, planets[i].pos.y - planets[i].size, 2 * planets[i].size, 2 * planets[i].size};
        SDL_RenderFillRect(renderer, &fillRect);
    }
    SDL_RenderPresent(renderer);
}

void update() {
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
}

void draw_bmp(std::vector<planet> pl, std::string id) {
    bitmap_image output(width,height);
    output.set_all_channels(0,0,0);
    image_drawer draw(output);

    draw.pen_color(255,255,255);

    for(int i = 0;i < pl.size();i ++) {
		if(pl[i].pos > vec(0,0) && pl[i].pos < vec(width, height)) {
			output.set_pixel((int)pl[i].pos.x, (int)pl[i].pos.y, 255, 255, 255);
		}
    }

    std::string name = "output/frame" + id + ".bmp";
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

int main(int argv, char** args){
    srand(time(0));

    for(int i = 0;i < 300;i ++) {
        planets.push_back(planet(vec(rand()%width, rand()%height), 1));
    }

    SDL_Init( SDL_INIT_VIDEO);
    window = SDL_CreateWindow("n body", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool quit = false;


    SDL_Event e;
    while(!quit){

        if(SDL_PollEvent(&e) !=0) {
            if(e.type == SDL_QUIT) {
                quit = true;
            }
        }

        for(int i = 0; i < stepsPerCall; i ++) {
            update();
        }

        draw();
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}
