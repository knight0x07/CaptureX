#pragma once
#include <optional>
#include <string>

class InputParser
{
public:

    enum struct CaptureInputMode
    {
        ProcName,
        Pid,
        FullScreen
    };

    struct ParsedInput
    {
        CaptureInputMode captureinputmode_val;
        std::string procname;
        int pid;

        ParsedInput(CaptureInputMode captureenummode)
            : captureinputmode_val(captureenummode)
        {
        }

        ParsedInput(int in_pid, CaptureInputMode captureenummode)
            : pid(in_pid), captureinputmode_val(captureenummode)
        {
        }

        ParsedInput(std::string& in_procname, CaptureInputMode captureenummode)
            : procname(in_procname), captureinputmode_val(captureenummode)
        {
        }
    };

    static std::optional<ParsedInput> Parse(int argc, char* argv[]);
    static bool IsInteger(std::string& arg_input, int* output_int_val);
};
