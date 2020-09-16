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

    for (long i = 0; i < 8; i++) {
        for (long j = 0; j < 8; j++) {
            json value = data["ids"][i][j];
            if (value.is_null()) {
                this->table[i][j] = 0;
            } else {
                this->table[i][j] = (value.dump()[0] == player) ? 1 : -1;
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
}

long State::get_knight(long i, long j) const {
    return this->table[i][j];
}

int State::is_end_game() const {
    long me = 0;
    long enemy = 0;

    for (long i = 0; i < 8; i++) {
        for (long j = 0; j < 8; j++) {
            if (this->table[i][j] == 1)
                me++;
            else if (this->table[i][j] == -1)
                enemy++;
        }
    }

    if (me == 0 && enemy > 0) {
        return -1;
    }

    if (enemy == 0 && me > 0) {
        return 1;
    }

    return 0;
}

const std::vector<Action> State::get_actions() const {
    std::vector<Action> actions;

    for (long i = 0; i < 8; i++) {
        for (long j = 0; j < 8; j++) {
            if (table[i][j] == -1) {
                for (const auto &a: posible_actions) {
                    long i_pos = i + a.first.first;
                    long j_pos = j + a.first.second;
                    if (i_pos >= 8 || i_pos < 0) continue;
                    if (j_pos >= 8 || j_pos < 0) continue;
                    if (this->table[i_pos][j_pos] == 1) {
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
                        if (this->table[i_pos][j_pos] == 1) {
                            actions.push_back(Action(i, j, i_pos, j_pos));
                        }
                    }
                }
            }
        }
    }


    return actions;
}

bool State::make_random_action() {
    std::vector<Action> actions = get_actions();

    if (actions.empty())
        return false;

    long idx = std::uniform_int_distribution<long>(0, actions.size() - 1)(rng);

    //std::cout << *this;
    transition(actions[idx]);
    //std::cout << *this;

    return true;
}

void State::transition(Action action) {
    if (table[action.i][action.j] != 0)
        table[action.i][action.j] = 0;
    table[action.i][action.j] = table[action.move_to_i][action.move_to_j];
    table[action.move_to_i][action.move_to_j] = 0;
}

Node::Node(State *state) {
    this->state = state;
    this->parent = nullptr;
    this->actions = state->get_actions();
    this->visits = 0;
    this->wins = 0;
}

Node::Node(State *state, Action move) {
    this->state = state;
    this->parent = nullptr;
    this->actions = state->get_actions();
    this->move = move;
    this->visits = 0;
    this->wins = 0;
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

State *Node::get_state() const {
    return this->state;
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

void Node::set_uct(double uct) {
    this->uct = uct;
}

double Node::get_uct() const {
    return this->uct;
}

long Node::get_visits() const {
    return this->visits;
}

long Node::get_wins() const {
    return this->wins;
}

Node *Node::get_parent() const {
    return this->parent;
}

void Node::update() {
    this->visits++;
    this->wins += state->is_end_game();
}

Action Node::get_move() const {
    return this->move;
}

Node *Node::best_child() {
    for (Node *n: children) {
        double uct = (n->get_wins() / n->get_visits()) + std::sqrt(2) * std::sqrt(2.0 * std::log(visits) / n->get_visits());
        n->set_uct(uct);
    }

    return *std::max_element(children.begin(), children.end(), [](Node *a, Node *b) {
         return a->get_uct() < b->get_uct();
    });
}

Node *expand(Node *node) {
    long idx = std::uniform_int_distribution<long>(0, node->get_actions().size() - 1)(rng);
    Action action = node->get_actions()[idx];
    node->get_actions().erase(node->get_actions().begin() + idx);
    State *s = new State(*node->get_state());
    s->transition(action);
    Node *v = new Node(s, action);
    v->set_parent(node);
    node->add_child(v);
    return v;
}

Node *tree_policy(Node *node) {
    Node *current = node;

    while (!current->empty_children() && current->get_actions().empty()) {
        current = current->best_child();
    }

    if (!current->get_actions().empty()) {
        current = expand(current);
    }

    return current;
}

Node *mcts(Node *root) {
    Node *current = root;

    for (long i = 0; i < 100; i++) {
        current = root;
        current = tree_policy(current);

        while(!current->get_state()->is_end_game()) {
            current->get_state()->make_random_action();
        }

        while (current != nullptr) {
            current->update();
            current = current->get_parent();
        }
    }

    current = root->best_child();

    return current;
}

}
