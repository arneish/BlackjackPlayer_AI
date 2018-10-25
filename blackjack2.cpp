#include "blackjack2.h"
#include <string>
#include <unordered_map>
#include <cmath>
#include <vector>
#include <cfloat>
#include <iostream>

using namespace std;

char BlackJackAgent::getActionChar(int action) {
    if(action == 1)
        return 'H';
    if(action == 2)
        return 'S';
    if(action == 3)
        return 'D';
    if(action == 4 || action == 5)
        return 'P';
}

int BlackJackAgent::getRow(BlackJackState* initState) {
    if(initState->AceStatePlayer == NO_ACE && !initState->isPair)
        return initState->handValuePlayer-5;
    if(initState->isPair && initState->AceStatePlayer == NO_ACE)
        return initState->handValuePlayer/2 -1 + 22;
    if(initState->AceStatePlayer == SOFT_HAND && initState->isPair)
        return 32;
    if(initState->AceStatePlayer == SOFT_HAND && !initState->isPair)
        return initState->handValuePlayer + 2;
}

string BlackJackAgent::getRowName(BlackJackState* state) {
    if(state->AceStatePlayer == NO_ACE && !state->isPair) {
        return to_string(state->handValuePlayer);
    }
    else if(state->AceStatePlayer == SOFT_HAND && state->isPair){
        return "AA";
    }
    else if(state->AceStatePlayer == SOFT_HAND && !state->isPair){
        return "A" + to_string(state->handValuePlayer - 11);
    }
    else if(state->AceStatePlayer == NO_ACE && state->isPair){
        return to_string(state->handValuePlayer/2) + to_string(state->handValuePlayer/2);
    }
}

void BlackJackAgent::printPolicy() {
    char policyArray[33][10];
    string rowNames[33];

    for(auto& state: allInitStates) {
        if(state->handValuePlayer == 21)
            continue;
        int bestAction;
        double bestQVal = -DBL_MAX;
        for(auto& action: state->allActions) {
            if(state->Qvalmap[action].first > bestQVal) {
                bestQVal = state->Qvalmap[action].first;
                bestAction = action;
            }
        }
        int row = getRow(state);
        string rowName = getRowName(state);
        policyArray[row][state->handValueDealer-2] = getActionChar(bestAction);
        rowNames[row] = rowName;
    }

    for(int i = 0; i < 33; i++) {
        cout << rowNames[i] << "\t";
        for(int j = 0; j < 10; j++)
            cout << policyArray[i][j] << " ";
        cout << endl;
    }
}


