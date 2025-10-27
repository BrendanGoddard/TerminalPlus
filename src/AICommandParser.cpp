#include <iostream>
#include <sstream>
#include <fstream>
#include <curl/curl.h>
#include "../include/nlohmann/json.hpp"
#include "utils.hpp"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helper: collect streamed output from Ollama
// ---------------------------------------------------------------------------
static size_t WriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* response = static_cast<std::string*>(userdata);
    response->append(ptr, size * nmemb);
    return size * nmemb;
}

// Optional helper to remove whitespace
std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \n\r\t");
    size_t end = s.find_last_not_of(" \n\r\t");
    if (start == std::string::npos || end == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

// ---------------------------------------------------------------------------
// Core function: send prompt to local Ollama server and get the AI’s command
// ---------------------------------------------------------------------------
std::string generateCommand(const std::string& userInput) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL.\n";
        return "ERROR: curl init failed.";
    }

    std::string url = "http://localhost:11434/api/generate";

    // Construct the JSON payload
    json payload = {
        {"model", "llama3"},
        {"prompt",
         "Convert the following natural language task into a valid Windows PowerShell command. "
         "Output ONLY the command and nothing else.\n\nRequest: " + userInput}
    };

    std::string data = payload.dump();

    // Set up HTTP headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Storage for streamed response
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the POST
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "CURL error: " << curl_easy_strerror(res) << "\n";
        return "ERROR: Request failed.";
    }

    // Parse Ollama's streamed JSON lines
    std::stringstream ss(response);
    std::string line;
    std::string fullText;

    try {
        while (std::getline(ss, line)) {
            if (line.empty()) continue;
            auto jsonLine = json::parse(line);
            if (jsonLine.contains("response")) {
                fullText += jsonLine["response"].get<std::string>();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Parse error: " << e.what() << "\n";
        return "ERROR: Failed to parse response.";
    }

    if (fullText.empty())
        return "ERROR: No response received.";

    return trim(fullText);
}
