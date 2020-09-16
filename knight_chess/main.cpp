//
//  main.cpp
//  knight_chess
//
//  Created by Matias Barrientos on 10-09-20.
//  Copyright © 2020 Matias Barrientos. All rights reserved.
//

#include <iostream>
#include "knight_chess.hpp"
#include <ctime>

using namespace kc;

std::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());

std::map<std::pair<long, long>, int> posible_actions {
    {std::make_pair(1,2), 1},
    {std::make_pair(2,1), 0},
    {std::make_pair(1,-2), 6},
    {std::make_pair(-2,1), 3},
    {std::make_pair(2,-1), 7},
    {std::make_pair(-1,2), 2},
    {std::make_pair(-1,-2), 5},
    {std::make_pair(-2,-1), 4},
};

int main(int argc, const char * argv[]) {
    json json = json::parse(argv[1]);

    State *initial = new State(json);
    Node *root = new Node(initial);

    Node *best = mcts(root);

    Action move = best->get_move();

    std::pair<long, long> id(move.i - move.move_to_i, move.j - move.move_to_j);

    std::cout << "{" << "\n";
    std::cout << "    \"knight_id\": "  << json["ids"][move.move_to_i][move.move_to_j] << "," << "\n";
    std::cout << "    \"knight_movement\": " << posible_actions[id] << "\n";
    std::cout << "}" << "\n";
    //std::cout << .array();


    return 0;
}
