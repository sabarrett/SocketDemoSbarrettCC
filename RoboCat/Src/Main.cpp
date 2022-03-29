

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
static void InitGame(bool isHost);
static void UpdateGame();
bool gameOver;


struct PhysObject {
    Vector2 position;
    Vector2 velocity;
    bool active;
    bool hasAuthority;
    int id;
};





struct Player : PhysObject {
    Vector2 size;
    Color playerColor;
    int score;
    Rectangle rect() {
        return Rectangle{ position.x - size.x / 2, position.y - size.y / 2, size.x, size.y };
    }
    bool isMe;
    int scoreFrames;
    int bumpFrames;
    std::string nickname;
};

struct Ball : PhysObject {
    Color ballColor;
    float radius;
    int ownerID;
    bool isDead;
    Vector2 trail[4];
    int framesTillRefresh;
    int bounceFrames;
};

struct Brick {
    Vector2 position;
    Color brickColor;
    int id;
    bool isDead;
    Rectangle rect()
    {
        return Rectangle{ position.x - bricks.x / 2, position.y - bricks.y / 2, screenWidth / BRICK_COLUMNS, 40 };
    }
};

Brick brickList[BRICK_ROWS][BRICK_COLUMNS] = { 0 };
// player 1 is always the host
Player player1, player2;
Ball ball1, ball2;


const int NUM_PHYS_OBJ = 20;
PhysObject* physicsObjects[NUM_PHYS_OBJ]; // id 0 & 1 are for players, the rest are balls

enum class StartupScreenAction {
    QUIT,
    CONNECT,
    HOST,
};

void HandleDestroy(TCPPacketDestroy* destroy) {
    printf("got destroy packet %i\n", destroy->X);
    brickList[destroy->X][destroy->Y].isDead = true;
    if (player1.hasAuthority) {
        player2.score++;
        player2.scoreFrames += 10;
    }
    else {
        player1.score++;
        player2.scoreFrames += 10;
    }
      
}

void HandleMove(TCPPacketMove* move) {
    PhysObject* obj = physicsObjects[move->objectID];
    if (obj->hasAuthority) {
        printf("got move packet but this client has authority\n");
        return;
    }

    printf("got move %i, ( %hf, %hf ) ( %hf, %hf)\n", move->objectID, move->position.x, move->position.y, move->velocity.x, move->velocity.y);
   

    obj->position = move->position;
    obj->velocity = move->velocity;
}

void HandlePlayerInfo(TCPPacketPlayerInfo* info) {
    if (player1.hasAuthority) {
        player2.nickname = info->nickname;
    }
    else {
        player1.nickname = info->nickname;
    }
}

void DrawTextbox(bool active, Rectangle rec, char* text, const char* placeholder, int framesCounter) {
    Color color = text[0] == 0 ? GRAY : DARKGRAY;
    const char* msg = text[0] == 0 ? placeholder : text;
   

    DrawRectangleRec(rec, LIGHTGRAY);
    DrawRectangleLinesEx(rec, 1.0, DARKGRAY);

    DrawText(msg, rec.x + 10, rec.y + 15, 20, color);

    if (active && (((framesCounter / 30) % 2) == 0))
     DrawText("|", (int)rec.x + 10 + MeasureText(text, 20), (int)rec.y + 16, 20, MAROON);
}

