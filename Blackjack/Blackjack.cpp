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
	std::string owner = "PLAYER";
	std::string type = "hard";
	std::vector<Card> cards;
	int aceCount = 0;

	//methods
	void draw(std::vector<Card>& deck) {
		this->cards.push_back(deck.back());
		if (deck.back().name.find("Ace") != std::string::npos) {
			this->aceCount++;
		}
		deck.pop_back();
		bestSoftHand();
	}

	void deal(std::vector<Card>& deck) {
		for (int i = 0; i < 2; i++) {
			this->draw(deck);
		}
	}

	void show(bool visible = 1) {	
		if (visible == 1 || this->owner == "PLAYER") {	
			std::cout << this->owner;
			std::cout << " CARDS (" << this->cards.size() << ", " << this->type << "): ";
			for (const Card& card : this->cards) {
				std::cout << "[" << card.name << "] ";
			}
			std::cout << std::endl;
			std::cout << this->owner;
			std::cout << " TOTAL: " << this->total();
		}
		else if (visible == 0 || this->owner == "DEALER") {
			std::cout << this->owner;
			std::cout << " CARDS (" << this->cards.size() << "): ";
			std::cout << "[" << this->cards[0].name << "] ";
			for (int i = 0; i < this->cards.size() - 1; i++) {
				std::cout << "[ ? ] ";
			}
			std::cout << std::endl;
			std::cout << this->owner;
			std::cout << " TOTAL: " << this->cards[0].value;
			for (int i = 0; i < this->cards.size() - 1; i++) {
				std::cout << " + ?";
			}
		}
		std::cout << std::endl;
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

	void bestSoftHand() {
		if (this->aceCount == 0) { return; }                 //if hand has no aces return
		if (this->type == "hard") { this->type = "soft"; }   //change handtype to soft if it's hard

		int total = this->total();
		size_t handSize = this->cards.size(); //size_t to avoid "possible loss of data"
		int aces = this->aceCount;

		if (total < 21) {
			while (aces-- && (total + 10 <= 21)) {
				for (int i = 0; i < handSize; i++) {
					if (this->cards[i].name.find("Ace") != std::string::npos) {
						this->cards[i].value = 11;
					}
				}
			}
		}
		else if (total > 21) {
			while (aces-- && (total > 21)) {
				for (int i = 0; i < handSize; i++) {
					if (this->cards[i].name.find("Ace") != std::string::npos) {
						this->cards[i].value = 1;
					}
				}
			}
		}
		else if (total == 21) { return; }
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
	std::cout << std::endl;
	visibleHand.show();
	hiddenHand.show(0);
	std::cout << std::endl;
}

void askInsurance(Player& player) {
	std::cout << "Dealer's has an ace. Enter insurance bet ";
	std::cout << "(max " << player.bet/2 << " or 0 for none) : ";
	std::cin >> player.insuranceBet;
	while (player.insuranceBet > player.bet / 2) {
		std::cout << "Insurance bet must be no higher than half the original bet (" << player.bet << ")!\n";
		std::cin >> player.insuranceBet;
	}
	if (player.insuranceBet != 0) {
		player.insurance = 1;
		std::cout << "Made insurance bet of " << player.insuranceBet << "!\n";
	}
}

void insuranceCheck(bool& insurance, int& insuranceBet, Player& player) {
	if (insurance == 1) {
		int insuranceWin = insuranceBet * 2;
		player.balance += insuranceWin;
		std::cout << "Won " << insuranceWin << "$ from insurance side bet!\n";
	}
}

bool checkBlackjack(Player& player, Player& dealer, bool& insurance, int& insuranceBet) {
	bool blackjack = 1;
	if (player.hand.total() == 21 && dealer.hand.total() == 21) {
		std::cout << "DRAW (Double blackjack) " << "Bet of " << player.bet << " returned.\n";;
		player.balance += player.bet;
		insuranceCheck(insurance, insuranceBet, player);
		std::cout << std::endl;
		dealer.hand.show();
		std::cout << std::endl;
	}
	else if (player.hand.total() == 21) {
		std::cout << "PLAYER BLACKJACK!! You won " << player.bet + (player.bet*3) / 2 << "$!\n";
		player.balance += player.bet + (player.bet * 3) / 2;
		std::cout << std::endl;
		dealer.hand.show();
		std::cout << std::endl;
	}
	else if (dealer.hand.total() == 21) {
		std::cout << "DEALER BLACKJACK! Lost "<< player.bet << "$ main bet.\n";
		insuranceCheck(insurance, insuranceBet, player);
		std::cout << std::endl;
		dealer.hand.show();
		std::cout << std::endl;
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
		std::cout << "\nError: No choice\n\n";
		return;
	}
	int playerHand = player.hand.total();
	int dealerHand = dealer.hand.total();

	std::string compare = "HANDS - PLAYER: " + std::to_string(playerHand) + "  DEALER: " + std::to_string(dealerHand) + "\n";

	if (dealerHand > 21) {
		std::cout << "YOU WIN " << player.bet * 2 << "$! (dealer bust) " << compare << "\n\n";
		player.balance += player.bet * 2;
		return;
	}
	else if (playerHand > 21) {
		std::cout << "YOU LOSE " << player.bet << "$. (player bust) " << compare << "\n\n";
		return;
	}

	if (playerHand == dealerHand) {
		std::cout << "DRAW (equal hands) " << "Bet of " << player.bet << " returned." << compare << "\n\n";
		player.balance += player.bet;
	}
	else if (playerHand > dealerHand) {
		std::cout << "YOU WIN " << player.bet * 2 << "$! (hand higher than dealer) " << compare << "\n\n";
		player.balance += player.bet * 2;
	}
	else if (playerHand < dealerHand) {
		std::cout << "YOU LOSE " << player.bet << "$. (dealer hand is higher) " << compare << "\n\n";
	}
}

void dealerAutoplay(Player& player, Player& dealer, std::vector<Card>& deck) {
	if (player.hand.total() == 21) {
		return;
	}
	while (dealer.hand.total() < 17) {
		dealer.hand.draw(deck);
		dealer.hand.show(0);
		std::cout << std::endl;
	}
}

int main()
{
	int minimumBet = 10;
	std::vector<Card> deck;
	Player player = { "PLAYER" }, dealer = { "DEALER" };
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

		//if blackjack, check who won etc. then start next round (back to game loop)
		if (checkBlackjack(player, dealer, player.insurance, player.insuranceBet) == 1) {
			continue;
		}

		//if dealer has ace, ask if player wants insurance
		if (dealer.hand.cards[0].name.find("Ace") != std::string::npos) {
			askInsurance(player);
		}

		//turn loop 
		int turn = 0;
		char choice = 'u';
		while (player.hand.total() <= 21 && dealer.hand.total() <= 21) {
			turn++;
			
			std::string options = ((turn == 1) ? ("Hit - h, Stand - s, Double-down - d, Surrender - x") : "Hit - h, Stand - s");
			std::cout << "[Bet $" << player.bet << "] ";
			std::cout << "Select choice (" << options << "): ", std::cin >> choice;
			
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
				player.hand.show();
				dealerAutoplay(player, dealer, deck);
				break; //ends the round loop after dealer finishes drawing
			}
			else if (turn != 1) {
				while (choice == 'd') {
					std::cout << "Can only double-down during first turn on the original cards. ";
					std::cout << "Options: Hit - h, Stand - s\n";
					std::cout << "Enter choice: ", std::cin >> choice;
				}
			} //end of double down

			if (choice == 's') {
				dealerAutoplay(player, dealer, deck);
				break;
			}

			if (choice == 'h') {
				player.hand.draw(deck);
			}

			int dealerHand = dealer.hand.total();
			if (dealerHand >= 17 && choice == 's') {
				break;
			}
			if (dealerHand < 17) {
				dealer.hand.draw(deck);
			}

			showHands(player.hand, dealer.hand);

		} //end of turn loop

		std::cout << std::endl;
		dealer.hand.show();
		std::cout << std::endl;

		roundOutcome(player, dealer, choice);

	} // end of game loop

	if (player.balance < minimumBet) {
		std::cout << "======================\nGame over!\nThe house always wins\n======================";
	}
};
