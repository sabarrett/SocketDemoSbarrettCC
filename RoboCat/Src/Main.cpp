

#include "RoboCatPCH.h"
#include <raymath.h>
#include <string>
#include <iostream>
#include "stdlib.h"

const int BRICK_COLUMNS = 10;
const int BRICK_ROWS = 5;

const int screenWidth = 800;
const int screenHeight = 450;
static Vector2 bricks;

static float RandomFloat(float min, float max);
static void InitGame();
static void UpdateGame();

struct Player {
    Vector2 position;
    Vector2 size;
    Color playerColor;
    int id;
    int score;
    int life;
};

struct Ball {
    Vector2 position;
    Vector2 velocity;
    Color ballColor;
    float radius;
    int id;
    int ownerID;
    bool isDead;
};

struct Brick {
    Vector2 position;
    Color brickColor;
    int id;
    bool isDead;
    Rectangle getRectangle()
    {
        return Rectangle{ position.x - bricks.x / 2, position.y - bricks.y / 2, screenWidth / BRICK_COLUMNS, 40 };
    }
};
static Brick brickList[BRICK_ROWS][BRICK_COLUMNS] = { 0 };
static Player player;
static Ball ball;

#if _WIN32
int main(int argc, const char** argv)
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#else
const char** __argv;
int __argc;
int main(int argc, const char** argv)
{
	__argc = argc;
	__argv = argv;
#endif

	SocketUtil::StaticInit();

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    InitGame();

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if (player.life > 0)
        {
            // Update
            //----------------------------------------------------------------------------------
            // TODO: Update your variables here
            UpdateGame();
            //----------------------------------------------------------------------------------

            // Draw
            //----------------------------------------------------------------------------------
            BeginDrawing();

            ClearBackground(RAYWHITE);

            //Draw Ball
            if (ball.isDead == false)
            {
                DrawCircleV(ball.position, ball.radius, ball.ballColor);
            }

            //Draw Player
            DrawRectangle(player.position.x - player.size.x / 2, player.position.y - player.size.y / 2, player.size.x, player.size.y, player.playerColor);

            //Draw Bricks
            for (int i = 0; i < BRICK_ROWS; i++)
            {
                for (int j = 0; j < BRICK_COLUMNS; j++)
                {
                    if (brickList[i][j].isDead == false)
                    {
                        DrawRectangleRec(brickList[i][j].getRectangle(), brickList[i][j].brickColor);
                    }
                }
            }
        }

        ClearBackground(RAYWHITE);

        if (player.life <= 0)
        {
            string playerScore = "Score: " + std::to_string(player.score);
            DrawText(playerScore.c_str(), GetScreenWidth() / 2 - MeasureText(playerScore.c_str(), 40)/2, GetScreenHeight() / 2 - 50, 40, BLACK);
        }

        EndDrawing();
        //----------------------------------------------------------------------------------

    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------


	SocketUtil::CleanUp();

	return 0;
}

void InitGame()
{
    //Setting bricks to screen
    bricks = { screenWidth/ BRICK_COLUMNS,40};

    //Temp Init Player
    player.position = { screenWidth / 2, screenHeight * 7 / 8 };
    player.size = { screenWidth / 10, 20 };
    player.playerColor = { 0,0,255,255 };
    player.id = 0;
    player.life = 3;

    //Temp Init Ball
    ball.position = { screenWidth / 2, screenHeight * 7 / 10};
    ball.velocity = { 0,4 };
    ball.ballColor = player.playerColor;
    ball.radius = 7.0;
    ball.id = 0;
    ball.ownerID = player.id;

    //Init Bricks
    for (int i = 0; i < BRICK_ROWS; i++)
    {
        for (int j = 0; j < BRICK_COLUMNS; j++)
        {
            float hue = RandomFloat(0.0f,360.0f);
            float saturation = RandomFloat(0.42f, 0.98f);
            float value = RandomFloat(0.4f,0.9f);
            brickList[i][j].position = { j * bricks.x + bricks.x / 2, i * bricks.y + 50 };
            brickList[i][j].brickColor = ColorFromHSV(hue, saturation, value);
            brickList[i][j].isDead = false;
        }
    }
}

float RandomFloat(float min, float max)
{
    float t = max - min;
    float x = (float)rand() / (float)(RAND_MAX / 1.0f);

    float out = x * t + min;

    return out;
}

void UpdateGame()
{
    ball.position.x += ball.velocity.x;
    ball.position.y += ball.velocity.y;

    //Spawn Ball after it dies;
    if (ball.isDead)
    {
        ball.isDead = false;
        ball.position = { screenWidth / 2, screenHeight * 7 / 10 };
        ball.velocity = { 0,-4 };
    }

    //Brick Collision
    for (int i = 0; i < BRICK_ROWS; i++)
    {
        for (int j = 0; j < BRICK_COLUMNS; j++)
        {
            if (CheckCollisionCircleRec(ball.position, ball.radius, brickList[i][j].getRectangle()))
            {
                if (brickList[i][j].isDead == false)
                {
                    Vector2 normal = { player.position.x - brickList[i][j].position.x, player.position.y - brickList[i][j].position.y };
                    normal = Vector2Normalize(normal);
                    ball.velocity = Vector2Reflect(ball.velocity, normal);
                    brickList[i][j].isDead = true;
                    player.score++;
                }
            }
        }
    }

    //Player Collision
    if (CheckCollisionCircleRec(ball.position, ball.radius, Rectangle{player.position.x - player.size.x / 2, player.position.y - player.size.y / 2, player.size.x, player.size.y }))
    {
        ball.velocity.y *= -1;
        ball.velocity.x = (ball.position.x - player.position.x) / (player.size.x / 2) * 5;
    }

    //Wall Collision
    if (ball.position.x + ball.radius >= screenWidth || ball.position.x - ball.radius <= 0)
    {
        ball.velocity.x *= -1;
    }
    if (ball.position.y - ball.radius <= 0)
    {
        ball.velocity.y *= -1;
    }
    if (ball.position.y + ball.radius >= screenHeight)
    {
        ball.velocity = { 0,0 };
        ball.isDead = true;

        player.life--;
    }

    //Player Movement
    if (IsKeyDown(KEY_A))
    {
        player.position.x -= 5;
    }
    if (player.position.x - player.size.x / 2 <= 0)
    {
        player.position.x = player.size.x / 2;
    }
    if (IsKeyDown(KEY_D))
    {
        player.position.x += 5;
    }
    if (player.position.x + player.size.x / 2 >= screenWidth)
    {
        player.position.x = screenWidth - player.size.x / 2;
    }
    
}