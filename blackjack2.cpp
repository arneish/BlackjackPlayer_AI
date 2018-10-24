#include "blackjack2.h"
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

void BlackJackAgent::executeValueIteration()
{
    /*initialise V to 0 for non-terminal states*/
    for (auto&playerState: keyToState)
    {
        playerState.second->stateValue.first = 0;
    }
    for (auto&dealerState: keyToStateDealer)
    {
        dealerState.second->stateValue.first = 0;
    }
    for (auto&playerState: keyToState)
    {
        BlackJackState* curState = playerState.second;
        for (auto&action: curState->allActions)
        {
            curState->
        }
    
    }
    



}

void BlackJackAgent::createNextDealerState(BlackJackState *parentState, int newDealerHand, bool isBlackJackDealer, int AceStateChild)
{
    // newdDealerHand <= 21 always, busts are handled in executeMove
    //key: playerHandValue + "$"+ IsPlayerBlackJack + "$" + DealerHandValue + "$" + AceStateDealer

    if (isBlackJackDealer)
    {
        if (parentState->isBlackjackPlayer())
        {
            parentState->standChildren.emplace_back(keyToState["0"]);
        }
        else
        {
            parentState->standChildren.emplace_back(keyToState["-1"]);
        }
        return;
    }

    string key = to_string(parentState->handValuePlayer) + "$" + to_string(parentState->isBlackjackPlayer()) + "$" + to_string(newDealerHand) + "$" + to_string(AceStateChild);
    if (keyToStateDealer.count(key))
    {
        keyToStateDealer[key]->isVisited = true;
        parentState->standChildren.emplace_back(keyToStateDealer[key]);
    }
    else
    {
        BlackJackState *nextState = new BlackJackState();
        nextState->isVisited = false;
        nextState->handValueDealer = newDealerHand;
        nextState->handValuePlayer = parentState->handValuePlayer;
        nextState->isPair = parentState->isPair;
        nextState->AceStateDealer = AceStateChild;
        nextState->AceStatePlayer = parentState->AceStatePlayer;
        keyToStateDealer[key] = nextState;
        parentState->standChildren.emplace_back(nextState);
    }
}

void BlackJackAgent::createNextPlayerState(BlackJackState *parentState, int newPlayerHand, int AceStateChild, int isPairChild)
{
    string key = to_string(AceStateChild) + "$" + to_string(isPairChild) + "$" + to_string(newPlayerHand) + "$" + to_string(parentState->handValueDealer);
    if (keyToState.count(key))
    {
        keyToState[key]->isVisited = true;
        parentState->children.emplace_back(keyToState[key]);
    }
    else
    {
        BlackJackState *nextState = new BlackJackState();
        /*make relevant updates here:*/
        nextState->isVisited = false;
        nextState->handValueDealer = parentState->handValueDealer;
        nextState->handValuePlayer = newPlayerHand;
        nextState->isPair = isPairChild;
        nextState->AceStatePlayer = AceStateChild;
        nextState->AceStateDealer = parentState->AceStateDealer;
        keyToState[key] = nextState;
        parentState->children.emplace_back(nextState);
    }
}

