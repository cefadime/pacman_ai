#include "icb_gui.h"
#include <cstdlib>
#include <ctime>
#include <queue>
#include <vector>
#include <tuple>
#include <cmath>
#include <fstream> // Loglama işlemi için gerekli


ICBYTES gameBoard, panel;
int score = 0;
int FRM1, BTN, MLE;
int pacmanX = 4, pacmanY = 4;
int ghostX = 1, ghostY = 1;
bool gameOver = false;

std::ofstream logFile("game_log.txt");


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

void LogMove(const std::string& entity, int x, int y, int scoreChange = 0) {
    if (logFile.is_open()) {
        logFile << entity << " moved to (" << x << ", " << y << ")";
        if (scoreChange != 0) {
            logFile << ", Score Change: " << scoreChange;
        }
        logFile << std::endl;
    }
}

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

    return {}; // Eğer yol bulunamazsa
}

// Weighted A* Algoritması
std::vector<Node> WeightedAStar(int startX, int startY, int targetX, int targetY, int ghostX, int ghostY, float w1, float w2) {
    std::priority_queue<Node> openList;
    bool closedList[10][10] = { false };

    Node start = { startX, startY, 0, 0, 0, 0, nullptr };
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

            // Mevcut pozisyonu hariç tut
            if (nx == startX && ny == startY) {
                continue; // Aynı pozisyona geri dönme
            }

            if (nx >= 0 && ny >= 0 && nx < 10 && ny < 10 && grid[ny][nx] == 0 && !closedList[nx][ny]) {
                int g = current.g + 1;
                int h_food = CalculateHCost(nx, ny, targetX, targetY);
                int h_ghost = CalculateHCost(nx, ny, ghostX, ghostY);
                int f = g + w1 * h_food - w2 * h_ghost;

                Node neighbor = { nx, ny, g, h_food, f, 0, new Node(current) };
                openList.push(neighbor);
            }
        }
    }

    return {};
}

// Pacman'ın Hareketi
void PacmanMove(void* lpParam) {
    float w1 = 1.5; // Yemlere öncelik
    float w2 = 3.0; // Hayaletten uzaklaşmaya öncelik

    std::vector<std::pair<int, int>> recentPositions; // Son pozisyonları izlemek için vektör
    int maxRecentPositions = 5; // İzlenecek maksimum pozisyon sayısı

    while (!gameOver) {
        int closestFoodX = -1, closestFoodY = -1, minDistance = INT_MAX;

        // En yakın yemi bul
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

        if (closestFoodX == -1 || closestFoodY == -1) break; // Yem kalmadıysa bitir

        // Weighted A* ile yeni yol hesapla
        auto path = WeightedAStar(pacmanX, pacmanY, closestFoodX, closestFoodY, ghostX, ghostY, w1, w2);

        for (auto& step : path) {
            // Eğer bu pozisyon son pozisyonlarda varsa atla
            if (std::find(recentPositions.begin(), recentPositions.end(), std::make_pair(step.x, step.y)) != recentPositions.end()) {
                continue;
            }

            pacmanX = step.x;
            pacmanY = step.y;

            // Yeni pozisyonu ekle
            recentPositions.push_back({ pacmanX, pacmanY });
            if (recentPositions.size() > maxRecentPositions) {
                recentPositions.erase(recentPositions.begin()); // Eski pozisyonları sil
            }

            int scoreChange = -1;
            score += scoreChange;
            LogMove("Pacman", pacmanX, pacmanY, scoreChange);

            if (food[pacmanY][pacmanX]) {
                food[pacmanY][pacmanX] = false;
                score += 5;
                LogMove("Pacman", pacmanX, pacmanY, 5);
            }

            GhostMove(); // Hayalet hareket etsin
            DrawGame();  // İkisini de son konumlarında çiz

            // Çarpışma kontrolü
            if (pacmanX == ghostX && pacmanY == ghostY) {
                DrawGame(); // Çarpışma anını ekranda göster
                gameOver = true;
                ICG_printf(MLE, "GAME OVER! SCORE: %d", score);
                logFile << "GAME OVER! Final Score: " << score << std::endl;
                return;
            }
            Sleep(200); // Hareketler arasındaki gecikme artırıldı

        }
    }
}

// Hayaletin Hareketi
void GhostMove() {
    auto path = AStarSearch(ghostX, ghostY, pacmanX, pacmanY);
    if (!path.empty() && path.size() > 1) {
        ghostX = path[1].x;
        ghostY = path[1].y;
        LogMove("Ghost", ghostX, ghostY);
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
    InitializeGhost();

    FRM1 = ICG_FrameMedium(20, 20, 300, 300);
    BTN = ICG_TButton(335, 50, 100, 50, "START", PacmanMove, NULL);
    ICG_Static(335, 110, 100, 55, "SCORE");
    MLE = ICG_MLEditSunken(335, 150, 100, 50, "", SCROLLBAR_V);
    ICG_printf(MLE, "%d", score);

    DrawGame();

    // Log dosyasını kapatırız
    atexit([] {
        if (logFile.is_open()) {
            logFile.close();
        }
        });

}
