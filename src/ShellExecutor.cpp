#include <iostream>
#include <cstdio>

void runShell(const std::string& command) {
    std::string cmd = "powershell.exe -Command \"" + command + "\"";
    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) {
        std::cerr << "Failed to run command.\n";
        return;
    }
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        std::cout << buffer;
    }
    _pclose(pipe);
}
