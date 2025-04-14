// emotion_engine.cpp
#include "q_state.h" // Include the consolidated header

#include <vector>
#include <complex>
#include <cmath>
#include <iostream> // For std::cerr
#include <algorithm> // For std::max, std::min
#include <limits>    // For epsilon

// --- Function Definition ---

QBrainState applyEmotionEngine(const QBrainState& input, const EmotionVector& emotion) {
    QBrainState modulated; // Create the state to return

    // Handle empty input state immediately
    if (input.empty()) {
        return modulated;
    }

    // Check for size mismatch between state and bias vector
    if (input.size() != emotion.bias.size()) {
        std::cerr << "ERROR in applyEmotionEngine: Mismatch between QBrainState size ("
                  << input.size() << ") and EmotionVector bias size ("
                  << emotion.bias.size() << "). Returning original state." << std::endl;
        return input; // Return original state to signal error
    }

    // Reserve space
    modulated.reserve(input.size());

    // --- Apply emotional modulation to magnitudes ---
    for (size_t i = 0; i < input.size(); ++i) {
        // Get original amplitude components
        const std::complex<double>& original_amp = input[i].amplitude;
        double original_mag = std::abs(original_amp);
        double original_phase = std::arg(original_amp);

        // Calculate modulation factor
        float bias = emotion.bias[i];
        float clamped_intensity = std::max(0.0f, std::min(1.0f, emotion.intensity));
        float delta = bias * clamped_intensity;
        float scale_factor = 1.0f + delta;

        // Calculate new magnitude, clamp >= 0
        double new_mag = original_mag * static_cast<double>(scale_factor);
        new_mag = std::max(0.0, new_mag); // Ensure non-negative magnitude

        // Keep original phase
        double new_phase = original_phase;

        // Reconstruct the complex amplitude
        std::complex<double> new_amp = std::polar(new_mag, new_phase);

        // Add the modulated state (COPY the vector, add new amplitude)
        modulated.push_back(QState{input[i].vector, new_amp});
    }

    // --- Normalize the new state USING the existing function ---
    // normalizeQBrainState modifies the state in-place
    normalizeQBrainState(modulated);

    return modulated; // Return the modulated and normalized state
}