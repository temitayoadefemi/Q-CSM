#pragma once

#include <string>
#include <vector>

// Forward declaration for CURL handle
typedef void CURL;

// Callback function for libcurl to write received data into a string
// Definition remains static in network_utils.cpp, so no declaration needed here.
// static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s);

/**
 * @brief Connects to the embedding server and retrieves the embedding vector for a given sentence.
 *
 * @param sentence The sentence to embed.
 * @return std::vector<float> The embedding vector, or an empty vector on failure.
 */
std::vector<float> getEmbeddingFromServer(const std::string& sentence);