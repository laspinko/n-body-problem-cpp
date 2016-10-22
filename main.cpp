#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <ctime>
#include <fstream>
#include "bitmap_image.hpp"
#include "barners-hut.hpp"




const double width = 10000;
const double height = 10000;

double timePerMove = 1;

std::ofstream save_file;
std::streampos start_of_save_file;
int saved_states;

std::vector<planet> planets;

void gravity(planet &obj, quadtree* tree);
void update();
void add_to_file();


void add_to_file() {
    saved_states ++;
    std::cout << saved_states << std::endl;
    save_file.seekp(start_of_save_file);
    binary_write(save_file,saved_states);
    save_file.seekp(0,std::ios::end);
    int size = planets.size();
    binary_write(save_file,size);
    for(int i = 0 ; i < size; i ++) {
        planets[i].saveTo(save_file);
    }
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
    double ang = (double)rand() / RAND_MAX * 3.14 * 2;
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
    std::string file_name;
    std::cout << "Save file name: ";
    std::cin >> file_name;

    int pl_count;
    std::cout << "Planet count: ";
    std::cin >> pl_count;

    int sp_range;
    std::cout << "Spawn range: ";
    std::cin >> sp_range;

    save_file.open(file_name, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
    std::string message = "https://github.com/laspinko/n-body-problem-cpp ";
    save_file.write(message.c_str(), message.length());
    start_of_save_file = save_file.tellp();
    saved_states = 0;



    for(int i = 0;i < pl_count;i ++) {
        planets.push_back(createPlanet(vec(0,0), sp_range));
    }


    while(true){
        add_to_file();
        update();
    }

    save_file.close();
}
