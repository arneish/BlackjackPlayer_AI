#include <vector>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <assert.h>
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
#define MAX_NUM_ACES 21
#define NUM_STATE_TYPES 3

#define NO_ACE 0
#define SOFT_HAND 1
#define HARD_HAND 2

using namespace std;

class BlackJackState
{
public:
  int state_type;
  int isPair;
  int handValuePlayer;
  int numCardsPlayer;
  int AceStatePlayer;
  int handValueDealer;

  double rewardOnReachingState;
  double stateValue;
  bool isTerminalState;
  vector<BlackJackState *> children;
  vector<int> allActions;
  vector<double> Q_action;

  bool isBustedPlayer();
  bool isBustedDealer();
  bool isBlackjackPlayer();
  bool isBlackjackDealer();

};

class BlackJackAgent
{
public:
  double probability;
  // indexing always begins from 1 in the bottom
  //numAces, hasPair, handValue, dealerCard - contains Null also
  BlackJackState *nonTerminalStates[NUM_STATE_TYPES][HAS_PAIR][MAX_HAND_VALUE + 1][MAX_HAND_VALUE + 1];
  /*initalize in constructor [dealer][player_i][player_j]
      null for any index = 1, and null for player_j < player_i*/
  unordered_map<string, BlackJackState *> keyToState;
  vector<BlackJackState *> allInitStates;

  void constructPolicyGraph();
  void initStates(int NumAces, int NumPairs, int player_it);
  void createTerminalStates();
  void constructStateSpace(BlackJackState *state, int PlayerID);
  void createNextPlayerState(BlackJackState* oldState, int newPlayerHand);
  BlackJackState* executeMove(BlackJackState* curState,int action, int PlayerID);
  void getPossibleActions(BlackJackState *curState, int PlayerID);
};
