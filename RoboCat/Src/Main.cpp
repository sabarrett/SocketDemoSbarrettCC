

#include "RoboCatPCH.h"

const int screenWidth = 800;
const int screenHeight = 450;

enum class StartupScreenAction {
    QUIT,
    CONNECT,
    HOST,
};

StartupScreenAction StartupScreen(SocketAddress* out_address) {
    bool complete = false;
    float centerX = screenWidth / 2.0f;
    float btnWidth = 400;

    Rectangle textBox = { centerX - (btnWidth * 0.5), 180, (btnWidth * 0.7), 50 };
    Rectangle connectBtn = { centerX + (btnWidth * 0.25), 180, (btnWidth * 0.25), 50 };
    Rectangle hostBtn = { centerX - (btnWidth * 0.5), 270, btnWidth, 50 };

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




    // Initialization
    //--------------------------------------------------------------------------------------


    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

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

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
       

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    Shutdown();

	return 0;
}


float randFloat(float min, float max) {
    float t = max - min;
    float r = (float)rand() / (float)(RAND_MAX / 1.0f); // 0 -> 1

    float out = r * t + min;

    return out;
}