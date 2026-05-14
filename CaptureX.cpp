#include "InputParser.hpp"
#include "WindowFinder.hpp"
#include "CaptureSpace.hpp"

int main(int argc,char* argv[])
{
    std::optional<InputParser::ParsedInput> parsed_input = InputParser::Parse(argc, argv);
    if (parsed_input.has_value())
    {
        SetProcessDPIAware();
        switch (parsed_input->captureinputmode_val)
        {
            case InputParser::CaptureInputMode::ProcName:
            {
                std::cout << "[+] CaptureX - ScreenShot Target Process Name: " << parsed_input->procname << std::endl;
                std::optional<std::vector<HWND>> prochandles = WindowFinder::GetWindowsByProcessName(parsed_input->procname);
                if (prochandles.has_value())
                {
                    for (const HWND& proc_window_handle : *prochandles)
                    {
                        HBITMAP ret_window_screen_capture_hbitmap_proc_name = CaptureSpace::ScreenCapture::WindowScreenCapture(proc_window_handle);
                        if (ret_window_screen_capture_hbitmap_proc_name == nullptr)
                        {
                            std::cout << "[-] Error: Process Name ScreenShot failed." << std::endl;
                            continue;
                        }
                        bool jpeg_save_proc_name = CaptureSpace::ScreenCapture::SaveScreenCapture(&ret_window_screen_capture_hbitmap_proc_name, parsed_input->procname);
                        if (jpeg_save_proc_name == true)
                        {
                            std::cout << "[+] CaptureX - Process Name ScreenShot saved to disk successfully." << std::endl;
                        }
                        else
                        {
                            std::cout << "[-] Error: Process Name ScreenShot not saved to disk successfully." << std::endl;
                        }
                        DeleteObject(ret_window_screen_capture_hbitmap_proc_name);
                    }
                    break;
                }
                else
                {
                    std::cout << "[-] Error: No Windows found for Target Process Name: " << parsed_input->procname << std::endl;
                    break;
                }
            }

            case InputParser::CaptureInputMode::Pid:
            {
                std::cout << "[+] CaptureX - ScreenShot Target PID: " << parsed_input->pid << std::endl;
                std::optional<HWND> window_handle = WindowFinder::FindWindowForPID(parsed_input->pid);
                if (window_handle.has_value())
                {
                    HBITMAP ret_window_screen_capture_hbitmap = CaptureSpace::ScreenCapture::WindowScreenCapture(*window_handle);
                    if (ret_window_screen_capture_hbitmap == nullptr)
                    {
                        std::cout << "[-] Error: PID ScreenShot failed." << std::endl;
                        break;
                    }
                    bool jpeg_save = CaptureSpace::ScreenCapture::SaveScreenCapture(&ret_window_screen_capture_hbitmap, parsed_input->pid);
                    if (jpeg_save == true)
                    {
                        std::cout << "[+] CaptureX - PID ScreenShot saved to disk successfully." << std::endl;
                    }
                    else
                    {
                        std::cout << "[-] Error: PID ScreenShot not saved to disk successfully." << std::endl;
                    }
                    DeleteObject(ret_window_screen_capture_hbitmap);
                    break;
                }
                else
                {
                    std::cout << "[-] Error: No Windows found for Target PID: " << parsed_input->pid << std::endl;
                    break;
                }
            }

            case InputParser::CaptureInputMode::FullScreen:
            {
                std::cout << "[+] CaptureX - FullScreen ScreenShot" << std::endl;
                HBITMAP ret_fullscreen_capture_bitmap = CaptureSpace::ScreenCapture::FullScreenCapture();
                if (ret_fullscreen_capture_bitmap == nullptr)
                {
                    std::cout << "[-] Error: FullScreen ScreenShot failed." << std::endl;
                    break;
                }
                bool jpeg_save = CaptureSpace::ScreenCapture::SaveScreenCapture(&ret_fullscreen_capture_bitmap, std::monostate{});
                if (jpeg_save == true)
                {
                    std::cout << "[+] CaptureX - FullScreen ScreenShot saved to disk successfully." << std::endl;
                }
                else
                {
                    std::cout << "[-] Error: FullScreen ScreenShot not saved to disk successfully." << std::endl;
                }
                DeleteObject(ret_fullscreen_capture_bitmap);
                break;
            }
        }
    }
    else
    {
        std::cout << "[-] CaptureX invalid cmdline arguments" << std::endl;
        std::cout << "[-] Usage: CaptureX.exe (--pid | --procname) (pid_value | process_name) or CaptureX.exe --fullscreen" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "[*] Exiting CaptureX" << std::endl;
}
