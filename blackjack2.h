#include <vector>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <assert.h>

#define HIT 1
#define STAND 2
#define DOUBLE 3
#define SPLIT 4
#define SPLIT_ACE 5

#define DEALER 1
#define PLAYER 2

//#define HAS_ACE 2
//
//#define HAS_PAIR 2
//#define MAX_HAND_VALUE 21
//#define NUM_STATE_TYPES 3

#define NO_ACE 0
#define SOFT_HAND 1
#define HARD_HAND 2

using namespace std;

class BlackJackState
{
public:
  int isPair;
  int handValuePlayer;
  int AceStatePlayer;
  int handValueDealer;
  int AceStateDealer;

  double rewardOnReachingState;
  double stateValue;
  bool isTerminalState;
  bool isVisited;
  bool isBlackJackPlayer;
  vector<BlackJackState *> children;
  vector<BlackJackState *> standChildren;
  vector<int> allActions;
  vector<double> Q_action;

//  bool isBustedPlayer();
//  bool isBustedDealer();
  bool isBlackjackPlayer();
//  bool isBlackjackDealer();

  BlackJackState(int NumAces, int NumPairs, int playerInitialValue, int dealerInitialCard);
  BlackJackState();
};

class BlackJackAgent
{
public:
  double probability;
  unordered_map<string, BlackJackState *> keyToState;
  unordered_map<string, BlackJackState*> keyToStateDealer;
  unordered_set<BlackJackState *> allInitStates;

  void constructPolicyGraph();
  void initStates(int NumAces, int NumPairs, int player_it);
  void createTerminalStates();
  void constructStateSpace(BlackJackState *curState, int PlayerID);
  void createNextDealerState(BlackJackState *parentState, int newDealerHand, bool isBlackJack, int AceStateChild);
  void createNextPlayerState(BlackJackState *curState, int newPlayerHand, int AceStateChild, int isPairChild);
  void executeMove(BlackJackState* curState,int action, int PlayerID);
  void getPossibleActions(BlackJackState *curState, int PlayerID);
};