void BlackJackAgent::executeValueIteration()
{
     /*initialise V to 0 for non-terminal states*/
     for (auto &playerState : keyToState)
     {
         playerState.second->stateValue.first = 0; /*old player state value init 0*/
         for(auto&action:playerState.second->allActions){
             playerState.second->Qvalmap[action].first = 0;
         }
     }
     for (auto &dealerState : keyToStateDealer)
     {
         dealerState.second->stateValue.first = 0; /*old dealer state value init 0*/
         for(auto&action:dealerState.second->allActions){
             dealerState.second->Qvalmap[action].first = 0;
         }
     }
//    double initerror = 0.0;
//    for (auto &playerState : keyToState)
//    {
//        if (playerState.second->isTerminalState)
//            continue;
//        BlackJackState *curState = playerState.second;
//        initerror+=abs(curState->stateValue.second - curState->stateValue.first);
//    }
//    for (auto &dealerState : keyToStateDealer)
//    {
//        BlackJackState *curState = dealerState.second;
//        initerror+=abs(curState->stateValue.second - curState->stateValue.first);
//    }
//    initerror = initerror/(keyToState.size() + keyToStateDealer.size());
//    cerr<<"OUTPUT initialERROR:"<<initerror<<"\n";


    /*Get Q (NEW) for all actions for all states based on V (OLD) & Q (OLD) values */
    /*Step 1: */
    int max_iter = 50;
    while (max_iter-- > 0)
    {
        double error = 0.0;
        for (auto &playerState : keyToState)
        {
            /*All terminal states are in keyToState map*/
            if (playerState.second->isTerminalState)
                continue;
            BlackJackState *curState = playerState.second;
            for (auto &action : curState->allActions)
            {
                if (action == HIT)
                {
                    curState->Qvalmap[action].second = 0; /*update new Q values for HIT for curState (PLAYER STATES) */
                    /*iterate over all PLAYER HIT-children and add old state values*/
                    for (auto &hitChildState : curState->children)
                    {
                        curState->Qvalmap[action].second += hitChildState.second * (hitChildState.first->rewardOnReachingState + hitChildState.first->stateValue.first);
                        /*Q-new-(s, hit) = SIGMA P(s'|s, hit) * [R(s',hit,s)+V-old-(s')] */
                    }
                }
                else if (action == STAND)
                {
                    curState->Qvalmap[action].second = 0;
                    for (auto &standChildState : curState->standChildren)
                    {
                        curState->Qvalmap[action].second += standChildState.second * (standChildState.first->rewardOnReachingState + standChildState.first->stateValue.first);
                    }
                }
                else if (action == DOUBLE)
                {
                    curState->Qvalmap[action].second = 0; /*update new Q values for HIT for curState (PLAYER STATES) */
                    /*iterate over all PLAYER HIT-children and add old state values*/
                    for (auto &hitChildState : curState->children)
                    {
                        //assert(hitChildState.first->rewardOnReachingState==0);
                        curState->Qvalmap[action].second += hitChildState.second * (hitChildState.first->rewardOnReachingState + hitChildState.first->Qvalmap[STAND].first);
                        /*Q-new-(s, hit) = SIGMA P(s'|s, hit) * [R(s',hit,s)+V-old-(s')] */
                    }
                    curState->Qvalmap[action].second *= 2;
                }
                else if (action == SPLIT)
                {
                    curState->Qvalmap[action].second = 0;
                    for (auto &splitChildState : curState->splitChildren)
                    {
                        curState->Qvalmap[action].second += splitChildState.second * (splitChildState.first->rewardOnReachingState + splitChildState.first->stateValue.first);
                    }
                    curState->Qvalmap[action].second *= 2;
                }
                else if (action == SPLIT_ACE)
                {
                    curState->Qvalmap[action].second = 0;
                    for (auto &splitaceChildState : curState->splitAceChildren)
                    {
                        curState->Qvalmap[action].second += splitaceChildState.second * (splitaceChildState.first->rewardOnReachingState + splitaceChildState.first->Qvalmap[STAND].first);
                    }
                    curState->Qvalmap[action].second *= 2;
                }
            }
            //Update V_new
            curState->stateValue.second = -DBL_MAX;
            for (auto &action : curState->allActions)
            {
                curState->stateValue.second = max(curState->stateValue.second, curState->Qvalmap[action].second);
            }
        }
        for (auto &dealerState : keyToStateDealer)
        {
            BlackJackState *curDealerState = dealerState.second;
            for (auto &dealer_action : curDealerState->allActions)
            {
                if (dealer_action == HIT)
                {
                    curDealerState->Qvalmap[dealer_action].second = 0;
                    /*iterate over all DEALER HIT children and add old state values*/
                    for (auto &hitChildState : curDealerState->children)
                    {
                        curDealerState->Qvalmap[dealer_action].second += hitChildState.second * (hitChildState.first->rewardOnReachingState + hitChildState.first->stateValue.first);
                    }
                }
                else if (dealer_action == STAND)
                {
                    curDealerState->Qvalmap[dealer_action].second = 0;
                    /*iterate over all DEALER STAND children and add old state values*/
                    for (auto &standChildState : curDealerState->standChildren)
                    {
                        curDealerState->Qvalmap[dealer_action].second += standChildState.second * (standChildState.first->rewardOnReachingState + standChildState.first->stateValue.first);
                    }
                }
            }
            //Update V_new
            curDealerState->stateValue.second = -DBL_MAX;
            for (auto &action : curDealerState->allActions)
            {
                curDealerState->stateValue.second = max(curDealerState->stateValue.second, curDealerState->Qvalmap[action].second);
            }
        }
        /*Step 2: Vold<-Vnew && Qold<-Qnew*/
        for (auto &playerState : keyToState)
        {
            if (playerState.second->isTerminalState)
                continue;
            BlackJackState *curState = playerState.second;
            error+=abs(curState->stateValue.second - curState->stateValue.first);
            curState->stateValue.first = curState->stateValue.second;
            for (auto &action : curState->allActions)
            {
                curState->Qvalmap[action].first = curState->Qvalmap[action].second;
            }
        }
        for (auto &dealerState : keyToStateDealer)
        {
            BlackJackState *curState = dealerState.second;
            error+=abs(curState->stateValue.second - curState->stateValue.first);
            curState->stateValue.first = curState->stateValue.second;
            for (auto &action : curState->allActions)
            {
                curState->Qvalmap[action].first = curState->Qvalmap[action].second;
            }
        }
        cerr<<"OUTPUT ERROR:"<<error<<"\n";
    }
}

