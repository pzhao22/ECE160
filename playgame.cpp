#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <vector>
#include <algorithm>
#include "monopoly.h"
using namespace std;


Player::Player(int n, int m = 1500, int pn = 0, vector<Square*> p = {}, int j = 0)
{
  number = n;
  money = m;
  position = pn;
  properties = p;
  jailtime = 0;
}

vector<Player> playerVector;  // Global variable defining all players
int turn = 0;                 // Turn number, starts at 0. global b/c used in multiple functions
Board boardtest;              // Global board
int doublesCount = 0;

// Initialize 4 players starting at board position 0.
// DEMO!
// income tax = 4, chance = 7, utilities = 12,
// Community chest = 17, Go to jail = 30
// Test 4, 7, 12, 17, 30
void initialize_players()
{
  for (int i = 1; i < 5; i++)
  {
    //playerVector.push_back(Player(i,100));
    playerVector.push_back(Player(i));
  }
}

void play_game()
{
  boardtest.init_Board();
  initialize_players();
  turn = 0;
  while(!game_over())
  {
    turn = turn % playerVector.size();
    player_move(turn);   // Vector index of player
    turn++;
  }
  return;
}


int play_again()
{
  char response;
  cout << "Would you like to play again? (y/n): ";
  cin >> response;
  if (response == 'y' || response == 'Y')
  {
    return 1;
  }
  return 0;
}

// Player moves on their turn.
// Account for rolling doubles.
void player_move(int p)
{
  cout << endl;
  char response = ' ';
  // DEMO!
  // For testing, change roll1 and roll2 to 0.
  //int roll1 = 0;
  //int roll2 = 0;
  int roll1 = rand() % 6 + 1;
  int roll2 = rand() % 6 + 1;

  // If player is in jail.
  if (playerVector[p].position == 10 && playerVector[p].jailtime > 0) {
    jail_logic(p);
    return;
  }
  cout << "Player " << playerVector[p].number << "'s turn!" << endl;
  while (response != 'r' && response != 'R') {
    cout << "Roll dice (r): ";
    cin >> response;
  }
  cout << "Rolling..." << endl;
  usleep(2000000);
  cout << "You rolled a " << roll1 << " and a " << roll2 <<  endl;

  if (doublesCount == 2 && roll1 == roll2) {
    cout << "You rolled doubles three times. Go to jail." << endl;
    traverse_board(p,playerVector[p].position+(30-playerVector[p].position));
    return;
  }

  int now = playerVector[p].position + roll1 + roll2;

  traverse_board(p, roll1+roll2);

  land_square(p);
  if (playerVector[p].money <= 0)
  {
    player_lost(p);
    return;
  }

  check_status(p);

  // DEMO ONLY roll1 is not 0
  // Roll doubles, roll again. Unless you landed on go to jail.
  if (roll1 == roll2 && now != 30 && roll1 != 0)
  {
    doublesCount++;
    player_move(p);
  }
  doublesCount = 0;
  cout << endl;
  return;
}

void jail_logic(int p) {
  // Last turn in jail.
  if (playerVector[p].jailtime == 1) {
    cout << "Player " << playerVector[p].number << "'s last turn in jail. Pay $50 to get out." << endl;
    playerVector[p].money -= 50;
    if (playerVector[p].money <= 0) {
      cout << "Insufficient funds!" << endl;
      player_lost(p);
      return;
    }
    else {
      player_move(p);
    }
  }
  char response;
  cout << "Player " << playerVector[p].number << " is in jail. Would you like to pay $50 to get out of jail? (y/n): ";
  cin >> response;
  if (response == 'y' || response == 'Y') {
    if (playerVector[p].money <= 50) {
      cout << "Insufficient funds." << endl;
    }
    else {
      cout << "You are now just visiting jail." << endl;
      playerVector[p].money -= 50;
      playerVector[p].jailtime = 0;
      check_status(p);
      return;
    }
  }
  cout << "Roll doubles to get out of jail (r): ";
  int roll1 = rand() % 6 + 1;
  int roll2 = rand() % 6 + 1;
  char response2;
  cin >> response2;
  while (response2 != 'r' && response2 != 'R') {
    cout << "Roll dice (r): ";
    cin >> response2;
  }
  cout << "Rolling..." << endl;
  usleep(2000000);
  cout << "You rolled a " << roll1 << " and a " << roll2 <<  endl;
  if (roll1 == roll2) {
    cout << "Doubles! Move forward." << endl;
    playerVector[p].jailtime = 0;
    traverse_board(p,roll1+roll2);
    land_square(p);
    check_status(p);
  }
  else {
    cout << "Not doubles. Stay in jail." << endl;
    playerVector[p].jailtime--;
  }
}

