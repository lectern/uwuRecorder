#include "recorder.h"

auto last_click = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

auto get_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

//pasted from git-eternal lol
namespace nt
{
    NTSYSAPI NTSTATUS	NTAPI NtDelayExecution(
        _In_ BOOLEAN Alertable,
        _In_opt_ PLARGE_INTEGER DelayInterval);

    inline decltype(&NtDelayExecution) pNtDelayExecution{};

    // custom sleep function to fix broken randomization
    //
    __forceinline static void sleep(std::uint64_t delay_interval)
    {
        // lambda for getting NtDelayExecution pointer
        static auto grab_nt_delay_execution = [&]() -> bool
        {
            pNtDelayExecution = reinterpret_cast<decltype(pNtDelayExecution)>(
                GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtDelayExecution"));

            return true;
        };

        static auto _ = grab_nt_delay_execution();

        // set our periodic timer resolution to 1ms
        timeBeginPeriod(1);

        LARGE_INTEGER _delay_interval{};
        _delay_interval.QuadPart = -static_cast<LONGLONG>(delay_interval * static_cast<std::uint64_t>(10'000));

        pNtDelayExecution(false, &_delay_interval);

        // reset periodic timer resolution
        timeEndPeriod(1);
    }
}

namespace Clicker {

    bool break_blocks = false;
    HWND minecraft = FindWindowA("LWJGL", nullptr);

    void send_lclick() {
        HWND minecraft = FindWindowA("LWJGL", nullptr);
        //HWND minecraft = GetForegroundWindow();

        POINT pos{};
        if (!GetCursorPos(&pos))
            return;

        if (GetForegroundWindow() == minecraft) {
            PostMessage(minecraft, (DWORD)WM_LBUTTONDOWN, (DWORD)MK_LBUTTON, MAKELPARAM(pos.x, pos.y));
            if (!break_blocks) {
                PostMessage(minecraft, (DWORD)WM_LBUTTONUP, (DWORD)MK_LBUTTON, MAKELPARAM(0, 0));
            }
        }
    }

    // thanks to https://github.com/b1scoito/clicker
    bool is_cursor_visible() {
        CURSORINFO ci{ sizeof(CURSORINFO) };
        if (!GetCursorInfo(&ci))
            return false;

        const auto handle = ci.hCursor;
        if ((handle > (HCURSOR)50000) && (handle < (HCURSOR)100000))
            return true;

        return false;
    }
}

namespace Recorder {

    bool do_record_clicks;
    bool do_replay_clicks;
    bool recording = false;

    bool replay_shift = false;
    bool replay_smartmode = false;

    std::vector<int> imported_clicks = {};
    std::vector<int> recorded_clicks = {};

    int num_clicks = 0;
    float multiplier = 1.0;

    char record_bind;
    char replay_bind;

    void record_clicks() {
        HWND minecraft = FindWindowA("LWJGL", nullptr);
        nt::sleep(1);

        if (GetAsyncKeyState(VK_LBUTTON) && (GetForegroundWindow() == minecraft) && !Clicker::is_cursor_visible()) {
            //mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

            if (!recording) {
                last_click = get_ms();
                recording = true;
            }
            else if (recording) {
                auto current_click = get_ms();
                
                recorded_clicks.push_back((current_click - last_click));
                last_click = current_click;
                num_clicks++;
            }
        }

        if (get_ms() - last_click >= 300) {
            recording = false;  // pause recording
            nt::sleep(50);
        }
    }

    void replay_clicks() {

        srand(time(0));
        int click_index = rand() % (imported_clicks.size() - 1);
        auto current_time = get_ms();

        while (do_replay_clicks) {

            current_time = get_ms();

            srand(time(0));
            bool shift_disable = !(replay_shift && GetAsyncKeyState(VK_LSHIFT));
            bool smart_disable = !(replay_smartmode && Clicker::is_cursor_visible());

            int cps = imported_clicks[click_index];
            if (click_index == (imported_clicks.size() - 1)) {
                click_index = rand() % (imported_clicks.size() - 1);
            }
            else {
                click_index++;
            }

            if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) && shift_disable && smart_disable) {
                auto skip_time = get_ms() - current_time;

                nt::sleep((int)((cps - skip_time) / multiplier));
                
                Clicker::send_lclick();
            }
            else {
                if (Clicker::break_blocks) {
                    SendMessage(Clicker::minecraft, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));
                }
                nt::sleep(50);
            }
        }
    }
}