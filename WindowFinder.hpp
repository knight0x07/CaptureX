#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <filesystem>
#include <psapi.h>
#include <optional>

class WindowFinder
{
private:
    struct EnumContext
    {
        DWORD target_pid;
        HWND result_hwnd = nullptr;
        std::string_view proc_name;
        std::vector<HWND> proc_handles;

        EnumContext(DWORD t_pid)
            : target_pid(t_pid)
        {
        }

        EnumContext(std::string_view processname)
            : proc_name(processname)
        {
        }
    };

private:

    static std::string GetProcessNameFromPid(DWORD pid);
    static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam);

public:

    static std::optional<HWND> FindWindowForPID(int pid_in);
    static std::optional<std::vector<HWND>> GetWindowsByProcessName(std::string_view process_name);
};
