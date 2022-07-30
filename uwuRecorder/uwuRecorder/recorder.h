#pragma once
#include <windows.h>
#include <thread>
#include <vector>
#include <chrono>

namespace Clicker {
	void send_lclick();
	bool is_cursor_visible();
}

namespace Recorder {
	extern bool replay_shift;
	extern bool replay_smartmode;
	extern bool recording;

	extern bool do_record_clicks;
	extern bool do_replay_clicks;

	extern std::vector<int> imported_clicks;
	extern std::vector<int> recorded_clicks;

	extern int num_clicks;
	extern float multiplier;

	void record_clicks();
	void replay_clicks();
}