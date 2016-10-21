#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <random>
#include <ctime>
#include<SDL2/SDL.h>
#include "bitmap_image.hpp"
#include "barners-hut.hpp"




const double width = 10000;
const double height = 10000;
const int window_width = 500;
const int window_height = 500;


double scale = std::max( window_width / width, window_height / height);
vec translate = vec(0, 0);

int stepsPerCall = 10;

SDL_Window *window = NULL;

SDL_Renderer *renderer = NULL;

std::vector<planet> planets;

void gravity(planet &obj, quadtree* tree);
void draw();
void update();
void init();
void close();

void init() {
    SDL_Init( SDL_INIT_VIDEO);
    window = SDL_CreateWindow("n body", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void close() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void draw() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear( renderer );
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

    for(int i = 0; i < planets.size(); i ++) {
        vec pos = (planets[i].pos + translate) * scale + vec(window_width / 2.0, window_height / 2.0);
        double size = planets[i].size;
        SDL_Rect fillRect = {pos.x - size, pos.y - size, 2 * size, 2 * size};
        SDL_RenderFillRect(renderer, &fillRect);
    }
    SDL_RenderPresent(renderer);
}

void update() {
    quadtree q(planets,-width ,- height ,width * 2);
    for(int i = 0; i < planets.size(); i++) {
        gravity(planets[i],&q);
    }
    for(int i = 0; i < planets.size(); i++) {
        planets[i].updateVelocity();

        if(planets[i].pos.x < -width || planets[i].pos.x > width) {
            planets[i].vel.x *= -1;
        }
        if(planets[i].pos.y < -height || planets[i].pos.y > height) {
            planets[i].vel.y *= -1;
        }
    }
    //std::cout << stepsPerCall << std::endl;
}

planet createPlanet(vec c, double r){
    double ang = (double)rand() / RAND_MAX * M_PI * 2;
    double mag = (double)rand() / RAND_MAX * r;

    vec pos(cos(ang) * mag, sin(ang) * mag);
    pos += c;


    planet p(pos, 1);

    //p.vel = vec(-sin(ang),cos(ang));
    //p.vel *= sqrt(pl.mass*G/mag);

    return p;
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

    for(int i = 0;i < 100;i ++) {
        planets.push_back(createPlanet(vec(0,0), 500));
    }

    init();

    std::cout << "Press:" << std::endl << "Z and X to zoom in and out" << std::endl << "WASD to navigate";

    bool quit = false;


    SDL_Event e;
    while(!quit){

        while(SDL_PollEvent(&e) !=0) {
            if(e.type == SDL_QUIT) {
                quit = true;
            }
            if(e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_z:
                    scale /= 0.9;
                    break;
                case SDLK_x:
                    scale *= 0.9;
                    break;
                case SDLK_w:
                    translate.y += 10 / scale;
                    break;
                case SDLK_a:
                    translate.x += 10 / scale;
                    break;
                case SDLK_s:
                    translate.y -= 10 / scale;
                    break;
                case SDLK_d:
                    translate.x -= 10 / scale;
                    break;

                }
            }
        }

        for(int i = 0; i < stepsPerCall; i ++) {
            update();
        }

        draw();
    }

    close();
}