StartupScreenAction StartupScreen(SocketAddress* out_address, std::string& nickname) {
    bool complete = false;
    float centerX = screenWidth / 2.0f;
    float btnWidth = 400;

    Rectangle nickBox = { centerX - (btnWidth * 0.5f), 100, (btnWidth * 0.7), 50 };
    Rectangle addrBox = { centerX - (btnWidth * 0.5f), 180, (btnWidth * 0.7), 50 };
    Rectangle connectBtn = { centerX + (btnWidth * 0.25f), 180, (btnWidth * 0.25), 50 };
    Rectangle hostBtn = { centerX - (btnWidth * 0.5f), 270, btnWidth, 50 };

    const int inputBufferSize = sizeof("000.000.000.000");
    const int inputCountMax = inputBufferSize-1;
    char addrBuf[inputBufferSize] = {};
    int inputCount = 0;
    
    const int nickBufSize = 20;
    const int nickCountMax = nickBufSize - 1;
    char nickBuf[nickBufSize] = {};
    int nickCount = 0;



    int framesCounter = 0;

    int selectedField = 2;

    while (!complete) {
        if (WindowShouldClose()) return StartupScreenAction::QUIT;
        framesCounter++;

        int key;
        // Check if more characters have been pressed on the same frame
        while ((key = GetKeyPressed()) > 0)
        {
            if (selectedField == 2) { // IP
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
                        if (addrBuf[i] == '.') {
                            afterDot = 0;
                            dotCount++;
                        }
                        else afterDot++;
                    }

                    if (dotCount == 3 && afterDot == 3) continue;
                    if (isKeyNum && afterDot == 3)
                        addrBuf[inputCount++] = '.';
                    if (!isKeyNum && afterDot == 0) continue; // no 2 dots in a row


                    addrBuf[inputCount++] = (char)key;
                }
            }
            if (selectedField == 1) { // nickname
                if (key == KEY_BACKSPACE) {
                    if (nickCount > 0) nickCount--;
                    continue;
                }
                if (nickCount >= nickCountMax) continue;
                if ((key >= 'A' && key <= 'Z')) {
                    nickBuf[nickCount++] = key;
                }
            }
        }

        // make sure there is a null terminator
        addrBuf[inputCount] = 0;
        nickBuf[nickCount] = 0;

        bool connectHvr = CheckCollisionPointRec(GetMousePosition(), connectBtn);
        bool hostHvr = CheckCollisionPointRec(GetMousePosition(), hostBtn);

        BeginDrawing();

        ClearBackground(RAYWHITE);


        DrawTextbox(selectedField == 1, nickBox, nickBuf,"Enter Nickname", framesCounter);

        DrawTextbox(selectedField == 2, addrBox, addrBuf, "Type IP", framesCounter);
        
       

      
        DrawRectangleRec(connectBtn, connectHvr ? DARKGRAY : GRAY);
        DrawText("Connect", (int)connectBtn.x + 10, (int)connectBtn.y + 15, 20, LIGHTGRAY);

        DrawText("OR", centerX - 20, 240, 20, GRAY);

        
        DrawRectangleRec(hostBtn, hostHvr ? DARKGRAY : GRAY);
        DrawText("Host", hostBtn.x + 10, hostBtn.y + 15, 20, LIGHTGRAY);

        
        EndDrawing();

        if (IsMouseButtonDown(0)) {
            if (CheckCollisionPointRec(GetMousePosition(), nickBox)) {
                selectedField = 1;
            }
            else if (CheckCollisionPointRec(GetMousePosition(), addrBox)) {
                selectedField = 2;
            }    
        }

        if (IsMouseButtonReleased(0)) {

            if (connectHvr) {
                
                unsigned char addr_sections[4] = {};
            
                bool valid = true;
                int sectionNum = 0;
                for (int i = 0; i < inputCount; i++) {
                    char c = addrBuf[i];
                    if (c == '.') {
                        sectionNum++;
                        continue;
                    }
                    unsigned char& ref = addr_sections[sectionNum];
                    unsigned int copy = ref;
                    ref *= 10;
                    copy *= 10;
                    if (ref != copy) valid = false; // address section overflow
                    
                    addr_sections[sectionNum] += c - '0';
                }
                if (valid) {
                    uint32_t addr = (addr_sections[0] << 24) + (addr_sections[1] << 16) + (addr_sections[2] << 8) + addr_sections[3];
                    *out_address = SocketAddress(addr, 1234);
                    nickname = std::string(nickBuf);
                    return  StartupScreenAction::CONNECT;
                }
            }

            if (hostHvr) {
                nickname = std::string(nickBuf);
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

void DrawBrick(Brick brick);
void DrawPlayer(Player player);
void DrawBall(Ball ball);

TCPSocketPtr masterSocket;
TCPPacketManager packetManager;

void frame_countdown(int& count) {
    if (count > 0) count--;
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

  

    packetManager.RegisterType<TCPPacketDestroy>();
    packetManager.RegisterType<TCPPacketMove>();
    packetManager.RegisterType<TCPPacketCreate>();
    packetManager.RegisterType<TCPPacketPlayerInfo>();

    packetManager.RegisterHandler<TCPPacketDestroy>(HandleDestroy);
    packetManager.RegisterHandler<TCPPacketMove>(HandleMove);
    packetManager.RegisterHandler<TCPPacketPlayerInfo>(HandlePlayerInfo);

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

   

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    std::string nickname;
    SocketAddress address;
    StartupScreenAction action = StartupScreen(&address, nickname);

    if (nickname.size() == 0) {
        // user did not pick a nickname, we will pick a random one
        const char* randNames[] = {
            "JOE",
            "JOHN",
            "EDISON",
            "TESLA",
            "GRAM"
        };
        nickname = std::string(randNames[GetRandomValue(0, 4)]);
    }

    if (action == StartupScreenAction::QUIT) {
        Shutdown();
        return 0;
    }

    

    
    if (action == StartupScreenAction::HOST)
        masterSocket = StartHost();

    if (action == StartupScreenAction::CONNECT)
       masterSocket = StartConnect(address);

    InitGame(action == StartupScreenAction::HOST);

    TCPPacketPlayerInfo playerInfo;
    playerInfo.type = TCPPacketPlayerInfo::TYPE;
    playerInfo.nickname = nickname;

    packetManager.SendPacket(masterSocket.get(), &playerInfo);

    if (player1.hasAuthority)
        player1.nickname = nickname;
    if (player2.hasAuthority)
        player2.nickname = nickname;

    bool shouldClose = false;
    // Main game loop
    while (!shouldClose)    // Detect window close button or ESC key
    {

        bool connected = packetManager.HandleInput(masterSocket.get());
        if (!connected) break;

        if (!gameOver)
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

            //Draw Player

            DrawPlayer(player1);
            DrawPlayer(player2);

            //Draw Ball
            if (ball1.active)
                DrawBall(ball1);
            if (ball2.active)
                DrawBall(ball2);
            


            //Draw Bricks
            for (int i = 0; i < BRICK_ROWS; i++)
            {
                for (int j = 0; j < BRICK_COLUMNS; j++)
                {
                    if (brickList[i][j].isDead == false)
                        DrawBrick(brickList[i][j]);
                       
                }
            }


            char scoreBuf[20];
  
            int textsize1 = 20 + player1.scoreFrames;    
            itoa(player1.score, scoreBuf, 10);
            DrawText(scoreBuf, 20, GetScreenHeight() - textsize1 - 45, textsize1, BLUE);

            int textsize2 = 20 + player2.scoreFrames;
            itoa(player2.score, scoreBuf, 10);
            DrawText(scoreBuf, screenWidth - MeasureText(scoreBuf, textsize2) - 45, GetScreenHeight() - textsize2 - 45, textsize2, RED);

            const char* nick1 = player1.nickname.c_str();
            DrawText(nick1, 20, GetScreenHeight() - 20, 20, BLUE);

            const char* nick2 = player2.nickname.c_str();
            DrawText(nick2, screenWidth - MeasureText(nick2, 20) - 20, GetScreenHeight() - 20, 20, RED);
        }
        EndDrawing();
        //----------------------------------------------------------------------------------
        shouldClose = WindowShouldClose();
    }

    while (!shouldClose) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        const char* msg = "Game Over";
        const int textWidth = MeasureText(msg, 20);
        DrawText(msg, screenWidth / 2.0f - textWidth / 2.0f, screenHeight / 2.0f - 10, 20, GRAY);
        shouldClose = WindowShouldClose();
        EndDrawing();
    }

    Shutdown();

	return 0;
}

