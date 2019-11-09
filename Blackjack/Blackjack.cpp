#include <iostream>
#include <vector>
#include <cstdlib>
#include <random>
#include <string>
#include <unordered_map>

struct Card {
	//attributes
	std::string name;
	int value;

	//constructor methods
	Card(std::string x, int y) {
		name = x;
		value = y;
	}
	Card(std::string x) {
		name = x;
	}
	Card(int x) {
		value = x;
	}
};

class Hand {
public:
	//attributes
	std::string owner = "Player";
	std::string type = "hard";
	std::vector<Card> cards;

	//methods
	void draw(std::vector<Card>& deck) {
		this->cards.push_back(deck.back());
		if (deck.back().value == 1) {
			this->calculateSoftHand();
		}
		deck.pop_back();
	}

	void deal(std::vector<Card>& deck) {
		for (int i = 0; i < 2; i++) {
			this->draw(deck);
		}
	}

	void show(bool visible = 1) {	
		if (visible == 1) {	
			std::cout << this->owner << " hand: ";
			int total = 0;
			for (const Card& card : this->cards) {
				std::cout << "[" << card.name << "] ";
				total += card.value;
			}
			std::cout << "## TOTAL: " << total << " [" << this->type << "]" << std::endl;
		}
		else if (visible == 0) {
			std::cout << this->owner << " hand: ";
			std::cout << "[" << this->cards[0].name << "] ";
			for (int i = 0; i < this->cards.size() - 1; i++) {
				std::cout << "[ ? ] ";
			}
			std::cout << "##   TOTAL: " << this->cards[0].value;
			for (int i = 0; i < this->cards.size() - 1; i++) {
				std::cout << " + ?";
			}
		}
	}

	int total() {
		int total = 0;
		for (const Card& card : this->cards) {
			total += card.value;
		}
		return total;
	}

	void reset() {
		cards.clear();
	}

	void calculateSoftHand() {
		this->type = "soft";
		int aceCount = 0;
		size_t handSize = this->cards.size(); //size_t to avoid "possible loss of data"
		for (int i = 0; i < handSize; i++) {
			if (this->cards[i].value == 1) {
				aceCount++;
			}
		}
		int total = this->total();

		//calculate best hand
		//as long as there are aces in the hand, and converting next ace won't bust
		//find the next ace in the hand, and change its value to 11
		while (aceCount-- && (total + 10 <= 21) ) {
			for (int i = 0; i < handSize; i++) {
				if (this->cards[i].value == 1) {
					this->cards[i].value = 11;
				}
			}
		}
	}

	//constructors
	Hand() {};
};

class Player : public Hand {
public:
	//attributes
	int balance = 0;
	int bet = 0;
	bool insurance = 0;
	int insuranceBet = 0;
	Hand hand;

	//methods
	void makeBet(int& minimumBet) {
		std::cout << "Enter bet amount: ";
		std::cin >> this->bet;
		std::cout << std::endl;

		while (!std::cin || ((this->bet < minimumBet) || (this->bet > balance))) {
			//if the input exceeds min/max limits, console won't be spammed
			std::cin.clear(); // clears the error flag (so that future I/O will work correctly)
			//cin ignore: number of characters to ignore, delimiter (\n)
			//numeric_limits<streamsize>::max() is upper limit on the size of a stream, effectively no limit
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			if (!std::cin) {
				std::cout << "Bet must be a number from " << minimumBet << " to " << this->balance << ".";
			}
			if (this->bet > this->balance) {
				std::cout << "[Bet: " << this->bet << "] Not enough balance! (Current: " << this->balance << ")";
			}
			else if (this->bet < 1) {
				std::cout << "[Bet: " << this->bet << "] Bet must be a number of " << minimumBet << " or higher.";
			}
			else if (this->bet < minimumBet) {
				std::cout << "[Bet: " << this->bet << "] Minimum bet is " << minimumBet << "!";
			}
			std::cout << "\nEnter new bet: ";
			std::cin >> this->bet;
		}
		this->balance -= this->bet;
	}

	//constructors
	Player(std::string type, int balance, int bet, Hand hand) {
		this->hand.owner = type;
		balance = balance;
		bet = bet;
		hand = hand;
	};
	Player(std::string x) {
		this->hand.owner = x;
	}
	Player(Hand hand) {
		hand = hand;
	}
	Player() {};
};

void generateDeck(std::vector<Card>& deck) {
	std::random_device random; //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(random()); //Standard mersenne_twister_engine seeded with random()
	//std::uniform_int_distribution<> dis(1, 13); // dis(gen) //std::shuffle(v.begin(), v.end(), gen);

	std::unordered_map<int, std::string> ranks = {
		{1, "Ace"}, {2, "Two"}, {3, "Three"}, {4, "Four"}, {5, "Five"}, {6, "Six"}, {7, "Seven"},
		{8, "Eight"}, {9, "Nine"}, {10, "Ten"}, {11, "Jack"}, {12, "Queen"}, {13, "King"}
	};
	std::unordered_map<int, std::string> suits = {
		{0, "Spades"}, {1, "Hearts"}, {2, "Diamonds"}, {3, "Clubs"}
	};

	
	for (int i = 1; i <= 10; i++) {
		for (int j = 0; j < 4; j++) {
			//assign value using int type constructor
			Card tempCard = { std::min(i, 10) }; //face cards are all worth 10
			std::string tempName;
			tempName = ranks[i] + " of " + suits[j];
			tempCard.name = tempName; //assign name
			deck.push_back(tempCard);
		}
	}

	//shuffle deck before returning
	std::shuffle(deck.begin(), deck.end(), gen);
}

