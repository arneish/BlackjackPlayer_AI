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

    bool isTerminalState;
    bool isVisited;
    bool isBlackJackPlayer;
    vector<pair<BlackJackState*, double>> children;
    vector<pair<BlackJackState*, double>> standChildren;
    vector<pair<BlackJackState*, double>> splitChildren;
    vector<pair<BlackJackState*, double>> splitAceChildren;
    vector<int> allActions;
    double rewardOnReachingState;
    pair<double,double> stateValue; //<oldStateval, newStateval>
    unordered_map<int, pair<double, double>> Qvalmap; //<action,<old_Qval_action, new_Qval_action>>

    bool isBlackjackPlayer();

    BlackJackState(int NumAces, int NumPairs, int playerInitialValue, int dealerInitialCard);
    BlackJackState();
};

class BlackJackAgent
{
public:
    double probability;
    unordered_map<string, BlackJackState*> keyToState;
    unordered_map<string, BlackJackState*> keyToStateDealer;
    unordered_set<BlackJackState*> allInitStates;

    void constructPolicyGraph();
    void initStates(int NumAces, int NumPairs, int player_it);
    void createTerminalStates();
    void constructStateSpace(BlackJackState *curState, int PlayerID);
    void createNextDealerState(BlackJackState *parentState, int newDealerHand, bool isBlackJack, int AceStateChild, double probChildState);
    void createNextPlayerState(BlackJackState *curState, int newPlayerHand, int AceStateChild, int isPairChild, double probChildState);
    void executeMove(BlackJackState* curState,int action, int PlayerID);
    void getPossibleActions(BlackJackState *curState, int PlayerID);
    void executeValueIteration();
    void printPolicy();
    char getActionChar(int action);
    int getRow(BlackJackState* initState);
    BlackJackAgent() {probability = 4.0/13.0;}
};
