#include <iostream>
#include <string>
#include "monopoly.h"

using namespace std;

int main()
{
  srand(time(NULL));
  while(1)
  {
    play_game();
    if(!play_again())
    {
      break;
    }
  }
  return 0;
}