void BlackJackAgent::executeMove(BlackJackState *curState, int action, int PlayerID)
{
    if (PlayerID == PLAYER)
    {
        if (action == HIT)
        {
            int oldPlayerHand = curState->handValuePlayer;
            int next_card = 0;
            for (next_card = 2; next_card <= 10; next_card++) /*iterating over Non-Ace next card*/
            {
                int newPlayerHand = oldPlayerHand + next_card;
                if (newPlayerHand <= 21)
                {
                    createNextPlayerState(curState, newPlayerHand, curState->AceStatePlayer, false);
                }
                else if (curState->AceStatePlayer == SOFT_HAND)
                {
                    newPlayerHand -= 10;
                    assert(newPlayerHand <= 21);
                    createNextPlayerState(curState, newPlayerHand, HARD_HAND, false);
                }
                else
                { /*Player is Busted*/
                    curState->children.emplace_back(keyToState["-1"]);
                }
            }

            // next_card is ACE
            if (curState->AceStatePlayer == NO_ACE)
            {
                int newPlayerHand = oldPlayerHand + 11;
                if (newPlayerHand > 21)
                {
                    newPlayerHand -= 10;
                    assert(newPlayerHand <= 21);
                    createNextPlayerState(curState, newPlayerHand, HARD_HAND, false);
                }
                else
                {
                    assert(newPlayerHand <= 21);
                    createNextPlayerState(curState, newPlayerHand, SOFT_HAND, false);
                }
            }
            else if (curState->AceStatePlayer == SOFT_HAND)
            {
                int newPlayerHand = oldPlayerHand + 1;
                assert(newPlayerHand <= 21);
                createNextPlayerState(curState, newPlayerHand, SOFT_HAND, false);
            }
            else
            {
                /*HARD_HAND*/
                int newPlayerHand = oldPlayerHand + 1;
                assert(newPlayerHand <= 21);
                createNextPlayerState(curState, newPlayerHand, HARD_HAND, false);
            }
        }
        else if (action == STAND)
        {
            /*Begin dealer moves*/
            if (curState->handValueDealer == 11) /*Dealer starts with an ACE*/
            {
                for (int next_card = 1; next_card <= 10; next_card++)
                {
                    if (next_card == 10)
                    {
                        createNextDealerState(curState, 21, true, curState->AceStateDealer);
                    }
                    else
                    {
                        if (curState->isBlackjackPlayer())
                            curState->standChildren.emplace_back(keyToState["15"]);
                        else
                            createNextDealerState(curState, curState->handValueDealer + next_card, false, SOFT_HAND);
                    }
                }
            }
            else if (curState->handValueDealer == 10) /*Dealer starts with a FACE*/
            {
                for (int next_card = 2; next_card <= 11; next_card++)
                {
                    if (next_card == 11)
                    {
                        createNextDealerState(curState, 21, true, curState->AceStateDealer);
                    }
                    else
                    {
                        if (curState->isBlackjackPlayer())
                            curState->standChildren.emplace_back(keyToState["15"]);
                        else
                            createNextDealerState(curState, curState->handValueDealer + next_card, false, NO_ACE);
                    }
                }
            }
            else /*Dealer starts with a non-ACE non-FACE */
            {
                if (curState->isBlackjackPlayer())
                    curState->standChildren.emplace_back(keyToState["15"]);
                else
                {
                    for (int next_card = 2; next_card <= 11; next_card++)
                    {
                        if (next_card == 11)
                            createNextDealerState(curState, curState->handValueDealer + next_card, false, SOFT_HAND);
                        else
                            createNextDealerState(curState, curState->handValueDealer + next_card, false, NO_ACE);
                    }
                }
            }
        }
        else if (action == SPLIT_ACE)
        {
            // add no children, simply take expectation over the possible combinations of hands
        }
        else if (action == SPLIT)
        {
            // add no children, simply take expectation over all possible combinations of hands during value iteration
        }
        else if (action == DOUBLE)
        {
            // add no children, take twice expectation of Q values
        }
    }
    else
    { /* Starts from Dealer's third card enumeration*/
        if (action == HIT)
        {
            int oldDealerHand = curState->handValueDealer;
            int next_card = 0;
            for (next_card = 2; next_card <= 10; next_card++) /*iterating over Non-Ace next card*/
            {
                int newDealerHand = oldDealerHand + next_card;
                if (newDealerHand <= 21)
                {
                    createNextDealerState(curState, newDealerHand, false, curState->AceStateDealer);
                }
                else if (curState->AceStateDealer == SOFT_HAND)
                {
                    newDealerHand -= 10;
                    assert(newDealerHand <= 21);
                    createNextDealerState(curState, newDealerHand, false, HARD_HAND);
                }
                else
                { /*Dealer is Busted*/
                    curState->standChildren.emplace_back(keyToState["1"]);
                }
            }

            // next_card is ACE
            if (curState->AceStateDealer == NO_ACE)
            {
                int newDealerHand = oldDealerHand + 11;
                if (newDealerHand > 21)
                {
                    newDealerHand -= 10;
                    assert(newDealerHand <= 21);
                    createNextDealerState(curState, newDealerHand, false, HARD_HAND);
                }
                else
                {
                    assert(newDealerHand <= 21);
                    createNextDealerState(curState, newDealerHand, false, SOFT_HAND);
                }
            }
            else if (curState->AceStateDealer == SOFT_HAND)
            {
                int newDealerHand = oldDealerHand + 1;
                assert(newDealerHand <= 21);
                createNextDealerState(curState, newDealerHand, false, SOFT_HAND);
            }
            else
            {
                /*HARD_HAND*/
                int newDealerHand = oldDealerHand + 1;
                assert(newDealerHand <= 21);
                createNextDealerState(curState, newDealerHand, false, HARD_HAND);
            }
        }
        else
        {
            // dealer STAND
            /* Game Decision */
            if (curState->handValuePlayer > curState->handValueDealer)
                curState->standChildren.emplace_back(keyToState["1"]);
            else if (curState->handValuePlayer < curState->handValueDealer)
                curState->standChildren.emplace_back(keyToState["-1"]);
            else
                curState->standChildren.emplace_back(keyToState["0"]);
        }
    }
}
void BlackJackAgent::getPossibleActions(BlackJackState *curState, int PlayerID)
{
    if (curState->isTerminalState)
        return;

    if (PlayerID == PLAYER)
    {
        if (curState->handValuePlayer == 21)
        {
            curState->allActions = {STAND};
            return;
        }
        curState->allActions = {HIT, STAND};
        //We can always HIT and STAND since it's not BUSTED if control reaches here

        if (allInitStates.count(curState)) //CHECK if initialstate
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

        if (curState->handValueDealer >= 17)
            curState->allActions = {STAND};
        else
            curState->allActions = {HIT};
    }
}

