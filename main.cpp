//
// Created by karthik on 24/10/18.
//
#include "blackjack2.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {

    BlackJackAgent agent;

    agent.constructPolicyGraph();

    agent.executeValueIteration();

    return 1;
}