void DrawBall(Ball ball) {
    float radius = ball.radius + (float)ball.bounceFrames * ball.bounceFrames;
    DrawCircleV(ball.position, radius, ball.ballColor);
    Vector2 start = ball.position;
    for (int i = 0; i < 4; i++) {
        DrawLineEx(start, ball.trail[i], 4 / (i + 1), ball.ballColor);
        start = ball.trail[i];
    }
}

void DrawBrick(Brick brick) {
    Rectangle rect = brick.rect();
    DrawRectangleRec(rect, brick.brickColor);
    Vector2 btmL = { rect.x, rect.y + rect.height };
    Vector2 topR = { rect.x + rect.width, rect.y };
    Vector2 points[6] = {
        { btmL.x, btmL.y },
        { btmL.x + 10, btmL.y + 10},
        { topR.x, btmL.y },
        { topR.x + 10, btmL.y + 10},
        { topR.x, topR.y },
        { topR.x + 10, topR.y + 10},
    };
    Color color = brick.brickColor;
    color.r *= 0.8;
    color.g *= 0.8;
    color.b *= 0.8;
    DrawTriangleStrip(points, 6, color);
}

void DrawPlayer(Player player) {
    Rectangle rect = player.rect();
    float bumpShrink = (float)player.bumpFrames;
    rect.x += bumpShrink;
    rect.y += bumpShrink;
    rect.width -= bumpShrink;
    rect.height -= bumpShrink;

    DrawRectangleRec(rect, player.playerColor);
    Vector2 btmL = { rect.x, rect.y + rect.height };
    Vector2 topR = { rect.x + rect.width, rect.y };
    Vector2 points[6] = {
        { btmL.x - 10, btmL.y - 10},
        { btmL.x, btmL.y },
        { btmL.x - 10, topR.y - 10},
        { btmL.x, topR.y},
        { topR.x - 10, topR.y - 10},
        { topR.x, topR.y}

        
       
       
    };
    Color color = player.playerColor;
    //color.r -= 10;
    //color.g -= 10;
    //color.b -= 10;

    DrawTriangleStrip(points, 6, BLACK);
}

