//
// Created by karthik on 24/10/18.
//
#include "blackjack2.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

    BlackJackAgent agent(4.0/13.0);

    agent.constructPolicyGraph();

    agent.executeValueIteration();

    agent.printPolicy();

//    for(auto state: agent.keyToStateDealer){
//        if(state.second->standChildren.size()) {
//            cout << state.first << " " << state.second->standChildren.at(0).first->isTerminalState  << endl;
//        }
//    }

    return 1;
}