// Player lost. Either transfer their stuff to the bank or to the player that bankrupted them.
void player_lost(int p) {
  cout << "Player " << playerVector[p].number << " lost. ";
  int p2 = boardtest.squares[playerVector[p].position]->isOwned;
  if(p2 != 0) {
    cout << "Player " << p2 << " gets their possessions." << endl;
    for (int i = 0; i < 40; i++) {
      if (boardtest.squares[i]->isOwned == playerVector[p].number) {
        boardtest.squares[i]->houses = 0;                                     // Reset to no houses
        boardtest.squares[i]->value = boardtest.squares[i]->valueVector[2];    // Reset to no house value
        boardtest.squares[i]->isOwned = p2;
      }
    }
  }
  else {
    cout << "Their properties are now up for grabs!" << endl;
    for (int i = 0; i < 40; i++) {
      if (boardtest.squares[i]->isOwned == playerVector[p].number) {
        boardtest.squares[i]->isOwned = 0;
        boardtest.squares[i]->isMortgaged = 0;
        boardtest.squares[i]->houses = 0;
        boardtest.squares[i]->value = boardtest.squares[i]->valueVector[0];
      }
    }
  }
  Square* ptr = boardtest.squares[playerVector[p].position];
  ptr->players.erase(remove(ptr->players.begin(), ptr->players.end(), playerVector[p].number), ptr->players.end()); // Erase sprite
  playerVector.erase(playerVector.begin()+p);      // Erase current player

  turn --;
  return;
}

// Move player, update the board state. Int p is the vector index of player, r is the roll of the player
void traverse_board(int p, int r)     // p is the player vector index, r is the dice roll
{
    int prev = playerVector[p].position;
    playerVector[p].position += r;
    int now = playerVector[p].position;

    if (now > 39)
    {
      cout << "You passed go. Collect $200." << endl;
      playerVector[p].money += 200;
      playerVector[p].position = playerVector[p].position % 40;
      now = now % 40;
    }

    Square* ptr = boardtest.squares[prev];
    Square* ptr1 = boardtest.squares[now];

    // Jail logic starts here
    if (now == 30)
    {
      now = 10;
      playerVector[p].position = 10;

      ptr = boardtest.squares[prev];
      ptr->players.erase(remove(ptr->players.begin(), ptr->players.end(), playerVector[p].number), ptr->players.end());  //Remove player p from old square

      ptr1 = boardtest.squares[now];
      ptr1->players.push_back(playerVector[p].number);           // Add player p to new square

      boardtest.draw_Board();
      cout << "Go to jail." << endl;
      playerVector[p].jailtime = 3;           // Player spends 3 turns in jail.
      return;
    }

    ptr->players.erase(remove(ptr->players.begin(), ptr->players.end(), playerVector[p].number), ptr->players.end());  //Remove player p from old square
    ptr1->players.push_back(playerVector[p].number);           // Add player p to new square

    boardtest.draw_Board();

    cout << "Player " << playerVector[p].number << " moved from " << ptr->name << " (" << prev + 1 << ")" << " to " << ptr1->name << " (" << now + 1 << ")" << endl;

    return;
}

// Player's available checks/options every turn.
void check_status(int p) {
  char response;
  char again = 'y';
  while (again == 'y' || again == 'Y') {
    cout << "Spaces (s) Balance (b) Properties (p) Deal (d) Mortgage (m) Unmortgage (u) Houses (h): ";
    cin >> response;
    if (response == 's' || response == 'S') {
      int i = 0;
      cout << "Enter a boardspace (Number between 1 and 40): ";
      cin >> i;
      while(cin.fail()) {
        cout << "Invalid input. Try again: ";
        cin.clear();
        cin.ignore(256,'\n');
        cin >> i;
    }
      if (i <= 40 && i >= 1) {
        check_space(i);
      }
      else {
        cout << "Invalid square." << endl;
      }
    }
    else if (response == 'm' || response == 'M') {
      mortgage(p);
    }
    else if (response == 'u' || response == 'U') {
      unmortgage(p);
    }
    else if (response == 'd' || response == 'D') {
      make_deal(p);
    }
    else if (response == 'b' || response == 'B') {
      cout << "You have $" << playerVector[p].money << endl;
    }
    else if (response == 'h' || response == 'H') {
      buy_houses(p);
    }
    else if (response == 'p' || response == 'P') {
      if (playerVector[p].properties.size() == 0) {
        cout << "You have no properties." << endl;
      }
      else {
        cout << "You have ";
        for (auto it : playerVector[p].properties)
        {
          cout << it->name << " ";
        }
        cout << endl;
      }
    }
    else {
      break;
    }
    cout << "Would you like to check anything else? (y/n): ";
    cin >> again;
  }
}

// Mortgages a properties.
void mortgage(int p) {
  string mortgageProperty;
  int playerNumber = playerVector[p].number;
  if (playerVector[p].properties.size() == 0) {
    cout << "You have no properties." << endl;
    return;
  }
  // Print unmortgaged properties
  cout << "Unmortgaged properties: You have ";
  for (int i = 0; i < 40; i++) {
    if (boardtest.squares[i]->isOwned == playerNumber && boardtest.squares[i]->isMortgaged == 0) {
    cout << boardtest.squares[i]->name << " ";
    }
  }
  cout << endl;

  cout << "Enter what you would like to mortgage: ";
  cin.ignore();               // Ignore the previous new line in input.
  getline(cin, mortgageProperty);
  for (int i = 0; i < 40; i++) {                  // Iterate through board to find property
    if (boardtest.squares[i]->name == mortgageProperty && boardtest.squares[i]->isOwned == playerNumber) {
      if (boardtest.squares[i]->isMortgaged == 1) {
        cout << "Property is already mortgaged." << endl;
      }
      else if (boardtest.squares[i]->houses > 0) {
        cout << "Property has houses." << endl;
      }
      else {
        cout << "Property is now mortgaged." << endl;
        boardtest.squares[i]->isMortgaged = 1;
        playerVector[p].money -= boardtest.squares[i]->valueVector[0] / 2;    // Receive half of the cost of rent
      }
    }
  }
}