void InitGame(bool isHost)
{
    gameOver = false;

    //Setting bricks to screen
    bricks = { screenWidth/ BRICK_COLUMNS,40};

    //Init Player
    player1.position = { screenWidth / 2, screenHeight * 7 / 8 };
    player1.size = { screenWidth / 10, 20 };
    player1.playerColor = { 0,0,255,255 };
    player1.id = 0;
    player1.active = true;
    player1.isMe = isHost;
    player1.hasAuthority = isHost;

    //Init Ball
    ball1.position = { screenWidth / 2, screenHeight * 7 / 10 };
    ball1.velocity = { -2,4 };
    ball1.ballColor = player1.playerColor;
    ball1.radius = 7.0;
    ball1.id = 1;
    ball1.ownerID = player1.id;
    ball1.active = true;
    ball1.hasAuthority = isHost;


    //Init Player 2
    player2.position = { screenWidth / 2, screenHeight * 7 / 8 };
    player2.size = { screenWidth / 10, 20 };
    player2.playerColor = { 255,0,0,255 };
    player2.id = 10;
    player2.active = true;
    player2.isMe = !isHost;
    player2.hasAuthority = !isHost;

    //Init Ball 2
    ball2.position = { screenWidth / 2, screenHeight * 7 / 10 };
    ball2.velocity = { 2,4 };
    ball2.ballColor = player2.playerColor;
    ball2.radius = 7.0;
    ball2.id = 11;
    ball2.ownerID = player2.id;
    ball2.active = true;
    ball2.hasAuthority = !isHost;

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

    physicsObjects[player1.id] = &player1;
    physicsObjects[player2.id] = &player2;
    physicsObjects[ball1.id] = &ball1;
    physicsObjects[ball2.id] = &ball2;
}

float RandomFloat(float min, float max)
{
    float t = max - min;
    float x = (float)rand() / (float)(RAND_MAX / 1.0f);

    float out = x * t + min;

    return out;
}

void UpdatePhysObjs() {
    for (int i = 0; i < NUM_PHYS_OBJ; i++) {
        PhysObject* obj = physicsObjects[i];
        if (obj)
            obj->position = Vector2Add(obj->position, obj->velocity);
        
    }
}

