// q_state.cpp
#include "q_state.h" // Include the consolidated header
#include <cmath>
#include <vector>
#include <complex>
#include <numeric>
#include <stdexcept> // Optional: if you want to throw exceptions on errors

// Computes the centroid (mean vector) of a list of vectors.
std::vector<float> computeCentroid(const std::vector<std::vector<float>>& vectors) {
    if (vectors.empty() || vectors[0].empty()) {
         return {};
    }
    size_t dim = vectors[0].size();
    std::vector<float> centroid(dim, 0.0f);
    size_t valid_vectors = 0;
    for (const auto& v : vectors) {
        if (v.size() != dim) {
             // Optionally log an error or throw
             // std::cerr << "Warning: Inconsistent vector dimension in computeCentroid." << std::endl;
             continue; // Skip vectors with wrong dimension
        }
        for (size_t i = 0; i < dim; ++i) {
            centroid[i] += v[i];
        }
        valid_vectors++;
    }
    if (valid_vectors > 0) {
         for (auto& val : centroid) {
            val /= static_cast<float>(valid_vectors);
        }
    } else {
        return {}; // No valid vectors found
    }
    return centroid;
}

// Computes the cosine similarity between two vectors.
float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
    if (a.size() != b.size() || a.empty()) {
        return 0.0f;
    }
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    float normProduct = std::sqrt(normA) * std::sqrt(normB);
    // Use a small epsilon for robustness
    const float epsilon = 1e-8f;
    if (normProduct < epsilon) {
        // Check if both vectors are essentially zero vectors
        return (normA < epsilon && normB < epsilon) ? 1.0f : 0.0f;
    }
    return dot / normProduct;
}

// Generates a QBrainState based on the similarity of embeddings to their centroid.
QBrainState generateQBrainState(const std::vector<std::vector<float>>& embeddings) {
    QBrainState brain;
    if (embeddings.empty()) {
        return brain;
    }
    auto centroid = computeCentroid(embeddings);
    if (centroid.empty()) {
         // std::cerr << "Warning: Centroid could not be computed in generateQBrainState." << std::endl;
         return brain; // Centroid calculation failed
    }

    brain.reserve(embeddings.size()); // Optimize allocation

    for (const auto& vec : embeddings) {
        // Skip vectors that don't match centroid dimension
        if (vec.empty() || vec.size() != centroid.size()) continue;

        float similarity = cosineSimilarity(vec, centroid);
        similarity = (similarity + 1.0f) / 2.0f; // Map -> [0, 1]

        double magnitude = static_cast<double>(similarity);
        // Ensure phase calculation uses a valid magnitude for edge cases if needed, though similarity is [0,1] here
        double phase = magnitude * M_PI; // Phase proportional to similarity
        std::complex<double> amplitude = std::polar(magnitude, phase);

        brain.push_back(QState{vec, amplitude});
    }
    return brain;
}

// Normalizes the amplitudes in a QBrainState so the sum of squared magnitudes is 1.
void normalizeQBrainState(QBrainState& brain) {
    if (brain.empty()) {
        return;
    }
    double totalSquaredMagnitude = 0.0;
    for (const auto& q : brain) {
        totalSquaredMagnitude += std::norm(q.amplitude); // std::norm(complex) = |amp|^2
    }

    const double epsilon = 1e-12; // Use double epsilon
    if (totalSquaredMagnitude < epsilon) {
        // Handle zero norm state
        size_t n_states = brain.size();
        if (n_states > 0) {
             // std::cerr << "Warning: State norm near zero in normalizeQBrainState. Setting to uniform." << std::endl;
            double uniform_mag = 1.0 / std::sqrt(static_cast<double>(n_states));
            for (auto& q : brain) {
                // Set uniform amp with phase 0, or preserve original phase? Let's use phase 0.
                q.amplitude = std::polar(uniform_mag, 0.0);
            }
        }
        return; // State is now uniform (or still empty)
    }

    double scale = std::sqrt(totalSquaredMagnitude);
    for (auto& q : brain) {
        q.amplitude /= scale;
    }
}