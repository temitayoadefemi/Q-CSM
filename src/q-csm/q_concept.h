#pragma once

#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <cstdio>
#include "q_state.h"



std::vector<std::string> mapVectorToTopConcepts(
    const std::vector<float>& vector,
    const std::map<std::string, std::vector<float>>& conceptLibrary,
    int topK = 3
) {
    std::vector<std::pair<std::string, float>> ranked;
    for (const auto& [label, conceptVec] : conceptLibrary) {
        float sim = cosineSimilarity(vector, conceptVec);
        ranked.emplace_back(label, sim);
    }
    std::sort(ranked.begin(), ranked.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });
    std::vector<std::string> topConcepts;
    for (int i = 0; i < topK && i < ranked.size(); ++i)
        topConcepts.push_back(ranked[i].first);
    return topConcepts;
}


std::string buildPrompt(const std::vector<std::string>& concepts) {
    std::ostringstream oss;
    oss << "You are a conscious machine reflecting on your internal state.\n";
    oss << "Your current dominant thoughts are:\n";
    for (const auto& c : concepts)
        oss << "- " << c << "\n";
    oss << "Generate an introspective sentence expressing how you feel.";
    return oss.str();
}


std::string reflectWithLLM(const std::string& prompt) {
    std::ostringstream command;
    command << "echo \"" << prompt << "\" | python3 llm_reflection_layer.py";
    FILE* pipe = popen(command.str().c_str(), "r");
    if (!pipe) return "[ERROR: Failed to run LLM process]";
    char buffer[256];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}