void unmortgage(int p) {
  int playerNumber = playerVector[p].number;
  string mortgageProperty;
  if (playerVector[p].properties.size() == 0) {
    cout << "You have no properties." << endl;
    return;
  }
  // Print out mortgaged properties.
  cout << "Mortgaged properties: You have ";
  for (int i = 0; i < 40; i++) {
    if (boardtest.squares[i]->isOwned == playerNumber && boardtest.squares[i]->isMortgaged == 1) {
    cout << boardtest.squares[i]->name << " ";
    }
  }
  cout << endl;

  cout << "Enter what you would like to unmortgage: ";
  cin.ignore();               // Ignore the previous new line in input.
  getline(cin, mortgageProperty);
  for (int i = 0; i < 40; i++) {                  // Iterate through board to find property
    if (boardtest.squares[i]->name == mortgageProperty && boardtest.squares[i]->isOwned == playerNumber) {
      if (boardtest.squares[i]->isMortgaged == 0) {
        cout << "Property is already unmortgaged." << endl;
      }
      else if (playerVector[p].money <= boardtest.squares[i]->valueVector[0]) {
        cout << "Insufficient funds." << endl;
      }
      else {
        cout << "Property is now unmortgaged." << endl;
        boardtest.squares[i]->isMortgaged = 0;
        playerVector[p].money += boardtest.squares[i]->valueVector[0];
      }
    }
  }
}

// i is the array index + 1.
// Gives player option to check a square on the board on their turn
void check_space(int i) {
  int index = i - 1;
  Square* pos = boardtest.squares[index];
  cout << pos->name << endl;
  if (pos->type == 'p' || pos->type == 'r' || pos->type == 'u') {
    if (pos->isOwned == 0) {
      cout << "Not owned by any player." << endl;
      cout << "Cost: " << pos->value << endl;
    }
    else if (pos->type == 'p') {
      cout << "Owned by Player " << pos->isOwned << endl;
      cout << "Rent: " << pos->value << endl;
      cout << "Number of houses: " << pos->houses << endl;
    }
    else if (pos->type == 'r') {
      cout << "Owned by Player " << pos->isOwned << endl;
    }
    else if (pos->type == 'u') {
      cout << "Owned by Player " << pos->isOwned << endl;
    }
  }
}

// Checks what player can do on the square they landed on
void land_square(int p) {
  Square* ptr = boardtest.squares[playerVector[p].position];
  // Special squares (Income tax, luxury, etc.)
  if (ptr->type == 's')
  {
    playerVector[p].money += ptr->value;
    cout << "You now have $" << playerVector[p].money << endl;
  }

  // Landed on a card
  else if (ptr->type == 'c') {
    draw_card(p);
  }

  // Properties/utilities/railroads that are not owned. Give player the option to buy.
  else if (!ptr->isOwned && (ptr->type == 'p' || ptr->type == 'r' || ptr->type == 'u'))
  {
    char response;
    cout << "Would you like to buy " << ptr->name << " ? (y/n): ";
    cin >> response;
    if ((response == 'y' || response == 'Y') && playerVector[p].money > abs(ptr->value))
    {
      ptr->isOwned = playerVector[p].number;
      playerVector[p].money += ptr->value;
      cout << "You now have $" << playerVector[p].money << endl;
      ptr->value = ptr->valueVector[2];      // Price of rent
      playerVector[p].properties.push_back(ptr);   // Add owned property to vector of square pointers belonging to player. Might give trouble, adding copy of original pointer.
    }
    else if ((response == 'y' || response == 'Y') && playerVector[p].money <= abs(ptr->value))
    {
      cout << "Insufficient funds." << endl;
    }
  }

  // Whoever owns the property gets paid.
  else if (ptr->type == 'p' && ptr->isOwned && playerVector[p].number != ptr->isOwned && !(ptr->isMortgaged))
  {
    playerVector[p].money += ptr->value;
    cout << "Property is owned. You now have $" << playerVector[p].money << endl;
    for (int i = 0; i < playerVector.size(); i++)
    {
      if (playerVector[i].number == ptr->isOwned)
      {
        playerVector[i].money -= ptr->value;
        cout << "Player " << playerVector[i].number << " owns this property. They now have $" << playerVector[i].money << endl;
      }
    }
  }

  // Whoever owns the railroad gets paid.
  else if (ptr->type == 'r' && ptr->isOwned && playerVector[p].number != ptr->isOwned && !(ptr->isMortgaged))
  {
    railroad(p);
  }

  // Whoever owns the utility gets paid.
  else if (ptr->type == 'u' && ptr->isOwned && playerVector[p].number != ptr->isOwned && !(ptr->isMortgaged))
  {
    utility(p);
  }

  // Property is mortgaged.
  else if (ptr->isOwned && playerVector[p].number != ptr->isOwned && ptr->isMortgaged) {
    cout << "Property is owned by Player " << ptr->isOwned << " but is mortgaged." << endl;
  }
}

