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
    //Player
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

    bool canSplit()
    {
        if (hasPairPlayer)
        {
            return !hasSplitAcePlayer;
        }
        return false;
    };

    bool canDouble()
    {
        return numTurnsPlayedPlayer == 0;
    }

    // return soft, hard value of Player
    pair<int, int> getValueHandPlayer()
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

    bool isBustedPlayer()
    {
        pair<int, int> values = getValueHandPlayer();
        return values.second > 21;
    }

    bool isBlackjackPlayer()
    {
        int numAces = count(cardsPlayer.begin(), cardsPlayer.end(), 0);
        int numFaces = count(cardsPlayer.begin(), cardsPlayer.end(), 10);
        return numAces == 1 && numFaces == 1 && cardsPlayer.size() == 2;
    }

    // Dealer
    int numCardsDealer;
    int valueCardsDealer;
    bool hasAceDealer;
    bool dealerToPlay; //true == dealer's turn

    vector<int> cardsDealer;

    pair<int, int> getValueHandDealer()
    {
        int numAces = count(cardsDealer.begin(), cardsDealer.end(), 0);
        if (numAces > 0)
        {
            int sum = accumulate(cardsDealer.begin(), cardsDealer.end(), 0);
            int softval = sum + 11 + (numAces - 1);
            return make_pair(softval, numAces + sum);
        }
        else
        {
            int val = accumulate(cardsDealer.begin(), cardsDealer.end(), 0);
            return make_pair(val, val);
        }
    }

    bool isBustedDealer()
    {
        pair<int, int> values = getValueHandDealer();
        return values.second > 21;
    }

    bool isBlackjackDealer()
    {
        int numAces = count(cardsDealer.begin(), cardsDealer.end(), 0);
        int numFaces = count(cardsDealer.begin(), cardsDealer.end(), 10);
        return numAces == 1 && numFaces == 1 && cardsDealer.size() == 2;
    }

    int getActionDealer()
    {
        pair<int, int> ValueHandDealer = getValueHandDealer();
        if (ValueHandDealer.first < 17 || ValueHandDealer.second < 17)
            return HIT;
        else
            return STAND;
    }

    //Game Manager
    double betPlayer;
    //CALL WITH bustedPerson = -1 for ignoring busts
    double endGame(int bustedPerson=-1)
    {
        if(bustedPerson==PLAYER) {
            return -betPlayer;
        }
        else if (bustedPerson==DEALER){
            return betPlayer;
        }
        //triggered when getActionDealer() returns STAND
        //return PROFITS for player
        if (isBlackjackPlayer()&&isBlackjackDealer())
            return 0; 
        else if (isBlackjackPlayer())
            return 1.5 * betPlayer;
        else if (isBlackjackDealer()) //TODO : SEE IF -1.5
            return -betPlayer;
        else 
        {
            int maxValueDealer=max(getValueHandDealer().first, getValueHandDealer().second);
            int maxValuePlayer = max(getValueHandPlayer().first, getValueHandPlayer().second);
            return (maxValueDealer>maxValuePlayer) ? -betPlayer : (maxValueDealer<maxValuePlayer ? betPlayer : 0);
        }
    }
};

