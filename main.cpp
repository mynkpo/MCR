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

struct renderItem {
    std::vector<vec2> pList;
    double rotation;
    vec2 coordinets;
    renderItem(std::vector<vec2> list, double rot, vec2 coords) {
        this->pList = list;
        this->coordinets = coords;
        this->rotation = rot;
    }
    void updateRot(double rot) {
        this->rotation = rot;
    }
    void updatePos(vec2 pos) {
        this->coordinets = pos;
    }
};

void rotatePoints(std::vector<vec2>& points, double angle) {
    double rad = angle * M_PI / 180.0; // Convert degrees to radians
    double cosTheta = cos(rad);
    double sinTheta = sin(rad);

    for (auto& p : points) {
        double x_new = p.x * cosTheta - p.y * sinTheta;
        double y_new = p.x * sinTheta + p.y * cosTheta;

        p.x = x_new;
        p.y = y_new;
    }
}
class PointHandler {
public:
    static std::list<vec2> pointList;
    static std::vector<renderItem*> toRender;
    static void addPoint(vec2 vector) {
        pointList.push_back(vector);
    }
    static void update() {
        pointList.clear();
        for (renderItem* item : toRender) {


            std::vector<vec2> tempList = item->pList;
            rotatePoints(tempList, item->rotation);

            for (vec2 vec : tempList) {
                addPoint(vec.addVec2(item->coordinets));
            }
        }
    }
    static void addTriangle(vec2 list[4]) {
        for (int i = 0; i < 4; i++) {
            for (int o = 0; o < 4; o++) {
                if (list[i].x == list[o].x && list[o].y != list[i].y) {
                    for (int p = 0; p < abs(list[o].y - list[i].y); p++) {
                        addPoint(vec2(list[i].x, list[i].y + p*((list[o].y - list[i].y)/abs(list[o].y - list[i].y))));
                    }
                } else if (list[i].y == list[o].y && list[o].x != list[i].x) {
                    for (int p = 0; p < abs(list[o].x - list[i].x); p++) {
                        addPoint(vec2(list[i].x +p*((list[o].x - list[i].x)/abs(list[o].x - list[i].x)), list[i].y));
                    }
                } else {
                    if (i == o) {
                        continue;
                    }
                    int heightDif = list[o].y - list[i].y;
                    int lengthDif = list[o].x - list[i].x;
                    double divisor = lengthDif/std::clamp(heightDif, 1, 100);
                    int almostFinal = divisor;
                    int final = ceil(divisor);
                    int currentY = list[i].y;
                    int currentX = list[i].x;
                    for (int p = 0; p < (int)abs(lengthDif/std::clamp(almostFinal, 1, 100)); p++) {
                        for (int p2 = 0; p2 < almostFinal; p2++) {
                            currentX += lengthDif/std::clamp(abs(lengthDif), 1, 100);
                            pointList.push_back(vec2(currentX, currentY));
                        }
                        currentY += heightDif/std::clamp(abs(heightDif), 1, 100);
                    }
                }

            }
        }
    }
    static void addSquare(vec2 centre, int length, double rotation) {
        if (length % 2 != 0) {
            length += 1;
        }
        std::vector<vec2>points = {vec2(length, length), vec2(-length, length), vec2(-length, -length), vec2(length, -length)};

        rotatePoints(points, rotation);

        for (vec2 vec : points) {
            addPoint(centre.addVec2(vec));
        }
        //addPoint(centre.addVec2(vec2(-length, length)));

    }

    static std::vector<vec2> getSquarePoints(vec2 centre, int length, double rotation) {
        if (length % 2 != 0) {
            length += 1;
        }
        std::vector<vec2> points = {vec2((int)length/2, (int)length/2), vec2(-(int)length/2, (int)length/2), vec2(-(int)length/2, -(int)length/2), vec2((int)length/2, -(int)length/2)};
        rotatePoints(points, rotation);
        return points;
        //addPoint(centre.addVec2(vec2(-length, length)));

    }

    static std::vector<vec2> getSquarePoints(vec2 centre, int length) {
        if (length % 2 != 0) {
            length += 1;
        }
        std::vector<vec2> points = {vec2((int)length/2, (int)length/2), vec2(-(int)length/2, (int)length/2), vec2(-(int)length/2, -(int)length/2), vec2((int)length/2, -(int)length/2)};
        return points;
        //addPoint(centre.addVec2(vec2(-length, length)));

    }
};
std::list<vec2> PointHandler::pointList;
std::vector<renderItem*> PointHandler::toRender;

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
                    bufferS[w] = *"🟩";
                }
            }
            std::cout << bufferS << "\n";
        }
    }

};





int main() {

    //vec2 temp[4] = {vec2(4, 20), vec2(20, 20), vec2(4, 4), vec2(20, 4)};
    //PointHandler::addTriangle(temp);
    //PointHandler::addSquare(vec2(15, 15), 4, 96);
    Renderer renderer;
    renderItem item = renderItem(PointHandler::getSquarePoints(vec2(15, 15), 10), 0, vec2(15, 15));
    PointHandler::toRender.push_back(&item);
    double rotangle = 0;
    while (true) {

        // "Clearing" the console
        for (int i = 0; i < renderer.height*5; i++) {
            std::cout << "\n";
        }

        //transformations
        rotangle += 10;
        item.rotation = rotangle;
        item.coordinets = vec2(25, 13);


        //PointHandler::pointList.clear();
        //PointHandler::addSquare(vec2(20, 15), 8, rotangle);



        //render
        PointHandler::update();
        renderer.render();



        using namespace std::chrono_literals;
        const auto start = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(100ms);
        const auto end = std::chrono::high_resolution_clock::now();

    }


}