//
// Created by karthik on 24/10/18.
//
#include "blackjack2.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

    if(argc != 2){
        cerr << "Run with 1 argument" << endl;
        exit(1);
    }
    BlackJackAgent agent(atof(argv[1]));
    agent.constructPolicyGraph();
    agent.executeValueIteration();
    agent.printPolicy();
    return 0;
}