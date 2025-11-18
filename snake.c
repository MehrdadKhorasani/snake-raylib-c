#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <raylib.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 320
#define MAX_SNAKE_LENGTH 256
#define CELL_SIZE 16

typedef struct {int x; int y;} Segment;
typedef struct {Segment body[MAX_SNAKE_LENGTH]; int length; int base_size; Color color;} Snake;
typedef enum {UP,DOWN,LEFT,RIGHT} Direction;
typedef struct {int x; int y; int size; Color color;} Apple;
typedef enum {PLAYING, GAME_OVER} GameStatus;

int high_score_function(int current_score) {
    int high_score = 0;
    FILE *fp = fopen("highscore.txt", "r");
    if (fp != NULL) {
        fscanf(fp, "%d", &high_score);
        fclose(fp);
    }

    if (current_score > high_score) {
        high_score = current_score;
        fp = fopen("highscore.txt", "w");
        if (fp != NULL) {
            fprintf(fp, "%d", high_score);
            fclose(fp);
        }
    }

    return high_score;
}

int main()
{
    int score = 0;
    int grow_pending = 0; 
    GameStatus state = PLAYING;

    srand(time(NULL));

    Snake snake = {
        .length = 1,
        .base_size = CELL_SIZE,
        .color = {61, 105, 72, 255}
    };
    snake.body[0].x = CELL_SIZE;
    snake.body[0].y = CELL_SIZE;

    Apple apple = {
        .x = (rand() % (SCREEN_WIDTH / CELL_SIZE)) * CELL_SIZE,
        .y = (rand() % (SCREEN_HEIGHT / CELL_SIZE)) * CELL_SIZE,
        .size = CELL_SIZE,
        .color = {255,5,5,255}
    };

    Direction dir = RIGHT;
    float timer = 0.0f;
    float move_delay = 0.5f;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake Game");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GREEN);

        if (state == PLAYING) {
            for (int i = 0; i < snake.length; i++) {
                DrawRectangle(snake.body[i].x, snake.body[i].y, snake.base_size, snake.base_size, snake.color);
            }

            DrawRectangle(apple.x, apple.y, apple.size, apple.size, apple.color);

            char score_text[10];
            sprintf(score_text, "Score: %d", score);
            DrawText(score_text, 10, 5, 15, WHITE);

            int best_score = high_score_function(score);
            char high_text[15];
            sprintf(high_text, "High: %d", best_score);
            DrawText(high_text, SCREEN_WIDTH - 90, 5, 15, YELLOW);

            timer += GetFrameTime();

            if (IsKeyPressed(KEY_DOWN) && dir != UP) dir = DOWN;
            else if (IsKeyPressed(KEY_UP) && dir != DOWN) dir = UP;
            else if (IsKeyPressed(KEY_LEFT) && dir != RIGHT) dir = LEFT;
            else if (IsKeyPressed(KEY_RIGHT) && dir != LEFT) dir = RIGHT;

            if (timer >= move_delay) {
                timer = 0;

                Segment tail_before = snake.body[snake.length - 1];

                for (int i = snake.length - 1; i > 0; i--) {
                    snake.body[i] = snake.body[i - 1];
                }

                switch (dir) {
                    case UP: snake.body[0].y -= snake.base_size; break;
                    case DOWN: snake.body[0].y += snake.base_size; break;
                    case LEFT: snake.body[0].x -= snake.base_size; break;
                    case RIGHT: snake.body[0].x += snake.base_size; break;
                }

                if (snake.body[0].x >= SCREEN_WIDTH)  snake.body[0].x = 0;
                if (snake.body[0].y >= SCREEN_HEIGHT) snake.body[0].y = 0;
                if (snake.body[0].x < 0)              snake.body[0].x = SCREEN_WIDTH - snake.base_size;
                if (snake.body[0].y < 0)              snake.body[0].y = SCREEN_HEIGHT - snake.base_size;

                if (snake.body[0].x == apple.x && snake.body[0].y == apple.y) {
                    bool valid_position = false;
                    int new_x, new_y;
                    while (!valid_position) {
                        new_x = (rand() % (SCREEN_WIDTH / CELL_SIZE)) * CELL_SIZE;
                        new_y = (rand() % (SCREEN_HEIGHT / CELL_SIZE)) * CELL_SIZE;
                        valid_position = true;
                        for (int i = 0; i < snake.length; i++) {
                            if (snake.body[i].x == new_x && snake.body[i].y == new_y) {
                                valid_position = false;
                                break;
                            }
                        }
                    }
                    apple.x = new_x;
                    apple.y = new_y;

                    score++;
                    grow_pending++;
                    if (move_delay > 0.1f) move_delay -= 0.02f;
                }

                for (int i = 1; i < snake.length; i++) {
                    if (snake.body[0].x == snake.body[i].x && snake.body[0].y == snake.body[i].y) {
                        state = GAME_OVER;
                        break;
                    }
                }

                if (grow_pending > 0 && snake.length < MAX_SNAKE_LENGTH) {
                    snake.body[snake.length] = tail_before;
                    snake.length++;
                    grow_pending--;
                }
            }
        }
        else if (state == GAME_OVER) {
            int best_score = high_score_function(score);

            DrawText("GAME OVER", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 - 20, 20, RED);
            char restart_text[50];
            sprintf(restart_text, "Press ENTER to restart | High: %d", best_score);
            DrawText(restart_text, SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 10, 15, WHITE);

            if (IsKeyPressed(KEY_ENTER)) {
                score = 0;
                snake.length = 1;
                snake.body[0].x = CELL_SIZE;
                snake.body[0].y = CELL_SIZE;
                dir = RIGHT;
                move_delay = 0.5f;
                grow_pending = 0;
                apple.x = (rand() % (SCREEN_WIDTH / CELL_SIZE)) * CELL_SIZE;
                apple.y = (rand() % (SCREEN_HEIGHT / CELL_SIZE)) * CELL_SIZE;
                state = PLAYING;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
