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
    bool hasAcePlayer;
    bool hasPairPlayer;
    vector<int> cardsPlayer;
    vector<BlackJackState *> children;
    BlackJackState(int numCardsPlayer, int numTurnsPlayedPlayer, bool hasDoubledPlayer, bool hasSplitPlayer,
                   bool hasSplitAcePlayer, bool hasAcePlayer, bool hasPairPlayer, const vector<int> &cardsPlayer,
                   const vector<BlackJackState *> &children, int numCardsDealer, int valueCardsDealer,
                   bool hasAceDealer, bool dealerToPlay, const vector<int> &cardsDealer, double betPlayer);
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
    BlackJackState *initialStates[11][11][11];
    /*initalize in constructor [dealer][player_i][player_j]
      null for any index = 1, and null for player_j < player_i*/
    void constructPolicyGraph();
    void constructStateSpace(BlackJackState *state);
};
