#include "icb_gui.h"
#include <cstdlib>
#include <ctime>
#include <queue>
#include <vector>
#include <tuple>
#include <cmath>

ICBYTES gameBoard, panel;
int score = 0;
int FRM1, BTN, MLE;
int pacmanX = 4, pacmanY = 4;
int ghostX = 1, ghostY = 1;
bool gameOver = false;

int grid[10][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 1, 1, 1, 1, 1, 0, 0, 0},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 1, 0, 0, 1, 1, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
};

bool food[10][10];

void GhostMove();
void DrawGame();
void ICGUI_Create();
void ICGUI_main();

// Node yapısı
struct Node {
    int x, y, g, h, f, cost;
    Node* parent;
    bool operator<(const Node& other) const {
        return f > other.f;
    }
};

// Yem ve Çarpışma Fonksiyonları
void InitializeFood() {
    srand(static_cast<unsigned>(time(0)));
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (grid[i][j] == 0) {
                food[i][j] = (rand() % 2 == 0);
            }
            else {
                food[i][j] = false;
            }
        }
    }
}
void InitializeGhost() {
    srand(static_cast<unsigned>(time(0))); // Rastgelelik için seed
    while (true) {
        ghostX = rand() % 10;
        ghostY = rand() % 10; 

        if (grid[ghostY][ghostX] == 0 && !(ghostX == pacmanX && ghostY == pacmanY)) {
            break; // Eğer geçerli bir pozisyonsa döngüden çık
        }
    }
    ICG_printf(MLE, "Ghost initialized at (%d, %d)", ghostX, ghostY); // Debug için
}

// A* Algoritması ile Yemlere Ulaşım
int CalculateHCost(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

std::vector<Node> AStarSearch(int startX, int startY, int targetX, int targetY) {
    std::priority_queue<Node> openList;
    bool closedList[10][10] = { false };

    Node start = { startX, startY, 0, CalculateHCost(startX, startY, targetX, targetY), 0, 0, nullptr };
    start.f = start.g + start.h;
    openList.push(start);

    int directions[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };

    while (!openList.empty()) {
        Node current = openList.top();
        openList.pop();

        if (current.x == targetX && current.y == targetY) {
            std::vector<Node> path;
            Node* pathNode = &current;

            while (pathNode) {
                path.push_back(*pathNode);
                pathNode = pathNode->parent;
            }

            std::reverse(path.begin(), path.end());
            return path;
        }

        closedList[current.x][current.y] = true;

        for (auto& dir : directions) {
            int nx = current.x + dir[0];
            int ny = current.y + dir[1];

            if (nx >= 0 && ny >= 0 && nx < 10 && ny < 10 && grid[ny][nx] == 0 && !closedList[nx][ny]) {
                Node neighbor = { nx, ny, current.g + 1, CalculateHCost(nx, ny, targetX, targetY), 0, current.g + 1, new Node(current) };
                neighbor.f = neighbor.g + neighbor.h;
                openList.push(neighbor);
            }
        }
    }

    return {};
}

// Pacman'ın A* ile Hareketi
void PacmanMove(void* lpParam) {
    while (!gameOver) {
        int closestFoodX = -1, closestFoodY = -1, minDistance = INT_MAX;

        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (food[i][j]) {
                    int distance = CalculateHCost(pacmanX, pacmanY, j, i);
                    if (distance < minDistance) {
                        minDistance = distance;
                        closestFoodX = j;
                        closestFoodY = i;
                    }
                }
            }
        }

        if (closestFoodX == -1 || closestFoodY == -1) break;

        auto path = AStarSearch(pacmanX, pacmanY, closestFoodX, closestFoodY);

        for (auto& step : path) {
            pacmanX = step.x;
            pacmanY = step.y;

            if (food[pacmanY][pacmanX]) {
                food[pacmanY][pacmanX] = false;
                score += 5;
            }

            if (pacmanX == ghostX && pacmanY == ghostY) {
                gameOver = true;
                ICG_printf(MLE, "GAME OVER! SCORE: %d", score);
                return;
            }

            GhostMove();
            DrawGame();
            Sleep(200);
        }
    }
}

void GhostMove() {
    auto path = AStarSearch(ghostX, ghostY, pacmanX, pacmanY);
    if (!path.empty() && path.size() > 1) {
        ghostX = path[1].x;
        ghostY = path[1].y;
    }
}

void DrawGame() {
    CreateImage(gameBoard, 300, 300, ICB_UINT);
    FillRect(gameBoard, 0, 0, 300, 300, 0x000000);

    // Duvarlar
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            int x = j * 30;
            int y = i * 30;
            if (grid[i][j] == 1) {
                FillRect(gameBoard, x, y, 30, 30, 0x8B4513);
            }
        }
    }

    // Yemler
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (food[i][j]) {
                int x = j * 30 + 15;
                int y = i * 30 + 15;
                FillCircle(gameBoard, x, y, 3, 0x00ff00);
            }
        }
    }

    FillCircle(gameBoard, pacmanX * 30 + 15, pacmanY * 30 + 15, 9, 0xffff00);
    FillCircle(gameBoard, ghostX * 30 + 15, ghostY * 30 + 15, 7, 0xff0000);

    DisplayImage(FRM1, gameBoard);
}

void ICGUI_Create() {
    ICG_MWTitle("PacmanAI");
    ICG_MWSize(500, 400);
    ICG_SetFont(30, 12, "");
}

void ICGUI_main() {
    InitializeFood();
    InitializeGhost(); //hayaleti rastgele yerde başlat

    FRM1 = ICG_FrameMedium(20, 20, 300, 300);
    BTN = ICG_TButton(335, 50, 100, 50, "START", PacmanMove, NULL);
    ICG_Static(335, 110, 100, 55, "SCORE");
    MLE = ICG_MLEditSunken(335, 150, 100, 50, "", SCROLLBAR_V);
    ICG_printf(MLE, "%d", score); //başlangıçta 0 yazar

    DrawGame();
}