//pair<int, int> BlackJackState::getValueHandPlayer()
//{
//    int numAces = count(cardsPlayer.begin(), cardsPlayer.end(), 0);
//    if (numAces > 0)
//    {
//        int sum = accumulate(cardsPlayer.begin(), cardsPlayer.end(), 0);
//        int softval = sum + 11 + (numAces - 1);
//        return make_pair(softval, numAces + sum);
//    }
//    else
//    {
//        int val = accumulate(cardsPlayer.begin(), cardsPlayer.end(), 0);
//        return make_pair(val, val);
//    }
//}
//
//bool BlackJackState::isBustedPlayer()
//{
//    return handValuePlayer > 21;
//}
//
//bool BlackJackState::isBustedDealer()
//{
//    return handValueDealer > 21;
//}
//
bool BlackJackState::isBlackjackPlayer()
{
    return isBlackJackPlayer;
}
//
//bool BlackJackState::isBlackjackDealer()
//{
//    return dealerVisibleCard == 1 && handValueDealer == 21 && numCardsDealer == 2;
//}
//
//int BlackJackState::getActionDealer()
//{
//    pair<int, int> ValueHandDealer = getValueHandDealer();
//    if (ValueHandDealer.first < 17 || ValueHandDealer.second < 17)
//        return HIO;
//    else
//        return STAND;
//}
//
//double BlackJackState::endGame(int bustedPerson = -1)
//{
//    /*triggered when getActionDealer() returns STAND
//    return PROFITS for player*/
//    if (bustedPerson == PLAYER)
//    {
//        return -betPlayer;
//    }
//    else if (bustedPerson == DEALER)
//    {
//        return betPlayer;
//    }
//    if (isBlackjackPlayer() && isBlackjackDealer())
//        return 0;
//    else if (isBlackjackPlayer())
//        return 1.5 * betPlayer;
//    else if (isBlackjackDealer()) //TODO : SEE IF -1.5
//        return -betPlayer;
//    else
//    {
//        int maxValueDealer = max(getValueHandDealer().first, getValueHandDealer().second);
//        int maxValuePlayer = max(getValueHandPlayer().first, getValueHandPlayer().second);
//        return (maxValueDealer > maxValuePlayer) ? -betPlayer : (maxValueDealer < maxValuePlayer ? betPlayer : 0);
//    }
//}
//
//int calcHandValue(int player_i, int player_j)
//{
//    if (player_i == player_j && player_i == 1)
//        return 12;
//    return player_i + player_j;
//}

