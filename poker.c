//Marc Miles
//12/19/2023
//Personal Project
//Texas Hold'em (No Limit) Simulator

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TOTALCARDS 52

//Structs
typedef struct card{
    //10, 11, 12 == Jack, Queen, King
    int value;
    //Suit number respectively - Spades, Hearts, Diamonds, Clubs
    int suit;
}card;

typedef struct node{
    card value;
    struct node* next;
} node;

typedef struct hand{
    card c1;
    card c2;
} hand;

//Functions
//*Insert a new card into the back of the deck
node* insert_card(node** head, card this_card) {
    //Skip first insertion because of head node creation
    if(this_card.suit == 0 && this_card.value == 0) {
        return *head;
    }
    //Create new node
    node* new_node = malloc(sizeof(node));
    new_node->value = this_card;
    new_node->next = NULL;
    //Insert node to back of linked list
    if(*head == NULL) {
        *head = new_node;
        return *head;
    }
    node* last = *head;
    while(last->next != NULL) {
        last = last->next;
    }
    last->next = new_node;
    return *head;
}

//*Create each card in the deck
void initialize_deck(node* head) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 13; j++) {
            card* new_card = malloc(sizeof(card));
            new_card->value = j;
            new_card->suit = i;
            head = insert_card(&head, *new_card);
        }
    }
}

//*Shuffle the cards
void shuffle_deck(node* head) {
    //Create a temporary list, node, and counting int
    card* temp_array = malloc(TOTALCARDS * sizeof(card));
    node* temp_node = head;
    int i = 0;
    //Copy the cards into the temporary list
    while(temp_node != NULL) {
        temp_array[i++] = temp_node->value;
        temp_node = temp_node->next;
    }
    //Shuffle the temporary array
    for(i = TOTALCARDS; i > 0; i--) {
        int j = rand() % (i+1);
        card temp_card = temp_array[i];
        temp_array[i] = temp_array[j];
        temp_array[j] = temp_card;
    }
    temp_node = head;
    //Replace the deck with the newly shuffled deck
    for(i = 0; i < TOTALCARDS; i++) {
        temp_node->value = temp_array[i];
        temp_node = temp_node->next;
    }
    //Free memory
    free(temp_array);
}

//*After dealing cards, remove dealt cards from the deck
void adjust_deck(node** head, int dealt_cards) {
    for(int i = 0; i < dealt_cards; i++) {
        node* temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}

//*Deal hole cards to each player
hand* deal_hole(node* head, int players) {
    hand* player_hands = malloc(sizeof(hand) * players);
    for(int i = 0; i < players; i++) {
        player_hands[i].c1 = head->value;
        head = head->next;
        player_hands[i].c2 = head->value;
        head = head->next;
        //*Print values of hands during dev
        //printf("Player %d, c1: %d %d, c2: %d %d\n", i, player_hands[i].c1.value, player_hands[i].c1.suit, player_hands[i].c2.value, player_hands[i].c2.suit);
    }
    return player_hands;
}

//*Begin a round of betting
//!Most is irrelevant until CPU AI is in place
int bet_chips(hand** player_hands, int* player_chips, int players) {
    char* command = malloc(sizeof(char) * 6);
    int betting = 0, raise, pot_addition = 0;
    while(betting != 1) {
        printf("Would you like to check, call, raise, or fold?\n");
        scanf("%s", command);
        if(strncmp(command, "check", 5) == 0) {
            printf("All players check.\n");
            return 0;
        }
        else if(strncmp(command, "call", 4) == 0) {
            printf("You are unable to call at this time.\n");
            continue;
        }
        else if(strncmp(command, "raise", 5) == 0) {
            printf("How much would you like to raise?\n");
            scanf("%d", &raise);
            if(player_chips[0] >= raise) {
                for(int i = 0; i < players; i++) {
                    if(player_chips[i] < raise) {
                        printf("Player %d only has %d chips, so they are by default, all in.\n", i, player_chips[i]);
                        pot_addition += player_chips[i];
                        player_chips[i] = 0;
                        continue;
                    }
                    player_chips[i] -= raise;
                    pot_addition += raise;
                    printf("Player %d is at %d chips.\n", i, player_chips[i]);
                    if(player_chips[i] == 0) {
                        printf("Player %d is all in.\n", i);
                    }
                }
            }
            else {
                printf("You do not have %d chips in your possession.\n", raise);
                continue;
            }
            return pot_addition;
        }
        else if(strncmp(command, "fold", 4) == 0) {
            printf("You have left the table.\n");
            exit(0);
            return 0;
        }
        else {
            printf("Please enter a valid command.\n");
            continue;
        }
    }
    return 0;
}

//*Test the deck during dev
void print_deck(node* head) {
    node* test = head;
    int count = 0;
    while(test != NULL) {
        printf("%d %d\n", test->value.value, test->value.suit);
        test = test->next;
        count++;
    }
    printf("Total cards in deck: %d\n", count);
}

//*Test the chip array during dev
void print_chips(int* chip_list, int players) {
    for(int i = 0; i < players; i++) {
        printf("Player %d has %d chips.\n", i, chip_list[i]);
    }
}

//Main program
int main(void) {
    //Seed RNG and create variables including deck
    srand(time(NULL));
    int players, starting_chips, pot = 0;
    node* head = malloc(sizeof(node));
    initialize_deck(head);
    //Shuffle
    shuffle_deck(head);
    //# of players and chips
    printf("Enter the amount of players desired(including yourself): ");
    scanf("%d", &players);
    printf("Enter the amount of chips each player starts with: ");
    scanf("%d", &starting_chips);
    //Create the array of player chips
    int* player_chips = malloc(sizeof(int) * players);
    for(int i = 0; i < players; i++) {
        player_chips[i] = starting_chips;
    }
    print_chips(player_chips, players);
    //Deal cards to each player
    hand* player_hands = deal_hole(head, players);
    adjust_deck(&head, (players*2));
    ////
    //*Print the current player hands
    for(int i = 0; i < players; i++) {
        printf("Player %d, c1: %d %d, c2: %d %d\n", i, player_hands[i].c1.value, player_hands[i].c1.suit, player_hands[i].c2.value, player_hands[i].c2.suit);
    }
    ////
    pot += bet_chips(&player_hands, player_chips, players);
    printf("The pot is currently at %d chips.\n", pot);
    return 0;
}
