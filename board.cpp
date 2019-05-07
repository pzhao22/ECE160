#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "monopoly.h"
#include <queue>

using namespace std;

Square::Square(string n, char t = 0, int v = 0, vector<int> vV ={0}, int i = 0, vector<int> p = {}) {
  name = n;
  type = t;
  value = v;
  valueVector = vV;
  isOwned = i;
  players = p;
  houses = 0;
  isMortgaged = 0;
}

Square::Square() {
  name = "DEFAULT";
  type = 'd';
  value = 0;
  valueVector = {0};
  isOwned = 0;
  players = {1,2,3,4};
  houses = 0;
  isMortgaged = 0;
}

Card::Card(string t, int v, int tr, int e) {
  text = t;
  value = v;
  traverse = tr;
  everyPlayer = e;
}

Card::Card() {
  text = " ";
  value = 0;
  traverse = 0;
  everyPlayer = 0;
}

Card::Card(const Card& c) {
  text = c.text;
  value = c.value;
  traverse = c.traverse;
  everyPlayer = c.everyPlayer;
}

// Initialize the board by initializing array of square pointers,
// queue of community chest cards, and queue of chance cards.
void Board::init_Board() {
  Square squareArray[40] = {
    {"GO",'s',400,{},0,{1,2,3,4}},
    {"MEDITERRANEAN AVENUE",'p',-60,{-60,-50,-2,-4,-10,-30,-90,-160,-250}},
    {"COMMUNITY CHEST",'c',0},
    {"BALTIC AVENUE",'p',-60,{-60,-50,-4,-8,-20,-60,-180,-320,-450}},
    {"INCOME TAX",'s',-200},
    {"READING RAILROAD",'r',-200},
    {"ORIENTAL AVENUE",'p',-100,{-100,-50,-6,-12,-30,-90,-270,-400,-550}},
    {"CHANCE",'c',0},
    {"VERMONT AVENUE",'p',-100,{-100,-50,-6,-12,-30,-90,-270,-400,-550}},
    {"CONNECTICUT AVENUE",'p',-120,{-120,-50,-8,-16,-40,-100,-300,-450,-600}},
    {"JAIL",'s',0},
    {"ST. CHARLES PLACE",'p',-140,{-140,-100,-10,-20,-50,-150,-450,-625,-750}},
    {"ELECTRIC COMPANY",'u',-150},
    {"STATES AVENUE",'p',-140,{-140,-100,-10,-20,-50,-150,-450,-625,-750}},
    {"VIRGINIA AVENUE",'p',-160,{-160,-100,-12,-24,-60,-180,-500,-700,-900}},
    {"PENNSYLVANIA RAILROAD",'r',-200},
    {"ST. JAMES PLACE",'p',-180,{-180,-100,-14,-28,-70,-200,-550,-750,-950}},
    {"COMMUNITY CHEST",'c',0},
    {"TENNESSEE AVENUE",'p',-180,{-180,-100,-14,-28,-70,-200,-550,-750,-950}},
    {"NEW YORK AVENUE",'p',-200,{-200,-100,-16,-32,-80,-220,-600,-800,-1000}},
    {"FREE PARKING",'s',0},
    {"KENTUCKY AVENUE",'p',-220,{-220,-150,-18,-36,-90,-250,-700,-875,-1050}},
    {"CHANCE",'c',0},
    {"INDIANA AVENUE",'p',-220,{-220,-150,-18,-36,-90,-250,-700,-875,-1050}},
    {"ILLINOIS AVENUE",'p',-240,{-240,-150,-20,-40,-100,-300,-750,-925,-1100}},
    {"B & O RAILROAD",'r',-200},
    {"ATLANTIC AVENUE",'p',-260,{-260,-150,-22,-44,-110,-330,-800,-975,-1150}},
    {"VENTNOR AVENUE",'p',-260,{-260,-22,-44,-110,-330,-800,-975,-1150}},
    {"WATER WORKS",'u',-150},
    {"MARVIN GARDENS",'p', -280,{-280,-150,-24,-48,-120,-360,-860,-1025,-1200}},
    {"GO TO JAIL",'s',0},
    {"PACIFIC AVENUE",'p',-300,{-300,-200,-26,-52,-130,-390,-900,-1100,-1275}},
    {"NORTH CAROLINA AVENUE",'p',-300,{-300,-200,-26,-52,-130,-390,-900,-1100,-1275}},
    {"COMMUNITY CHEST",'c',0},
    {"PENNSYLVANIA AVENUE",'p',-320,{-320,-200,-28,-56,-150,-450,-1000,-1200,-1400}},
    {"SHORT LINE",'r',-200},
    {"CHANCE",'c',0},
    {"PARK PLACE",'p',-350,{-350,-200,-35,-70,-175,-500,-1100,-1300,-1500}},
    {"LUXURY TAX",'s',-100},
    {"BOARDWALK",'p',-400,{-400,-200,-50,-100,-200,-600,-1400,-1700,-2000}}
  };
  for (int i = 0; i < 40; i++)
  {
    squares[i] = new Square(squareArray[i]);
  }

  Card communityChestArray[15] = {
    {"Advance to Go.",-200,1,0},
    {"Bank error in your favor. Collect $200.",200,0,0},
    {"Doctor's fees. Pay $50.",-50,0,0},
    {"From sale of stock you get $50.",50,0,0},
    {"Go to jail.",0,31,0},
    {"Grand opera night. Collect $50 from every player for opening night seats.",50,0,1},
    {"Holiday fund matures. Collect $100.",100,0,0},
    {"Income tax refund. Collect $20.",20,0,0},
    {"It's your birthday. Collect $10 from every player.",10,0,1},
    {"Life insurance matures. Collect $100.",100,0,0},
    {"Hospital fees. Pay $50.",-50,0,0},
    {"School fees. Pay $50.",-50,0,0},
    {"Reveive $25 consultancy fee.",25,0,0},
    {"You have won second prize in a beauty contest. Collect $10.",10,0,0},
    {"You inherit $100.",100,0,0}
  };
  for (int i = 0; i < 15; i++)
  {
    communityChest.push(new Card(communityChestArray[i]));
  }

  Card chanceArray[13] = {
    {"Advance to Go.",-200,1,0},
    {"Advance to Illinois Ave.",0,24,0},
    {"Advance to St. Charles Place.",0,12,0},
    {"Take a trip to Reading Railroad.",0,6,0},
    {"Bank pays you dividend of $50.",50,0,0},
    {"Go to jail.",0,31,0},
    {"Pay poor tax of $15.",-15,0,0},
    {"Talk a walk on the Boardwalk.",0,40,0},
    {"You have been elected Chairman of the Board. Pay each player $50.",-50,0,1},
    {"Life insurance matures. Collect $100.",100,0,0},
    {"You now have a job at Bloomberg. Collect $1000 signing bonus.",1000,0,0},
    {"Your building and loan matures. Collect $150.",150,0,0},
    {"You have won a crossword competition. Collect $100.",100,0,0}
  };
  for (int i = 0; i < 13; i++)
  {
    chance.push(new Card(chanceArray[i]));
  }
}

