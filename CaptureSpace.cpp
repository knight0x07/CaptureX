#include "CaptureSpace.hpp"

std::optional<LONG> CaptureSpace::ScreenCapture::RestoreWindowState(HWND hWnd)
{

    LONG ret_WindowInfo = GetWindowLongA(hWnd, GWL_EXSTYLE);
    if (ret_WindowInfo == NULL)
    {
        return std::nullopt;
    }
    LONG set_WindowInfo = SetWindowLongA(hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED | ret_WindowInfo);
    if (set_WindowInfo == NULL)
    {
        return std::nullopt;
    }
    BOOL set_WindowAttrib = SetLayeredWindowAttributes(hWnd, NULL, NULL, LWA_ALPHA);
    if (set_WindowAttrib == FALSE)
    {
        return std::nullopt;
    }
    ShowWindow(hWnd, SW_RESTORE);
    BOOL ret_UpdateWin = UpdateWindow(hWnd);
    if (ret_UpdateWin == FALSE)
    {
        return std::nullopt;
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return ret_WindowInfo;
}

bool CaptureSpace::ScreenCapture::BitMapCaptureScreen(std::optional<HWND> hWnd, CaptureSpace::ScreenCapture::InitScreenCapture& isccap, CaptureSpace::h_DC& hdcscreen, CaptureSpace::h_compatdc& hdccompat)
{
    if (hWnd.has_value())
    {
        BOOL ret_window_rect = GetWindowRect(*hWnd, &isccap.screen_rect_struct);
        if (ret_window_rect == false)
        {
            return false;
        }
        isccap.screen_width = isccap.screen_rect_struct.right - isccap.screen_rect_struct.left;
        isccap.screen_height = isccap.screen_rect_struct.bottom - isccap.screen_rect_struct.top;
        if (isccap.screen_width <= 0 || isccap.screen_height <= 0)
        {
            return false;
        }

        BITMAPINFO bitmapinfo{};
        bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmapinfo.bmiHeader.biWidth = isccap.screen_width;
        bitmapinfo.bmiHeader.biHeight = -isccap.screen_height;
        bitmapinfo.bmiHeader.biPlanes = 1;
        bitmapinfo.bmiHeader.biBitCount = 32;
        bitmapinfo.bmiHeader.biCompression = BI_RGB;

        void* pBits = nullptr;
        isccap.hBitmap = CreateDIBSection(hdcscreen.Get(), &bitmapinfo, DIB_RGB_COLORS, &pBits, nullptr, NULL);
        if (isccap.hBitmap == nullptr)
        {
            return false;
        }
        SelectObject(hdccompat.Get(), isccap.hBitmap);
        BOOL ret_printwinapi = PrintWindow(*hWnd, hdccompat.Get(), PW_RENDERFULLCONTENT);
        if (ret_printwinapi == false)
        {
            ret_printwinapi = BitBlt(hdccompat.Get(), NULL, NULL, isccap.screen_width, isccap.screen_height, hdcscreen.Get(), isccap.screen_rect_struct.left, isccap.screen_rect_struct.top, SRCCOPY);
            if (ret_printwinapi == false)
            {
                return false;
            }
        }
        return true;
    }
    else if (hWnd == std::nullopt)
    {
        BITMAPINFO bitmapinfo{};
        bitmapinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmapinfo.bmiHeader.biWidth = isccap.w_axis;
        bitmapinfo.bmiHeader.biHeight = -isccap.h_axis;
        bitmapinfo.bmiHeader.biPlanes = 1;
        bitmapinfo.bmiHeader.biBitCount = 32;
        bitmapinfo.bmiHeader.biCompression = BI_RGB;

        void* pBits = nullptr;
        isccap.hBitmap = CreateDIBSection(hdcscreen.Get(), &bitmapinfo, DIB_RGB_COLORS, &pBits, nullptr, NULL);
        if (isccap.hBitmap == nullptr)
        {
            return false;
        }
        SelectObject(hdccompat.Get(), isccap.hBitmap);
        BOOL ret_printwinapi = BitBlt(hdccompat.Get(), NULL, NULL, isccap.w_axis, isccap.h_axis, hdcscreen.Get(), isccap.x_axis, isccap.y_axis, SRCCOPY);
        if (ret_printwinapi == NULL)
        {
            return false;
        }
        return true;
    }
    else
    {
        return false;
    }
}

std::optional<UINT> CaptureSpace::ScreenCapture::GetWindowState(HWND hWnd)
{
    WINDOWPLACEMENT winplacement{};
    winplacement.length = sizeof(winplacement);
    if (!GetWindowPlacement(hWnd, &winplacement))
    {
        return std::nullopt;
    }
    return winplacement.showCmd;
}

bool CaptureSpace::ScreenCapture::SaveScreenCapture(HBITMAP* hbitmap_ptr, const std::variant<std::monostate, int, std::string>& varinput)
{
    static std::random_device rand_device;
    static std::mt19937 generator(rand_device());
    static std::uniform_int_distribution<int> distri(1000, 9999);
    int rand_num = distri(generator);
    std::wstring ss_file_name;
    const wchar_t* w_ss_file_name = nullptr;
    if (std::holds_alternative<int>(varinput))
    {
        ss_file_name = L"screen_capture_pid_" + std::to_wstring(std::get<int>(varinput)) + L".jpg";
        w_ss_file_name = ss_file_name.c_str();
    }
    else if (std::holds_alternative<std::monostate>(varinput))
    {

        ss_file_name = L"full_screen_capture_" + std::to_wstring(rand_num) + L".jpg";
        w_ss_file_name = ss_file_name.c_str();
    }
    else if (std::holds_alternative<std::string>(varinput))
    {
        std::wstring wprocname(std::get<std::string>(varinput).begin(), std::get<std::string>(varinput).end());
        ss_file_name = L"screen_capture_procname_" + wprocname + L"_" + std::to_wstring(rand_num) + L".jpg";
        w_ss_file_name = ss_file_name.c_str();
    }
    else
    {
        //Invalid input
        return false;
    }
    CImage image_obj;
    image_obj.Attach(*hbitmap_ptr);
    HRESULT ret_save = image_obj.Save(w_ss_file_name);
    image_obj.Detach();
    if (ret_save == S_OK)
    {
        return true;
    }
    return false;
}

HBITMAP CaptureSpace::ScreenCapture::WindowScreenCapture(HWND hWnd)
{
    std::optional<UINT> window_state = CaptureSpace::ScreenCapture::GetWindowState(hWnd);
    if (window_state.has_value())
    {
        CaptureSpace::ScreenCapture::InitScreenCapture init_capture;
        CaptureSpace::h_DC Screen_hdc;
        if (Screen_hdc.Get() == nullptr)
        {
            return nullptr;
        }
        CaptureSpace::h_compatdc Screen_hdc_compatdc(Screen_hdc.Get());
        if (Screen_hdc_compatdc.Get() == nullptr)
        {
            return nullptr;
        }
        if (*window_state == SW_SHOWMINIMIZED)
        {
            //Target window is minimized
            std::optional<LONG> ret_WindowInformation = CaptureSpace::ScreenCapture::RestoreWindowState(hWnd);
            if (ret_WindowInformation.has_value())
            {
                bool hBitMapRetVal = CaptureSpace::ScreenCapture::BitMapCaptureScreen(hWnd, init_capture, Screen_hdc, Screen_hdc_compatdc);
                if (hBitMapRetVal == false)
                {
                    return nullptr;
                }
                ShowWindow(hWnd, SW_MINIMIZE);
                SetWindowLongA(hWnd, GWL_EXSTYLE, *ret_WindowInformation);
                SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                return init_capture.hBitmap;
            }
            else
            {
                return nullptr;
            }
        }
        else
        {
            bool hBitMapRetVal = CaptureSpace::ScreenCapture::BitMapCaptureScreen(hWnd, init_capture, Screen_hdc, Screen_hdc_compatdc);
            if (hBitMapRetVal == false)
            {
                return nullptr;
            }
            return init_capture.hBitmap;
        }
    }
    else
    {
        return nullptr;
    }
}

HBITMAP CaptureSpace::ScreenCapture::FullScreenCapture()
{
    CaptureSpace::ScreenCapture::InitScreenCapture init_capture_fullscreen(true);
    CaptureSpace::h_DC hdc_object;
    if (hdc_object.Get() == nullptr)
    {
        return nullptr;
    }
    CaptureSpace::h_compatdc hdc_compat_object(hdc_object.Get());
    if (hdc_compat_object.Get() == nullptr)
    {
        return nullptr;
    }
    bool hBitMapReturnVal = CaptureSpace::ScreenCapture::BitMapCaptureScreen(std::nullopt, init_capture_fullscreen, hdc_object, hdc_compat_object);
    if (hBitMapReturnVal == false)
    {
        return nullptr;
    }
    return init_capture_fullscreen.hBitmap;
}
