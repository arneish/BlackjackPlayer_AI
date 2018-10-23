#include <vector>
#include <numeric>
#include <algorithm>
#define HIT 1
#define STAND 2
#define DOUBLE 3
#define SPLIT 4
#define SPLIT_ACE 5
#define DEALER 1
#define PLAYER 2


#define HAS_ACE 2
#define HAS_PAIR 2
#define MAX_HAND_VALUE 21
#define MAX_DEALER_CARD 10
#define MAX_NUM_ACES 21

using namespace std;

class BlackJackState
{
  public:
    /* Player's Features */
    int numCardsPlayer;
    int numTurnsPlayedPlayer;
    //int valueCardsPlayerSoft;
    //int valueCardsPlayerHard;
    bool hasDoubledPlayer;
    bool hasSplitPlayer;
    bool hasSplitAcePlayer;
    vector<int> cardsPlayer;
    vector<BlackJackState *> children;


    bool hasAcePlayer;
    bool hasPairPlayer;
    int handValuePlayer;



    inline bool canSplit()
    {
        if (hasPairPlayer)
        {
            return !hasSplitAcePlayer;
        }
        return false;
    };
    inline bool canDouble() { return numTurnsPlayedPlayer == 0; }
    pair<int, int> getValueHandPlayer(); /*returns soft, hard value of Player's hand*/
    bool isBustedPlayer();
    bool isBlackjackPlayer();

    /* Dealer's Features */
    int numCardsDealer;
    int valueCardsDealer;
    bool hasAceDealer;
    bool dealerToPlay; //true == dealer's turn
    vector<int> cardsDealer;
    pair<int, int> getValueHandDealer();
    bool isBustedDealer();
    bool isBlackjackDealer();
    int getActionDealer();

    /* Game Manager */
    double betPlayer;
    double stateValue;
    bool isTerminalState;
    double endGame(int bustedPerson);
};

class BlackJackAgent
{
  public:
    double probability;
    // indexing always begins from 1 in the bottom
    //numAces, hasPair, handValue, dealerCard - contains Null also
    BlackJackState *initialStateMap[MAX_NUM_ACES + 1][HAS_PAIR][MAX_HAND_VALUE + 1][MAX_DEALER_CARD + 1];
    /*initalize in constructor [dealer][player_i][player_j]
      null for any index = 1, and null for player_j < player_i*/
    void constructPolicyGraph();
    void constructStateSpace(BlackJackState *state);
};
