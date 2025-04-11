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
    NORMAL,
    HARD
} Difficulty;

typedef enum {
    STATE_START,
    STATE_PLAYING,
    STATE_GAMEOVER,
    STATE_ENDING
} GameState;

Sound hitSound;
Sound gameOverSound;
Sound clearSound;

//블록 초기화
void ResetBlocks(Rectangle blocks[MAX_BLOCKS], bool active[MAX_BLOCKS]) {
    for (int i = 0; i < MAX_BLOCKS; i++) {
        blocks[i] = (Rectangle){ 15 + (i % 25) * 63, 50 + (i / 25) * 30, 60, 20 };
        active[i] = true;
    }
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Breakout Game");
    SetTargetFPS(60);
    InitAudioDevice();
    hitSound = LoadSound("resources/hit.wav");
    clearSound = LoadSound("resources/clear.wav");
    gameOverSound = LoadSound("resources/gameover.wav");

    int score = 0;
    int lifes = 5;
    bool gameOverSoundPlayed = false;
    bool clearSoundPlayed = false;

    // 패들과 공
    Rectangle paddle = { SCREEN_WIDTH / 2 - 100, 800, 150, 20 };
    Vector2 ball = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };//좌우 상하 위치
    Vector2 speed = { 5, -5 };//볼의 좌우상하 속도
    float radius = 10;//공의 사이즈

    // 블록들
    Rectangle blocks[MAX_BLOCKS];
    Color colors[6] = { RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE };//블록의 색깔지정
    bool active[MAX_BLOCKS] = { 0 };
    ResetBlocks(blocks, active);

    Difficulty difficulty = NORMAL;
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
            gameOverSoundPlayed = false;
            clearSoundPlayed = false;
        }
        BeginDrawing();

        //게임 시작화면 구성
        if (gamestate == STATE_START) {
            ClearBackground(RAYWHITE);
            DrawText("Breakout Game", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 - 50, 60, BLACK);
            DrawText("Press SPACE to change Controll", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 20, 20, BLACK);
            DrawText("Press TAB to change Difficulty", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 50, 20, BLACK);
            DrawText("Press ENTER to Start", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 110, 30, BLACK);

            //조작방식 전환
            if (IsKeyPressed(KEY_SPACE)) {
                controlMode = (controlMode == CONTROL_KEYBOARD) ? CONTROL_MOUSE : CONTROL_KEYBOARD;
            }
            if (controlMode == CONTROL_MOUSE)
                DrawText("Control: Mouse (Press SPACE to switch)", 20, 870, 20, BLACK);
            else
                DrawText("Control: Keyboard (Press SPACE to switch)", 20, 870, 20, BLACK);

            //난이도 전환
            if (IsKeyPressed(KEY_TAB)) {
                if (difficulty == EASY)
                    difficulty = NORMAL;
                else if (difficulty == NORMAL)
                    difficulty = HARD;
                else
                    difficulty = EASY;
            }
            if (difficulty == EASY) {
                DrawText("Difficulty : EASY", 20, 840, 20, BLACK);
                speed = (Vector2){ 3, -3 };
            }
            else if (difficulty == NORMAL) {
                DrawText("Difficulty : NORMAL", 20, 840, 20, BLACK);
                speed = (Vector2){ 5, -5 };
            }
            else {
                DrawText("Difficulty : HARD", 20, 840, 20, BLACK);
                speed = (Vector2){ 7, -7 };
            }
        }

        //게임오버 화면
        else if (gamestate == STATE_GAMEOVER) {
            ClearBackground(BLACK);
            DrawText("GAME OVER...", SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 - 30, 60, RED);
            DrawText("Press R to go Start Screen", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 40, 20, WHITE);

            if (!gameOverSoundPlayed) {
                PlaySound(gameOverSound);
                gameOverSoundPlayed = true;
            }

            if (IsKeyPressed(KEY_R)) {
                gamestate = STATE_START;
                score = 0;
                lifes = 5;
                ball = (Vector2){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
                speed = (Vector2){ 5, -5 };
                ResetBlocks(blocks, active);
                StopSound(gameOverSound);
                gameOverSoundPlayed = false;
            }
        }

        //게임 클리어 화면
        else if (gamestate == STATE_ENDING) {
            ClearBackground(WHITE);
            DrawText("Congratulation!!!", SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT / 2 - 50, 60, BLACK);
            DrawText("Press R to go Start Screen", SCREEN_WIDTH / 2 - 180, SCREEN_HEIGHT / 2 + 40, 30, BLACK);

            if (!clearSoundPlayed) {
                PlaySound(clearSound);
                clearSoundPlayed = true;
            }

            if (IsKeyPressed(KEY_R)) {
                gamestate = STATE_START;
                score = 0;
                lifes = 5;
                ball = (Vector2){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
                speed = (Vector2){ 5, -5 };
                ResetBlocks(blocks, active);
                StopSound(clearSound);
                clearSoundPlayed = false;
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
            if (ball.y > SCREEN_HEIGHT) {
                ball = (Vector2){ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
                lifes--;
            }

            // 패들 충돌
            if (CheckCollisionCircleRec(ball, radius, paddle)) {
                speed.y *= -1;
                ball.y = paddle.y - radius;
                PlaySound(hitSound);
            }

            // 블록 충돌
            for (int i = 0; i < MAX_BLOCKS; i++) {
                if (active[i] && CheckCollisionCircleRec(ball, radius, blocks[i])) {
                    active[i] = false;
                    speed.y *= -1;
                    score += 10;
                    PlaySound(hitSound);
                    break;
                }
            }

            // 첫 번째 활성화된 블록을 비활성화
            if (IsKeyDown(KEY_L)) {
                for (int i = 0; i < MAX_BLOCKS; i++) {
                    if (active[i]) {
                        active[i] = false;
                        score += 10;
                        break;
                    }
                }
            }

            ClearBackground(BLACK);
            DrawRectangleRec(paddle, WHITE);
            DrawCircleV(ball, radius, WHITE);
            for (int i = 0; i < MAX_BLOCKS; i++) {
                if (active[i]) DrawRectangleRec(blocks[i], colors[i / 25]);
            }

            DrawText(TextFormat("Score: %d", score), 20, 20, 20, WHITE);
            DrawText(TextFormat("Lifes: %d", lifes), SCREEN_WIDTH - 100, 20, 20, RED);

            if (lifes < 0) gamestate = STATE_GAMEOVER;
            if (score >= 1500) gamestate = STATE_ENDING;
        }

        EndDrawing();
    }

    UnloadSound(hitSound);
    UnloadSound(clearSound);
    UnloadSound(gameOverSound);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
