#include "icb_gui.h"
#include <cstdlib> // random sayı için gerekli
#include <ctime>   //random sayıların değişkenliği için

ICBYTES gameBoard, panel;
int FRM1, BTN;
int pacmanX = 4, pacmanY = 4;
int ghostX = 1, ghostY = 1;

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

void InitializeFood() {
    srand(static_cast<unsigned>(time(0))); //yemlerin rastgele yerlerde bulunması için klendi

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (grid[i][j] == 0) { // Sadece boşluklara yem yerleştir
                food[i][j] = (rand() % 2 == 0); // Yem var mı yok mu rastgele belirle %50 olasılık var
            }
            else {
                food[i][j] = false;
            }
        }
    }
}

void ICGUI_Create() {
    ICG_MWTitle("PacmanAI");
    ICG_MWSize(500, 400);
    ICG_SetFont(30, 12, "");
}

void DrawGame() {
    CreateImage(gameBoard, 300, 300, ICB_UINT);
    FillRect(gameBoard, 0, 0, 300, 300, 0x000000);

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            int x = j * 30;
            int y = i * 30;
            if (grid[i][j] == 1) {
                FillRect(gameBoard, x, y, 30, 30, 0x8B4513); // Duvarlar
            }
        }
    }
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (food[i][j]) {
                int x = j * 30 + 15;
                int y = i * 30 + 15;
                FillCircle(gameBoard, x, y, 3, 0x00ff00); // Yemlerin çizimi
            }
        }
    }

    //pacmanin çizimi
    FillCircle(gameBoard, pacmanX * 30 + 15, pacmanY * 30 + 15, 9, 0xffff00);

    //hayaletin çizimi
    FillCircle(gameBoard, ghostX * 30 + 15, ghostY * 30 + 15, 7, 0xff0000);

    DisplayImage(FRM1, gameBoard);
}

void GhostMove() {
    // AI ile hayaletin hareketi buraya gelecek
}

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
        }
        if (food[pacmanY][pacmanX]) {
            food[pacmanY][pacmanX] = false; // Yem yenildiğinde false yapılır
        }

        GhostMove();
        DrawGame();

        if (pacmanX == ghostX && pacmanY == ghostY) {
            // Oyun bitti mesajı ekle bitir
            exit(0);
        }

        Sleep(200);
    }
}

void ICGUI_main() {
    InitializeFood();
    FRM1 = ICG_FrameMedium(20, 20, 300, 300);
    BTN = ICG_TButton(335, 50, 100, 50, "START", PacmanMove, NULL);
    DrawGame();
}

int main() {
    return 0;
}
