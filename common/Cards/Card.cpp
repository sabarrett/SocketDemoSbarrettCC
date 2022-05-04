#include "Card.h"

Card::Card(Value val, Suit suit)
	:mVal(val)
	, mSuit(suit)
{
}

bool Card::operator==(const Card& rhs) const
{
	return (mVal == rhs.mVal && mSuit == rhs.mSuit);
}

bool Card::valLess(const Card& rhs) const
{
	return mVal < rhs.mVal;
}

//static
bool Card::valComp(const Card& lhs, const Card& rhs)
{
	return lhs.valLess(rhs);
}

bool Card::suitLess(const Card& rhs) const
{
	return mSuit < rhs.mSuit;
}

//static
bool Card::suitComp(const Card& lhs, const Card& rhs)
{
	return lhs.suitLess(rhs);
}

bool Card::isValid() const
{
	return (mVal >= ACE && mVal <= KING && mSuit >= SPADES && mSuit <= DIAMONDS);
}

std::ostream& operator<<(std::ostream& theStream, const Card& theCard)
{
	static char suitChar[4] = { 'S', 'C', 'H', 'D' };
	static char valChar[13] = { 'A', '2', '3', '4', '5', '6', '7', '8', '9', 'X', 'J', 'Q', 'K' };
	theStream << valChar[theCard.mVal] << suitChar[theCard.mSuit] << ' ';
	return theStream;
}
