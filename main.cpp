#include<iostream>
#include<vector>
using namespace std;

class vec{
public:
    vec(double cx = 0,double cy = 0) {
        x = cx;
        y = cy;
    }
    vec(const vec& other) {
        x = other.x;
        y = other.y;
    }

    vec operator+(const vec& other) {
        return vec(x+other.x,y+other.y);
    }
    vec operator+=(const vec& other) {
        x += other.x;
        y += other.y;
    }
    vec operator-(const vec& other) {
        return vec(x-other.x,y-other.y);
    }
    vec operator-=(const vec& other) {
        x -= other.x;
        y -= other.y;
    }
    vec operator*(double s) {
        return vec(x*s,y*s);
    }
    vec operator*=(double s) {
        x *= s;
        y *= s;
    }
    vec operator/(double s) {
        return vec(x/s,y/s);
    }
    vec operator/=(double s) {
        x /= s;
        y /= s;
    }

    double* getX(){
        return &x;
    }
    double* getY(){
        return &y;
    }

    void print() const {
        cout<<"("<<x<<","<<y<<")";
    }


    double x,y;
protected:
private:
};

class planet {
public:
    planet(vec p = vec(0,0),double m = 0) {
        pos = p;
        mass = m;
    }

    void print() const{
        cout<<"planet m = "<<mass<<" pos = ";
        pos.print();
    }
    vec pos;
    double mass;
protected:
private:
};

class quadtree {
public:
    quadtree(vector<planet> planets,double sx,double sy,double s) {
        x = sx+s/2;
        y = sy+s/2;

        size = s;

        mass = 0;

        leaf = false;

        qu1 = false;
        qu2 = false;
        qu3 = false;
        qu4 = false;


        if(planets.size() <= 1){
            leaf = true;
            if(planets.size() == 1){
                pl = planets[0];
                mass = planets[0].mass;
            }
        }else{
            vector<planet> quad1,quad2,quad3,quad4;
            for(int i = 0; i < planets.size(); i ++){
                if(planets[i].pos.x <= x && planets[i].pos.y <= y)    quad2.push_back(planets[i]);
                if(planets[i].pos.x > x && planets[i].pos.y <= y)    quad1.push_back(planets[i]);
                if(planets[i].pos.x > x && planets[i].pos.y > y)    quad4.push_back(planets[i]);
                if(planets[i].pos.x <= x && planets[i].pos.y > y)    quad3.push_back(planets[i]);
                mass += planets[i].mass;
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

    void print() const{
        if(leaf){
            pl.print();
        }else{
            cout<<"(m = "<<mass<<" q1: ";
            if(qu1) q1->print();
            cout<<" q2: ";
            if(qu2) q2->print();
            cout<<" q3: ";
            if(qu3) q3->print();
            cout<<" q4: ";
            if(qu4) q4->print();
            cout<<")";
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
    double x,y;
protected:
private:
};
int main(){
    vector<planet> planets;
    planets.push_back(planet(vec(100,10),1));
    planets.push_back(planet(vec(13,65),1));
    planets.push_back(planet(vec(12,41),1));
    planets.push_back(planet(vec(75,65),1));
    planets.push_back(planet(vec(1,99),1));

    quadtree q(planets,0,0,100);

    q.print();
}
