#include <iostream>
#include <Windows.h>
#include <vector>
#include <conio.h>
#include <thread>
#include <chrono>

using namespace std;

// Game constants
const int BOARD_WIDTH = 12;
const int BOARD_HEIGHT = 18;
const char BLOCK = '#';
const char EMPTY = ' ';

// Tetromino shapes
const vector<vector<vector<bool>>> SHAPES = {
    // I
    {
        {1,1,1,1}
    },
    // O
    {
        {1,1},
        {1,1}
    },
    // T
    {
        {0,1,0},
        {1,1,1}
    },
    // S
    {
        {0,1,1},
        {1,1,0}
    },
    // Z
    {
        {1,1,0},
        {0,1,1}
    },
    // J
    {
        {1,0,0},
        {1,1,1}
    },
    // L
    {
        {0,0,1},
        {1,1,1}
    }
};

class Tetris {
private:
    vector<vector<char>> board;
    vector<vector<bool>> currentPiece;
    int currentX, currentY;
    int score;
    bool gameOver;

    void initBoard() {
        board = vector<vector<char>>(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, EMPTY));
        for (int i = 0; i < BOARD_HEIGHT; i++) {
            board[i][0] = board[i][BOARD_WIDTH - 1] = '|';
        }
        for (int i = 0; i < BOARD_WIDTH; i++) {
            board[BOARD_HEIGHT - 1][i] = '-';
        }
    }

    void spawnPiece() {
        currentPiece = SHAPES[rand() % SHAPES.size()];
        currentX = BOARD_WIDTH / 2 - currentPiece[0].size() / 2;
        currentY = 0;

        if (!canMove(currentX, currentY)) {
            gameOver = true;
        }
    }

    bool canMove(int newX, int newY) {
        for (int i = 0; i < currentPiece.size(); i++) {
            for (int j = 0; j < currentPiece[i].size(); j++) {
                if (currentPiece[i][j]) {
                    int x = newX + j;
                    int y = newY + i;
                    if (x < 1 || x >= BOARD_WIDTH - 1 || y >= BOARD_HEIGHT - 1 ||
                        (y >= 0 && board[y][x] != EMPTY)) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void mergePiece() {
        for (int i = 0; i < currentPiece.size(); i++) {
            for (int j = 0; j < currentPiece[i].size(); j++) {
                if (currentPiece[i][j] && currentY + i >= 0) {
                    board[currentY + i][currentX + j] = BLOCK;
                }
            }
        }
    }

    void clearLines() {
        for (int i = BOARD_HEIGHT - 2; i >= 0; i--) {
            bool lineFull = true;
            for (int j = 1; j < BOARD_WIDTH - 1; j++) {
                if (board[i][j] == EMPTY) {
                    lineFull = false;
                    break;
                }
            }
            if (lineFull) {
                for (int k = i; k > 0; k--) {
                    board[k] = board[k - 1];
                }
                board[0] = vector<char>(BOARD_WIDTH, EMPTY);
                board[0][0] = board[0][BOARD_WIDTH - 1] = '|';
                score += 100;
            }
        }
    }

    void rotatePiece() {
        vector<vector<bool>> rotated(currentPiece[0].size(), vector<bool>(currentPiece.size()));
        for (int i = 0; i < currentPiece.size(); i++) {
            for (int j = 0; j < currentPiece[i].size(); j++) {
                rotated[j][currentPiece.size() - 1 - i] = currentPiece[i][j];
            }
        }
        
        auto oldPiece = currentPiece;
        currentPiece = rotated;
        if (!canMove(currentX, currentY)) {
            currentPiece = oldPiece;
        }
    }

public:
    Tetris() {
        srand(time(0));
        initBoard();
        score = 0;
        gameOver = false;
        spawnPiece();
    }

    void draw() {
        system("cls");
        vector<vector<char>> tempBoard = board;
        
        // Draw current piece
        for (int i = 0; i < currentPiece.size(); i++) {
            for (int j = 0; j < currentPiece[i].size(); j++) {
                if (currentPiece[i][j] && currentY + i >= 0) {
                    tempBoard[currentY + i][currentX + j] = BLOCK;
                }
            }
        }

        // Draw board
        for (const auto& row : tempBoard) {
            for (char cell : row) {
                cout << cell << ' ';
            }
            cout << endl;
        }
        cout << "Score: " << score << endl;
    }

    bool isGameOver() const {
        return gameOver;
    }

    void moveLeft() {
        if (canMove(currentX - 1, currentY)) {
            currentX--;
        }
    }

    void moveRight() {
        if (canMove(currentX + 1, currentY)) {
            currentX++;
        }
    }

    void moveDown() {
        if (canMove(currentX, currentY + 1)) {
            currentY++;
        }
        else {
            mergePiece();
            clearLines();
            spawnPiece();
        }
    }

    void rotate() {
        rotatePiece();
    }
};

int main() {
    Tetris game;
    auto lastUpdate = chrono::steady_clock::now();
    const auto updateInterval = chrono::milliseconds(500);

    while (!game.isGameOver()) {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
            case 'a': game.moveLeft(); break;
            case 'd': game.moveRight(); break;
            case 's': game.moveDown(); break;
            case 'w': game.rotate(); break;
            case 'q': return 0;
            }
        }

        auto now = chrono::steady_clock::now();
        if (now - lastUpdate >= updateInterval) {
            game.moveDown();
            lastUpdate = now;
        }

        game.draw();
        this_thread::sleep_for(chrono::milliseconds(50));
    }

    cout << "Game Over!" << endl;
    return 0;
}