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

    vec2 addVec2 (vec2 add) {
        return vec2(this->x + add.x, this->y + add.y);
    }
};

class PointHandler {
public:
    static std::list<vec2> pointList;
    static void addPoint(vec2 vector) {
        pointList.push_back(vector);
    }
    static void addSquare(vec2 centre, int length) {
        if (length % 2 != 0) {
            length += 1;
        }
        addPoint(centre.addVec2(vec2(length, length)));
        addPoint(centre.addVec2(vec2(length, -length)));
        addPoint(centre.addVec2(vec2(-length, -length)));
        addPoint(centre.addVec2(vec2(-length, length)));

    }
    static void addTriangle(vec2 list[3]) {
        for (int i = 0; i < 3; i++) {
            for (int o = 0; o < 3; o++) {
                if (i == o) {
                    continue;
                }

                if (list[i].x == list[o].x && (list[o].y - list[i].y) != 0) {
                    for (int p = 0; p < abs(list[o].y - list[i].y); p++) {
                        addPoint(vec2(list[i].x, p*((list[o].y - list[i].y)/abs(list[o].y - list[i].y))));
                    }
                } else if (list[i].y == list[o].y && (list[o].x - list[i].x) != 0) {
                    for (int p = 0; p < abs(list[o].x - list[i].x); p++) {
                        addPoint(vec2(list[i].y, p*((list[o].x - list[i].x)/abs(list[o].x - list[i].x))));
                    }
                } else {

                    int startX = list[i].x;
                    int startY = list[i].y;
                    int endX = list[o].x;
                    int endY = list[o].y;
                    int diffX = endX-startX;
                    int diffY = endY-startY;
                    if (diffX != 0 && diffY != 0) {
                        int modX = diffX/abs(diffX);
                        int modY = diffY/abs(diffY);
                        //is div/0? We will see :) YAYAYYAYAYAYAYYAY GOLDENN BUZZER DEVIDED BY ZERO PLEAS HELPPPPP
                        int ndiffX = abs(endX-startX);
                        int ndiffY = abs(endY-startY);

                        int hypotenous = sqrt(pow(ndiffX, 2) + pow(ndiffY, 2));
                        if (ndiffX < ndiffY) {
                            int diff = hypotenous/std::clamp(diffX, 1, 100);
                            int times = abs(hypotenous/std::clamp(diff, 1, 100));
                            int currentX = startX;
                            for (int b = 0; b < times; b++) {
                                int currentY = diffY/std::clamp(b, 1, 100);
                                for (int g = 0; g < diff; g++) {
                                    currentX += 1;
                                    addPoint(vec2(currentX, currentY));
                                }
                            }
                        } else {
                            int diff = hypotenous/std::clamp(diffY, 1, 100);
                            int times = abs(hypotenous/std::clamp(diff, 1, 100));
                            int currentX = startX;
                            for (int b = 1; b < times+1; b++) {
                                int currentY = diffY/std::clamp(b, 1, 100);
                                for (int g = 1; g < diff+1; g++) {
                                    currentX += 1;
                                    addPoint(vec2(currentX, currentY));
                                }
                            }
                        }
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

    vec2 temp[3] = {vec2(4, 16), vec2(4, 16), vec2(4, 5)};
    //PointHandler::addTriangle(temp);
    Renderer renderer;

    PointHandler::addSquare(vec2(15, 15), 8);

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