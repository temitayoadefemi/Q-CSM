// q_decision_layer.h
#pragma once

#include <vector>
#include <complex>
#include <random>
#include <cmath>
#include "q_state.h"

// Collapse a QBrainState to a single QState based on probability weights
QState collapseQBrainState(const QBrainState& brain) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    double r = dis(gen);
    double cumulative = 0.0;

    for (const auto& q : brain) {
        cumulative += std::norm(q.amplitude);
        if (r <= cumulative) {
            return q;
        }
    }
    return brain.back(); // fallback
}

// Evolve QBrainState into a weighted average direction vector
std::vector<float> evolveQBrainState(const QBrainState& brain) {
    if (brain.empty()) return {};

    size_t dim = brain[0].vector.size();
    std::vector<float> result(dim, 0.0f);

    for (const auto& q : brain) {
        double weight = std::norm(q.amplitude);
        for (size_t i = 0; i < dim; ++i) {
            result[i] += static_cast<float>(weight * q.vector[i]);
        }
    }

    return result;
}

// Unified structure for decision result
struct DecisionResult {
    bool collapsed;
    QState collapsedState;               // Used if collapsed == true
    std::vector<float> evolvedDirection; // Used if collapsed == false
};

// Top-level decision API
DecisionResult makeDecision(const QBrainState& brain, bool doCollapse = true) {
    DecisionResult result;
    if (doCollapse) {
        result.collapsed = true;
        result.collapsedState = collapseQBrainState(brain);
    } else {
        result.collapsed = false;
        result.evolvedDirection = evolveQBrainState(brain);
    }
    return result;
}