void BlackJackAgent::createNextDealerState(BlackJackState *parentState, int newDealerHand, bool isBlackJackDealer, int AceStateChild, double probChildState)
{
    // newdDealerHand <= 21 always, busts are handled in executeMove
    //key: playerHandValue + "$"+ IsPlayerBlackJack + "$" + DealerHandValue + "$" + AceStateDealer
    if (isBlackJackDealer)
    {
        if (parentState->isBlackjackPlayer())
        {
            parentState->standChildren.emplace_back(make_pair(keyToState["0"], probChildState));
        }
        else
        {
            parentState->standChildren.emplace_back(make_pair(keyToState["-1"], probChildState));
        }
        return;
    }

    string key = to_string(parentState->handValuePlayer) + "$" + to_string(parentState->isBlackjackPlayer()) + "$" + to_string(newDealerHand) + "$" + to_string(AceStateChild);
    if (keyToStateDealer.count(key))
    {
//        keyToStateDealer[key]->isVisited = true;
        parentState->standChildren.emplace_back(make_pair(keyToStateDealer[key], probChildState));
    }
    else
    {
        auto nextState = new BlackJackState();
        nextState->isVisited = false;
        nextState->handValueDealer = newDealerHand;
        nextState->handValuePlayer = parentState->handValuePlayer;
        nextState->isPair = parentState->isPair;
        nextState->AceStateDealer = AceStateChild;
        nextState->AceStatePlayer = parentState->AceStatePlayer;
        keyToStateDealer[key] = nextState;
        parentState->standChildren.emplace_back(make_pair(nextState, probChildState));
    }
}

void BlackJackAgent::createNextPlayerState(BlackJackState *parentState, int newPlayerHand, int AceStateChild, int isPairChild, double probChildState)
{
    string key = to_string(AceStateChild) + "$" + to_string(isPairChild) + "$" + to_string(newPlayerHand) + "$" + to_string(parentState->handValueDealer);
    if (keyToState.count(key))
    {
//        keyToState[key]->isVisited = true;
        parentState->children.emplace_back(make_pair(keyToState[key], probChildState));
    }
    else
    {
        auto nextState = new BlackJackState();
        /*make relevant updates here:*/
        nextState->isVisited = false;
        nextState->handValueDealer = parentState->handValueDealer;
        nextState->handValuePlayer = newPlayerHand;
        nextState->isPair = isPairChild;
        nextState->AceStatePlayer = AceStateChild;
        nextState->AceStateDealer = parentState->AceStateDealer;
        keyToState[key] = nextState;
        parentState->children.emplace_back(make_pair(nextState, probChildState));
    }
}