void showHands(Hand& visibleHand, Hand& hiddenHand) {
	std::cout << "============================================================================\n";
	visibleHand.show();
	hiddenHand.show(0);
	std::cout << "\n============================================================================\n";
}

void askInsurance(Player& player) {
	std::cout << "Dealer's has an ace. Enter insurance bet (or 0 for none): ";
	std::cin >> player.insuranceBet;
	while (player.insuranceBet > player.bet / 2) {
		std::cout << "Insurance bet must be no higher than half the original bet (" << player.bet << ")!\n";
		std::cin >> player.insuranceBet;
	}
	if (player.insuranceBet != 0) {
		player.insurance = 1;
	}
}

void insuranceCheck(bool& insurance, int& insuranceBet, Player& player) {
	if (insurance == 1) {
		int insuranceWin = insuranceBet * 2;
		player.balance += insuranceWin;
		std::cout << "Won " << insuranceWin << "$ from insurance side bet!";
	}
}

bool checkBlackjack(Player& player, Player& dealer, bool& insurance, int& insuranceBet) {
	bool blackjack = 1;
	if (player.hand.total() == 21 && dealer.hand.total() == 21) {
		std::cout << "PUSH - PLAYER AND DEALER BLACKJACK\n";
		std::cout << "Bet of " << player.bet << " returned.\n";
		player.balance += player.bet;
		insuranceCheck(insurance, insuranceBet, player);
	}
	else if (player.hand.total() == 21) {
		std::cout << "PLAYER BLACKJACK!! You won " << player.bet + (player.bet*3) / 2 << "$!\n";
		player.balance += player.bet + (player.bet * 3) / 2;
	}
	else if (dealer.hand.total() == 21) {
		std::cout << "Dealer blackjack. ";
		insuranceCheck(insurance, insuranceBet, player);
	}
	else {
		blackjack = 0;
	}
	return blackjack;
}

void roundOutcome(Player& player, Player& dealer, char choice) {
	if (choice == 'x') {
		return;
	}
	if (choice == 'u') {
		std::cout << "\nError: No choice\n";
		return;
	}
	int playerHand = player.hand.total();
	int dealerHand = dealer.hand.total();

	if (dealerHand > 21) {
		std::cout << "Dealer BUST, you win " << player.bet * 2 << "$!\n";
		player.balance += player.bet * 2;
		return;
	}
	else if (playerHand > 21) {
		std::cout << "Player BUST, bet lost.\n";
		return;
	}

	if (playerHand == dealerHand) {
		std::cout << "PUSH - Hands value is equal.\n";
		player.balance += player.bet;
	}
	else if (playerHand > dealerHand) {
		std::cout << "PLAYER WINS! You won " << player.bet * 2 << "$!\n";
		player.balance += player.bet * 2;
	}
	else if (playerHand < dealerHand) {
		std::cout << "Dealer wins.\n";
	}
}

int main()
{
	int minimumBet = 10;
	std::vector<Card> deck;
	Player player = { "Player" }, dealer = { "Dealer" };
	player.balance = 1000;

	//game loop
	while (player.balance > minimumBet) {
		player.hand.reset();
		dealer.hand.reset();
		player.insurance = 0;
		std::cout << "Current balance: " << player.balance << "$";
		std::cout << std::endl;
		player.makeBet(minimumBet);

		generateDeck(deck);
		player.hand.deal(deck);
		dealer.hand.deal(deck);

		showHands(player.hand, dealer.hand);
		std::cout << std::endl;

		//if dealer has ace, ask if player wants insurance
		if (dealer.hand.cards[0].value == 1) {
			askInsurance(player);
		}

		//if blackjack, check who won etc. then start next round (back to game loop)
		if (checkBlackjack(player, dealer, player.insurance, player.insuranceBet) == 1) {
			continue;
		}

		//turn loop 
		int turn = 0;
		char choice = 'u';
		while (player.hand.total() <= 21 && dealer.hand.total() <= 21) {
			turn++;
			std::cout << ((turn == 1) ? "Options: Hit - h, Stand - s, Double-down - d, Surrender - x\n" : "Options: Hit - h, Stand - s\n");
			std::cout << "Enter choice: ", std::cin >> choice; //s (or any other input) = do nothing
			
			if (choice == 'x') {
				player.balance += player.bet / 2;
				std::cout << "Hand withdrew - You got " << player.bet / 2 << "$ back (half the original bet).\n";
				break;
			}

			//double down
			if (choice == 'd' && turn == 1) {
				player.balance -= player.bet;
				player.bet += player.bet;
				player.hand.draw(deck);
				std::cout << "Bet increased to: " << player.bet << ".\n";
				if (player.hand.total() == 21) {
					break;
				}
				while (dealer.hand.total() < 17) {
					dealer.hand.draw(deck);
					dealer.hand.show(0);
				}
				break; //ends the round loop after dealer finishes drawing
			}
			else if (turn != 1) {
				while (choice == 'd') {
					std::cout << "Can only double-down during first turn on the original cards. ";
					std::cout << "Options: Hit - h, Stand - s\n";
					std::cout << "Enter choice: ", std::cin >> choice;
				}
			} //end of double down

			if (choice == 'h') {
				player.hand.draw(deck);
			}

			int dealerHand = dealer.hand.total();
			if (dealerHand >= 17 && choice != 'h') {
				break;
			}
			if (dealerHand < 17) {
				dealer.hand.draw(deck);
			}

			showHands(player.hand, dealer.hand);

		} //end of turn loop

		dealer.hand.show();

		roundOutcome(player, dealer, choice);

	} // end of game loop

	if (player.balance < minimumBet) {
		std::cout << "======================\nGame over!\nThe house always wins\n======================";
	}
};
