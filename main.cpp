#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <ctime>
#include <fstream>
#include "barners-hut.hpp"




const double width = 10000;
const double height = 10000;

double timePerMove = 1;

std::fstream save_file;
std::fstream h_save_file;
int saved_states;
std::string message = "https://github.com/laspinko/n-body-problem-cpp";

std::vector<planet> planets;

void gravity(planet &obj, quadtree* tree);
void update();
void addToFile();

void loadFromFile(std::string file_name){
    std::fstream h_load_file;

    h_load_file.open(file_name + ".hnbp", std::ios_base::in | std::ios_base::binary);

    h_load_file.seekp(message.size());
    binary_read(h_load_file, saved_states);
    int last;
    h_load_file.seekg((saved_states - 1) * sizeof(int), std::ios_base::cur);
    binary_read(h_load_file, last);

    h_load_file.close();

    std::fstream load_file;
    load_file.open(file_name + ".nbp", std::ios_base::in | std::ios_base::binary);


    load_file.seekg(last);
    int pl;
    binary_read(load_file,pl);
    planets.clear();
    planets.reserve(pl);
    for(int j = 0; j < pl; j ++){
        planet temp;
        temp.loadFrom(load_file);
        planets.push_back(temp);
    }

    load_file.close();
}

void addToFile() {
    int start_loc = save_file.tellp();

    int size = planets.size();
    binary_write(save_file,size);
    for(int i = 0 ; i < size; i ++) {
        planets[i].saveTo(save_file);
    }


    saved_states ++;
    h_save_file.seekp(message.size());
    binary_write(h_save_file, saved_states);
    h_save_file.seekp(0,std::ios_base::end);
    binary_write(h_save_file, start_loc);

    std::cout << "Step " << saved_states << std::endl;
}

void update() {

    double minx = width, miny = height, maxx = -width, maxy = - height;

    for(int i = 0; i < planets.size(); i++) {
        if(minx > planets[i].pos.x) minx = planets[i].pos.x;
        if(maxx < planets[i].pos.x) maxx = planets[i].pos.x;
        if(miny > planets[i].pos.y) miny = planets[i].pos.y;
        if(maxy < planets[i].pos.y) maxy = planets[i].pos.y;
    }

    //std::cout << minx << "-" << maxx << "," << miny << "-" << maxy << std::endl;
    quadtree q(planets,minx ,miny , maxx - minx, maxy - miny);
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
    double ang = (double)rand() / RAND_MAX * 3.14 * 2;
    double mag = (double)rand() / RAND_MAX * r;

    vec pos(cos(ang) * mag, sin(ang) * mag);
    pos += c;


    planet p(pos, 1);

    /*p.vel.x = -sin(ang) * G;
    p.vel.y = cos(ang) * G;*/

    return p;
}

void gravity(planet &obj, quadtree* tree) {
    if((tree->leaf && !(tree->pl.pos == obj.pos) ) || std::max(tree->width, tree->height) / (obj.pos - tree->center).dist() < 0.5) {
        obj.addGravity(planet(tree->center, 0,tree->mass), timePerMove);
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
    std::string file_name;
    std::cout << "Save file name: ";
    std::cin >> file_name;

    char cont;
    do{
        std::cout << "Continue save file (y/n): ";
        std::cin >> cont;
    }while(cont!='y' && cont!='n');

    if(cont == 'n'){
        int pl_count;
        std::cout << "Planet count: ";
        std::cin >> pl_count;

        int sp_range;
        std::cout << "Spawn range: ";
        std::cin >> sp_range;

        for(int i = 0;i < pl_count;i ++) {
            planets.push_back(createPlanet(vec(0,0), sp_range));
        }
        save_file.open(file_name + ".nbp", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
        save_file.write(message.c_str(), message.length());

        h_save_file.open(file_name + ".hnbp", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
        h_save_file.write(message.c_str(), message.length());

        saved_states = 0;

    }else{
        loadFromFile(file_name);
        save_file.open(file_name + ".nbp",std::ios_base::in | std::ios_base::out | std::ios_base::ate | std::ios_base::binary);
        save_file.seekp(0, std::ios_base::end);

        h_save_file.open(file_name + ".hnbp", std::ios_base::in | std::ios_base::out | std::ios_base::ate | std::ios_base::binary);
        h_save_file.seekp(0, std::ios_base::end);

        std::cout << "Starting from " << saved_states << std::endl;
    }

    int steps_left = 1;

    do{
        std::cout << "Run the simulation for (0 to terminate): ";
        std::cin >> steps_left;
        for(int i = 0; i < steps_left; i++){
            update();
            addToFile();
        }
    }while(steps_left > 0);

    h_save_file.close();
    save_file.close();
}
