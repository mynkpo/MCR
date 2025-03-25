#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <list>
#include <cmath>
#include <windows.h>
#include <map>
#include <algorithm>

std::string getKeyPressed() {
    for (int key = 8; key <= 255; key++) {
        if (GetAsyncKeyState(key) & 0x8000) {
            char name[128];
            if (GetKeyNameTextA(MapVirtualKeyA(key, MAPVK_VK_TO_VSC) << 16, name, sizeof(name))) {
                return std::string(name);
            }
        }
    }
    return "";
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

struct vec2 {
    int x, y;

    vec2 addVec2(vec2 add) {
        return vec2(this->x + add.x, this->y + add.y);
    }

    bool operator==(const vec2& other) const {
        return x == other.x && y == other.y;
    }
};

struct vec3 {
    int x, y, z;

    vec3 addVec3(vec3 add) {
        return vec3(this->x + add.x, this->y + add.y, this->z + add.z);
    }
};

struct vec3f {
    float x, y, z;

    vec3f addVec3(vec3f add) {
        return vec3f(this->x + add.x, this->y + add.y, this->z + add.z);
    }
};

struct vec3d {
    double x, y, z;

    vec3d addVec3(vec3d add) {
        return vec3d(this->x + add.x, this->y + add.y, this->z + add.z);
    }
};

class GLOBAL_DAT {
public:
    static vec3 CAMPOS;
    static vec3d CAMROT;
};

vec3 GLOBAL_DAT::CAMPOS = {0,0,0};
vec3d GLOBAL_DAT::CAMROT = {0,0,0};

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

std::vector<vec2> retrieveProjectedPoints(std::vector<vec3> list, int distance) {
    std::vector<vec2> final;
    for (vec3 vec : list) {
        final.push_back(vec2((vec.x * distance)/std::clamp(vec.z, 1, 1000),
                            (vec.y * distance)/std::clamp(vec.z, 1, 1000)));
    }
    return final;
}

std::vector<vec3> rotate3DPoints(std::vector<vec3> points2, vec3 rotation) {
    std::vector<vec3> points = points2;
    double radX = rotation.x * M_PI / 180.0;
    double radY = rotation.y * M_PI / 180.0;
    double radZ = rotation.z * M_PI / 180.0;

    for (vec3 &point : points) {
        double y1 = point.y * cos(radX) - point.z * sin(radX);
        double z1 = point.y * sin(radX) + point.z * cos(radX);
        double x2 = point.x * cos(radY) + z1 * sin(radY);
        double z2 = -point.x * sin(radY) + z1 * cos(radY);
        double x3 = x2 * cos(radZ) - y1 * sin(radZ);
        double y3 = x2 * sin(radZ) + y1 * cos(radZ);

        point.x = x3;
        point.y = y3;
        point.z = z2;
    }
    return points;
}

std::vector<vec3> applyCameraTransform(std::vector<vec3> points, vec3 cameraPos, vec3d cameraAngles) {
    std::vector<vec3> final;
    vec3d caN = vec3d(cameraAngles.x * M_PI / 180.0,
                      cameraAngles.y * M_PI / 180.0,
                      cameraAngles.z * M_PI / 180.0);

    for (vec3 vec : points) {
        final.push_back(vec3(
            (vec.x - cameraPos.x)*(cos(caN.y) * cos(caN.z)) + (vec.y-cameraPos.y)*(cos(caN.y)*sin(caN.z)) + (vec.z - cameraPos.z)*(-sin(caN.y)),
            (vec.x - cameraPos.x)*(sin(caN.x)*sin(caN.y)*cos(caN.z) - cos(caN.x)*sin(caN.z)) + (vec.y-cameraPos.y)*(sin(caN.x)*sin(caN.y)*sin(caN.z) + cos(caN.x)*cos(caN.z)) + (vec.z - cameraPos.z)*(sin(caN.x)*cos(caN.y)),
            (vec.x - cameraPos.x)*(cos(caN.x)*sin(caN.y)*cos(caN.z) + sin(caN.x)*sin(caN.z)) + (vec.y-cameraPos.y)*(cos(caN.x)*sin(caN.y)*sin(caN.z) - sin(caN.x)*cos(caN.z)) + (vec.z - cameraPos.z)*(cos(caN.x)*cos(caN.y))
        ));
    }
    return final;
}

void rotatePoints(std::vector<vec2>& points, double angle) {
    double rad = angle * M_PI / 180.0;
    double cosTheta = cos(rad);
    double sinTheta = sin(rad);

    for (auto& p : points) {
        double x_new = p.x * cosTheta - p.y * sinTheta;
        double y_new = p.x * sinTheta + p.y * cosTheta;
        p.x = x_new;
        p.y = y_new;
    }
}

static std::vector<vec2> retrieveConnectionOf2Points(vec2 point1, vec2 point2) {
    if (point1.x == point2.x && point1.y == point2.y) {
        return {point1};
    }
    std::vector<vec2> final;
    int dX = point2.x-point1.x;
    int dY = point2.y-point1.y;
    double d = sqrt(pow(dX, 2) + pow(dY, 2));
    double sX = dX/d;
    double sY = dY/d;

    for (int i = 0; i<static_cast<int>(d); i++) {
        final.push_back(vec2(point1.x + static_cast<int>((sX)*i),
                           point1.y + static_cast<int>((sY)*i)));
    }
    return final;
}

static std::vector<vec3> retrieveConnectionOf2Points3D(vec3 point1, vec3 point2) {
    if (point1.x == point2.x && point1.y == point2.y && point1.z == point2.z) {
        return {point1};
    }
    std::vector<vec3> final;
    int dX = point2.x-point1.x;
    int dY = point2.y-point1.y;
    int dZ = point2.z-point1.z;
    double d = sqrt(pow(dX, 2) + pow(dY, 2) + pow(dZ, 2));
    double sX = dX/d;
    double sY = dY/d;
    double sZ = dZ/d;

    for (int i = 0; i<static_cast<int>(d); i++) {
        final.push_back(vec3(point1.x + static_cast<int>((sX)*i),
                           point1.y + static_cast<int>((sY)*i),
                           point1.z + static_cast<int>((sZ)*i)));
    }
    return final;
}

static std::vector<vec2> connectCubePoints(std::vector<vec2> points) {
    std::vector<vec2> final;

    // Bottom face
    for (vec2 vec : retrieveConnectionOf2Points(points[0], points[1])) final.push_back(vec);
    for (vec2 vec : retrieveConnectionOf2Points(points[1], points[5])) final.push_back(vec);
    for (vec2 vec : retrieveConnectionOf2Points(points[5], points[4])) final.push_back(vec);
    for (vec2 vec : retrieveConnectionOf2Points(points[4], points[0])) final.push_back(vec);

    // Top face
    for (vec2 vec : retrieveConnectionOf2Points(points[3], points[2])) final.push_back(vec);
    for (vec2 vec : retrieveConnectionOf2Points(points[2], points[6])) final.push_back(vec);
    for (vec2 vec : retrieveConnectionOf2Points(points[6], points[7])) final.push_back(vec);
    for (vec2 vec : retrieveConnectionOf2Points(points[7], points[3])) final.push_back(vec);

    // Vertical edges
    for (vec2 vec : retrieveConnectionOf2Points(points[0], points[3])) final.push_back(vec);
    for (vec2 vec : retrieveConnectionOf2Points(points[1], points[2])) final.push_back(vec);
    for (vec2 vec : retrieveConnectionOf2Points(points[5], points[6])) final.push_back(vec);
    for (vec2 vec : retrieveConnectionOf2Points(points[4], points[7])) final.push_back(vec);

    return final;
}

struct renderItem3D {
    std::vector<vec3> pList;
    vec3 rotation;
    vec3 coordinets;
    std::vector<vec2> renderedPoints;

    renderItem3D(std::vector<vec3> list, vec3 rot, vec3 coords) {
        this->pList = list;
        this->coordinets = coords;
        this->rotation = rot;
    }

    std::vector<vec3> recursiveAddition(std::vector<vec3> first, vec3 addition) {
        std::vector<vec3> final;
        for (vec3 vec: first) {
            final.push_back(vec.addVec3(addition));
        }
        return final;
    }

    double distanceReturn() {
        return sqrt((pow((this->coordinets.x - GLOBAL_DAT::CAMPOS.x), 2)) +
                   (pow((this->coordinets.y - GLOBAL_DAT::CAMPOS.y), 2)) +
                   (pow((this->coordinets.z - GLOBAL_DAT::CAMPOS.z), 2)));
    }

    void renderPoints(vec3 camPos, vec3d camRot) {
        auto projected = retrieveProjectedPoints(
            applyCameraTransform(
                rotate3DPoints(
                    recursiveAddition(this->pList, this->coordinets),
                    this->rotation),
                camPos,
                camRot),
            distanceReturn());
        this->renderedPoints = connectCubePoints(projected);
    }
};

struct cubeItem3D {
    vec3 rotation;
    vec3 coordinets;
    std::vector<vec2> renderedPoints;
    std::vector<vec3> listP;
    vec3 bclP, bcrP, bflP, bfrP, tclP, tcrP, tflP, tfrP;

    cubeItem3D(int length, vec3 rot, vec3 coords) {
        bclP = {length + coords.x, length + coords.y, length + coords.z};
        bcrP = {-length + coords.x, length + coords.y, length + coords.z};
        bflP = {-length + coords.x, -length + coords.y, length + coords.z};
        bfrP = {-length + coords.x, -length + coords.y, -length + coords.z};
        tclP = {length + coords.x, -length + coords.y, -length + coords.z};
        tcrP = {length + coords.x, length + coords.y, -length + coords.z};
        tflP = {-length + coords.x, length + coords.y, -length + coords.z};
        tfrP = {length + coords.x, -length + coords.y, length + coords.z};
        this->coordinets = coords;
        this->rotation = rot;
    }

    std::vector<vec3> recursiveAddition(std::vector<vec3> first, vec3 addition) {
        std::vector<vec3> final;
        for (vec3 vec: first) {
            final.push_back(vec.addVec3(addition));
        }
        return final;
    }

    double distanceReturn() {
        return sqrt((pow((this->coordinets.x - GLOBAL_DAT::CAMPOS.x), 2)) +
                   (pow((this->coordinets.y - GLOBAL_DAT::CAMPOS.y), 2)) +
                   (pow((this->coordinets.z - GLOBAL_DAT::CAMPOS.z), 2)));
    }

    void renderPoints(vec3 camPos, vec3d camRot) {
        std::vector<vec3> points = {bclP, bcrP, bflP, bfrP, tclP, tcrP, tflP, tfrP};
        std::vector<vec3> final;

        for (vec3 vec : points) {
            for (vec3 sVec : points) {
                if ((vec.x == sVec.x && vec.y == sVec.y && vec.z != sVec.z) ||
                    (vec.x == sVec.x && vec.z == sVec.z && vec.y != sVec.y) ||
                    (vec.y == sVec.y && vec.z == sVec.z && vec.x != sVec.x)) {
                    std::vector<vec3> temp = retrieveConnectionOf2Points3D(vec, sVec);
                    final.insert(final.end(), temp.begin(), temp.end());
                }
            }
        }

        this->listP = final;
        this->renderedPoints = retrieveProjectedPoints(
            applyCameraTransform(rotate3DPoints(final, this->rotation), camPos, camRot),
            distanceReturn());
    }
};

class PointHandler {
public:
    static std::list<vec2> pointList;
    static std::vector<renderItem*> toRender;
    static std::vector<renderItem3D*> toRender3D;
    static std::vector<cubeItem3D*> toRenderCube3D;

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

    static void addPointList(std::vector<vec2> list) {
        for (vec2 vec : list) {
            addPoint(vec);
        }
    }

    static void update3D(vec3 camPos, vec3d camRot) {
        for (renderItem3D* item : toRender3D) {
            item->renderPoints(camPos, camRot);
            addPointList(item->renderedPoints);
        }
        for (cubeItem3D* item : toRenderCube3D) {
            item->renderPoints(camPos, camRot);
            addPointList(item->renderedPoints);
        }
    }

    static std::vector<vec3> getCubePoints(vec3 centre, int sideLength) {
        int halfSide = sideLength/2;
        return {
            vec3(-halfSide + centre.x, -halfSide + centre.y, -halfSide + centre.z),
            vec3(halfSide + centre.x, -halfSide + centre.y, -halfSide + centre.z),
            vec3(halfSide + centre.x, halfSide + centre.y, -halfSide + centre.z),
            vec3(-halfSide + centre.x, halfSide + centre.y, -halfSide + centre.z),
            vec3(-halfSide + centre.x, -halfSide + centre.y, halfSide + centre.z),
            vec3(halfSide + centre.x, -halfSide + centre.y, halfSide + centre.z),
            vec3(halfSide + centre.x, halfSide + centre.y, halfSide + centre.z),
            vec3(-halfSide + centre.x, halfSide + centre.y, halfSide + centre.z)
        };
    }
};

// Static member definitions
std::list<vec2> PointHandler::pointList;
std::vector<renderItem*> PointHandler::toRender;
std::vector<renderItem3D*> PointHandler::toRender3D;
std::vector<cubeItem3D*> PointHandler::toRenderCube3D;

class Renderer {
public:
    const int width = 210;
    const int height = width * 0.2625;

    void render() {
        for (int h = 0; h < height; h++) {
            char bufferS[width];
            for (int w = 0; w < width; w++) {
                bool dr = std::find(PointHandler::pointList.begin(),
                                  PointHandler::pointList.end(),
                                  vec2(w, h)) != PointHandler::pointList.end();
                bufferS[w] = dr ? ' ' : '#';
            }
            std::cout << bufferS << "\n";
        }
    }
};

int main() {
    Renderer renderer;
    renderItem3D item_3d3 = renderItem3D(PointHandler::getCubePoints(vec3(5,4,0), 4),
                                         vec3(0,0,0),
                                         vec3(0, 0, 0));
    PointHandler::toRender3D.push_back(&item_3d3);

    double rotangle = 0;
    std::string nextMove;

    while (true) {
        if (nextMove == "W") GLOBAL_DAT::CAMPOS.z += 1;
        else if (nextMove == "A") GLOBAL_DAT::CAMPOS.x -= 1;
        else if (nextMove == "S") GLOBAL_DAT::CAMPOS.z -= 1;
        else if (nextMove == "D") GLOBAL_DAT::CAMPOS.x += 1;
        else if (nextMove == "U") GLOBAL_DAT::CAMROT.x += 1;
        else if (nextMove == "H") GLOBAL_DAT::CAMROT.y -= 1;
        else if (nextMove == "J") GLOBAL_DAT::CAMROT.x -= 1;
        else if (nextMove == "K") GLOBAL_DAT::CAMROT.y += 1;
        else if (nextMove == "Z") GLOBAL_DAT::CAMPOS.y -= 1;
        else if (nextMove == "X") GLOBAL_DAT::CAMPOS.y += 1;
        else if (nextMove == "C") GLOBAL_DAT::CAMPOS.z += 1;

        rotangle += 4;
        Sleep(100);

        nextMove = getKeyPressed();
        system("cls");
        PointHandler::update();
        PointHandler::update3D(GLOBAL_DAT::CAMPOS, GLOBAL_DAT::CAMROT);
        renderer.render();
    }
}