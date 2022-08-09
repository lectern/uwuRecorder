#include "recorder.h"

auto last_click = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

auto get_ms() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

namespace Clicker {

    bool break_blocks = false;
    HWND minecraft = FindWindowA("LWJGL", nullptr);

    void send_lclick() {
        HWND minecraft = FindWindowA("LWJGL", nullptr);

        if (GetForegroundWindow() == minecraft) {
            SendMessage(minecraft, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(0, 0));
            if (!break_blocks) {
                SendMessage(minecraft, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));
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

    void record_clicks() {
        HWND minecraft = FindWindowA("LWJGL", nullptr);
        Sleep(1);

        if (GetAsyncKeyState(VK_LBUTTON) && (GetForegroundWindow() == minecraft) && !Clicker::is_cursor_visible()) {
            mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

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

        if (get_ms() - last_click >= 300000000) {
            recording = false;  // pause recording
            Sleep(50);
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
                //current_time = get_ms();
                
                
                //std::cout << (int)(cps / (multiplier)) << "\n";
                //std::cout << skip_time << "\n";
                std::this_thread::sleep_for(std::chrono::nanoseconds( (int)((cps - skip_time) / multiplier)) );
                
                Clicker::send_lclick();
            }
            else {
                if (Clicker::break_blocks) {
                    SendMessage(Clicker::minecraft, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));
                }
                Sleep(50);
            }
        }
    }
}