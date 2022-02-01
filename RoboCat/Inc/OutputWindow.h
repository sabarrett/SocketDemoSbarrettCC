
// Encapsulates a simulated "output window," where
// new messages are written from top to bottom
// and input is always written in a reserved
// buffer at the bottom of the screen.
//
// Doesn't really work on small console windows (less than 20 rows).
class OutputWindow
{
public:
	OutputWindow();
	void Write(std::string msg);
	void WriteFromStdin(std::string msg);

    void MoveBackwards(int lines = 1);
	void ClearLine(int y = 2);
	int Scroll(int lines);
	void MoveCursorToScreenBottom();

private:
	HANDLE output;
	short lineN;
};