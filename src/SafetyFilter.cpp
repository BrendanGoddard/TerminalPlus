#include <string>
#include <vector>

bool isSafe(const std::string& cmd) {
    static const std::vector<std::string> dangerous = {
        "rm", "del", "format", "shutdown", "erase", "rmdir", "kill"
    };
    for (const auto& word : dangerous) {
        if (cmd.find(word) != std::string::npos) return false;
    }
    return true;
}
