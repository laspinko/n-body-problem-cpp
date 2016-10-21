#include <iostream>
#include <vector>
#include <fstream>
#include <SDL2/SDL.h>
#include "barners-hut.hpp"

const double width = 10000;
const double height = 10000;
const int window_width = 500;
const int window_height = 500;

double scale = std::max( window_width / width, window_height / height);
vec translate = vec(0, 0);

SDL_Window *window = NULL;

SDL_Renderer *renderer = NULL;

std::ifstream load_file;

std::vector<std::vector<planet> > planets_states;

void read_from_file() {
    int states;
    std::string message = "https://github.com/laspinko/n-body-problem-cpp ";
    load_file.seekg(message.size(),std::ios_base::beg);
    binary_read(load_file,states);
    for(int i = 0; i < states; i ++) {
        int pl;
        binary_read(load_file,pl);
        std::vector<planet> planets;
        for(int j = 0; j < pl; j ++){
            planet temp;
            temp.loadFrom(load_file);
            planets.push_back(temp);
        }
        planets_states.push_back(planets);
    }
    std::cout << "Loaded "<< planets_states.size() << " states" << std::endl;
}

void init(std::string file_name = "simulation.nbp") {
    SDL_Init( SDL_INIT_VIDEO);
    window = SDL_CreateWindow("n body", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    load_file.open(file_name, std::ios_base::in | std::ios_base::binary);
}

void close() {
    load_file.close();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void draw(std::vector<planet> planets) {
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


int main(int argv, char** args){
    std::string file_name;
    std::cout << "Load file name: ";
    std::cin >> file_name;

    init(file_name);
    read_from_file();


    std::cout << "Press:" << std::endl << "Z and X to zoom in and out" << std::endl << "WASD to navigate";

    bool quit = false;


    SDL_Event e;

    int time = 0;

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
        draw(planets_states[time%planets_states.size()]);

        time ++;
    }
    close();
}
