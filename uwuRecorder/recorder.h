#pragma once
#include <windows.h>
#include <thread>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>

#include <iostream>

#pragma comment(lib, "winmm.lib")

namespace nt {
	void sleep(std::uint64_t delay_interval);
}

namespace Clicker {
	extern bool break_blocks;
	extern HWND minecraft;

	void send_lclick();
	bool is_cursor_visible();
}

namespace Recorder {
	extern bool replay_shift;
	extern bool replay_smartmode;
	extern bool recording;

	extern bool do_record_clicks;
	extern bool do_replay_clicks;

	extern char record_bind;
	extern char replay_bind;

	extern std::vector<int> imported_clicks;
	extern std::vector<int> recorded_clicks;

	extern int num_clicks;
	extern float multiplier;

	void record_clicks();
	void replay_clicks();
}