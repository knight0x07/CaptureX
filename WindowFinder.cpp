#include "WindowFinder.hpp"

std::string WindowFinder::GetProcessNameFromPid(DWORD pid)
{
    std::string process_name = "INVALID_PROCESS_NAME";
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProcess != nullptr)
    {
        char buffer[MAX_PATH] = {};
        GetProcessImageFileNameA(hProcess, buffer, MAX_PATH);
        process_name.assign(buffer);
        process_name.assign(std::filesystem::path(process_name).filename().string());
        CloseHandle(hProcess);
    }
    return process_name;
}

BOOL CALLBACK WindowFinder::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    EnumContext* enum_context_callback = reinterpret_cast<EnumContext*>(lParam);
    DWORD WndProcessPid = 0;
    GetWindowThreadProcessId(hWnd, &WndProcessPid);
    if (!enum_context_callback->proc_name.empty() && IsWindowVisible(hWnd))
    {
        //get process_name from wndprocesspid
        std::string process_name_from_pid = WindowFinder::GetProcessNameFromPid(WndProcessPid);
        if (process_name_from_pid == enum_context_callback->proc_name)
        {
            //Add target hWnd to the proc_handles vector
            enum_context_callback->proc_handles.push_back(hWnd);
        }
    }
    else if (WndProcessPid == enum_context_callback->target_pid && IsWindowVisible(hWnd))
    {
        enum_context_callback->result_hwnd = hWnd;
        return FALSE;
    }
    return TRUE;
}

std::optional<HWND> WindowFinder::FindWindowForPID(int pid_in)
{
    EnumContext enum_context(static_cast<DWORD>(pid_in));
    EnumWindows(&EnumWindowsProc, reinterpret_cast<LPARAM>(&enum_context));
    if (enum_context.result_hwnd != nullptr)
    {
        return enum_context.result_hwnd;
    }
    else
    {
        return std::nullopt;
    }
}

std::optional<std::vector<HWND>> WindowFinder::GetWindowsByProcessName(std::string_view process_name)
{
    EnumContext procnameenumcontext(process_name);
    EnumWindows(&EnumWindowsProc, reinterpret_cast<LPARAM>(&procnameenumcontext));
    if (!procnameenumcontext.proc_handles.empty())
    {
        return procnameenumcontext.proc_handles;
    }
    else
    {
        return std::nullopt;
    }
}
