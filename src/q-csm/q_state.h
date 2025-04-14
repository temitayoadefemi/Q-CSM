// q_state.h
#ifndef Q_STATE_H
#define Q_STATE_H

#include <vector>
#include <complex> // Use standard complex numbers
#include <string>  // Include string as QState might be used where strings are relevant

// Define M_PI if not available (e.g., not defined by <cmath> in strict C++)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- Struct Definitions ---

// Represents a single basis state with its vector and complex amplitude
struct QState {
    std::vector<float> vector;            // The embedding vector defining the state
    std::complex<double> amplitude = {0.0, 0.0}; // Complex amplitude (defaults to zero)
};

// QBrainState is a collection (superposition) of QStates
using QBrainState = std::vector<QState>;

// Represents an emotion vector with overall intensity and per-state bias
// (Assuming bias vector corresponds element-wise to QBrainState)
struct EmotionVector {
    float intensity = 0.0f;      // Overall strength of the emotion (0.0 to 1.0)
    std::vector<float> bias; // Per-state influence factor (e.g., -1.0 to 1.0)
};


// --- Function Declarations ---

// Computes the centroid (mean vector) of a list of vectors.
std::vector<float> computeCentroid(const std::vector<std::vector<float>>& vectors);

// Computes the cosine similarity between two vectors.
float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b);

// Generates a QBrainState based on the similarity of embeddings to their centroid.
QBrainState generateQBrainState(const std::vector<std::vector<float>>& embeddings);

// Normalizes the amplitudes in a QBrainState so the sum of squared magnitudes is 1.
// Modifies the input state IN-PLACE.
void normalizeQBrainState(QBrainState& brain);

/**
 * @brief Modulates a QBrainState based on an EmotionVector.
 * Creates and returns a NEW modulated state (does not modify input).
 * // ... (rest of documentation comment) ...
 */
QBrainState applyEmotionEngine(const QBrainState& input, const EmotionVector& emotion);


#endif // Q_STATE_H