void BlackJackAgent::initStates(int NumAces, int NumPairs, int player_it)
{
    for (int dealer_it = 2; dealer_it <= 11; dealer_it++)
    {
        BlackJackState *state = new BlackJackState(NumAces, NumPairs, player_it, dealer_it);
        string key = to_string(NumAces) + "$" + to_string(NumPairs) + "$" + to_string(player_it) + "$" + to_string(dealer_it);
        keyToState[key] = state;
        allInitStates.insert(state);
    }
}

void BlackJackAgent::createTerminalStates()
{
    vector<string> terminalKeys = {"1", "15", "0", "-1"};
    for (auto &key : terminalKeys)
    {
        BlackJackState *terminalState = new BlackJackState();
        terminalState->isTerminalState = true;
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
                for (int player_it = 4; player_it <= 20; player_it += 2)
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
}

void BlackJackAgent::constructStateSpace(BlackJackState *curState, int PlayerID)
{
    /* "state" is the current_state in which the move (if possible) is to be made by PlayerID */

    //    if (PlayerID == PLAYER) /*Game play for PLAYER*/
    //    {
    //        if (curState->isBlackjackPlayer())
    //        { /*terminal-test while PlayerID==PLAYER; Check if DEALER can get a BlackJack*/
    //            if (curState->handValueDealer == 11 || curState->handValueDealer == 10)
    //            {
    //                curState->getActionDealer();
    //            }
    //            else
    //            {
    //                curState->children.emplace_back(keyToState["15"]);
    //            }
    //        }
    //        /*Compute all actions for PLAYER*/
    //        this->getPossibleActions(PLAYER);
    //        for (auto &action : curState->allActions)
    //        {
    //            this->executeMove(action, PLAYER);
    //        }
    //    }
    //    else
    //    {
    //        /*Game play for DEALER*/
    //    }
    getPossibleActions(curState, PlayerID);

    for (int action : curState->allActions)
        executeMove(curState, action, PlayerID);

    for (BlackJackState *child : curState->children)
    {
        if (!child->isVisited)
            constructStateSpace(child, PLAYER);
    }

    for (BlackJackState *child : curState->standChildren)
    {
        if (!child->isVisited)
            constructStateSpace(child, DEALER);
    }
}

BlackJackState::BlackJackState(int NumAces, int NumPairs, int playerInitialValue, int dealerInitialCard) : isPair(NumPairs), handValuePlayer(playerInitialValue), handValueDealer(dealerInitialCard)
{
    if (NumAces == 0)
        AceStatePlayer = NO_ACE;
    else if (NumAces == 1)
        AceStatePlayer = SOFT_HAND;

    if (dealerInitialCard == 11)
        AceStateDealer = SOFT_HAND;
    else
        AceStateDealer = NO_ACE;

    if (NumAces == 1 && playerInitialValue == 21 && NumPairs == 0)
        isBlackJackPlayer = true;
    else
        isBlackJackPlayer = false;

    stateValue = 0;
    isTerminalState = false;
    isVisited = false;
    rewardOnReachingState = 0;
}

BlackJackState::BlackJackState()
{
    isPair = 0;
    handValuePlayer = 0;
    handValueDealer = 0;
    AceStateDealer = NO_ACE;
    AceStatePlayer = NO_ACE;

    isTerminalState = false;
    isVisited = false;
    rewardOnReachingState = 0;
    stateValue = 0;
}

/*
  int isPair;
  int handValuePlayer;
  int AceStatePlayer;
  int handValueDealer;
  int AceStateDealer;

  double rewardOnReachingState;
  double stateValue;
  bool isTerminalState;
  bool isVisited;
 */