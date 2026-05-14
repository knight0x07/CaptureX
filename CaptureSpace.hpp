#pragma once
#include <Windows.h>
#include <optional>
#include <variant>
#include <string>
#include <chrono>
#include <thread>
#include <atlimage.h>
#include <random>

namespace CaptureSpace
{

    class h_DC
    {
    private:

        HDC sc_hdc = nullptr;

    public:

        h_DC()
        {
            sc_hdc = GetDC(nullptr);
        }

        ~h_DC()
        {
            if (sc_hdc != nullptr)
            {
                ReleaseDC(nullptr, sc_hdc);
            }
        }

        HDC Get() const
        {
            return sc_hdc;
        }
    };

    class h_compatdc
    {
    private:

        HDC sc_compatdc = nullptr;

    public:

        h_compatdc(HDC sc_hdc)
        {
            sc_compatdc = CreateCompatibleDC(sc_hdc);
        }

        ~h_compatdc()
        {
            if (sc_compatdc != nullptr)
            {
                DeleteDC(sc_compatdc);
            }
        }

        HDC Get() const
        {
            return sc_compatdc;
        }
    };

    class ScreenCapture
    {
    private:

        struct InitScreenCapture
        {

            RECT screen_rect_struct;
            int screen_width;
            int screen_height;
            HBITMAP hBitmap = nullptr;
            int x_axis, y_axis, w_axis, h_axis;
            bool is_fullscreen;

            InitScreenCapture(bool set_fullscreen_val = false)
                : is_fullscreen(set_fullscreen_val)
            {
                if (is_fullscreen)
                {
                    x_axis = GetSystemMetrics(SM_XVIRTUALSCREEN);
                    y_axis = GetSystemMetrics(SM_YVIRTUALSCREEN);
                    w_axis = GetSystemMetrics(SM_CXVIRTUALSCREEN);
                    h_axis = GetSystemMetrics(SM_CYVIRTUALSCREEN);
                }
            }
        };

        static std::optional<LONG> RestoreWindowState(HWND hWnd);

        static bool BitMapCaptureScreen(std::optional<HWND> hWnd, CaptureSpace::ScreenCapture::InitScreenCapture& isccap, CaptureSpace::h_DC & hdcscreen, CaptureSpace::h_compatdc & hdccompat);

        static std::optional<UINT> GetWindowState(HWND hWnd);

    public:

        static bool SaveScreenCapture(HBITMAP* hbitmap_ptr, const std::variant<std::monostate, int, std::string>& varinput);

        static HBITMAP WindowScreenCapture(HWND hWnd);

        static HBITMAP FullScreenCapture();
    };
}
