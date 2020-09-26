//
//  knight_chess.cpp
//  knight_chess
//
//  Created by Matias Barrientos on 13-09-20.
//  Copyright © 2020 Matias Barrientos. All rights reserved.
//

#include "knight_chess.hpp"
#include <string>

namespace kc {

State::State(json &data) {
    char player = data["my_knights_dict"].items().begin().key()[0];
    this->player = player - '0';

    for (long i = 0; i < 8; i++) {
        for (long j = 0; j < 8; j++) {
            json value = data["ids"][i][j];
            if (value.is_null()) {
                this->table[i][j] = 0;
            } else {
                this->table[i][j] = (value.dump()[0] == player) ? this->player : 3 - this->player;
            }
        }
    }

}

State::State(const State &s) {
    for (long i = 0; i < 8; i++) {
        for (long j = 0; j < 8; j++) {
            this->table[i][j] = s.table[i][j];
        }
    }

    this->player = s.player;
}

long State::get_knight(long i, long j) const {
    return this->table[i][j];
}

unsigned long State::hash() const {
    unsigned long value = 0;
    for (long i = 0; i < 8; i++) {
        for (long j = 0; j < 8; j++) {
            value = 65537 * value + table[i][j];
        }
    }

    return value;
}

bool State::is_end_game() const {
    long me = 0;
    long enemy = 0;
    int enemy_player = 3 - player;

    for (long i = 0; i < 8; i++) {
        for (long j = 0; j < 8; j++) {
            if (this->table[i][j] == player)
                me++;
            else if (this->table[i][j] == enemy_player)
                enemy++;
        }
    }

    if (me == 0 || enemy == 0) {
        return true;
    }

    return false;
}

double State::get_reward(int player) const {
    long player1 = 0;
    long player2 = 0;

    for (long i = 0; i < 8; i++) {
        for (long j = 0; j < 8; j++) {
            if (this->table[i][j] == 1)
                player1++;
            else if (this->table[i][j] == 2)
                player2++;
        }
    }

    if (player1 == player2)
        return 0.5;

    int winner_player = 0;

    if (player1 > player2) {
        winner_player = 1;
    } else {
        winner_player = 2;
    }

    if (winner_player == player) {
        return 0.0;
    } else {
        return abs(player1 - player2);
    }

    return 0.0;
}

bool State::valid_action(long i, long j) const {
    int enemy_player = 3 - this->player;
    int enemy = 0;
    int me = 0;

    for (const auto &a: posible_actions) {
        long i_pos = i + a.first.first;
        long j_pos = j + a.first.second;
        if (i_pos >= 8 || i_pos < 0) continue;
        if (j_pos >= 8 || j_pos < 0) continue;
        if (this->table[i_pos][j_pos] == enemy_player) {
            enemy++;
        } else if (this->table[i_pos][j_pos] == player) {
            me++;
        }
    }

    return me - enemy >= 1;
}

const std::vector<Action> State::get_actions() const {
    std::vector<Action> actions;
    int enemy_player = 3 - this->player;

    for (long i = 0; i < 8; i++) {
        for (long j = 0; j < 8; j++) {
            if (table[i][j] == enemy_player) {
                for (const auto &a: posible_actions) {
                    long i_pos = i + a.first.first;
                    long j_pos = j + a.first.second;
                    if (i_pos >= 8 || i_pos < 0) continue;
                    if (j_pos >= 8 || j_pos < 0) continue;
                    if (this->table[i_pos][j_pos] == player) {
                        actions.push_back(Action(i, j, i_pos, j_pos));
                    }
                }
            }
        }
    }


    if (actions.empty()) {
        for (long i = 0; i < 8; i++) {
            for (long j = 0; j < 8; j++) {
                if (table[i][j] == 0) {
                    for (const auto &a: posible_actions) {
                        long i_pos = i + a.first.first;
                        long j_pos = j + a.first.second;
                        if (i_pos >= 8 || i_pos < 0) continue;
                        if (j_pos >= 8 || j_pos < 0) continue;
                        if (this->table[i_pos][j_pos] == player) {
                            actions.push_back(Action(i, j, i_pos, j_pos));
                        }
                    }
                }
            }
        }
    }


    return actions;
}

bool State::make_random_action(std::mt19937_64 &random_engine) {
    std::vector<Action> actions = get_actions();

    if (actions.empty())
        return false;

    long idx = std::uniform_int_distribution<long>(0, actions.size() - 1)(random_engine);
    transition(actions[idx]);

    return true;
}

int State::get_player() const {
    return this->player;
}

void State::change_player() {
    this->player = 3 - player;
}

void State::transition(Action action) {
    if (table[action.i][action.j] != 0)
        table[action.i][action.j] = 0;
    table[action.i][action.j] = table[action.move_to_i][action.move_to_j];
    table[action.move_to_i][action.move_to_j] = 0;
    player = 3 - player;
}

Node::Node(State &state) {
    this->parent = nullptr;
    this->actions = state.get_actions();
    this->visits = 0;
    this->wins = 0.0;
    this->player = state.get_player();
}

Node::Node(State &state, Action move) {
    this->parent = nullptr;
    this->actions = state.get_actions();
    this->move = move;
    this->visits = 0;
    this->wins = 0.0;
    this->player = state.get_player();
}

void Node::set_parent(Node *parent) {
    this->parent = parent;
}

void Node::add_child(Node *child) {
    this->children.push_back(child);
}

bool Node::empty_children() const {
    return this->children.empty();
}

std::vector<Action> &Node::get_actions() {
    return this->actions;
}

std::ostream &operator<<(std::ostream &os, const State &s) {
    for (long i = 0;i < 8; i++){
        for (long j = 0; j < 8; j++){
                os << s.get_knight(i, j) << " ";
        }
        os << "\n";
    }

    return os;
}

double Node::get_uct() const {
    if (visits == 0)
        return 0.0;

    return wins / visits + 1.45 * std::sqrt(2 * std::log(parent->visits) / visits);
}

long Node::get_visits() const {
    return this->visits;
}

double Node::get_wins() const {
    return this->wins;
}

Node *Node::get_parent() const {
    return this->parent;
}

void Node::update(double reward) {
    this->visits++;
    this->wins += reward;
}

int Node::get_player() const {
    return this->player;
}

Action Node::get_move() const {
    return this->move;
}

const std::list<Node *> &Node::get_children() const {
    return this->children;
}

Node *Node::best_child() {
    if (player == 1)
        return *std::max_element(children.begin(), children.end(), [](Node *a, Node *b) {
             return a->get_uct() < b->get_uct();
        });
    else
        return *std::min_element(children.begin(), children.end(), [](Node *a, Node *b) {
             return a->get_uct() > b->get_uct();
        });
}

MCTS::MCTS(std::mt19937_64::result_type initial_seed) {
    this->random_engine = std::mt19937_64(initial_seed);
}

Node *MCTS::expand(Node *node, State &state) {
    long idx = std::uniform_int_distribution<long>(0, node->get_actions().size() - 1)(random_engine);
    Action action = node->get_actions()[idx];
    node->get_actions().erase(node->get_actions().begin() + idx);
    state.transition(action);
    Node *v = new Node(state, action);
    v->set_parent(node);
    node->add_child(v);
    return v;
}

Node *MCTS::tree_policy(Node *node, State &state) {
    Node *current = node;

    while (!state.is_end_game()) {
        if (!current->get_actions().empty()) {
             return expand(current, state);
        } else {
            current = current->best_child();
            state.transition(current->get_move());
        }
    }

    return current;
}

Node *MCTS::search(State root_state) {
    Node *root = new Node(root_state);
    Node *current = nullptr;
    State state = root_state;

    for (long i = 0; i < 10000; i++) {
        state = root_state;
        current = tree_policy(root, state);

        while (!state.is_end_game()) {
            state.make_random_action(random_engine);
        }

        while (current != nullptr) {
            current->update(state.get_reward(current->get_player()));
            current = current->get_parent();
        }
    }

    return root;
}

}
