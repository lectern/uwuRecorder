#include "recorder.h" 

bool Recorder::do_record_clicks;
bool Recorder::do_replay_clicks;
bool Recorder::recording = false;

bool Recorder::replay_shift = false;
bool Recorder::replay_smartmode = false;

std::vector<int> Recorder::imported_clicks = {};
std::vector<int> Recorder::recorded_clicks = {};

int Recorder::num_clicks = 0;
float Recorder::multiplier = 1;

auto last_click = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

auto get_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void Clicker::send_lclick() {
    HWND minecraft = FindWindowA("LWJGL", nullptr);

    if (GetForegroundWindow() == minecraft) {
        SendMessage(minecraft, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(0, 0));
        SendMessage(minecraft, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));
    }
}

// thanks to https://github.com/b1scoito/clicker
bool Clicker::is_cursor_visible() {
    CURSORINFO ci{ sizeof(CURSORINFO) };
    if (!GetCursorInfo(&ci))
        return false;

    const auto handle = ci.hCursor;
    if ((handle > (HCURSOR)50000) && (handle < (HCURSOR)100000))
        return true;

    return false;
}

void Recorder::record_clicks() {
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
            recorded_clicks.push_back(current_click - last_click);
            num_clicks++;
            last_click = current_click;
        }
    }

    if (get_ms() - last_click >= 300) {
        recording = false;  // pause recording
        Sleep(50);
    }
}

void Recorder::replay_clicks() {

    srand(time(0));
    int click_index = rand() % (imported_clicks.size() - 1);

    while (do_replay_clicks) {
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

        if (GetAsyncKeyState(VK_LBUTTON) && shift_disable && smart_disable) {
            Sleep(cps / multiplier);
            Clicker::send_lclick();
        }
        else {
            Sleep(50);
        }
    }
}