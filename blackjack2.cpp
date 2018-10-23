#include "blackjack2.h"
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

void BlackJackAgent::createNextPlayerState(BlackJackState *curState, int newPlayerHand)
{
    BlackJackState *nextState;
    string key = to_string(curState->AceStatePlayer) + "$" + to_string(curState->isPair) + "$" + to_string(newPlayerHand) + "$" + to_string(curState->handValueDealer);
    if (keyToState.count(key))
    {
        curState->children.emplace_back(keyToState[key]);
    }
    else
    {
        nextState = new BlackJackState();
        /*make relevant updates here:



        */
        keyToState[key] = nextState;
        curState->children.emplace_back(nextState);
    }
}

BlackJackState* BlackJackAgent::executeMove(BlackJackState *curState, int action, int PlayerID)
{
    if (PlayerID == PLAYER)
    {
        if (action == HIT)
        {
            int oldPlayerHand = curState->handValuePlayer;
            int next_card;
            for (next_card = 2; next_card <= 10; next_card++) /*iterating over Non-Ace next card*/
            {
                int newPlayerHand = oldPlayerHand + next_card;
                if (newPlayerHand <= 21)
                {
                    createNextPlayerState(curState, newPlayerHand);
                }
                else if (curState->AceStatePlayer == SOFT_HAND)
                {
                    newPlayerHand -= 10;
                    assert(newPlayerHand <= 21);
                    createNextPlayerState(curState, newPlayerHand);
                }
                else 
                {   /*Player is Busted*/
                    curState->children.emplace_back(keyToState["-1"]);
                }
            }
            next_card = 1;
            if (curState->AceStatePlayer==NO_ACE)
            //TO FINISH

        }
        else if (action == STAND)
        {
        }
        else if (action == SPLIT_ACE)
        {
        }
        else if (action == SPLIT)
        {
        }
        else if (action == DOUBLE)
        {
        }
    }
    else
    {
    }
}
void BlackJackAgent::getPossibleActions(BlackJackState *curState, int PlayerID)
{
    if (PlayerID == PLAYER)
    {
        curState->allActions = {HIT, STAND};
        //We can always HIT and STAND since it's not BUSTED if control reaches here
        if (curState->numCardsPlayer == 2)
        {
            curState->allActions.emplace_back(DOUBLE);
        }
        if (curState->isPair)
        {
            if (curState->AceStatePlayer == SOFT_HAND)
                curState->allActions.emplace_back(SPLIT_ACE);
            else
                curState->allActions.emplace_back(SPLIT);
        }
    }
    else /*Dealer's deterministic moves*/
    {
    }
}

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

bool BlackJackState::isBustedPlayer()
{
    return handValuePlayer > 21;
}

bool BlackJackState::isBustedDealer()
{
    return handValueDealer > 21;
}

bool BlackJackState::isBlackjackPlayer()
{
    return numAcesPlayer == 1 && handValuePlayer == 21 && numCardsPlayer == 2;
}

bool BlackJackState::isBlackjackDealer()
{
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

int calcHandValue(int player_i, int player_j)
{
    if (player_i == player_j && player_i == 1)
        return 12;
    return player_i + player_j;
}

void BlackJackAgent::initStates(int NumAces, int NumPairs, int player_it)
{
    for (int dealer_it = 2; dealer_it <= 11; dealer_it++)
    {
        BlackJackState *state = new BlackJackState();
        string key = to_string(NumAces) + "$" + to_string(NumPairs) + "$" + to_string(player_it) + "$" + to_string(dealer_it);
        keyToState[key] = state;
        allInitStates.emplace_back(state);
    }
}

void BlackJackAgent::createTerminalStates()
{
    vector<string> terminalKeys = {"1", "15", "0", "-1", "-15"};
    for (auto &key : terminalKeys)
    {
        BlackJackState *terminalState = new BlackJackState();
        keyToState[key] = terminalState;
    }
}

void BlackJackAgent::constructPolicyGraph()
{
    /*Construct initial state
    Basis: 1.NumAces[NO_ACE,SOFT_HAND,HARD_HAND] 2.NumPairs[0,1] 3.PlayerHandValues[MAX_HAND_VALUE] 4.DealerHandValues
    Key: NumAces+"$"+NumPairs+"$"+PlayerHandValues+"$"+DealerHandValues
    */
    for (int NumAces = 0; NumAces < 2; NumAces++)
    {
        for (int NumPairs = 0; NumPairs < 2; NumPairs++)
        {
            if (NumAces == 0 && NumPairs == 0)
            {
                for (int player_it = 5; player_it <= 19; player_it++)
                {
                    /*Non-Ace Non-Pair Cards*/
                    initStates(NumAces, NumPairs, player_it);
                }
            }
            else if (NumAces == 0 && NumPairs == 1)
            {
                /*All Non-Ace Pairs*/
                for (int player_it = 4; player_it <= 20; player_it++)
                {
                    initStates(NumAces, NumPairs, player_it);
                }
            }
            else if (NumAces == 1 && NumPairs == 0)
            {
                /*Single Ace [SOFT_HAND]*/
                for (int player_it = 13; player_it <= 21; player_it++)
                {
                    initStates(NumAces, NumPairs, player_it);
                }
            }
            else if (NumAces == 1 && NumPairs == 1)
            {
                /*Ace Pairs*/
                initStates(NumAces, NumPairs, 12);
            }
        }
    }
    /*Initialise Terminal states */
    createTerminalStates();

    /*Iterate over all initial states one-by-one in a DFS Manner constructing the graph*/
    for (auto &initState : allInitStates)
    {
        constructStateSpace(initState, PLAYER);
    }

    for (int i = 0; i < MAX_NUM_ACES + 1; i++)
        for (int j = 0; j < HAS_PAIR; j++)
            for (int k = 0; k < MAX_HAND_VALUE; k++)
                for (int l = 0; l < MAX_DEALER_CARD; l++)
                    constructStateSpace(initialStateMap[i][j][k][l]);
}

void BlackJackAgent::constructStateSpace(BlackJackState *curState, int PlayerID)
{
    /* "state" is the current_state in which the move (if possible) is to be made by PlayerID */
    if (PlayerID == PLAYER) /*Game play for PLAYER*/
    {
        if (curState->isBustedPlayer())
        { /*terminal-test while PlayerID==PLAYER; PLAYER does not make any move.*/
            curState->children.emplace_back(keyToState["-1"]);
        }
        else if (curState->isBlackjackPlayer())
        { /*terminal-test while PlayerID==PLAYER; Check if DEALER can get a BlackJack*/
            if (curState->handValueDealer == 11 || curState->handValueDealer == 10)
            {
                curState->getActionDealer();
            }
            else
            {
                curState->children.emplace_back(keyToState["15"]);
            }
        }
        /*Compute all actions for PLAYER*/
        curState->getPossibleActions(PLAYER);
        for (auto &action : curState->allActions)
        {
            curState->executeMove(action, PLAYER);
        }
    }
    else
    {
        /*Game play for DEALER*/
    }
    // check terminals
    if (state->isBustedPlayer() || state->isBustedDealer() || state->isBlackjackPlayer() || state->isBlackjackDealer())
    {
        state->stateValue = 0;
        state->isTerminalState = true;
        return;
    }

    vector<char> possibleActions = getPossibleActions(state);

    for (char action : possibleActions)
        executeMove(action);
}

vector<char> BlackJackAgent(BlackJackState *state)
{
}

void BlackJackAgent::executeMove(char moveType)
{
    switch (moveType)
    {
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
