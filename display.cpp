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

std::fstream load_file;
std::string message = "https://github.com/laspinko/n-body-problem-cpp";

std::vector<int> frames;
std::vector<planet> planets;

void init(std::string);
void loadFiles(std::string);
void readFromFile(int i);
void draw(int i);
void close();

void readFromFile(int i) {
    load_file.seekg(frames[i]);
    int pl;
    binary_read(load_file,pl);
    planets.clear();
    planets.reserve(pl);
    for(int j = 0; j < pl; j ++){
        planet temp;
        temp.loadFrom(load_file);
        planets.push_back(temp);
    }
}

void init(std::string file_name) {
    SDL_Init( SDL_INIT_VIDEO);
    window = SDL_CreateWindow("n body", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    loadFiles(file_name);
}

void loadFiles(std::string file_name){
    std::fstream h_load_file;

    load_file.open(file_name + ".nbp", std::ios_base::in | std::ios_base::binary);
    h_load_file.open(file_name + ".hnbp", std::ios_base::in | std::ios_base::binary);

    h_load_file.seekp(message.size());
    int num_states;
    binary_read(h_load_file, num_states);
    frames.reserve(num_states);
    for(int i = 0; i < num_states; i ++){
        int temp;
        binary_read(h_load_file, temp);
        frames.push_back(temp);
    }

    h_load_file.close();
}

void close() {
    load_file.close();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void draw(int i) {
    readFromFile(i);

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
        draw(time%frames.size());
        //SDL_Delay(10);
        time ++;
    }
    close();
}
