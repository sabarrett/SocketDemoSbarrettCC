#include <RoboCatPCH.h>
#include <iostream>

OutputWindow::OutputWindow(InputProcessor inputProcessor) : inputCharCount(0), cursorPos(0), messageTail(0), bufferIn(), ProcessInput(inputProcessor)
{
    SetConsoleCP(1200);
    hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    DWORD hConsoleInputMode;
    DWORD hConsoleOutputMode;
    CONSOLE_CURSOR_INFO consoleOutputCursorInfo;

  

    GetConsoleMode(hConsoleInput, &hConsoleInputMode);
    GetConsoleMode(hConsoleOutput, &hConsoleOutputMode);
    GetConsoleCursorInfo(hConsoleOutput, &consoleOutputCursorInfo);

    DWORD newInputMode = 0;
    newInputMode |= ENABLE_WINDOW_INPUT;
    newInputMode |= ENABLE_MOUSE_INPUT;
    SetConsoleMode(hConsoleInput, newInputMode);

    DWORD newOutputMode = 0;

    SetConsoleMode(hConsoleOutput, newOutputMode);


    CONSOLE_SCREEN_BUFFER_INFO conInfo;
    GetConsoleScreenBufferInfo(hConsoleOutput, &conInfo);

    height = conInfo.srWindow.Bottom - conInfo.srWindow.Top + 1; 
    width = conInfo.srWindow.Right - conInfo.srWindow.Left + 1;

    messageCapacity = height - 2;
    SetConsoleScreenBufferSize(hConsoleOutput, COORD{ width, height });

    messageBuffer = new CHAR_INFO[width * height];
    for (int i = 0; i < width * height; i++) {
        messageBuffer[i].Char.UnicodeChar = 0;
        messageBuffer[i].Attributes = 0x07;
    }

    inputBuffer = new CHAR_INFO[width];
    for (int i = 0; i < width; i++) {
        inputBuffer[i].Char.UnicodeChar = 0;
        inputBuffer[i].Attributes = 0x07;
    }
}


void OutputWindow::Write(std::string msg, char color)
{
   

    
    CHAR_INFO* lineBuffer = getLine(messageTail);

    for (size_t i = 0; i < msg.size(); i++) {
        lineBuffer[i].Char.UnicodeChar = msg[i];
        lineBuffer[i].Attributes = color;
    }

    messageTail = (messageTail + 1) % (messageCapacity);

}


void OutputWindow::HandleEvents() {


    CONSOLE_SCREEN_BUFFER_INFO conInfo;
    GetConsoleScreenBufferInfo(hConsoleOutput, &conInfo);

    short currentHeight = conInfo.srWindow.Bottom - conInfo.srWindow.Top + 1;
    short currentWidth = conInfo.srWindow.Right - conInfo.srWindow.Left + 1;

    if (currentHeight != height || currentWidth != width) {
        width = currentWidth;
        height = currentHeight;
        SetConsoleScreenBufferSize(hConsoleOutput, COORD{ width, height });
        messageBuffer = new CHAR_INFO[width * height];
   
    }
    
    DWORD ret = WaitForSingleObject(hConsoleInput, 100);
    if (ret != 0) return;

    INPUT_RECORD buffer[10] = {};
    DWORD itemsRead;
    BOOL err = ReadConsoleInputW(hConsoleInput, buffer, 10, &itemsRead);




    for (DWORD i = 0; i < itemsRead; i++) {
        INPUT_RECORD input = buffer[i];
     
        switch (input.EventType) {
        case KEY_EVENT:
            HandleKeyEvent(input.Event.KeyEvent);
            break;
        case FOCUS_EVENT:
            break;
        case MOUSE_EVENT:
            break;
        case WINDOW_BUFFER_SIZE_EVENT:

            break;
        }


    }
}

void OutputWindow::HandleKeyEvent(KEY_EVENT_RECORD record) {
    if (!record.bKeyDown) return;
    //CHAR_INFO* inputBuffer = getLine(height - 1);

    WORD key = record.wVirtualKeyCode;

    if (key == VK_BACK) {
        if (inputCharCount == 0) return;
        inputCharCount--;
        cursorPos--;
        cursorMoved = true;
        inputChanged = true;
        bufferIn[inputCharCount] = 0;
        return;
    }

    if (key == VK_RETURN) {
        std::string input(bufferIn);

        memset(bufferIn, 0, sizeof(bufferIn));
        inputCharCount = 0;
        cursorPos = 0;
        cursorMoved = true;
        inputChanged = true;

        ProcessInput(input);
    }

    char c = key & 0xFFFF;
    bool upper = record.dwControlKeyState & SHIFT_PRESSED;


  
    // convert virtual codes to ascci
    // not sure why i felt the need to deal with this
    switch (key) {
    case VK_OEM_1: 
        c = upper ? ':' : ';';
        break;
    case VK_OEM_PLUS:
        c = upper ? '+' : '=';
        break;
    case VK_OEM_COMMA:
        c = upper ? '<' : ',';
        break;
    case VK_OEM_MINUS:
        c = upper ? '_' : '-';
        break;
    case VK_OEM_PERIOD:
        c = upper ? '>' : '.';
        break;
    case VK_OEM_2:
        c = upper ? '?' : '/';
        break;
    case VK_OEM_3:
        c = upper ? '~' : '`';
        break;
    case VK_OEM_4:
        c = upper ? '{' : '[';
        break;
    case VK_OEM_5:
        c = upper ? '|' : '\\';
        break;
    case VK_OEM_6:
        c = upper ? '}' : ']';
        break;
    case VK_OEM_7:
        c = upper ? '"' : '\'';
        break;
    }

    if (c < 32 || c > 126) return;

    if ( (c >= 'A' && c <= 'Z') && !upper)
       c += 32; // lowercase 

    inputChanged = true;
    bufferIn[inputCharCount] = c;
    inputCharCount++;
    cursorPos++;
    cursorMoved = true;
}



void OutputWindow::Draw() {
    if (cursorMoved) { // prevents cursor flicker when still
        COORD coord = { cursorPos, height - 1 };
        SetConsoleCursorPosition(hConsoleOutput, coord);
        cursorMoved = false;
    }

    if (inputChanged) {
        int i = 0;
        for (; i < width; i++) {
            inputBuffer[i].Char.UnicodeChar = bufferIn[i];
            inputBuffer[i].Attributes = 0x07; // white
        }
        inputChanged = false;
    }


    COORD messageBufferSize = { width, height };

    // 
    {
        SMALL_RECT writeRegion = {
            0,
            0,
            width - 1,
            messageCapacity - messageTail-1,
        };

        WriteConsoleOutputW(hConsoleOutput, messageBuffer, messageBufferSize, { 0, messageTail }, &writeRegion);
    }
    // message buffer bottom section
    {
        SMALL_RECT writeRegion = {
            0,
            messageCapacity - messageTail,
            width - 1,
            messageCapacity - 1,
        };
        WriteConsoleOutputW(hConsoleOutput, messageBuffer, messageBufferSize, { 0, 0 }, &writeRegion);

    }
 

   

     {
        COORD bufferSize = { width, 1 };
        COORD bufferCoord = { 0, 0 };
        SMALL_RECT writeRegion = {
            0,
            height-1,
            width-1,
            height,
        };

        BOOL err = WriteConsoleOutputW(hConsoleOutput, inputBuffer, bufferSize, bufferCoord, &writeRegion);
        if (err == 0) {
            DWORD err_num = GetLastError();

            int i = 0;
        }
    }

}