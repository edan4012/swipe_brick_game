#include "raylib.h"

#define MAX_BLOCKS 150
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

typedef enum {
    CONTROL_KEYBOARD,
    CONTROL_MOUSE
} ControlMode;

typedef enum {
    EASY,
    NOMAL,
    HARD
} Difficulty;

typedef enum {
    STATE_START,
    STATE_PLAYING,
    STATE_GAMEOVER,
    STATE_ENDING
} GameState;

int ball_speed = 5;

void ResetBlocks(Rectangle blocks[MAX_BLOCKS], bool active[MAX_BLOCKS]) {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        blocks[i] = (Rectangle){ 15 + (i % 25) * 63, 50 + (i / 25) * 30, 60, 20 };
        active[i] = true;
    }
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout Game"); //가로 세로 사이즈
    SetTargetFPS(60);

    int score = 0;
    int lifes = 5;

    // 패들과 공
    Rectangle paddle = { SCREEN_WIDTH / 2 - 100, 800, 150, 20 }; // 좌우위치, 높이, 길이, 두께
    Vector2 ball = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}; //좌우 상하 위치
    Vector2 speed = { 5, -5}; //볼의 좌우상하 속도
    float radius = 10; //공의 사이즈

    // 블록들
    Rectangle blocks[MAX_BLOCKS];
    Color colors[6] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE }; //블록의 색깔지정
    bool active[MAX_BLOCKS] = { 0 };
    for (int i = 0; i < MAX_BLOCKS; i++) {
        blocks[i] = (Rectangle){ 15 + (i % 25) * 63, 50 + (i / 25) * 30, 60, 20 }; //블록의 좌표와 크기 지정 x, y, width, height
        active[i] = true;
    }

    Difficulty difficulty = NOMAL;
    ControlMode controlMode = CONTROL_KEYBOARD;
    GameState gamestate = STATE_START;
    

    while (!WindowShouldClose()) {
        if (gamestate == STATE_START && IsKeyPressed(KEY_ENTER)) {
            gamestate = STATE_PLAYING;
            score = 0;
            lifes = 5;
            ball = (Vector2){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            speed = (Vector2){ 5, -5 };
            ResetBlocks(blocks, active);
        }
        if (gamestate == STATE_START) { //게임 시작화면 구성
            ClearBackground(RAYWHITE);
            DrawText("Breakout Game", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 50, 60, BLACK);
            DrawText("Press SPACE to change Controll", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 20, 20, BLACK);
            DrawText("Press TAB to change Difficulty", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 50, 20, BLACK);
            DrawText("Press ENTER to Start", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 110, 30, BLACK);
            
            //조작방식 전환
            if (IsKeyPressed(KEY_SPACE)) {
                if (controlMode == CONTROL_KEYBOARD)
                    controlMode = CONTROL_MOUSE;
                else
                    controlMode = CONTROL_KEYBOARD;
            }
            if (controlMode == CONTROL_MOUSE)
                DrawText("Control: Mouse (Press SPACE to switch)", 20, 870, 20, BLACK);
            else
                DrawText("Control: Keyboard (Press SPACE to switch)", 20, 870, 20, BLACK);

            //난이도 전환
            if (IsKeyPressed(KEY_TAB)) {
                if (difficulty == EASY)
                    difficulty = NOMAL;
                else if (difficulty == NOMAL)
                    difficulty = HARD;
                else
                    difficulty = EASY;
            }
            if (difficulty == EASY) {
                DrawText("Difficulty : EASY", 20, 840, 20, BLACK);
                speed = (Vector2){ 3, -3 };
            }
            else if (difficulty == NOMAL) {
                DrawText("Difficulty : NOMAL", 20, 840, 20, BLACK);
                speed = (Vector2){ 5, -5 };
            }
            else {
                DrawText("Difficulty : HARD", 20, 840, 20, BLACK);
                speed = (Vector2){ 7, -7 };
            }

        }
        else if (gamestate == STATE_GAMEOVER) {
            //게임오버 화면
            ClearBackground(BLACK);
            DrawText("GAME OVER...", SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 - 30, 60, RED);
            DrawText("Press R to go Start Screen", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 40, 20, WHITE);

            if (IsKeyPressed(KEY_R)) {
                gamestate = STATE_START;
                score = 0;
                lifes = 5;
                ball = (Vector2){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
                speed = (Vector2){ 5,-5 };
                ResetBlocks(blocks, active);
            }
        }
        else if (gamestate == STATE_ENDING) {
            ClearBackground(WHITE);
            DrawText("Congratulation!!!", SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 - 50, 60, BLACK);
            DrawText("Press R to go Start Screen", SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 + 40, 30, BLACK);

            if (IsKeyPressed(KEY_R)) {
                gamestate = STATE_START;
                score = 0;
                lifes = 5;
                ball = (Vector2){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
                speed = (Vector2){ 5,-5 };
                ResetBlocks(blocks, active);
            }
        }
        else if (gamestate == STATE_PLAYING) {
            //조작방식에 따른 패들의 움직임방식 전환
            if (controlMode == CONTROL_MOUSE) {
                Vector2 mouse = GetMousePosition();
                paddle.x = mouse.x - paddle.width / 2;
            }
            else {
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) paddle.x -= 8;
                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) paddle.x += 8;
            }


            //패들이 화면 밖으로 벗어나지 않게
            if (paddle.x < 0) paddle.x = 0;
            if (paddle.x + paddle.width > SCREEN_WIDTH)  paddle.x = SCREEN_WIDTH - paddle.width;

            // 공 이동
            ball.x += speed.x;
            ball.y += speed.y;

            // 벽 충돌
            if (ball.x < radius || ball.x > SCREEN_WIDTH - radius) speed.x *= -1;
            if (ball.y < radius) speed.y *= -1;

            // 바닥
            if (ball.y > SCREEN_HEIGHT) ball = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, lifes--;  //공이 바닥을 벗어나면 위치 초기화


            // 패들 충돌
            if (CheckCollisionCircleRec(ball, radius, paddle)) {
                speed.y *= -1;
                ball.y = paddle.y - radius;
            }

            // 블록 충돌
            for (int i = 0; i < MAX_BLOCKS; i++) {
                if (active[i] && CheckCollisionCircleRec(ball, radius, blocks[i])) {
                    active[i] = false;
                    speed.y *= -1;
                    score += 10;
                    break;
                }
            }
            if (IsKeyDown(KEY_L)) {
                // 첫 번째 활성화된 블록을 비활성화
                for (int i = 0; i < MAX_BLOCKS; i++) {
                    if (active[i]) {
                        active[i] = false;
                        score += 10;
                        break;
                    }
                }
            }

            //점수와 생명 표시
            DrawText(TextFormat("Score: %d", score), 20, 20, 20, WHITE);
            DrawText(TextFormat("Lifes: %d", lifes), SCREEN_WIDTH - 100, 20, 20, RED);

            // 그리기
            BeginDrawing();
            ClearBackground(BLACK);
            DrawRectangleRec(paddle, WHITE);
            DrawCircleV(ball, radius, WHITE);
            for (int i = 0; i < MAX_BLOCKS; i++) {
                if (active[i]) DrawRectangleRec(blocks[i], colors[i / 25]);
            }
            if (lifes == -1) {
                gamestate = STATE_GAMEOVER;
            }
            if (score == 1500) {
                gamestate = STATE_ENDING;
            }
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}