#include <iostream>
#include <vector>
#include <string>

#if (defined(_WIN32) || defined(_WIN64)) && !(defined(__MINGW32__) || defined(__MINGW64__))
#include <windows.h>
#define _WINAPI
#else
#include <sys/wait.h>
#include <unistd.h>
#undef _WINAPI
#endif

using std::cerr;
using std::string;
using std::vector;

int run(const vector<string>&);

#ifdef _WINAPI
int run(const vector<string>& args) {
    string command;
    for (const auto& arg : args) {
        command += '"' + arg + '"' + " ";
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    if (!CreateProcessA(nullptr, command.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi)) {
        cerr << "Error: failed to execute command.\n";
        return 1;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exit_code;
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return exit_code == 0 ? 1 : 0;
}
#else
int run(const vector<string>& args) {
    auto pid = fork();
    if (pid == -1) {
        cerr << "Error: failed to fork.\n";
        return 1;
    } else if (pid == 0) {
        setsid();
        vector<char*> exec_args;
        for (const auto& arg : args) {
            exec_args.push_back(const_cast<char*>(arg.c_str()));
        }
        exec_args.push_back(nullptr);
        execvp(exec_args[0], exec_args.data());
        cerr << "Error: failed to execute command.\n";
        _exit(127);
    }
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status) == 0 ? 1 : 0;
    }
    return 1;
}
#endif

int main(int argc, char* argv[]) {
    vector<string> args(argv + 1, argv + argc);
    if (args.empty()) {
        cerr << "Usage: not <command> [arguments...]\n";
        return 1;
    }
    return run(args);
}
