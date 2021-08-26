#ifndef INPUT_HANDLE_H
#define INPUT_HANDLE_H

#include <Windows.h>
#include "search.h"
#include <fcntl.h>
#include <io.h>
#include <stdio.h>

namespace InputHandle {

	[[nodiscard]] int input_processing() noexcept {
		static int init{};
		static int pipe{};
		static HANDLE inh;
		DWORD dw;
		if (!init) {
			init = 1;
			inh = GetStdHandle(STD_INPUT_HANDLE);
			pipe = !GetConsoleMode(inh, &dw);
			if (!pipe) {
				SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
				FlushConsoleInputBuffer(inh);
			}
		}
		if (pipe) {
			if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
			return dw;
		}
		else {
			GetNumberOfConsoleInputEvents(inh, &dw);
			return dw <= 1 ? 0 : dw;
		}
	}

	void read_input(Search::SearchInformations& si) noexcept {
		int bytes{};
		char input[256] = "";
		char* endc;
		if (input_processing()) {
			si.m_stopped = true;
			do {
				bytes = _read(_fileno(stdin), input, 256);
			} while (bytes < 0);
			endc = strchr(input, '\n');
			if (endc) *endc = 0;
			if (strlen(input) > 0) {
				if (!strncmp(input, "quit", 4)) {
					si.m_quit = true;
				}
			}
			return;
		}
	}
}

#endif