// Pop from queue, push back card after done.
void draw_card(int p) {
  char response = ' ';
  string n = boardtest.squares[playerVector[p].position]->name;
  if (n == "COMMUNITY CHEST") {
    while (response != 'd' && response != 'D') {
      cout << "Draw community chest card (d): ";
      cin >> response;
    }
    cout << "Drawing..." << endl;
    usleep(1000000);
    Card* ptr = new Card(*(boardtest.communityChest.front()));
    boardtest.communityChest.pop();
    boardtest.communityChest.push(ptr);
    cout << ptr->text << endl;
    usleep(1000000);
    // Check value, traverse, and everyPlayer
    if (ptr->traverse == 0) {
      if (ptr->everyPlayer == 0) {
        playerVector[p].money += ptr->value;
      }
      else {
        // Collect money from every other player
        playerVector[p].money += ptr->value * (playerVector.size() - 1);
        for (int i = 0; i < playerVector.size(); i++) {
          if (i != p) {
            playerVector[i].money -= ptr->value;
          }
        }
      }
    }
    else {
      playerVector[p].money += ptr->value;
      traverse_board(p,(ptr->traverse - 1) - playerVector[p].position);
      land_square(p);
    }
  }
  else if (n == "CHANCE") {
    while (response != 'd' && response != 'D') {
      cout << "Draw chance card (d): ";
      cin >> response;
    }
    cout << "Drawing..." << endl;
    usleep(1000000);
    Card* ptr = new Card(*(boardtest.chance.front()));
    boardtest.chance.pop();
    boardtest.chance.push(ptr);
    cout << ptr->text << endl;
    usleep(1000000);
    // Check value, traverse, and everyPlayer
    if (ptr->traverse == 0) {
      if (ptr->everyPlayer == 0) {
        playerVector[p].money += ptr->value;
      }
      else {
        // Collect money from every other player
        playerVector[p].money += ptr->value * (playerVector.size() - 1);
        for (int i = 0; i < playerVector.size(); i++) {
          if (i != p) {
            playerVector[i].money -= ptr->value;
          }
        }
      }
    }
    else {
      playerVector[p].money += ptr->value;
      traverse_board(p,(ptr->traverse - 1) - playerVector[p].position);
      land_square(p);
    }
  }
}

void railroad(int p) {
  int index = 0;
  int p2 = boardtest.squares[playerVector[p].position]->isOwned;
  int railroadsOwned = 0;
  for (int i = 0; i < playerVector.size(); i++) {
    if (playerVector[i].number == p2) {
      index = i;
    }
  }
  // How many railroads does the other player own?
  for (int i = 0; i < 40; i++) {
    if (boardtest.squares[i]->isOwned == p2 && boardtest.squares[i]->type == 'r') {
      railroadsOwned++;
    }
  }
  if (railroadsOwned == 1) {
    cout << "Player " << p2 << " owns 1 railroad. Pay $25." << endl;
    playerVector[p].money -= 25;
    playerVector[index].money += 25;
  }
  else if (railroadsOwned == 2) {
    cout << "Player " << p2 << " owns 2 railroads. Pay $50." << endl;
    playerVector[p].money -= 50;
    playerVector[index].money += 50;
  }
  else if (railroadsOwned == 3) {
    cout << "Player " << p2 << " owns 3 railroads. Pay $100." << endl;
    playerVector[p].money -= 100;
    playerVector[index].money += 100;
  }
  else if (railroadsOwned == 4) {
    cout << "Player " << p2 << " owns 4 railroads. Pay $200." << endl;
    playerVector[p].money -= 200;
    playerVector[index].money += 200;
  }
  return;
}

// 4 times amount shown on dice, or 10 times based on utilities
void utility(int p) {
  int index = 0;
  int p2 = boardtest.squares[playerVector[p].position]->isOwned;
  int utilitiesOwned = 0;
  int roll1 = rand() % 6 + 1;
  int roll2 = rand() % 6 + 1;
  char response;
  cout << "Roll the dice to determine how much rent to pay." << endl;
  while (response != 'r' && response != 'R') {
    cout << "Roll dice (r): ";
    cin >> response;
  }
  cout << "Rolling..." << endl;
  usleep(2000000);
  cout << "You rolled a " << roll1 << " and a " << roll2 <<  endl;

  for (int i = 0; i < playerVector.size(); i++) {
    if (playerVector[i].number == p2) {
      index = i;
    }
  }
  // How many utilities does the other player own?
  for (int i = 0; i < 40; i++) {
    if (boardtest.squares[i]->isOwned == p2 && boardtest.squares[i]->type == 'u') {
      utilitiesOwned++;
    }
  }
  if (utilitiesOwned == 1) {
    cout << "Player " << p2 << " owns 1 utility. Pay 4 times your dice roll." << endl;
    playerVector[p].money -= 4*(roll1 + roll2);
    playerVector[index].money += 4*(roll1 + roll2);
  }
  else if (utilitiesOwned == 2) {
    cout << "Player " << p2 << " owns 2 utilities. Pay 10 times your dice roll." << endl;
    playerVector[p].money -= 10*(roll1 + roll2);
    playerVector[index].money += 10*(roll1 + roll2);
  }
  return;
}

