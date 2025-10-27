#pragma once
#include <string>
#include <fstream>
#include <sstream>

inline std::string readEnvVar(const std::string& key) {
    std::ifstream file(".env");
    std::string line;
    std::string prefix = key + "=";

    while (std::getline(file, line)) {
        if (line.rfind(prefix, 0) == 0)
            return line.substr(prefix.size());
    }
    return "";
}