// Draw the board, with the players inside it.
// Convert vector of players into string to display player sprites.
void Board::draw_Board() {

  char p[160];

  for (int i = 0; i < 160; i++)
  {
    p[i] = ' ';
  }

  for (int i = 0; i < 40; i++)
  {
    if (find(squares[i]->players.begin(), squares[i]->players.end(), 1) != squares[i]->players.end())  //Player 1 sprite
    {
      p[i*4] = '1';
    }
    if (find(squares[i]->players.begin(), squares[i]->players.end(), 2) != squares[i]->players.end())  //Player 1 sprite
    {
      p[(i*4)+1] = '2';
    }
    if (find(squares[i]->players.begin(), squares[i]->players.end(), 3) != squares[i]->players.end())  //Player 1 sprite
    {
      p[(i*4)+2] = '3';
    }
    if (find(squares[i]->players.begin(), squares[i]->players.end(), 4) != squares[i]->players.end())  //Player 1 sprite
    {
      p[(i*4)+3] = '4';
    }
  }

  for (int i = 0; i < 45; i++)
  {
    // Every symbol in row is '#'
    if (i == 0 || i == 44 || i == 4 || i == 40)
    {
      for (int j = 0; j < 89; j++)
      {
        cout << '#';
      }
    }

    else if (i == 2)  // Upper and lower rows, containing player sprites. Similar for 38 (right below)
    {
      for (int j = 0; j < 11; j++)
      {
        cout << "#" << p[j*4] << " " << p[(j*4)+1] << " " << p[(j*4)+2] << " " << p[(j*4)+3];
        if (j == 10)
        {
          cout << "#";
        }
      }
    }

    else if (i == 42)
    {
      for (int j = 30; j > 19; j--)
      {
        cout << "#" << p[j*4] << " " << p[(j*4)+1] << " " << p[(j*4)+2] << " " << p[(j*4)+3];
        if (j == 20)
        {
          cout << "#";
        }
      }
    }

    // Squares 39 & 11
    else if (i == 6)    // Similar for 6, 10, 14, 18, 22, 26, 30, 34, 38
    {
      for (int j = 0; j < 89; j++)
      {
        if(j == 0 || j == 8 || j == 80 || j == 88)
          cout << '#';
        else if (j == 1)
        {
          cout << p[156];
        }
        else if (j == 3)
        {
          cout << p[157];
        }
        else if (j == 5)
        {
          cout << p[158];
        }
        else if (j == 7)
        {
          cout << p[159];
        }
        else if (j == 81)
        {
          cout << p[44];
        }
        else if (j == 83)
        {
          cout << p[45];
        }
        else if (j == 85)
        {
          cout << p[46];
        }
        else if (j == 87)
        {
          cout << p[47];
        }
        else
          cout << ' ';
        }
      }
    // Squares 38 & 12
    else if (i == 10)    // Similar for 6, 10, 14, 18, 22, 26, 30, 34, 38
    {
      for (int j = 0; j < 89; j++)
      {
        if(j == 0 || j == 8 || j == 80 || j == 88)
          cout << '#';
        else if (j == 1)
        {
          cout << p[152];
        }
        else if (j == 3)
        {
          cout << p[153];
        }
        else if (j == 5)
        {
          cout << p[154];
        }
        else if (j == 7)
        {
          cout << p[155];
        }
        else if (j == 81)
        {
          cout << p[48];
        }
        else if (j == 83)
        {
          cout << p[49];
        }
        else if (j == 85)
        {
          cout << p[50];
        }
        else if (j == 87)
        {
          cout << p[51];
        }
        else
          cout << ' ';
        }
      }
    // Squares 37 & 13
    else if (i == 14)    // Similar for 6, 10, 14, 18, 22, 26, 30, 34, 38
    {
      for (int j = 0; j < 89; j++)
      {
        if(j == 0 || j == 8 || j == 80 || j == 88)
          cout << '#';
        else if (j == 1)
        {
          cout << p[148];
        }
        else if (j == 3)
        {
          cout << p[149];
        }
        else if (j == 5)
        {
          cout << p[150];
        }
        else if (j == 7)
        {
          cout << p[151];
        }
        else if (j == 81)
        {
          cout << p[52];
        }
        else if (j == 83)
        {
          cout << p[53];
        }
        else if (j == 85)
        {
          cout << p[54];
        }
        else if (j == 87)
        {
          cout << p[55];
        }
        else
          cout << ' ';
        }
      }
    // Squares 36 & 14
    else if (i == 18)    // Similar for 6, 10, 14, 18, 22, 26, 30, 34, 38
    {
      for (int j = 0; j < 89; j++)
      {
        if(j == 0 || j == 8 || j == 80 || j == 88)
          cout << '#';
        else if (j == 1)
        {
          cout << p[144];
        }
        else if (j == 3)
        {
          cout << p[145];
        }
        else if (j == 5)
        {
          cout << p[146];
        }
        else if (j == 7)
        {
          cout << p[147];
        }
        else if (j == 81)
        {
          cout << p[56];
        }
        else if (j == 83)
        {
          cout << p[57];
        }
        else if (j == 85)
        {
          cout << p[58];
        }
        else if (j == 87)
        {
          cout << p[59];
        }
        else
          cout << ' ';
        }
      }
    // Squares 35 & 15
    else if (i == 22)    // Similar for 6, 10, 14, 18, 22, 26, 30, 34, 38
    {
      for (int j = 0; j < 89; j++)
      {
        if(j == 0 || j == 8 || j == 80 || j == 88)
          cout << '#';
        else if (j == 1)
        {
          cout << p[140];
        }
        else if (j == 3)
        {
          cout << p[141];
        }
        else if (j == 5)
        {
          cout << p[142];
        }
        else if (j == 7)
        {
          cout << p[143];
        }
        else if (j == 81)
        {
          cout << p[60];
        }
        else if (j == 83)
        {
          cout << p[61];
        }
        else if (j == 85)
        {
          cout << p[62];
        }
        else if (j == 87)
        {
          cout << p[63];
        }
        else
          cout << ' ';
        }
      }
    // Squares 34 & 16
    else if (i == 26)
    {
      for (int j = 0; j < 89; j++)
      {
        if(j == 0 || j == 8 || j == 80 || j == 88)
          cout << '#';
        else if (j == 1)
        {
          cout << p[136];
        }
        else if (j == 3)
        {
          cout << p[137];
        }
        else if (j == 5)
        {
          cout << p[138];
        }
        else if (j == 7)
        {
          cout << p[139];
        }
        else if (j == 81)
        {
          cout << p[64];
        }
        else if (j == 83)
        {
          cout << p[65];
        }
        else if (j == 85)
        {
          cout << p[66];
        }
        else if (j == 87)
        {
          cout << p[67];
        }
        else
          cout << ' ';
        }
      }
    // Squares 33 & 17
    else if (i == 30)
    {
      for (int j = 0; j < 89; j++)
      {
        if(j == 0 || j == 8 || j == 80 || j == 88)
          cout << '#';
        else if (j == 1)
        {
          cout << p[132];
        }
        else if (j == 3)
        {
          cout << p[133];
        }
        else if (j == 5)
        {
          cout << p[134];
        }
        else if (j == 7)
        {
          cout << p[135];
        }
        else if (j == 81)
        {
          cout << p[68];
        }
        else if (j == 83)
        {
          cout << p[69];
        }
        else if (j == 85)
        {
          cout << p[70];
        }
        else if (j == 87)
        {
          cout << p[71];
        }
        else
          cout << ' ';
        }
      }
    // Squares 32 & 18
    else if (i == 34)
    {
      for (int j = 0; j < 89; j++)
      {
        if(j == 0 || j == 8 || j == 80 || j == 88)
          cout << '#';
        else if (j == 1)
        {
          cout << p[128];
        }
        else if (j == 3)
        {
          cout << p[129];
        }
        else if (j == 5)
        {
          cout << p[130];
        }
        else if (j == 7)
        {
          cout << p[131];
        }
        else if (j == 81)
        {
          cout << p[72];
        }
        else if (j == 83)
        {
          cout << p[73];
        }
        else if (j == 85)
        {
          cout << p[74];
        }
        else if (j == 87)
        {
          cout << p[75];
        }
        else
          cout << ' ';
        }
      }
    // Squares 31 & 19
    else if (i == 38)
    {
      for (int j = 0; j < 89; j++)
      {
        if(j == 0 || j == 8 || j == 80 || j == 88)
          cout << '#';
        else if (j == 1)
        {
          cout << p[124];
        }
        else if (j == 3)
        {
          cout << p[125];
        }
        else if (j == 5)
        {
          cout << p[126];
        }
        else if (j == 7)
        {
          cout << p[127];
        }
        else if (j == 81)
        {
          cout << p[76];
        }
        else if (j == 83)
        {
          cout << p[77];
        }
        else if (j == 85)
        {
          cout << p[78];
        }
        else if (j == 87)
        {
          cout << p[79];
        }
        else
          cout << ' ';
        }
      }

    // Row edges of squares (excluding ones on inside and outside edges of board)
    else if (i % 4 == 0)
    {
      for (int j = 0; j < 89; j++)
      {
        if ((j >= 0 && j <= 8) || (j >= 80 && j <= 88))
          cout << '#';
        else
          cout << ' ';
      }
    }

    // Column edges of squares on right & left of board
    else if (i >=5 && i <= 39)
    {
      for (int j = 0; j < 89; j++)
      {
        if(j == 0 || j == 8 || j == 80 || j == 88)
          cout << '#';
        else
          cout << ' ';
      }
    }

    // Column edges of squares on top & bottom of board
    else
    {
      for (int j = 0; j < 89; j++)
      {
        if(j % 8 == 0)
          cout << '#';
        else
          cout << ' ';
      }
    }

    cout << endl;
  }
  return;
}
