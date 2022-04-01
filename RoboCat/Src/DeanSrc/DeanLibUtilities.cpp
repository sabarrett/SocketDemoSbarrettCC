#include "DeanLibUtilities.h"
#include <sstream>

using namespace std;

string peekString(istream& stream)
{
	if (!stream.good() || stream.eof())
	{
		return "";
	}

	int charCnt = 0;
	auto currPos = stream.tellg();

	string theString;

	getline(stream, theString);

	stringstream sStream(theString);

	string extractedString;
	sStream >> extractedString;

	stream.seekg(currPos);
	return extractedString;
}

int peekInt(std::istream& stream)
{
	string extractedString = peekString(stream);
	int theInt = 0;
	stringstream sStream(extractedString);
	sStream >> theInt;
	return theInt;
}

