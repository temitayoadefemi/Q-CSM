#include <iostream>
#include <vector>
#include <string>
#include <complex>
#include <cmath>
#include <map>
#include <stdexcept>
#include <curl/curl.h> 


#include "q-csm/q_state.h"
#include "q-csm/q_decision_layer.h"
#include "q-csm/q_concept.h"
#include "network_utils/network_utils.h" 

int main(int argc, char *argv[]) {
    // --- Argument Handling ---
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " \"sentence 1\" \"sentence 2\" ..." << std::endl;
        std::cerr << "  Provide at least one sentence as a command-line argument." << std::endl;
        return 1;
    }

    // --- Initialize CURL ---
    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
        std::cerr << "FATAL: curl_global_init() failed!" << std::endl;
        return 1;
    }

    // --- Collect Input Texts ---
    std::vector<std::string> texts;
    for (int i = 1; i < argc; ++i) {
        texts.push_back(argv[i]);
    }

    // --- Get Embeddings ---
    std::cout << "Requesting embeddings for " << texts.size() << " texts..." << std::endl;
    std::vector<std::vector<float>> embeddings_list;
    embeddings_list.reserve(texts.size());

    for (const std::string& text : texts) {
        std::vector<float> result = getEmbeddingFromServer(text);
        if (!result.empty()) {
            embeddings_list.push_back(result);
        } else {
            std::cerr << "  ERROR: Failed to get embedding for: \"" << text << "\". Skipping this input." << std::endl;
           
           
        }
    }

    // --- Q-CSM Processing ---
    if (!embeddings_list.empty() && embeddings_list.size() == texts.size()) { // Ensure all embeddings were successful for simplicity
        std::cout << "\nSuccessfully retrieved " << embeddings_list.size() << " embeddings." << std::endl;
        std::cout << "Generating and normalizing initial QBrainState..." << std::endl;

        QBrainState brain = generateQBrainState(embeddings_list);
        normalizeQBrainState(brain); // Normalize the state

        // Ensure brain state size matches input text size after potential embedding failures
        if (brain.size() != texts.size()) {
             std::cerr << "ERROR: Mismatch between number of texts (" << texts.size()
                       << ") and generated QStates (" << brain.size()
                       << ") after embedding failures or generation issues." << std::endl;
             curl_global_cleanup();
             return 1;
        }


        std::cout << "\n--- Initial Normalized QBrainState (" << brain.size() << " states) ---" << std::endl;
        double total_prob_initial = 0;
        for (size_t i = 0; i < brain.size(); ++i) {
            const auto& q = brain[i];
            double probability = std::norm(q.amplitude); // Probability = |amplitude|^2
            total_prob_initial += probability;
            std::cout << "  State " << i << " (\"" << texts[i] << "\"):" << std::endl;
            std::cout << "    Amplitude: " << q.amplitude << " (Mag: " << std::abs(q.amplitude) << ", Phase: " << std::arg(q.amplitude) << ")" << std::endl;
            std::cout << "    Probability: " << probability * 100.0 << "%" << std::endl;
        }
        std::cout << "  Total Initial Probability Check: " << total_prob_initial << std::endl;


        std::cout << "\nApplying Emotion Engine... (Skipped for command-line simplicity)" << std::endl;
        QBrainState modulated_brain = brain; // No change in this simplified version
    
        std::cout << "\n--- \"Emotionally Modulated\" QBrainState (" << modulated_brain.size() << " states) ---" << std::endl;
         double total_prob_modulated = 0;
        if (!modulated_brain.empty()) {
            for (size_t i = 0; i < modulated_brain.size(); ++i) {
                 const auto& q = modulated_brain[i];
                 double probability = std::norm(q.amplitude);
                 total_prob_modulated += probability;
                 std::cout << "  State " << i << " (\"" << texts[i] << "\"):" << std::endl;
                 std::cout << "    Amplitude: " << q.amplitude << " (Mag: " << std::abs(q.amplitude) << ", Phase: " << std::arg(q.amplitude) << ")" << std::endl;
                 std::cout << "    Probability: " << probability * 100.0 << "%" << std::endl;
            }
             std::cout << "  Total Modulated Probability Check: " << total_prob_modulated << std::endl;

            bool collapse_state = true;
            std::cout << "\nMaking Decision (Collapse = " << (collapse_state ? "true" : "false") << ")..." << std::endl;
            DecisionResult decision = makeDecision(modulated_brain, collapse_state);

            std::vector<float> final_vector;
            std::string result_type_str;

            if (decision.collapsed) {
                std::string collapsed_text = "[Unknown - state vector mismatch]";
                for(size_t i = 0; i < embeddings_list.size(); ++i) {
                    if (cosineSimilarity(decision.collapsedState.vector, embeddings_list[i]) > 0.9999f) {
                         collapsed_text = texts[i];
                         break;
                    }
                }

                std::cout << "\nFinal Collapsed Thought: \"" << collapsed_text << "\"" << std::endl;
                std::cout << "  Amplitude: " << decision.collapsedState.amplitude << "\n";
                final_vector = decision.collapsedState.vector;
                result_type_str = "Collapsed State Vector";

            } else {
                std::cout << "\nEvolved Cognitive Direction (Vector):" << std::endl;
                 std::cout << "  [";
                for (size_t i=0; i < 5 && i < decision.evolvedDirection.size(); ++i) std::cout << decision.evolvedDirection[i] << (i<4 ? ", " : "");
                std::cout << "... ] (" << decision.evolvedDirection.size() << " dimensions)" << std::endl;
                final_vector = decision.evolvedDirection;
                 result_type_str = "Evolved Direction Vector";
            }


            if (!final_vector.empty()) {
                 std::cout << "\nMapping " << result_type_str << " to concepts..." << std::endl;

                 std::map<std::string, std::vector<float>> conceptLibrary;
                 for (size_t i = 0; i < embeddings_list.size(); ++i) {
                     std::string concept_name = "input_" + std::to_string(i) + ": " + texts[i].substr(0, 20) + (texts[i].length() > 20 ? "..." : ""); // Use input index + snippet
                     conceptLibrary[concept_name] = embeddings_list[i];
                 }

                 int topK = std::min(3, (int)conceptLibrary.size()); // Get top 3 or fewer if less inputs
                 auto topConcepts = mapVectorToTopConcepts(final_vector, conceptLibrary, topK);

                 std::cout << "Top " << topConcepts.size() << " concepts identified:" << std::endl;
                 for(const auto& concept : topConcepts) {
                     std::cout << "  - " << concept << std::endl;
                 }

                 std::cout << "\nGenerating LLM prompt..." << std::endl;
                 std::string prompt = buildPrompt(topConcepts);
                 // std::cout << "--- Prompt Start ---\n" << prompt << "\n--- Prompt End ---" << std::endl; // Optional: view prompt


                 std::cout << "\nRequesting reflection from LLM..." << std::endl;
                 std::string reflection = reflectWithLLM(prompt); // Ensure q_concept.cpp implements this

                 std::cout << "\n🧠 LLM Reflection:\n" << reflection << std::endl;
            } else {
                 std::cerr << "ERROR: Final vector for concept mapping is empty." << std::endl;
            }

        } else {
            std::cerr << "ERROR: Modulated brain state is empty. Cannot proceed." << std::endl;
        }
    } else if (embeddings_list.empty()){
        std::cerr << "ERROR: No embeddings were successfully retrieved. Cannot proceed." << std::endl;
    } else {
         std::cerr << "ERROR: Could not retrieve embeddings for all input sentences. Cannot proceed." << std::endl;
    }

    // --- Cleanup CURL ---
    curl_global_cleanup();
    return 0;
}