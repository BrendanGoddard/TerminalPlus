#include <iostream>

std::string generateCommand(const std::string& userInput);
void runShell(const std::string& command);
bool isSafe(const std::string& cmd);

int main() {
    std::cout << "AI Terminal Assistant\n";
    std::cout << "Type a task (or 'exit' to quit):\n";

    std::string input;

    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, input);
        if (input == "exit") break;

        std::cout << "\n Generating command...\n";

        std::string command = generateCommand(input);
        if (command.rfind("ERROR", 0) == 0) {
            std::cout << command << "\n";
            continue;
        }

        std::cout << "\n Generated Command: " << command << "\n";

        if (!isSafe(command)) {
            std::cout << "⚠️ Command flagged as unsafe.\n";
            continue;
        }

        runShell(command);
    }

    std::cout << "\nGoodbye!\n";
    return 0;
}
