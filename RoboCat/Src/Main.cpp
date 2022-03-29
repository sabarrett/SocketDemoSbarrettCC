

#include "RoboCatPCH.h"
#include <raymath.h>
#include <string>
#include <iostream>
#include "stdlib.h"
#include "TCPPacket.h"

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

enum class StartupScreenAction {
    QUIT,
    CONNECT,
    HOST,
};

void HandleDestroy(TCPPacketDestroy* destroy) {
    printf("got destroy packet %i\n", destroy->objectID);
}

StartupScreenAction StartupScreen(SocketAddress* out_address) {
    bool complete = false;
    float centerX = screenWidth / 2.0f;
    float btnWidth = 400;

    Rectangle textBox = { centerX - (btnWidth * 0.5f), 180, (btnWidth * 0.7), 50 };
    Rectangle connectBtn = { centerX + (btnWidth * 0.25f), 180, (btnWidth * 0.25), 50 };
    Rectangle hostBtn = { centerX - (btnWidth * 0.5f), 270, btnWidth, 50 };

    const int inputBufferSize = sizeof("000.000.000.000");
    const int inputCountMax = inputBufferSize-1;
    char inputBuffer[inputBufferSize];
    int inputCount = 0;

    int framesCounter = 0;

    while (!complete) {
        if (WindowShouldClose()) return StartupScreenAction::QUIT;

        framesCounter++;

        int key;
        // Check if more characters have been pressed on the same frame
        while ((key = GetKeyPressed()) > 0)
        {
            if (key == KEY_BACKSPACE) {
                inputCount--;
                if (inputCount < 0) inputCount = 0;
                continue;
            }

            bool isKeyNum = ((key >= '0') && (key <= '9'));
            bool isValidChar = isKeyNum || key == '.';
            if (isValidChar && (inputCount < inputCountMax)) {
                char afterDot = 0;
                char dotCount = 0;
                // get characters since last dot
                for (int i = 0; i < inputCount; i++) {
                    if (inputBuffer[i] == '.') {
                        afterDot = 0;
                        dotCount++;
                    }
                    else afterDot++;
                }

                if (dotCount == 3 && afterDot == 3) continue;
                if (isKeyNum && afterDot == 3)
                    inputBuffer[inputCount++] = '.';
                if (!isKeyNum && afterDot == 0) continue; // no 2 dots in a row
              

                inputBuffer[inputCount++] = (char)key;
            }
            
        }

        // make sure there is a null terminator
        inputBuffer[inputCount] = 0;

        bool connectHvr = CheckCollisionPointRec(GetMousePosition(), connectBtn);
        bool hostHvr = CheckCollisionPointRec(GetMousePosition(), hostBtn);

        BeginDrawing();

        ClearBackground(RAYWHITE);

        
        DrawRectangleRec(textBox, LIGHTGRAY);
        DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);


        if (inputCount == 0)
            DrawText("Type IP", textBox.x + 10, textBox.y + 15, 20, GRAY);
        else 
            DrawText(inputBuffer, textBox.x + 10, textBox.y + 15, 20, DARKGRAY);

        if (((framesCounter / 30) % 2) == 0)
            DrawText("|", (int)textBox.x + 10 + MeasureText(inputBuffer, 20), (int)textBox.y + 16, 20, MAROON);


      
        DrawRectangleRec(connectBtn, connectHvr ? DARKGRAY : GRAY);
        DrawText("Connect", (int)connectBtn.x + 10, (int)connectBtn.y + 15, 20, LIGHTGRAY);

        DrawText("OR", centerX - 20, 240, 20, GRAY);

        
        DrawRectangleRec(hostBtn, hostHvr ? DARKGRAY : GRAY);
        DrawText("Host", hostBtn.x + 10, hostBtn.y + 15, 20, LIGHTGRAY);

        
        EndDrawing();

        if (IsMouseButtonReleased(0)) {

            if (connectHvr) {
                {
                    addrinfo hint;
                    memset(&hint, 0, sizeof(hint));
                    hint.ai_family = AF_INET;

                    addrinfo* result;
                    int error = getaddrinfo(inputBuffer, "1234", &hint, &result);
                    if (error != 0 && result != nullptr)
                    {
                        SocketUtil::ReportError("got an error with addrinfo");
                        return StartupScreenAction::QUIT;
                    }

                    while (!result->ai_addr && result->ai_next)
                    {
                        result = result->ai_next;
                    }

                    if (!result->ai_addr)
                    {
                        SocketUtil::ReportError("couldnt get ai_addr");
                        return StartupScreenAction::QUIT;
                    }

                    *out_address = SocketAddress(*result->ai_addr);

                    freeaddrinfo(result);
                }
                return  StartupScreenAction::CONNECT;
            }

            if (hostHvr) {
                return StartupScreenAction::HOST;
            }
        }
    }
    return StartupScreenAction::QUIT;
}

TCPSocketPtr StartHost() {
    SocketAddress clientAddr;
    TCPSocketPtr hostSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
    hostSocket->Bind(SocketAddress(2130706433, 1234)); // localhost:1234
    hostSocket->Listen();

    bool listening = true;
    unsigned int frameCount = 0;
    char msg[] = "Waiting for connection....";
    char* dots = msg + (sizeof("Waiting for connection") - 1);
    const int textWidth = MeasureText(msg, 20);
    while (listening) {
        frameCount++;
        if (hostSocket->HasRead()) {
            return hostSocket->Accept(clientAddr);
        }
        
        unsigned int o = frameCount / 10;
        unsigned int p = o % 4;
        dots[p] = '.';
        dots[p + 1] = 0;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText(msg, screenWidth / 2.0f - textWidth / 2.0f, screenHeight / 2.0f - 10, 20, GRAY);
     
        EndDrawing();
    }

    return nullptr;
}

TCPSocketPtr StartConnect(SocketAddress host_address) {

    TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
    clientSocket->Bind(SocketAddress(2130706433, 1235));
    clientSocket->Connect(host_address);

    return clientSocket;
}

void Shutdown() {

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------


    SocketUtil::CleanUp();
}

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

    TCPPacketManager packetManager;

    packetManager.RegisterType<TCPPacketDestroy>();
    packetManager.RegisterType<TCPPacketMove>();
    packetManager.RegisterType<TCPPacketCreate>();

    packetManager.RegisterHandler<TCPPacketDestroy>(HandleDestroy);

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    InitGame();

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    SocketAddress address;
    StartupScreenAction action = StartupScreen(&address);

    if (action == StartupScreenAction::QUIT) {
        Shutdown();
        return 0;
    }

    TCPSocketPtr socket;

    
    if (action == StartupScreenAction::HOST)
        socket = StartHost();

    if (action == StartupScreenAction::CONNECT)
       socket = StartConnect(address);

    TCPPacketDestroy testDestroy;
    testDestroy.type = TCPPacketType::DESTROY;
    testDestroy.objectID = 54;
  
    packetManager.SendPacket(socket.get(), &testDestroy);
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        packetManager.HandleInput(socket.get());
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

    Shutdown();

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