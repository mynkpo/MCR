#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono> // std::chrono::microseconds
#include <thread> // std::this_thread::sleep_for
#include <list>
#include <cmath>


struct vec2 {
    int x;
    int y;
};

class PointHandler {
public:
    static std::list<vec2> pointList;
    static void addPoint(vec2 vector) {
        pointList.push_back(vector);
    }
    static void addTriangle(vec2 list[3]) {
        for (int i = 0; i < 3; i++) {
            for (int o; o < 2; o++) {

                if (list[i].x == list[o].x && (list[o].y - list[i].y) != 0) {
                    for (int p = 0; p < abs(list[o].y - list[i].y); p++) {
                        addPoint(vec2(list[i].x, p*((list[o].y - list[i].y)/abs(list[o].y - list[i].y))));
                    }
                }
                if (list[i].y == list[o].y && (list[o].x - list[i].x) != 0) {
                    for (int p = 0; p < abs(list[o].x - list[i].x); p++) {
                        addPoint(vec2(list[i].y, p*((list[o].x - list[i].x)/abs(list[o].x - list[i].x))));
                    }
                }

            }
        }
    }
};
std::list<vec2> PointHandler::pointList;
//PointHandler::pointList = {};
class Renderer {

    public:

    const int width = 100;
    const int height = width * 0.2625;

    void render() {
        for (int h = 0; h < height; h++) {
            char bufferS[width];
            for (int w = 0; w < width; w++) {
                bool dr = false;
                PointHandler handler;
                for (vec2 vec : PointHandler::pointList) {
                    if (h == vec.y && w == vec.x) {
                        dr = true;
                        break;
                    }
                }
                if (dr) {
                    bufferS[w] = *"-";
                } else {
                    bufferS[w] = *"#";
                }
            }
            std::cout << bufferS << "\n";
        }
    }

};





int main() {

    vec2 temp[3] = {vec2(4, 3), vec2(4, 2), vec2(12, 5)};
    PointHandler::addTriangle(temp);
    Renderer renderer;


    while (true) {
        for (int i = 0; i < renderer.height*5; i++) {
            std::cout << "\n";
        }
        renderer.render();
        using namespace std::chrono_literals;
        const auto start = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(500ms);
        const auto end = std::chrono::high_resolution_clock::now();

    }


}