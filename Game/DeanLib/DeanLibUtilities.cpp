#include "DeanLibUtilities.h"

using namespace std;

string peekString(ifstream& stream)
{
	if (!stream.good() || stream.eof())
	{
		return "";
	}

	int charCnt = 0;
	auto currPos = stream.cur;

	string theString;
	while (!stream.eof())
	{
		char c;
		stream >> c;
		if (c != ' ')
		{
			charCnt++;
			theString += c;
		}
	}

	if (charCnt > 0)
	{
		stream.seekg(currPos + charCnt);
		return theString;
	}
	else
	{
		return "";
	}
}

int peekInt(std::ifstream& stream)
{
	return 0;
}