void BlackJackAgent::executeMove(BlackJackState *curState, int action, int PlayerID)
{
    if (PlayerID == PLAYER)
    {
        double probCardFace = this->probability;
        double probCardNonFace = (1.0 - probCardFace) / 9.0;
        double probChildState;
        if (action == HIT)
        {
            int oldPlayerHand = curState->handValuePlayer;
            int next_card = 0;
            /* Case1: iterating over Non-Ace next card*/
            for (next_card = 2; next_card <= 10; next_card++)
            {
                if (next_card != 10)
                    probChildState = probCardNonFace;
                else
                    probChildState = probCardFace;
                int newPlayerHand = oldPlayerHand + next_card;
                if (newPlayerHand <= 21) /*Not Busted*/
                {
                    createNextPlayerState(curState, newPlayerHand, curState->AceStatePlayer, false, probChildState);
                }
                else if (curState->AceStatePlayer == SOFT_HAND) /*SOFT to HARD and NOT busted*/
                {
                    newPlayerHand -= 10;
                    assert(newPlayerHand <= 21);
                    createNextPlayerState(curState, newPlayerHand, HARD_HAND, false, probChildState);
                }
                else /*Player is certainly Busted*/
                {
                    curState->children.emplace_back(make_pair(keyToState["-1"], probChildState));
                }
            }
            /* Case2: Next card drawn is ACE*/
            probChildState = probCardNonFace;
            if (curState->AceStatePlayer == NO_ACE)
            {
                int newPlayerHand = oldPlayerHand + 11;
                if (newPlayerHand > 21)
                {
                    newPlayerHand -= 10;
                    assert(newPlayerHand <= 21);
                    createNextPlayerState(curState, newPlayerHand, HARD_HAND, false, probChildState);
                }
                else
                {
                    assert(newPlayerHand <= 21);
                    createNextPlayerState(curState, newPlayerHand, SOFT_HAND, false, probChildState);
                }
            }
            else if (curState->AceStatePlayer == SOFT_HAND)
            {
                int newPlayerHand = oldPlayerHand + 1;
                assert(newPlayerHand <= 21);
                createNextPlayerState(curState, newPlayerHand, SOFT_HAND, false, probChildState);
            }
            else
            {
                /*HARD_HAND*/
                int newPlayerHand = oldPlayerHand + 1;
                assert(newPlayerHand <= 21);
                createNextPlayerState(curState, newPlayerHand, HARD_HAND, false, probChildState);
            }
        }
        else if (action == STAND)
        { /* Begin dealer moves*/
            /* Case1: Dealer started with an ACE*/
            if (curState->handValueDealer == 11)
            {
                for (int next_card = 1; next_card <= 10; next_card++)
                {
                    if (next_card == 10)
                    {
                        createNextDealerState(curState, 21, true, curState->AceStateDealer, probCardFace);
                    }
                    else
                    {
                        if (curState->isBlackjackPlayer())
                            curState->standChildren.emplace_back(make_pair(keyToState["15"], probCardNonFace));
                        else
                            createNextDealerState(curState, curState->handValueDealer + next_card, false, SOFT_HAND, probCardNonFace);
                    }
                }
            }
            /* Case2: Dealer starts with a FACE*/
            else if (curState->handValueDealer == 10)
            {
                for (int next_card = 2; next_card <= 11; next_card++)
                {
                    if (next_card == 11)
                    {
                        createNextDealerState(curState, 21, true, curState->AceStateDealer, probCardNonFace);
                    }
                    else
                    {
                        if (next_card == 10)
                        {
                            probChildState = probCardFace;
                        }
                        else
                        {
                            probChildState = probCardNonFace;
                        }
                        if (curState->isBlackjackPlayer())
                            curState->standChildren.emplace_back(make_pair(keyToState["15"],probChildState));
                        else
                            createNextDealerState(curState, curState->handValueDealer + next_card, false, NO_ACE, probChildState);
                    }
                }
            }
            else /*Dealer starts with a non-ACE non-FACE */
            {
                if (curState->isBlackjackPlayer())
                    curState->standChildren.emplace_back(keyToState["15"], 1.0);
                else
                {
                    for (int next_card = 2; next_card <= 11; next_card++)
                    {
                        if (next_card == 10)
                            probChildState = probCardFace;
                        else
                            probChildState = probCardNonFace;
                        if (next_card == 11)
                            createNextDealerState(curState, curState->handValueDealer + next_card, false, SOFT_HAND, probChildState);
                        else
                            createNextDealerState(curState, curState->handValueDealer + next_card, false, NO_ACE, probChildState);
                    }
                }
            }
        }
        else if (action == SPLIT_ACE)
        {
            for(int card = 2; card <= 11; card++) {
                int AceState = SOFT_HAND;
                int isPair = false;
                int newPlayerHand = 11;
                if(card == 10) {
                    AceState = NO_ACE;
                    newPlayerHand += card;
                }
                else if(card == 11) {
                    isPair = true;
                    newPlayerHand += 1;
                }
                else {
                    newPlayerHand += card;
                }
                string key = to_string(AceState) + "$" + to_string(isPair) + "$" + to_string(newPlayerHand) + "$" + to_string(curState->handValueDealer);
                if (card != 10) {
                    key += "$init";
                    curState->splitAceChildren.emplace_back(make_pair(keyToState[key], probCardNonFace));
                }
                else
                {
                    if(keyToState.count(key) == 0) {
                        auto child = new BlackJackState();
                        child->isVisited = false;
                        child->handValueDealer = curState->handValueDealer;
                        child->handValuePlayer = newPlayerHand;
                        child->isPair = isPair;
                        child->AceStatePlayer = AceState;
                        child->AceStateDealer = curState->AceStateDealer;
                        keyToState[key] = child;
                        curState->splitAceChildren.emplace_back(make_pair(child, probCardFace));
                    }
                    else
                        curState->splitAceChildren.emplace_back(make_pair(keyToState[key], probCardNonFace));
                }
            }
        }
        else if (action == SPLIT)
        {
            for(int card = 2; card <= 11; card++) {
                int AceState = NO_ACE;
                int isPair = false;
                int newPlayerHand = curState->handValuePlayer/2;
                if(card == 11)
                    AceState = SOFT_HAND;
                if (card == newPlayerHand)
                    isPair = true;
                newPlayerHand += card;
                string key = to_string(AceState) + "$" + to_string(isPair) + "$" + to_string(newPlayerHand) + "$" + to_string(curState->handValueDealer) + "$init";

                // this state is always present
                if (card == 10)
                    probChildState = probCardFace;
                else
                    probChildState = probCardNonFace;
                curState->splitChildren.emplace_back(make_pair(keyToState[key], probChildState));
            }
        }
        else if (action == DOUBLE)
        {
            // add no children, take 2 * expectation of Q values of stand action of hit children during value iteration
        }
    }
    else
    {
        /* Starts from Dealer's THIRD CARD DRAW*/
        double probCardFace = this->probability;
        double probCardNonFace = (1.0 - probCardFace) / 9.0;
        double probChildState;
        if (action == HIT)
        {
            int oldDealerHand = curState->handValueDealer;
            int next_card = 0;
            for (next_card = 2; next_card <= 10; next_card++) /*Iterating over Non-Ace next card*/
            {
                if (next_card == 10)
                    probChildState = probCardFace;
                else
                    probChildState = probCardNonFace;
                int newDealerHand = oldDealerHand + next_card;
                if (newDealerHand <= 21)
                {
                    createNextDealerState(curState, newDealerHand, false, curState->AceStateDealer, probChildState);
                }
                else if (curState->AceStateDealer == SOFT_HAND)
                {
                    newDealerHand -= 10;
                    assert(newDealerHand <= 21);
                    createNextDealerState(curState, newDealerHand, false, HARD_HAND, probChildState);
                }
                else
                { /*Dealer is Busted*/
                    curState->standChildren.emplace_back(make_pair(keyToState["1"], probChildState));
                }
            }
            /*Next_card is ACE */
            if (curState->AceStateDealer == NO_ACE)
            {
                int newDealerHand = oldDealerHand + 11;
                if (newDealerHand > 21)
                {
                    newDealerHand -= 10;
                    assert(newDealerHand <= 21);
                    createNextDealerState(curState, newDealerHand, false, HARD_HAND, probCardNonFace);
                }
                else
                {
                    assert(newDealerHand <= 21);
                    createNextDealerState(curState, newDealerHand, false, SOFT_HAND, probCardNonFace);
                }
            }
            else if (curState->AceStateDealer == SOFT_HAND)
            {
                int newDealerHand = oldDealerHand + 1;
                assert(newDealerHand <= 21);
                createNextDealerState(curState, newDealerHand, false, SOFT_HAND, probCardNonFace);
            }
            else
            {
                /*HARD_HAND*/
                int newDealerHand = oldDealerHand + 1;
                assert(newDealerHand <= 21);
                createNextDealerState(curState, newDealerHand, false, HARD_HAND, probCardNonFace);
            }
        }
        else /* EXECUTE DEALER STAND*/
        {
            /* Game Decision */
            if (curState->handValuePlayer > curState->handValueDealer)
                curState->standChildren.emplace_back(make_pair(keyToState["1"], 1.0));
            else if (curState->handValuePlayer < curState->handValueDealer)
                curState->standChildren.emplace_back(keyToState["-1"], 1.0);
            else
                curState->standChildren.emplace_back(keyToState["0"], 1.0);
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
//    else if (isBlackjackDealer())
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
        auto state = new BlackJackState(NumAces, NumPairs, player_it, dealer_it);
        string key = to_string(NumAces) + "$" + to_string(NumPairs) + "$" + to_string(player_it) + "$" + to_string(dealer_it) + "$init";
        keyToState[key] = state;
        allInitStates.insert(state);
    }
}

void BlackJackAgent::createTerminalStates()
{
    vector<string> terminalKeys = {"1", "15", "0", "-1"};
    for (auto &key : terminalKeys)
    {
        auto terminalState = new BlackJackState();
        terminalState->isTerminalState = true;
        if (key == "1")
        {
            terminalState->rewardOnReachingState = 1;
        }
        else if (key == "15")
        {
            terminalState->rewardOnReachingState = 1.5;
        }
        else if (key == "0")
        {
            terminalState->rewardOnReachingState = 0;
        }
        else if (key == "-1")
        {
            terminalState->rewardOnReachingState = -1.5;
        }
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
//    if(curState->isVisited)
//        return;
    curState->isVisited = true;

    getPossibleActions(curState, PlayerID);

    for (int action : curState->allActions)
        executeMove(curState, action, PlayerID);

    for (auto& child : curState->children)
    {
        if (!child.first->isVisited)
            constructStateSpace(child.first, PLAYER);
    }

    for (auto& child : curState->standChildren)
    {
        if (!child.first->isVisited)
            constructStateSpace(child.first, DEALER);
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

    isBlackJackPlayer = NumAces == 1 && playerInitialValue == 21 && NumPairs == 0;

    stateValue = make_pair(0, 0);

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
    stateValue = make_pair(0, 0);
}
