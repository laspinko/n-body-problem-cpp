#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <random>
#include <ctime>
#include <fstream>
#include <SDL2/SDL.h>
#include "bitmap_image.hpp"
#include "barners-hut.hpp"




const double width = 10000;
const double height = 10000;
const int window_width = 500;
const int window_height = 500;


double timePerMove = 1;


double scale = std::max( window_width / width, window_height / height);
vec translate = vec(0, 0);

int stepsPerCall = 1;

SDL_Window *window = NULL;

SDL_Renderer *renderer = NULL;

std::ofstream save_file;
std::streampos start_of_save_file;
int saved_states;

std::ifstream load_file;

std::vector<planet> planets;

void gravity(planet &obj, quadtree* tree);
void draw();
void update();
void init();
void close();
void add_to_file();


void add_to_file() {
    save_file.seekp(start_of_save_file);
    saved_states ++;
    save_file.write(reinterpret_cast<char*>(&saved_states),sizeof(int));
    save_file.seekp(0,std::ios::end);
    int size = planets.size();
    save_file.write(reinterpret_cast<char*>(&size),sizeof(int));
    for(int i = 0 ; i < size; i ++) {
        planets[i].saveTo(save_file);
    }
}
void read_from_file() {
    int states;
    std::string message = "https://github.com/laspinko/n-body-problem-cpp ";
    load_file.seekg(message.size(),std::ios_base::beg);
    load_file.read(reinterpret_cast<char*>(&states),sizeof(int));
    std::cout<<states;
    for(int i = 0; i < states; i ++) {
        int pl;
        load_file.read(reinterpret_cast<char*>(&pl),sizeof(int));
        for(int j = 0; j < pl; j ++){
            planet temp;
            temp.loadFrom(load_file);
            planets.push_back(temp);
        }
    }
}

void init() {
    SDL_Init( SDL_INIT_VIDEO);
    window = SDL_CreateWindow("n body", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    /*save_file.open("simulation.nbp", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    std::string message = "https://github.com/laspinko/n-body-problem-cpp ";
    save_file.write(message.c_str(), message.length());
    start_of_save_file = save_file.tellp();
    saved_states = 0;
    save_file.write(reinterpret_cast<char*>(&saved_states),sizeof(int));*/

    load_file.open("simulation.nbp", std::ios_base::in | std::ios_base::binary);
}

void close() {
    load_file.close();

    save_file.close();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void draw() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear( renderer );

    double pixel_map[window_width][window_height] = {0.0};

    double occupacy = std::min(1 / (scale * scale), 32.0);

    for(int i = 0; i < planets.size(); i ++) {
        vec pos = (planets[i].pos + translate) * scale + vec(window_width / 2.0, window_height / 2.0);
        if(pos > vec(0,0) && pos < vec(window_width, window_height) )   pixel_map[(int)(pos.x)][(int)(pos.y)] += planets[i].mass / occupacy;
    }

    for(int x = 0; x < window_width; x ++){
        for(int y = 0; y < window_width; y ++){
            if(pixel_map[x][y] != 0) {
                int c = std::min((int)(pixel_map[x][y] * 255), 255);
                SDL_SetRenderDrawColor(renderer, c, c, c, c);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void update() {
    quadtree q(planets,-width ,- height ,width * 2);
    for(int i = 0; i < planets.size(); i++) {
        gravity(planets[i],&q);
    }
    for(int i = 0; i < planets.size(); i++) {
        planets[i].updateVelocity(timePerMove);

        if(planets[i].pos.x < -width || planets[i].pos.x > width) {
            planets[i].vel.x *= -1;
        }
        if(planets[i].pos.y < -height || planets[i].pos.y > height) {
            planets[i].vel.y *= -1;
        }
    }
}

planet createPlanet(vec c, double r){
    double ang = (double)rand() / RAND_MAX * M_PI * 2;
    double mag = (double)rand() / RAND_MAX * r;

    vec pos(cos(ang) * mag, sin(ang) * mag);
    pos += c;


    planet p(pos, 1);

    /*p.vel.x = -sin(ang) * G;
    p.vel.y = cos(ang) * G;*/

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
        obj.addGravity(planet(tree->center,tree->size,tree->mass), timePerMove);
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

    /*for(int i = 0;i < 5;i ++) {
        planets.push_back(createPlanet(vec(0,0), 500));
    }*/

    init();

    //add_to_file();
    read_from_file();

    std::cout << "Press:" << std::endl << "Z and X to zoom in and out" << std::endl << "WASD to navigate" << std::endl << "F and N for fast and normal speed";

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
                case SDLK_f:
                    timePerMove = 10;
                    stepsPerCall = 10;
                    break;
                case SDLK_n:
                    timePerMove = 1;
                    stepsPerCall = 1;
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
