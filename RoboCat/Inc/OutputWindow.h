
// Encapsulates a simulated "output window," where
// new messages are written from top to bottom
// and input is always written in a reserved
// buffer at the bottom of the screen.
//
// Doesn't really work on small console windows (less than 20 rows).
class OutputWindow
{
public:
	using InputProcessor = void (*)(std::string input);

	OutputWindow(InputProcessor inputProcessor);
	void Write(std::string msg, char color = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN /* white */);
	void WriteFromStdin(std::string msg);

    void MoveBackwards(int lines = 1);
	void ClearLine(int y = 2);
	int Scroll(int lines);
	void MoveCursorToScreenBottom();


	void HandleEvents();
	

	void Draw();


private:
	//void ProcessInput();
	void HandleMessage(std::string);
	void HandleCommand(std::string);
	void HandleKeyEvent(KEY_EVENT_RECORD);

	CHAR_INFO* getLine(short y) { return messageBuffer + (width * y); }

	HANDLE hConsoleInput;
	HANDLE hConsoleOutput;

	bool inputChanged;
	char bufferIn[500];

	short width, height;

	short messageTail;
	short messageCapacity;
	CHAR_INFO* messageBuffer;


	CHAR_INFO* inputBuffer;

	
	InputProcessor ProcessInput;
	short inputCharCount;
	short cursorPos;
	bool cursorMoved;
};