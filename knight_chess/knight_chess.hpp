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
#include <map>
#include <nlohmann/json.hpp>
#include <iostream>
#include <chrono>
#include <random>

using json = nlohmann::json;
extern std::mt19937 rng;
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

};

class State {
    long table[8][8];
public:
    State(json &data);
    State(const State &s);
    long get_knight(long i, long j) const;
    int is_end_game() const;
    const std::vector<Action> get_actions() const;
    void transition(Action action);
    bool make_random_action();
};

class Node {
    long visits;
    long wins;
    State *state;
    Node *parent;
    std::list<Node *> children;
    std::vector<Action> actions;
    Action move;
    double uct;
public:
    Node(State *state);
    Node(State *state, Action move);
    void set_parent(Node *parent);
    void add_child(Node *child);
    bool empty_children() const;
    void update();

    void set_uct(double uct);
    double get_uct() const;
    State *get_state() const;
    Node *best_child();
    long get_visits() const;
    long get_wins() const;
    Node *get_parent() const;
    Action get_move() const;
    std::vector<Action> &get_actions();
};

std::ostream &operator<<(std::ostream &os, const State &s);

Node *expand(Node *node);
Node *tree_policy(Node *node);
Node *mcts(Node *root);

}

#endif /* knight_chess_hpp */
