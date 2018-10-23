#include "blackjack2.h"

using namespace std;


pair<int, int> BlackJackState::getValueHandPlayer()
{
    int numAces = count(cardsPlayer.begin(), cardsPlayer.end(), 0);
    if (numAces > 0)
    {
        int sum = accumulate(cardsPlayer.begin(), cardsPlayer.end(), 0);
        int softval = sum + 11 + (numAces - 1);
        return make_pair(softval, numAces + sum);
    }
    else
    {
        int val = accumulate(cardsPlayer.begin(), cardsPlayer.end(), 0);
        return make_pair(val, val);
    }
}

bool BlackJackState::isBustedPlayer() {
    return handValuePlayer > 21;
}

bool BlackJackState::isBustedDealer() {
   return handValueDealer > 21;
}

bool BlackJackState::isBlackjackPlayer() {
    return numAcesPlayer == 1 && handValuePlayer == 21 && numCardsPlayer == 2;
}

bool BlackJackState::isBlackjackDealer() {
    return dealerVisibleCard == 1 && handValueDealer == 21 && numCardsDealer == 2;
}

int BlackJackState::getActionDealer()
{
    pair<int, int> ValueHandDealer = getValueHandDealer();
    if (ValueHandDealer.first < 17 || ValueHandDealer.second < 17)
        return HIT;
    else
        return STAND;
}

double BlackJackState::endGame(int bustedPerson = -1)
{
    /*triggered when getActionDealer() returns STAND
    return PROFITS for player*/
    if (bustedPerson == PLAYER)
    {
        return -betPlayer;
    }
    else if (bustedPerson == DEALER)
    {
        return betPlayer;
    }
    if (isBlackjackPlayer() && isBlackjackDealer())
        return 0;
    else if (isBlackjackPlayer())
        return 1.5 * betPlayer;
    else if (isBlackjackDealer()) //TODO : SEE IF -1.5
        return -betPlayer;
    else
    {
        int maxValueDealer = max(getValueHandDealer().first, getValueHandDealer().second);
        int maxValuePlayer = max(getValueHandPlayer().first, getValueHandPlayer().second);
        return (maxValueDealer > maxValuePlayer) ? -betPlayer : (maxValueDealer < maxValuePlayer ? betPlayer : 0);
    }
}

int calcHandValue(int player_i, int player_j) {
    if(player_i == player_j && player_i == 1)
        return 12;
    return player_i + player_j;
}

void BlackJackAgent::constructPolicyGraph() {
    //construct initial state
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++){
            if(i==0 && j==0) {
                for(int player_it = 5; player_it <= 19; player_it++) {
                    for(int dealer_it = 1; dealer_it <= 10; dealer_it++) {
                        BlackJackState *state = new BlackJackState();

                    }
                }
            }

            if(i==0 && j==1) {

            }

            if(i==1 && j==0) {

            }

            if(i==1 && j==1) {

            }
        }
    }

    for(int i = 0; i < MAX_NUM_ACES + 1; i++)
        for(int j = 0; j < HAS_PAIR; j++)
            for(int k = 0; k < MAX_HAND_VALUE; k++)
                for(int l = 0; l < MAX_DEALER_CARD; l++)
                    constructStateSpace(initialStateMap[i][j][k][l]);
}

void BlackJackAgent::constructStateSpace(BlackJackState *state)
{
    // check terminals
    if (state->isBustedPlayer() || state->isBustedDealer() || state->isBlackjackPlayer() || state->isBlackjackDealer()) {
        state->stateValue = 0;
        state->isTerminalState = true;
        return;
    }

    vector<char> possibleActions = getPossibleActions(state);

    for(char action : possibleActions)
        executeMove(action);

}

vector<char> BlackJackAgent(BlackJackState* state){

}

void BlackJackAgent::executeMove(char moveType) {
    switch(moveType){
        case 'H':
            break;
        case 'S':
            break;
        case 'D':
            break;
        case 'P':
            break;
        default:
            return;
    }
}


