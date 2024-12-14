#include "icb_gui.h"

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

int food[10][10];
void InitializeFood() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (grid[i][j] == 0) {
                food[i][j] = 1; // tum bosluklara yem koyuldu
            }
            else {
                food[i][j] = 0; 
            }
        }
    }
}


void ICGUI_Create() {
    ICG_MWTitle("PacmanAI");
    ICG_MWSize(500, 400);
    ICG_SetFont(30, 12, "");
}

// �izilen k�s�m
void DrawGame() {
    CreateImage(gameBoard, 300, 300, ICB_UINT);
    FillRect(gameBoard, 0, 0, 300, 300, 0x000000);

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            int x = j * 30;
            int y = i * 30;
            if (grid[i][j] == 1) {
                FillRect(gameBoard, x, y, 30, 30, 0x8B4513);
            }
        }
    }
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (food[i][j] == 1) {
                int x = j * 30 + 15;
                int y = i * 30 + 15;
                FillCircle(gameBoard, x, y, 3, 0x00ff00); //yemler
            }
        }
    }

    // Pacmanin cizimi 
    FillCircle(gameBoard, pacmanX * 30 + 15, pacmanY * 30 + 15, 9, 0xffff00);
    FillCircle(gameBoard, ghostX * 30 + 15, ghostY * 30 + 15, 7, 0xff0000); //hayalet


    DisplayImage(FRM1, gameBoard);
}
void GhostMove() {

    //ai ile hareket edecek
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

        if (nextX >= 0 && nextX < 10 && nextY >= 0 && nextY < 10 && grid[nextY][nextX] == 0) { //sinirlari ve duvari kontrol eder
            pacmanX = nextX;
            pacmanY = nextY;
        }
        if (food[pacmanY][pacmanX] == 1) {
            food[pacmanY][pacmanX] = 0; // Yem yenilmiştir
        }
        GhostMove();
        DrawGame();
        if (pacmanX == ghostX && pacmanY == ghostY) {
            //oyun bitti mesajı 
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
