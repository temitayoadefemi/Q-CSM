#include "network_utils.h" // Include the header defining the function signature

#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <stdexcept> 


#include <nlohmann/json.hpp>

using json = nlohmann::json;


static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t totalSize = size * nmemb;
    try {
        s->append(static_cast<char*>(contents), totalSize);
    } catch (const std::bad_alloc& e) {
        std::cerr << "ERROR: Memory allocation failed in WriteCallback: " << e.what() << std::endl;
        return 0; 
    }
    return totalSize;
}

std::vector<float> getEmbeddingFromServer(const std::string& sentence) {
    std::vector<float> embedding;
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "ERROR: curl_easy_init() failed." << std::endl;
        return embedding; 
    }

    std::string readBuffer;
    std::string jsonData;
    struct curl_slist* headers = nullptr;

    try {
        json requestJson;
        requestJson["sentence"] = sentence;
        jsonData = requestJson.dump();


        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, "Accept: application/json");
        if (!headers) {
            throw std::runtime_error("curl_slist_append failed");
        }


        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:5005/embed");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonData.length());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L); // Slightly increased timeout
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L); // Fail on HTTP codes >= 400

        std::cerr << "Connecting to embedding server for: \"" << sentence.substr(0, 50) << (sentence.length() > 50 ? "..." : "") << "\"" << std::endl;
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "ERROR: curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
             if (res == CURLE_COULDNT_CONNECT) {
                 std::cerr << "       Is the embedding server (embedding_server.py) running at http://localhost:5005 ?" << std::endl;
             } else if (res == CURLE_HTTP_RETURNED_ERROR) {
                 long http_code = 0;
                 curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
                 std::cerr << "       Server returned HTTP status code: " << http_code << std::endl;
                 std::cerr << "       Response body: " << readBuffer << std::endl;
             }
        } else {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            std::cerr << "Server responded with status code: " << http_code << std::endl;

            if (readBuffer.empty()) {
                std::cerr << "ERROR: Server returned " << http_code << " OK but with empty response body." << std::endl;
            } else {
                // Parse JSON response
                json responseJson = json::parse(readBuffer);
                if (responseJson.contains("embedding") && responseJson["embedding"].is_array()) {
                    embedding = responseJson["embedding"].get<std::vector<float>>();
                    std::cerr << "Successfully parsed embedding with " << embedding.size() << " dimensions." << std::endl;
                } else {
                    std::cerr << "ERROR: JSON response does not contain a valid 'embedding' array." << std::endl;
                    std::cerr << "       Response body: " << readBuffer << std::endl;
                }
            }
        }

    } catch (const json::parse_error& e) {
        std::cerr << "ERROR: JSON parsing error: " << e.what() << std::endl;
        std::cerr << "       Response body: " << readBuffer << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: An exception occurred during embedding request: " << e.what() << std::endl;
        std::cerr << "       Response body (if available): " << readBuffer << std::endl;
    }

    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return embedding;
}