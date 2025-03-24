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
            for (int o; o < 3; o++) {
                int heightDif = list[o].y - list[i].y;
                int lengthDif = list[o].x - list[i].x;
                double divisor = lengthDif/heightDif;
                int almostFinal = divisor;
                int final = ceil(divisor);
                int currentY = list[i].y;
                int currentX = list[i].x;
                for (int p = 0; p < (int)lengthDif/almostFinal; p++) {
                    for (int p2 = 0; p2 < almostFinal; p2++) {
                        currentX += lengthDif/abs(lengthDif);
                        pointList.push_back(vec2(currentX, currentY));
                    }
                    currentY += heightDif/abs(heightDif);
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

    vec2 temp[3] = {vec2(4, 3), vec2(7, 5), vec2(12, 6)};
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