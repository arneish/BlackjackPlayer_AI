#include "blackjack.h"

using namespace std;

BlackJackState::BlackJackState(int numCardsPlayer, int numTurnsPlayedPlayer, bool hasDoubledPlayer, bool hasSplitPlayer,
                   bool hasSplitAcePlayer, bool hasAcePlayer, bool hasPairPlayer, const vector<int> &cardsPlayer,
                   const vector<BlackJackState *> &children, int numCardsDealer, int valueCardsDealer,
                   bool hasAceDealer, bool dealerToPlay, const vector<int> &cardsDealer, double betPlayer)
        : numCardsPlayer(numCardsPlayer), numTurnsPlayedPlayer(numTurnsPlayedPlayer),
          hasDoubledPlayer(hasDoubledPlayer), hasSplitPlayer(hasSplitPlayer), hasSplitAcePlayer(hasSplitAcePlayer),
          hasAcePlayer(hasAcePlayer), hasPairPlayer(hasPairPlayer), cardsPlayer(cardsPlayer), children(children),
          numCardsDealer(numCardsDealer), valueCardsDealer(valueCardsDealer), hasAceDealer(hasAceDealer),
          dealerToPlay(dealerToPlay), cardsDealer(cardsDealer), betPlayer(betPlayer) {}

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
    pair<int, int> values = getValueHandPlayer();
    return values.second > 21;
}

bool BlackJackState::isBlackjackPlayer()
{
    int numAces = count(cardsPlayer.begin(), cardsPlayer.end(), 0);
    int numFaces = count(cardsPlayer.begin(), cardsPlayer.end(), 10);
    return numAces == 1 && numFaces == 1 && cardsPlayer.size() == 2;
}

bool BlackJackState::isBustedDealer()
{
    pair<int, int> values = getValueHandDealer();
    return values.second > 21;
}

bool BlackJackState::isBlackjackDealer()
{
    int numAces = count(cardsDealer.begin(), cardsDealer.end(), 0);
    int numFaces = count(cardsDealer.begin(), cardsDealer.end(), 10);
    return numAces == 1 && numFaces == 1 && cardsDealer.size() == 2;
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

void BlackJackAgent::constructPolicyGraph()
{
    //construct initial states
    for (int dealer_it = 0; dealer_it < 11; dealer_it++)
    {
        if (dealer_it != 1)
        {
            for (int player_i = 0; player_i < 11; player_i++)
            {
                if (player_i != 1)
                {
                    for (int player_j = player_i; player_j < 11; player_j++)
                    {
                        if (player_j != 1)
                        {
                            BlackJackState *State = new BlackJackState(); //TODO: put in values for this constructor
                            initialStates[dealer_it][player_i][player_j] = State;
                        }
                    }
                }
            }
        }
    }
    for (int dealer_it = 0; dealer_it < 11; dealer_it++)
    {
        if (dealer_it != 1)
        {
            for (int player_i = 0; player_i < 11; player_i++)
            {
                if (player_i != 1)
                {
                    for (int player_j = player_i; player_j < 11; player_j++)
                    {
                        if (player_j != 1)
                        {
                            constructStateSpace(initialStates[dealer_it][player_i][player_j]);
                        }
                    }
                }
            }
        }
    }
}

void BlackJackAgent::constructStateSpace(BlackJackState *state)
{
    // check terminals
    if (state->isBustedPlayer() || state->isBustedDealer() || state->isBlackjackPlayer() || state->isBlackjackDealer())
    {
        state->isTerminalState = true;
        return;
    }
    // hit
    for (int i = 0; i < 11; i++)
    {
        if (i != 1)
        {
            BlackJackState *child = new BlackJackState(state);
            child->executeHitPlayer(i);
            state->children.emplace_back(child);
            constructStateSpace(child);
        }
    }
    // stand
    executeDealerPolicy();
    // double
    if (state->canDouble())
    {
        for (int i = 0; i < 11; i++)
        {
            if (i != 1)
            {
                BlackJackState *child = new BlackJackState(state);
                child->betPlayer *= 2;
                child->executeHitPlayer(i);
                state->children.emplace_back(child);
                executeDealerPolicy();
            }
        }
    }
    //split
    if (state->canSplit())
    {
    }
}