void UpdateBall(Player& owner, Ball& ball) {

    Vector2 startVel = ball.velocity;
    //Brick Collision
    for (int i = 0; i < BRICK_ROWS; i++)
    {
        for (int j = 0; j < BRICK_COLUMNS; j++)
        {
            if (brickList[i][j].isDead == false && CheckCollisionCircleRec(ball.position, ball.radius, brickList[i][j].rect()))
            {
                Vector2 normal = { ball.position.x - brickList[i][j].position.x, ball.position.y - brickList[i][j].position.y };
                normal = Vector2Normalize(normal);
                ball.velocity = Vector2Reflect(ball.velocity, normal);
                brickList[i][j].isDead = true;
                if (ball.hasAuthority) {
                    owner.score++;
                    owner.scoreFrames += 10;

                    TCPPacketDestroy testDestroy;
                    testDestroy.type = TCPPacketType::DESTROY;
                    testDestroy.X = i;
                    testDestroy.Y = j;

                    packetManager.SendPacket(masterSocket.get(), &testDestroy);
                }
            }
        }
    }


    //Player Collision
    if (CheckCollisionCircleRec(ball.position, ball.radius, owner.rect()))
    {
        ball.velocity.y *= -1;
        ball.velocity.x = (ball.position.x - owner.position.x) / (owner.size.x / 2) * 5;
        ball.position.y = owner.position.y - ball.radius - owner.size.x/2;
        owner.bumpFrames = 10;
    }

    //Wall Collision
    if (ball.position.x + ball.radius >= screenWidth)
    {
        ball.velocity.x *= -1;
        ball.position.x = screenWidth - ball.radius; // resolve pen
    } 
    else if (ball.position.x - ball.radius <= 0)
    {
        ball.velocity.x *= -1;
        ball.position.x = ball.radius;
    }

  

    

    if (ball.position.y - ball.radius >= screenHeight)
    {
        ball.position = { screenWidth / 2, screenHeight * 7 / 10 };
        ball.velocity = { 0,-4 };
    }
    else if (ball.position.y < 0)
    {
        ball.velocity.y *= -1;
        ball.position.y = ball.radius;
    }

   
    if (ball.framesTillRefresh == 0) {
        ball.framesTillRefresh = 10;
        ball.trail[3] = ball.trail[2];
        ball.trail[2] = ball.trail[1];
        ball.trail[1] = ball.trail[0];
        ball.trail[0] = ball.position;
    }
    ball.framesTillRefresh--;

    Vector2 velDelta = Vector2Subtract(startVel, ball.velocity);

    if (velDelta.x != 0 || velDelta.y != 0) {
        ball.bounceFrames = 5;
    }

    if (ball.hasAuthority && (startVel.x != ball.velocity.x || startVel.y != ball.velocity.y) ) {
        TCPPacketMove movePacket;
        movePacket.type = TCPPacketMove::TYPE;
        movePacket.objectID = ball.id;
        movePacket.position = ball.position;
        movePacket.velocity = ball.velocity;

        packetManager.SendPacket(masterSocket.get(), &movePacket);
    }

    if (ball.bounceFrames > 0) ball.bounceFrames--;
}


void UpdatePlayer(Player& player) {
    float startX = player.velocity.x;

    if (player.hasAuthority) {
        float speed = 5;
        //Player Movement
        if (IsKeyDown(KEY_A))
        {
            player.velocity.x -= speed;
        }

        if (IsKeyDown(KEY_D))
        {
            player.velocity.x += speed;
        }


        player.velocity.x = Clamp(player.velocity.x, -20, 20);



        if (player.velocity.x != startX) {
            TCPPacketMove movePacket;
            movePacket.type = TCPPacketMove::TYPE;
            movePacket.objectID = player.id;
            movePacket.position = player.position;
            movePacket.velocity = player.velocity;

            packetManager.SendPacket(masterSocket.get(), &movePacket);
        }
    }

    // apply damping force and prevent going through sides
    player.velocity.x = player.velocity.x * 0.8;
    if (player.position.x - player.size.x / 2 < 0)
    {
        player.position.x = player.size.x / 2;
        player.velocity.x = 0;
    }
    if (player.position.x + player.size.x / 2 > screenWidth)
    {
        player.position.x = screenWidth - player.size.x / 2;
        player.velocity.x = 0;
    }


    if (player.bumpFrames > 0)  player.bumpFrames--;
    if (player.scoreFrames > 0) player.scoreFrames--;
}

void UpdateGame()
{

   
    UpdatePlayer(player1);
    UpdatePlayer(player2);
   
    UpdateBall(player1, ball1);
    UpdateBall(player2, ball2);

    UpdatePhysObjs();

    //Reset
    gameOver = true;
    for (int i = 0; i < BRICK_ROWS; i++)
    {
        for (int j = 0; j < BRICK_COLUMNS; j++)
        {
            if (brickList[i][j].isDead == false)
            {
                gameOver = false;
            }
        }
    }
    if (gameOver)
    {
        InitGame(player1.isMe);
    }
}