// Make deal with another player.
void make_deal(int p)
{
  int p2 = 0;
  char p2_before_cast;
  char offer;
  while (p2 > 4 || p2 < 1) {
    cout << "With which player? Enter their number (1-4): ";
    cin >> p2_before_cast;
    p2 = p2_before_cast - 48;
  }
  // Check if player even exists.
  int exists = 0;
  for (int i = 0; i < playerVector.size(); i++) {
    if (playerVector[i].number == p2) {
      exists = 1;
    }
  }
  // Can only make deals with other players
  if (playerVector[p].number == p2) {
    cout << "You cannot make a deal with yourself." << endl;
    return;
  }
  else if (exists == 0) {
    cout << "That player lost already." << endl;
    return;
  }

  // Identify where other player (p2) is located in the vector.
  int index;
  for (int i = 0; i < playerVector.size(); i++)
  {
    if(playerVector[i].number == p2)
    {
      index = i;
    }
  }

  // What kind of deal?
  cout << "What are you offering? Property or money? (p/m): ";
  cin >> offer;

  //Your money for their property
  if (offer == 'M' || offer == 'm')
  {
    int amount;
    cout << "How much are you offering? You have $" << playerVector[p].money << ": ";
    cin >> amount;
    while(cin.fail()) {
      cout << "Invalid input. Try again: ";
      cin.clear();
      cin.ignore(256,'\n');
      cin >> amount;
    }
    // Check if you have enough money
    if(playerVector[p].money > amount && amount >= 0)
    {
      string property;
      if (playerVector[index].properties.size() == 0)
      {
        cout << "Player " << p2 << " has no properties. No deal can be made." << endl;
      }
      else
      {
        cout << "What do you want? Player " << p2 << " has ";
        for (int i = 0; i < playerVector[index].properties.size(); i++)
        {
          cout << playerVector[index].properties[i]->name << " ";
        }
        cout << ": ";
        cin.ignore();               // Ignore the previous new line in input.
        getline(cin, property);

        char agreement;
        cout << "Both parties agree? (y/n): ";
        cin >> agreement;

        // Check if both players agree to the deal
        if (agreement == 'Y' || 'y')
        {
          playerVector[index].money += amount;
          playerVector[p].money -= amount;
          for (int i = 0; i < 40; i++)
          {
            if (boardtest.squares[i]->name == property)
            {
              if (boardtest.squares[i]->houses > 0) {
                cout << "Player " << p2 << " has properties built on this square. Cannot buy." << endl;
                // Add mortgage option
              }
              else {
                boardtest.squares[i]->isOwned = playerVector[p].number;
              }
            }
          }
          /* Locate property (square pointer) in their vector of pointers
           * Delete from their vector, add to your vector
           * Give them money, you lose money */
          for ( int i = 0; i < playerVector[index].properties.size(); i++)
          {
            if(playerVector[index].properties[i]->name == property)
            {
              Square* ptr = playerVector[index].properties[i];
              playerVector[p].properties.push_back(ptr);
              playerVector[index].properties.erase(playerVector[index].properties.begin()+i);
            }
          }
        }
      }
    }
    else if (amount <= 0) {
      cout << "Money has to be greater than zero." << endl;
    }
    // You don't have enough money to make the deal
    else
    {
      cout << "Insufficient funds." << endl;
    }
  }
  // Your property for their money
  else if (offer == 'P' || offer == 'p')
  {
    int amount;
    string property;
    if (playerVector[p].properties.size() == 0)
    {
      cout << "You have no properties. No deal can be made." << endl;
    }
    else
    {
      cout << "Which property? You have ";
      for (int i = 0; i < playerVector[p].properties.size(); i++)
      {
        cout << playerVector[p].properties[i]->name << " ";
      }
      cout << ": ";
      cin.ignore();               // Ignore the previous new line in input.
      getline(cin, property);
      cout << "How much do you want? Player " << p2 << " has $" << playerVector[index].money << ": ";
      cin >> amount;
      while(cin.fail()) {
        cout << "Invalid input. Try again: ";
        cin.clear();
        cin.ignore(256,'\n');
        cin >> amount;
      }
      // Check if they have enough money
      if(playerVector[index].money > amount && amount > 0)
      {
        char agreement;
        cout << "Both parties agree? (y/n): ";
        cin >> agreement;
        // Check for agreement
        if (agreement == 'Y' || 'y')
        {
          playerVector[p].money += amount;
          playerVector[index].money -= amount;
          // Update isOwned to match number of player receiving the property
          for (int i = 0; i < 40; i++)
          {
            if (boardtest.squares[i]->name == property) {
              if (boardtest.squares[i]->houses > 0) {
                cout << "You have properties built on this square. Cannot sell." << endl;
              }
              else {
                boardtest.squares[i]->isOwned = playerVector[index].number;
              }
            }
          }
          // Delete pointer to property in one player's vector, add in the other's
          for ( int i = 0; i < playerVector[p].properties.size(); i++)
          {
            if(playerVector[p].properties[i]->name == property)
            {
              Square* ptr = playerVector[p].properties[i];
              playerVector[index].properties.push_back(ptr);
              playerVector[p].properties.erase(playerVector[p].properties.begin()+i);
            }
          }
        }
      }
      else if (amount <= 0) {
        cout << "Money has to be greater than zero." << endl;
      }
      else
      {
        cout << "Insufficient funds." << endl;
      }
    }
  }
}


