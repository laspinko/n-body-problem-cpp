#include<iostream>
#include<vector>
#include<cmath>
#include<sstream>
#include<random>
#include<ctime>
#include<SDL2/SDL>
#include"bitmap_image.hpp"
#include"barners-hut.hpp"



using namespace std;

int width = 500,height = 500;

void draw(vector<planet> pl, string id) {
    bitmap_image output(width,height);
    output.set_all_channels(0,0,0);
    image_drawer draw(output);

    draw.pen_color(255,255,255);
    //8draw.pen_width(1);

    for(int i = 0; i < pl.size(); i++) {
        if(pl[i].pos > vec(0,0) && pl[i].pos < vec(width, height) )
                output.set_pixel((int)pl[i].pos.x, (int)pl[i].pos.y, 255, 255, 255);
    }

    string name = "bmp outputs/output"+id+".bmp";
    output.save_image(name);
}

void gravity(planet &obj, quadtree* tree) {
    if((tree->leaf && !(tree->pl.pos == obj.pos) ) || tree->size / (obj.pos - tree->center).dist() < 0.5) {
        obj.addGravity(planet(tree->center,tree->size,tree->mass));
        return ;
    }else{
        if(tree->qu1)    gravity(obj, tree->q1);
        if(tree->qu2)    gravity(obj, tree->q2);
        if(tree->qu3)    gravity(obj, tree->q3);
        if(tree->qu4)    gravity(obj, tree->q4);
        return ;
    }
}

int main(){
    srand(time(0));

    vector<planet> planets;

    for(int i = 0 ;i < 100; i++) {
        planets.push_back(planet(vec(rand()%width, rand()%height), 1, 3.14));
    }



    for(int _ = 0; _ <10000; _++){
        quadtree q(planets,0,0,width);
        for(int i = 0; i < planets.size(); i++){
            gravity(planets[i],&q);
        }
        for(int i = 0; i < planets.size(); i++){
            planets[i].updateVelocity();

            if(planets[i].pos.x < 0 || planets[i].pos.x > width) {
                planets[i].vel.x *= -1;
            }
            if(planets[i].pos.y < 0 || planets[i].pos.y >height) {
                planets[i].vel.y *= -1;
            }
        }

        if(_%100 == 0){
            stringstream ss;
            ss << _/100;
            string num = ss.str();
            draw(planets,num );
            cout<<_/100<<endl;
        }
    }
}
