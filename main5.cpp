#include <graphics.h>
#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define DELAY 100
#define LEFT_WALL 50
#define RIGHT_WALL 590
#define TOP_WALL 50
#define BOTTOM_WALL 430

typedef struct SnakeSegment {
    int x, y;
    struct SnakeSegment* next;
} SnakeSegment;

SnakeSegment* head = NULL;
SnakeSegment* tail = NULL;

int direction;
int foodX, foodY;
int score;

void playTone(int frequency, int duration) {
    sound(frequency);
    delay(duration);
    nosound();
}

void playEatSound() {
    playTone(1000, 100);
}

void playCrashSound() {
    playTone(300, 300);
}

void playGameOverSound() {
    playTone(800, 150);
    playTone(600, 150);
    playTone(400, 150);
}

void addSegment(int x, int y) {
    SnakeSegment* newSegment = (SnakeSegment*) malloc(sizeof(SnakeSegment));
    newSegment->x = x;
    newSegment->y = y;
    newSegment->next = NULL;

    if (head == NULL) {
        head = tail = newSegment;
    } else {
        tail->next = newSegment;
        tail = newSegment;
    }
}

void moveSnake(int grow) {
    int newX = head->x;
    int newY = head->y;

    switch (direction) {
        case 0: newY -= 10; break;
        case 1: newX += 10; break;
        case 2: newY += 10; break;
        case 3: newX -= 10; break;
    }

    SnakeSegment* newHead = (SnakeSegment*) malloc(sizeof(SnakeSegment));
    newHead->x = newX;
    newHead->y = newY;
    newHead->next = head;
    head = newHead;

    if (!grow) {
        SnakeSegment* temp = head;
        while (temp->next != tail) {
            temp = temp->next;
        }
        free(tail);
        tail = temp;
        tail->next = NULL;
    }
}

void drawSnake() {
    SnakeSegment* temp = head;
    while (temp != NULL) {
        rectangle(temp->x, temp->y, temp->x + 10, temp->y + 10);
        floodfill(temp->x + 1, temp->y + 1, WHITE);
        temp = temp->next;
    }
}

void drawFood() {
    setcolor(RED);
    rectangle(foodX, foodY, foodX + 10, foodY + 10);
    floodfill(foodX + 1, foodY + 1, RED);
    setcolor(WHITE);
}

void generateFood() {
    foodX = (rand() % ((RIGHT_WALL - LEFT_WALL) / 10)) * 10 + LEFT_WALL;
    foodY = (rand() % ((BOTTOM_WALL - TOP_WALL) / 10)) * 10 + TOP_WALL;
}

int checkFoodEaten() {
    return (head->x == foodX && head->y == foodY);
}

int checkSelfCollision() {
    SnakeSegment* temp = head->next;
    while (temp != NULL) {
        if (head->x == temp->x && head->y == temp->y) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

void changeDirection(char key) {
    switch (key) {
        case 'w': if (direction != 2) direction = 0; break;
        case 'd': if (direction != 3) direction = 1; break;
        case 's': if (direction != 0) direction = 2; break;
        case 'a': if (direction != 1) direction = 3; break;
    }
}

void drawScore() {
    char scoreStr[20];
    sprintf(scoreStr, "Score: %d", score);
    outtextxy(LEFT_WALL, TOP_WALL - 30, scoreStr);
}

void drawBoundary() {
    rectangle(LEFT_WALL, TOP_WALL, RIGHT_WALL, BOTTOM_WALL);
}

void freeSnake() {
    SnakeSegment* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
    head = tail = NULL;
}

void showStartScreen() {
    cleardevice();
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(160, 200, "Welcome to SNAKE GAME");
    outtextxy(130, 240, "Use W A S D to move. Avoid walls and yourself!");
    outtextxy(160, 280, "Press any key to start...");
    getch();
}

int showGameOverScreen() {
    cleardevice();
    char scoreStr[50];
    sprintf(scoreStr, "Game Over! Your score: %d", score);
    outtextxy(180, 200, scoreStr);
    outtextxy(160, 240, "Play Again? (Y/N)");
    char choice;
    do {
        choice = getch();
    } while (choice != 'y' && choice != 'Y' && choice != 'n' && choice != 'N');

    return (choice == 'y' || choice == 'Y');
}

void initSnake() {
    for (int i = 0; i < 5; i++) {
        addSegment(200 - i * 10, 200);
    }
    direction = 1;
    score = 0;
}

int main() {
    int gd = DETECT, gm;
    initgraph(&gd, &gm, "C:\\Turboc3\\BGI");
    srand(time(NULL));

    int playAgain = 1;

    while (playAgain) {
        showStartScreen();
        initSnake();
        generateFood();

        int gameOver = 0;
        while (!gameOver) {
            if (kbhit()) {
                char key = getch();
                changeDirection(key);
            }

            cleardevice();
            drawBoundary();
            drawSnake();
            drawFood();
            drawScore();
            delay(DELAY);

            int grow = checkFoodEaten();
            if (grow) {
                score += 10;
                playEatSound();
                generateFood();
            }

            moveSnake(grow);

            if (head->x < LEFT_WALL || head->x >= RIGHT_WALL ||
                head->y < TOP_WALL || head->y >= BOTTOM_WALL) {
                playCrashSound();
                gameOver = 1;
                outtextxy(200, 200, "Game Over - Hit the Wall!");
            }

            if (checkSelfCollision()) {
                playCrashSound();
                gameOver = 1;
                outtextxy(200, 220, "Game Over - You Ate Yourself!");
            }
        }

        playGameOverSound();
        delay(1000);
        playAgain = showGameOverScreen();
        freeSnake();
    }

    closegraph();
    return 0;
}