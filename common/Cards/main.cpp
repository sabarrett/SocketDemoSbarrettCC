#include "Card.h"
#include "Deck.h"
#include "Hand.h"

#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>

int main()
{
/*pseudo code
	Seed the random number generator with time
	Ask the user if they would like to use a test deck or a standard deck
	Create a Deck (of the correct type) - test deck contains only 9, 10, J, Q, K's in just Spades and Hearts
	Shuffle the Deck
	ask the user how may hands they wish to have dealt - store this as numHands
	do this numHands times
		deal 5 cards to the hand
		if deck runs out of cards in the middle of the deal clear the hand, shuffle the deck, and re-deal all 5 cards
		write out the contents of the hand
		evaluate what multiples the hand contains and write them out
		evaluate if the hand contains a straight - if so write it out
		evaluate if the hand contains a flush - if so write it out
		clear the hand before the next deal
*/
	system("pause");
	return 0;
}