// Checks to see if player[p] has a monopoly. Returns all the monopolies they have in the form of an int array.
int* check_monopoly(int p) {
  static int colorArray[8] = {0,0,0,0,0,0,0,0};
  int n = playerVector[p].number;
  if (boardtest.squares[1]->isOwned == n && boardtest.squares[3]->isOwned == n
      && boardtest.squares[1]->isMortgaged == 0 && boardtest.squares[3]->isMortgaged == 0) {
    cout << n;
    colorArray[0] = 1;        // Brown
  } else {
    colorArray[0] = 0;
  }
  if (boardtest.squares[6]->isOwned == n && boardtest.squares[8]->isOwned == n && boardtest.squares[9]->isOwned == n
      && boardtest.squares[6]->isMortgaged == 0 && boardtest.squares[8]->isMortgaged == 0 && boardtest.squares[9]->isMortgaged == 0) {
    cout << n;
    colorArray[1] = 1;        // Light blue
  } else {
    colorArray[1] = 0;
  }
  if (boardtest.squares[11]->isOwned == n && boardtest.squares[13]->isOwned == n && boardtest.squares[14]->isOwned == n
      && boardtest.squares[11]->isMortgaged == 0 && boardtest.squares[13]->isMortgaged == 0 && boardtest.squares[14]->isMortgaged == 0) {
    colorArray[2] = 1;        // Purple
  }  else {
    colorArray[2] = 0;
  }
  if (boardtest.squares[16]->isOwned == n && boardtest.squares[18]->isOwned == n && boardtest.squares[19]->isOwned == n
      && boardtest.squares[16]->isMortgaged == 0 && boardtest.squares[18]->isMortgaged == 0 && boardtest.squares[19]->isMortgaged == 0) {
    colorArray[3] = 1;        // Orange
  } else {
    colorArray[3] = 0;
  }
  if (boardtest.squares[21]->isOwned == n && boardtest.squares[23]->isOwned == n && boardtest.squares[24]->isOwned == n
      && boardtest.squares[21]->isMortgaged == 0 && boardtest.squares[23]->isMortgaged == 0 && boardtest.squares[24]->isMortgaged == 0) {
    colorArray[4] = 1;        // Red
  } else {
    colorArray[4] = 0;
  }
  if (boardtest.squares[26]->isOwned == n && boardtest.squares[27]->isOwned == n && boardtest.squares[29]->isOwned == n
      && boardtest.squares[26]->isMortgaged == 0 && boardtest.squares[27]->isMortgaged == 0 && boardtest.squares[29]->isMortgaged == 0) {
    colorArray[5] = 1;        // Yellow
  } else {
    colorArray[5] = 0;
  }
  if (boardtest.squares[31]->isOwned == n && boardtest.squares[32]->isOwned == n && boardtest.squares[34]->isOwned == n
      && boardtest.squares[31]->isMortgaged == 0 && boardtest.squares[32]->isMortgaged == 0 && boardtest.squares[34]->isMortgaged == 0) {
    colorArray[6] = 1;        // Green
  } else {
    colorArray[6] = 0;
  }
  if (boardtest.squares[37]->isOwned == n && boardtest.squares[39]->isOwned == n
      && boardtest.squares[37]->isMortgaged == n && boardtest.squares[39]->isMortgaged == n) {
    colorArray[7] = 1;        // Blue
  } else {
    colorArray[7] = 0;
  }
  return colorArray;
}

// Global array and enum used to make life easier and code more readable
enum Colors{BROWN = 0, LIGHT_BLUE, PURPLE, ORANGE, RED, YELLOW, GREEN, BLUE};
string colors[] = {"brown","light blue","purple","orange","red","yellow","green","blue"};

// Gives players the option to buy properties on their monopolies.
void buy_houses(int p) {
  int* colorArray = check_monopoly(p);
  int count = 0;
  char response;
  int numHouses;
  for (int i = 0; i < 8; i++) {
    if (colorArray[i] == 1) {
      cout << "You have a monopoly on the " << colors[i] << " spaces. Would you like to buy houses? (y/n): ";
      cin >> response;
      if (response == 'y' || response == 'Y') {
        allocate_property(i,p);
      }
      count++;
    }
  }
  if (count == 0) {
    cout << "You have no monopolies. Cannot build houses." << endl;
  }
  return;
}

