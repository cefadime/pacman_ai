#include "icb_gui.h"

ICBYTES gameBoard, panel;
int FRM1, BTN;
int pacmanX = 4, pacmanY = 4;

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

void ICGUI_Create() {
    ICG_MWTitle("PacmanAI");
    ICG_MWSize(500, 400);
    ICG_SetFont(30, 12, "");
}

// çizilen kýsým
void DrawGame() {
    CreateImage(gameBoard, 300, 300, ICB_UINT);
    FillRect(gameBoard, 0, 0, 300, 300, 0x000000); // Siyah arkaplan

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            int x = j * 30;
            int y = i * 30;
            if (grid[i][j] == 1) {
                FillRect(gameBoard, x, y, 30, 30, 0x808158);
            }
        }
    }

    // Pacmanin çizimi 
    FillCircle(gameBoard, pacmanX * 30 + 15, pacmanY * 30 + 15, 15, 0xffff00);

    DisplayImage(FRM1, gameBoard);
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

        if (nextX >= 0 && nextX < 10 && nextY >= 0 && nextY < 10 && grid[nextY][nextX] == 0) { //sýnýrlarý ve duvarý kontrol eder
            pacmanX = nextX;
            pacmanY = nextY;
        }

        DrawGame();
        Sleep(200);
    }
}

void ICGUI_main() {
    FRM1 = ICG_FrameMedium(20, 20, 300, 300);
    BTN = ICG_TButton(335, 50, 100, 50, "START", PacmanMove, NULL);

    DrawGame();
}

int main() {
    return 0; 
}
