#include <queue>

class Square {
public:
  std::string name;                 // Name of square
  char type;                        // p = property, u = utility, r = railroad, c = cards, s = special.
  int value;                        // Current value (cost of rent, etc.)
  std::vector<int> valueVector;     // cost, cost of houses/hotels, rent, rent w/ color set, 1 rent, 2 rent, 3 rent, 4 rent, hotel rent
  int isOwned;                      // Default zero. Change to number of player if property is owned.
  std::vector<int> players;         // Players currently on this square.
  int houses;                       // Number of houses
  int isMortgaged;                  // Mortgaged? (0 or 1)
  Square();
  Square(std::string n, char t, int v, std::vector<int> vV, int i, std::vector<int> p);
};

class Card {
public:
  std::string text;                 // Text display
  int value;                        // Value of card
  int traverse;                     // Square number, which is array index + 1. This is so that Go is traverse = 1, not zero.
  int everyPlayer;                  // Collect money from every player? (0 or 1)
  Card();
  Card(std::string t, int v, int tr, int e);
  Card(const Card& c);
};

class Board {
public:
  Square* squares[40];
  std::queue<Card*> communityChest;
  std::queue<Card*> chance;
  void init_Board();
  void draw_Board();
};

class Player {
public:
  int number;
  int money;
  int position;
  std::vector<Square*> properties;
  Player(int n, int m, int pn, std::vector<Square*> p);
};

// General game functions
void init_Board();
void draw_Board();
void play_game();
int play_again();
void initialize_players();
void player_lost(int p);
void player_move(int p);
int game_over();
// Player functions (optional moves)
void check_status(int p);
void check_space(int i);
void make_deal(int p);
void buy_houses(int p);
int* check_monopoly(int p);
void allocate_property(int i, int p);
void mortgage(int p);
void unmortgage(int p);
// Player functions (required moves)
void traverse_board(int p, int r);
void land_square(int p);
void draw_card(int p);
void railroad(int p);
void utility(int p);
