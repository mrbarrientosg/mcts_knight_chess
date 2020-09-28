//
//  knight_chess.hpp
//  knight_chess
//
//  Created by Matias Barrientos on 13-09-20.
//  Copyright © 2020 Matias Barrientos. All rights reserved.
//

#ifndef knight_chess_hpp
#define knight_chess_hpp

#include <list>
#include <vector>
#include <set>
#include "json.hpp"
#include <iostream>
#include <chrono>
#include <random>

using json = nlohmann::json;
extern std::map<std::pair<long, long>, int> posible_actions;

namespace kc {

struct Action {
    long i;
    long j;
    long move_to_i;
    long move_to_j;

    Action(): i(-1), j(-1), move_to_i(-1), move_to_j(-1) {}
    Action(long move_i, long move_j): i(-1), j(-1), move_to_i(move_i), move_to_j(move_j) {}
    Action(long i, long j, long move_i, long move_j): i(i), j(j), move_to_i(move_i), move_to_j(move_j) {}

    bool operator <(const Action& rhs) const {
        if (i < rhs.i) return true;
        if (i > rhs.i) return false;

        if (j < rhs.j) return true;
        if (j > rhs.j) return false;

        if (move_to_i < rhs.move_to_i) return true;
        if (move_to_i > rhs.move_to_i) return false;

        if (move_to_j < rhs.move_to_j) return true;
        if (move_to_j > rhs.move_to_j) return false;

        return false;
    }

    bool operator ==(const Action &rhs) const {
        return i == rhs.i && j == rhs.j && move_to_i == rhs.move_to_i && move_to_j == rhs.move_to_j;
    }
};

class State {
    long table[8][8];
    int player;
public:
    State(json &data);
    State(const State &s);
    long get_knight(long i, long j) const;
    double get_reward(int player) const;
    bool is_end_game() const;
    const std::vector<Action> get_actions() const;
    void transition(Action action);
    bool make_random_action(std::mt19937_64 &random_engine);
    int get_player() const;
    void change_player();
};

class Node {
    long visits;
    double wins;
    int player;
    Node *parent;
    std::list<Node *> children;
    std::vector<Action> actions;
    Action move;
public:
    Node(State &state);
    Node(State &state, Action move);
    void set_parent(Node *parent);
    void add_child(Node *child);
    bool empty_children() const;
    void update(double player_n);

    double get_uct() const;
    Node *best_child();
    long get_visits() const;
    double get_wins() const;
    Node *get_parent() const;
    Action get_move() const;
    int get_player() const;
    std::vector<Action> &get_actions();
    const std::list<Node *> &get_children() const;
};

class MCTS {
    std::mt19937_64 random_engine;
public:
    MCTS(std::mt19937_64::result_type initial_seed);
    Node *expand(Node *node, State &state);
    Node *tree_policy(Node *node, State &state);
    Node *search(State state);
};

std::ostream &operator<<(std::ostream &os, const State &s);


}

#endif /* knight_chess_hpp */
