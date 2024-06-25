#include"iostream"
#include"vector"
#include"thread"
#include"mutex"
#include"chrono"
#include <random>

using namespace std;
const int WIDTH = 1000;
const int HEIGHT = 1000;
const double V = 1.0;

struct Entity {

    double x, y;
    double targetx, targety;
    bool moving;

    Entity(double x, double y, double tx, double ty) :x(x), y(y), targetx(tx), targety(ty) {

    }

    void move() {
        if (moving) {
            double dx = targetx - x;
            double dy = targety - y;
            double distance = sqrt(dx * dx + dy * dy);
            if (distance < V) {
                x = targetx;
                y = targety;
                moving = false;
            }
            else {
                x += V * dx / distance;
                y += V * dy / distance;
            }
        }
    }
};

vector<Entity>entities;
mutex mtx;


void simulateMovement(Entity& entity) {
    while (entity.moving) {
        {
            lock_guard<mutex>lock(mtx);
            entity.move();
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

double randomDouble(double min, double max) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_real_distribution<> dis(min, max);
    return dis(gen);
}


void createEntities(bool isLegalEntity, int count) {
    for (int i = 0; i < count; ++i) {
        double startX, startY, targetX, targetY;
        if (isLegalEntity) {
            startX = randomDouble(0, WIDTH / 2);
            startY = randomDouble(0, HEIGHT / 2);
            targetX = randomDouble(0, WIDTH / 2);
            targetY = randomDouble(0, HEIGHT / 2);
        }
        else {
            startX = randomDouble(WIDTH / 2, WIDTH);
            startY = randomDouble(HEIGHT / 2, HEIGHT);
            targetX = randomDouble(WIDTH / 2, WIDTH);
            targetY = randomDouble(HEIGHT / 2, HEIGHT);
        }
        entities.emplace_back(startX, startY, targetX, targetY);
    }
}

int main() {
    setlocale(LC_CTYPE, "ukr");
    int legalEntityCount = 10;
    int physicalEntityCount = 10;

    createEntities(true, legalEntityCount);
    createEntities(false, physicalEntityCount);

    vector<thread> threads;
    for (auto& entity : entities) {
        threads.emplace_back(simulateMovement, ref(entity));
    }

    for (auto& t : threads) {
        t.join();
    }

    cout << "The simulation is complete. Coordinates of legal entities and physical persons:\n";
    for (const auto& entity : entities) {
        cout << "Entity at (" << entity.x << ", " << entity.y << ")\n";
    }
}
