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
#include <future>

using namespace kc;

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

    State initial(json);
    int threads = 2;

    std::vector<std::future<Node *>> nodes;
    nodes.reserve(threads);

    for (long i = 0; i < threads; i++) {
        auto func = [i, initial] () -> Node* {
            MCTS mcts(1012411 * i + std::chrono::high_resolution_clock::now().time_since_epoch().count());
            return mcts.search(initial);
        };
        nodes.push_back(std::async(std::launch::async, func));
    }

    std::vector<Node *> roots;
    roots.reserve(threads);

    for (int t = 0; t < threads; ++t) {
        roots.push_back(std::move(nodes[t].get()));
    }

    std::map<Action, int> visits;
    std::map<Action, double> wins;
    for (int t = 0; t < threads; ++t) {
        Node *node = roots[t];
        for (auto child = node->get_children().cbegin(); child != node->get_children().cend(); ++child) {
            visits[(*child)->get_move()] += (*child)->get_visits();
            wins[(*child)->get_move()]   += (*child)->get_wins();
        }
    }

    double best_score = -1;
    Action best_move;
    for (auto itr: visits) {
        auto move = itr.first;
        double v = itr.second;
        double w = wins[move];

        
        double rate = w / v;
        if (rate > best_score) {
            best_move = move;
            best_score = rate;
        }
    }

    std::pair<long, long> id(best_move.i - best_move.move_to_i, best_move.j - best_move.move_to_j);

    std::cout << "{" << "\n";
    std::cout << "    \"knight_id\": "  << json["ids"][best_move.move_to_i][best_move.move_to_j] << "," << "\n";
    std::cout << "    \"knight_movement\": " << posible_actions[id] << "\n";
    std::cout << "}" << "\n";


    return 0;
}
