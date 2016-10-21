// TODO NEEDS REFACTORING
#include<iostream>
#include<vector>
#include<cmath>

const double G = 6.674e-5;

class vec{
public:
    double x,y;

    vec(double cx = 0,double cy = 0) {
        x = cx;
        y = cy;
    }

    vec(const vec& other) {
        x = other.x;
        y = other.y;
    }

    vec operator+(const vec& other) const{
        return vec(x+other.x,y+other.y);
    }

    vec operator+=(const vec& other) {
        x += other.x;
        y += other.y;
    }

    vec operator-(const vec& other) const{
        return vec(x-other.x,y-other.y);
    }

    vec operator-=(const vec& other) {
        x -= other.x;
        y -= other.y;
    }

    vec operator*(double s) const{
        return vec(x*s,y*s);
    }

    vec operator*=(double s) {
        x *= s;
        y *= s;
    }

    vec operator/(double s) const{
        return vec(x/s,y/s);
    }

    vec operator/=(double s) {
        x /= s;
        y /= s;
    }

    bool operator==(const vec& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const vec& other) const {
        return x != other.x || y != other.y;
    }

    bool operator>(const vec& other) const {
        return x > other.x && y > other.y;
    }

    bool operator>=(const vec& other) const {
        return x >= other.x && y >= other.y;
    }

    bool operator<(const vec& other) const {
        return x < other.x && y < other.y;
    }

    bool operator<=(const vec& other) const {
        return x <= other.x && y <= other.y;
    }

    double dist() const {
        return sqrt(x*x+y*y);
    }

    double dist2() const {
        return x*x+y*y;
    }

    vec normalize() {
        return (*this)/dist();
    }

    void print() const {
        std::cout<<"("<<x<<","<<y<<")";
    }
};

class planet {
public:
    vec pos;
    vec vel;
    double size;
    double mass;

    planet(vec p = vec(0,0),double s = 0, double m = 0) {
        pos = p;
        size = s;
        if(m == 0)  m = s*s*3.14;
        mass = m;
        vel = vec(0,0);
    }

    void addGravity(const planet& other, double time) {
        if( (other.pos-pos).dist2() > other.size + size) {
            vec dir = (other.pos - pos).normalize();
            double scal = mass*other.mass/ (other.pos-pos).dist2() * G;
            vel += dir * scal * time;
        }
    }

    void updateVelocity(double time = 1) {
        pos += vel * time;
    }

    void print() const {
        std::cout<<"planet s = "<<size<<" m = "<<mass<<" pos = ";
        pos.print();
        std::cout<<" vel =";
        vel.print();
    }
};

class quadtree {
public:
    quadtree(std::vector<planet> planets,double sx,double sy,double s) {


        double x = sx+s/2, y = sy+s/2;

        size = s;

        mass = 0;

        leaf = false;

        qu1 = false;
        qu2 = false;
        qu3 = false;
        qu4 = false;



        center = vec(0,0);
        for(int i = 0; i < planets.size(); i ++){
            center += planets[i].pos * planets[i].mass;
            mass += planets[i].mass;
        }
        center /= mass;

        if(planets.size() <= 1){
            leaf = true;
            if(planets.size() == 1){
                pl = planets[0];
                mass = planets[0].mass;
            }
        }else{
            std::vector<planet> quad1,quad2,quad3,quad4;
            for(int i = 0; i < planets.size(); i ++){
                if(planets[i].pos.x <= x && planets[i].pos.y <= y)    quad2.push_back(planets[i]);
                if(planets[i].pos.x > x && planets[i].pos.y <= y)    quad1.push_back(planets[i]);
                if(planets[i].pos.x > x && planets[i].pos.y > y)    quad4.push_back(planets[i]);
                if(planets[i].pos.x <= x && planets[i].pos.y > y)    quad3.push_back(planets[i]);
            }
            if(quad1.size() > 0){
                q1 = new quadtree(quad1,x,sy,size/2);
                qu1 = true;
            }
            if(quad2.size() > 0){
                q2 = new quadtree(quad2,sx,sy,size/2);
                qu2 = true;
            }
            if(quad3.size() > 0){
                q3 = new quadtree(quad3,sx,y,size/2);
                qu3 = true;
            }
            if(quad4.size() > 0){
                q4 = new quadtree(quad4,x,y,size/2);
                qu4 = true;
            }
        }
    }

    ~quadtree(){
        if(qu1) delete q1;
        if(qu2) delete q2;
        if(qu3) delete q3;
        if(qu4) delete q4;
    }

    void print() const {
        if(leaf) {
            pl.print();
        } else {
            std::cout << "(m = " << mass << " q1: ";
            if(qu1) q1->print();
            std::cout << " q2: ";
            if(qu2) q2->print();
            std::cout << " q3: ";
            if(qu3) q3->print();
            std::cout << " q4: ";
            if(qu4) q4->print();
            std::cout <<")";
        }
    }

    quadtree *q1;// upper right
    bool qu1;
    quadtree *q2;// upper left
    bool qu2;
    quadtree *q3;// bottom left
    bool qu3;
    quadtree *q4;// bottom right
    bool qu4;

    planet pl;
    bool leaf;
    double mass;
    double size;
    vec center;
};

