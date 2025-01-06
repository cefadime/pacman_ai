#include "icb_gui.h"
#include <cstdlib>
#include <ctime>
#include <queue>
#include <vector>
#include <tuple>
#include <cstdio>

ICBYTES gameBoard, panel, score;
int FRM1, BTN, MLE;
int pacmanX = 4, pacmanY = 4;
int ghostX = 1, ghostY = 1;
void ScoreFunct();

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

struct Node {
    int x, y, cost;
    bool operator>(const Node& other) const {
        return cost > other.cost;
    }
};

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

// Uniform Cost Search: Hayaletin en kısa yolu bulması için
std::vector<std::pair<int, int>> UniformCostSearch(int startX, int startY, int targetX, int targetY) {
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
    bool visited[10][10] = { false };
    std::pair<int, int> parent[10][10];

    pq.push({ startX, startY, 0 });
    parent[startX][startY] = { -1, -1 };

    int directions[4][2] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };

    while (!pq.empty()) {
        Node current = pq.top();
        pq.pop();

        int cx = current.x, cy = current.y;

        if (visited[cx][cy]) continue;
        visited[cx][cy] = true;

        // Hedefe ulaşıldı mı?
        if (cx == targetX && cy == targetY) {
            std::vector<std::pair<int, int>> path;
            while (cx != -1 && cy != -1) {
                path.push_back({ cx, cy });
                std::tie(cx, cy) = parent[cx][cy];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        // 4 yöne hareket
        for (auto& dir : directions) {
            int nx = cx + dir[0];
            int ny = cy + dir[1];

            // Sınır kontrolü ve geçilebilirlik
            if (nx >= 0 && nx < 10 && ny >= 0 && ny < 10 && grid[nx][ny] == 0 && !visited[nx][ny]) {
                pq.push({ nx, ny, current.cost + 1 });
                parent[nx][ny] = { cx, cy };
            }
        }
    }

    return {}; // Hedefe ulaşılmazsa boş bir yol döner
}

// Hayaletin Hareketi
void GhostMove() {
    auto path = UniformCostSearch(ghostX, ghostY, pacmanX, pacmanY);
    if (!path.empty() && path.size() > 1) {
        ghostX = path[1].first;
        ghostY = path[1].second;
    }
}

// Oyun alanının çizimi
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

    // Pacman ve hayalet
    FillCircle(gameBoard, pacmanX * 30 + 15, pacmanY * 30 + 15, 9, 0xffff00);
    FillCircle(gameBoard, ghostX * 30 + 15, ghostY * 30 + 15, 7, 0xff0000);

    DisplayImage(FRM1, gameBoard);
}

// Pacman'in Hareketi
void PacmanMove(void* lpParam) {
    while (1) {
        int key = ICG_LastKeyPressed();
        int nextX = pacmanX;
        int nextY = pacmanY;

        if (key == 37) nextX--;
        else if (key == 39) nextX++;
        else if (key == 38) nextY--;
        else if (key == 40) nextY++;

        if (nextX >= 0 && nextX < 10 && nextY >= 0 && nextY < 10 && grid[nextY][nextX] == 0) {
            pacmanX = nextX;
            pacmanY = nextY;
            score -= 1; // Her adımda -1 puan ekler

        }
        if (food[pacmanY][pacmanX]) {
            food[pacmanY][pacmanX] = false;
            score += 5; // Yem yendiğinde 5 puan ekler

        }

        GhostMove();
        DrawGame();

        if (pacmanX == ghostX && pacmanY == ghostY) {
            while (1) {
                Print(MLE, score); // Puanı yazdırmak için ama bu da çalışmadı
                // Kullanıcı kapatana kadar bekler
                Sleep(200);
            }
        }

        Sleep(200);
    }
}

void ICGUI_Create() {
    ICG_MWTitle("PacmanAI");
    ICG_MWSize(500, 400);
    ICG_SetFont(30, 12, "");
}

void ICGUI_main() {
    InitializeFood();
    FRM1 = ICG_FrameMedium(20, 20, 300, 300);
    BTN = ICG_TButton(335, 50, 100, 50, "START", PacmanMove, NULL);
    ICG_Static(335, 110, 100, 55, "SCORE");
    MLE = ICG_MLEditSunken(335, 150, 100, 50, "", SCROLLBAR_V);
    ICG_printf(MLE, "%s\n", score);

    //Burda skor mle içine yazılacak ama şuan çalışmıyor.

    DrawGame();
}

int main() {
    return 0;
}