// i refers to the color of the monopoly. n refers to the number of houses to allocate.
void allocate_property(int i, int p) {
  int n;
  cout << "How many houses per property? Enter a valid number: ";
  cin >> n;
  int sumHouses = 0;
  switch(i) {
    case BROWN:
    sumHouses = boardtest.squares[1]->houses + boardtest.squares[3]->houses;
    if (playerVector[p].money < (n*2)*boardtest.squares[1]->valueVector[1]) {
      cout << "Insufficient funds." << endl;
      return;
    }
    else if (sumHouses + (n*2) > 10) {
      cout << "Max houses exceeded on brown squares." << endl;
      return;
    }
    else {
      playerVector[p].money += (n*2)*boardtest.squares[1]->valueVector[1];
      cout << "You now have $" << playerVector[p].money << endl;
    }
    boardtest.squares[1]->houses += n;
    boardtest.squares[3]->houses += n;
    cout << "You now have " << boardtest.squares[1]->houses << " houses on each brown property." << endl;
    // Update value of squares, depending on number of properties.
    boardtest.squares[1]->value = boardtest.squares[1]->valueVector[boardtest.squares[1]->houses + 3];
    boardtest.squares[3]->value = boardtest.squares[3]->valueVector[boardtest.squares[3]->houses + 3];
    break;

    case LIGHT_BLUE:
    sumHouses = boardtest.squares[6]->houses + boardtest.squares[8]->houses + boardtest.squares[9]->houses;
    if (sumHouses + (n*3) > 15) {
      cout << "Max houses exceeded on light blue squares." << endl;
      return;
    }
    else if (playerVector[p].money < (n*3)*boardtest.squares[6]->valueVector[1]) {
      cout << "Insufficient funds." << endl;
      return;
    }
    else {
      playerVector[p].money += (n*3)*boardtest.squares[6]->valueVector[1];
      cout << "You now have $" << playerVector[p].money << endl;
    }
    boardtest.squares[6]->houses += n;
    boardtest.squares[8]->houses += n;
    boardtest.squares[9]->houses += n;
    cout << "You now have " << boardtest.squares[6]->houses << " houses on each brown property." << endl;
    // Update value of squares, depending on number of properties.
    boardtest.squares[6]->value = boardtest.squares[6]->valueVector[boardtest.squares[6]->houses + 3];
    boardtest.squares[8]->value = boardtest.squares[8]->valueVector[boardtest.squares[8]->houses + 3];
    boardtest.squares[9]->value = boardtest.squares[9]->valueVector[boardtest.squares[9]->houses + 3];
    break;

    case PURPLE:
    sumHouses = boardtest.squares[11]->houses + boardtest.squares[13]->houses + boardtest.squares[14]->houses;
    if (sumHouses + (n*3) > 15) {
      cout << "Max houses exceeded on purple squares." << endl;
      return;
    }
    else if (playerVector[p].money < (n*3)*boardtest.squares[11]->valueVector[1]) {
      cout << "Insufficient funds." << endl;
      return;
    }
    else {
      playerVector[p].money += (n*3)*boardtest.squares[11]->valueVector[1];
      cout << "You now have $" << playerVector[p].money << endl;
    }
    boardtest.squares[11]->houses += n;
    boardtest.squares[13]->houses += n;
    boardtest.squares[14]->houses += n;
    cout << "You now have " << boardtest.squares[11]->houses << " houses on each brown property." << endl;
    // Update value of squares, depending on number of properties.
    boardtest.squares[11]->value = boardtest.squares[11]->valueVector[boardtest.squares[11]->houses + 3];
    boardtest.squares[13]->value = boardtest.squares[13]->valueVector[boardtest.squares[13]->houses + 3];
    boardtest.squares[14]->value = boardtest.squares[14]->valueVector[boardtest.squares[14]->houses + 3];
    break;

    case ORANGE:
    sumHouses = boardtest.squares[16]->houses + boardtest.squares[18]->houses + boardtest.squares[19]->houses;
    if (sumHouses + (n*3) > 15) {
      cout << "Max houses exceeded on orange squares." << endl;
      return;
    }
    else if (playerVector[p].money < (n*3)*boardtest.squares[16]->valueVector[1]) {
      cout << "Insufficient funds." << endl;
      return;
    }
    else {
      playerVector[p].money += (n*3)*boardtest.squares[16]->valueVector[1];
      cout << "You now have $" << playerVector[p].money << endl;
    }
    boardtest.squares[16]->houses += n;
    boardtest.squares[18]->houses += n;
    boardtest.squares[19]->houses += n;
    cout << "You now have " << boardtest.squares[16]->houses << " houses on each brown property." << endl;
    // Update value of squares, depending on number of properties.
    boardtest.squares[16]->value = boardtest.squares[16]->valueVector[boardtest.squares[16]->houses + 3];
    boardtest.squares[18]->value = boardtest.squares[18]->valueVector[boardtest.squares[18]->houses + 3];
    boardtest.squares[19]->value = boardtest.squares[19]->valueVector[boardtest.squares[19]->houses + 3];
    break;

    case RED:
    sumHouses = boardtest.squares[21]->houses + boardtest.squares[23]->houses + boardtest.squares[24]->houses;
    if (sumHouses + (n*3) > 15) {
      cout << "Max houses exceeded on red squares." << endl;
      return;
    }
    else if (playerVector[p].money < (n*3)*boardtest.squares[21]->valueVector[1]) {
      cout << "Insufficient funds." << endl;
      return;
    }
    else {
      playerVector[p].money += (n*3)*boardtest.squares[21]->valueVector[1];
      cout << "You now have $" << playerVector[p].money << endl;
    }
    boardtest.squares[21]->houses += n;
    boardtest.squares[23]->houses += n;
    boardtest.squares[24]->houses += n;
    cout << "You now have " << boardtest.squares[21]->houses << " houses on each brown property." << endl;
    // Update value of squares, depending on number of properties.
    boardtest.squares[21]->value = boardtest.squares[21]->valueVector[boardtest.squares[21]->houses + 3];
    boardtest.squares[23]->value = boardtest.squares[23]->valueVector[boardtest.squares[23]->houses + 3];
    boardtest.squares[24]->value = boardtest.squares[24]->valueVector[boardtest.squares[24]->houses + 3];
    break;

    case YELLOW:
    sumHouses = boardtest.squares[26]->houses + boardtest.squares[27]->houses + boardtest.squares[29]->houses;
    if (sumHouses + (n*3) > 15) {
      cout << "Max houses exceeded on yellow squares." << endl;
      return;
    }
    else if (playerVector[p].money < (n*3)*boardtest.squares[26]->valueVector[1]) {
      cout << "Insufficient funds." << endl;
      return;
    }
    else {
      playerVector[p].money += (n*3)*boardtest.squares[26]->valueVector[1];
      cout << "You now have $" << playerVector[p].money << endl;
    }
    boardtest.squares[26]->houses += n;
    boardtest.squares[27]->houses += n;
    boardtest.squares[29]->houses += n;
    cout << "You now have " << boardtest.squares[26]->houses << " houses on each brown property." << endl;
    // Update value of squares, depending on number of properties.
    boardtest.squares[26]->value = boardtest.squares[26]->valueVector[boardtest.squares[26]->houses + 3];
    boardtest.squares[27]->value = boardtest.squares[27]->valueVector[boardtest.squares[27]->houses + 3];
    boardtest.squares[29]->value = boardtest.squares[29]->valueVector[boardtest.squares[29]->houses + 3];
    break;

    case GREEN:
    sumHouses = boardtest.squares[31]->houses + boardtest.squares[32]->houses + boardtest.squares[34]->houses;
    if (sumHouses + n > 15) {
      cout << "Max houses exceeded on green squares." << endl;
      return;
    }
    else if (playerVector[p].money < (n*3)*boardtest.squares[31]->valueVector[1]) {
      cout << "Insufficient funds." << endl;
      return;
    }
    else {
      playerVector[p].money += (n*3)*boardtest.squares[31]->valueVector[1];
      cout << "You now have $" << playerVector[p].money << endl;
    }
    boardtest.squares[31]->houses += n;
    boardtest.squares[32]->houses += n;
    boardtest.squares[34]->houses += n;
    cout << "You now have " << boardtest.squares[31]->houses << " houses on each brown property." << endl;
    // Update value of squares, depending on number of properties.
    boardtest.squares[31]->value = boardtest.squares[31]->valueVector[boardtest.squares[31]->houses + 3];
    boardtest.squares[32]->value = boardtest.squares[32]->valueVector[boardtest.squares[32]->houses + 3];
    boardtest.squares[34]->value = boardtest.squares[34]->valueVector[boardtest.squares[34]->houses + 3];
    break;

    case BLUE:
    sumHouses = boardtest.squares[37]->houses + boardtest.squares[39]->houses;
    if (sumHouses + n > 10) {
      cout << "Max houses exceeded on blue squares." << endl;
      return;
    }
    else if (playerVector[p].money < (n*2)*boardtest.squares[37]->valueVector[1]) {
      cout << "Insufficient funds." << endl;
      return;
    }
    else {
      playerVector[p].money += (n*2)*boardtest.squares[37]->valueVector[1];
      cout << "You now have $" << playerVector[p].money << endl;
    }
    boardtest.squares[37]->houses += n;
    boardtest.squares[39]->houses += n;
    cout << "You now have " << boardtest.squares[37]->houses << " houses on each brown property." << endl;
    // Update value of squares, depending on number of properties.
    boardtest.squares[37]->value = boardtest.squares[37]->valueVector[boardtest.squares[37]->houses + 3];
    boardtest.squares[39]->value = boardtest.squares[39]->valueVector[boardtest.squares[39]->houses + 3];
    break;
  }
  return;
}

int game_over()
{
  cout << endl;
  // Only one player left in vector
  if (playerVector.size() == 1)
  {
    cout << "Player " << playerVector[0].number << " wins!" << endl;
    playerVector.erase(playerVector.begin());
    return 1;
  }
  return 0;
}
