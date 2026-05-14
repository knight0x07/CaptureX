#include "InputParser.hpp"

bool InputParser::IsInteger(std::string& arg_input, int* output_int_val)
{
    size_t position;
    try
    {
        *output_int_val = std::stoi(arg_input, &position);
    }
    catch (const std::exception&)
    {
        return false;
    }
    if (position != arg_input.length())
    {
        return false;
    }
    return true;
}

std::optional<InputParser::ParsedInput> InputParser::Parse(int argc, char* argv[])
{
    if (argc > 3 || argc <= 1)
    {
        return std::nullopt;
    }
    else if (argc == 2)
    {
        if (std::string_view(argv[1]) == "--fullscreen")
        {
            return InputParser::ParsedInput(InputParser::CaptureInputMode::FullScreen);
        }
        else
        {
            return std::nullopt;
        }
    }
    else if (argc == 3)
    {
        if (std::string_view(argv[1]) == "--pid")
        {
            std::string arg_pid = argv[2];
            int input_pid{};
            if (!InputParser::IsInteger(arg_pid, &input_pid))
            {
                return std::nullopt;
            }  
            return InputParser::ParsedInput(input_pid, InputParser::CaptureInputMode::Pid);
        }
        else if (std::string_view(argv[1]) == "--procname")
        {
            std::string process_name = argv[2];
            int ret_value_int{};
            if (InputParser::IsInteger(process_name, &ret_value_int))
            {
                return std::nullopt;
            }
            return InputParser::ParsedInput(process_name, InputParser::CaptureInputMode::ProcName);
        }
        else
        {
            return std::nullopt;
        }
    }
    else
    {
        return std::nullopt;
    }

}
