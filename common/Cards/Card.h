#pragma once

#include <iostream>

class Card
{
public:

	enum Suit
	{
		INVALID_SUIT = -1,
		SPADES,
		CLUBS,
		HEARTS,
		DIAMONDS
	};

	enum Value
	{
		INVALID_VALUE = -1,
		ACE,
		TWO, 
		THREE,
		FOUR, 
		FIVE,
		SIX,
		SEVEN,
		EIGHT,
		NINE,
		TEN,
		JACK,
		QUEEN,
		KING
	};

	Card(Value val, Suit suit);
	Card() {};

	bool operator==(const Card& rhs) const;

	//returns true if value of card is less than that of rhs
	bool valLess(const Card& rhs) const;
	//returns true if value of lhs is less than that of rhs
	static bool valComp(const Card& lhs, const Card& rhs);
	//returns true if suit of card is less than that of rhs
	bool suitLess(const Card& rhs) const;
	//returns true if suit of lhs is less than that of rhs
	static bool suitComp(const Card& lhs, const Card& rhs);

	bool isValid() const;

	Value getValue() const { return mVal; };
	Suit getSuit() const { return mSuit; };

	friend std::ostream& operator<<(std::ostream& theStream, const Card& theCard);
private:
	Value mVal=Value::INVALID_VALUE;
	Suit mSuit=Suit::INVALID_SUIT;
};

std::ostream& operator<<(std::ostream& theStream, const Card& theCard);