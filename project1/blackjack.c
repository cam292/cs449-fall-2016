/*
* Project 1: Blackjack
* made for Pitt cs449 fall 2016
* @author Craig Mazzotta
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct Hand {
  int value; //sum of cards in hand
  int aces; //number of aces in hand
  int card; //last card drawn
};

int eval(struct Hand*);
int dealCard(struct Hand*);
int randomCard();


/*
* The main game loop. Assign 1 card to dealer and 2 to the
* user. Prompt user to hit or stay, deal a card to the user
* when they hit. If they stay, keep dealing cards to the dealer
* until their total is over 17 or over 21. Player with the score
* closest to 21 without going over is the winner.
*/
int main(){
  srand((unsigned)time(NULL)); //seed random number generator

  struct Hand dealer;
  struct Hand player;

  dealer.value = 0;
  dealer.aces = 0;
  player.value = 0;
  player.aces = 0;

  dealCard(&dealer); //deal first card to dealer
  dealCard(&dealer);
  dealCard(&player); //deal first card to user
  //display cards
  printf("\nDealer:\n? + %d\n\n", dealer.card);
  printf("Player:\n%d + ", player.value);
  dealCard(&player);
  printf("%d = %d ", player.card,player.value);

  if(player.value == 21){ //hand dealt to start was blackjack
    printf("Blackjack!\n\nBlackjack! You win!\n");
    return 0;
  }

  char input[10];
  printf("\n \n");
  printf("Would you like to hit or stand? "); //prompt for user action
  scanf("%s", input);
  printf("\n");

  const char *hit = "hit";
  int result;
  result = strcmp(input,hit); //result is 0 if user enters 'hit'

  while(result == 0){ //while user hits, deal new cards
    printf("Dealer:\n? + %d\n\n", dealer.card);
    printf("Player:\n%d + ", player.value);
    dealCard(&player);
    printf("%d = %d ", player.card,player.value);

    if(player.value == 21){ //player gets blackjack
      printf("Blackjack!\n\nBlackjack! You win!\n");
      return 0;
    }
    else if(player.value > 21){ //player busts
      printf("Busted!\n\nYou busted. Dealer wins.");
      printf("\n");
      return 0;
    }
    else{ //player hand value is less than 21, prompt for action
      printf("\n\nWould you like to hit or stand? ");
      scanf("%s", input);
      printf("\n");
      result = strcmp(input,hit);
    }
  }

  if(player.value < 21){ //player hasn't busted or had black jack
    printf("Dealer:\n%d + %d ", dealer.value-dealer.card, dealer.card);
    while(dealer.value < 17 || dealer.value < player.value){ //deal cards to dealer while hand is less than 21 or they're losing to player
      dealCard(&dealer);
      printf("+ %d ", dealer.card);
    }
    printf("= %d ", dealer.value);

    if(dealer.value == 21){ //dealer gets blackjack
      printf("Blackjack!\n\n");
    }
    else if(dealer.value > 21){ //dealer busts
      printf("Busted!\n\n");
    }
    else{
      printf("\n\n");
    }
    printf("Player:\n%d\n\n", player.value);
  }

  if(dealer.value == 21){ //dealer got blackjack
    printf("Blackjack! Dealer wins.\n");
  }
  else if(dealer.value > 21){ //dealer busted
    printf("Dealer busted! You win!\n");
  }
  else if(dealer.value > player.value){ //dealer is closer to 21
    printf("Dealer is closer to 21. Dealer wins.\n");
  }

  return 0;
}


/**
* Evaluates a new hand value if the current value is over 21
* and the hand contains aces.
*
* @param player Value of hand after a new card is added
* @param aces The number of aces in a players hand
* @return The sum of the players hand
*/
int eval(struct Hand *player){
  while(player->value > 21 && player->aces > 0){
    player->value -= 10; //change ace value from 11 to 1
    player->aces--; //subtract an ace from the hand
    player->card = 1; //set previous card value from 11 to 1
  }
  return player->value;
}

/**
* Generates next card for player and adds to total,
* checking if there are aces and handling it
*
* @param *player Pointer to the players struct
* @return The sum of the players hand
*/
int dealCard(struct Hand *player){
  int newCard = randomCard();
  if(newCard == 11) player->aces++; //card drawn was an ace
  player->card = newCard;
  player->value += newCard;
  return eval(player); //handle value over 21 with aces in hand
}

/**
* Generates a random card from a deck
* @return card The value of the card drawn
*/
int randomCard(){
  int card = rand() % (13); //numbers 0 through 12
  if(0<= card && card <=9) card+=2; //card drawn is 2-10, add offset
  else if(9<= card && card<=11) card = 10; //card drawn is a face card
  else if(card == 12) card = 11; //card is an ace
  return card;
}
