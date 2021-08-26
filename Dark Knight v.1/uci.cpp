#include "uci.h"
#include "board.h"
#include "search.h"
#include "print.h"
#include "evaluation.h"

#include <iostream>
#include <chrono>

#define START_POS "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0 "

namespace UCI {
	void go_parser(const std::string& str, Search::SearchInformations& info, Boards::Board& pos) noexcept {
		int depth{ Enums::NOT_SET };
		int movet{ Enums::NOT_SET };
		int time{ Enums::NOT_SET };
		int moves2go{ 30 };

		int found = str.find("infinite");
		if (found != std::string::npos) {
			info.m_time_s = false;
		}

		found = str.find("binc");
		int inc{};
		if (found != std::string::npos && pos.turn() == Enums::BLACK) {
			std::string temp_str(str.begin() + found + 5, str.end());
			inc = stoi(temp_str);
		}

		found = str.find("winc");
		if (found != std::string::npos && pos.turn() == Enums::WHITE) {
			std::string temp_str(str.begin() + found + 5, str.end());
			inc = stoi(temp_str);
		}

		found = str.find("wtime");
		if (found != std::string::npos && pos.turn() == Enums::WHITE) {
			std::string temp_str(str.begin() + found + 6, str.end());
			time = stoi(temp_str);
		}

		found = str.find("btime");
		if (found != std::string::npos && pos.turn() == Enums::BLACK) {
			std::string temp_str(str.begin() + found + 6, str.end());
			time = stoi(temp_str);
		}

		found = str.find("movestogo");
		if (found != std::string::npos) {
			std::string temp_str(str.begin() + found + 10, str.end());
			moves2go = stoi(temp_str);
		}

		found = str.find("movetime");
		if (found != std::string::npos) {
			std::string temp_str(str.begin() + found + 9, str.end());
			movet = stoi(temp_str);
		}

		found = str.find("depth");
		if (found != std::string::npos) {
			std::string temp_str(str.begin() + found + 6, str.end());
			depth = stoi(temp_str);
		}

		if (movet != Enums::NOT_SET) {
			time = movet;
			moves2go = 1;
		}
		using namespace std::chrono;
		info.m_start_t = static_cast<int>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
		info.m_depth = depth;

		if (time != Enums::NOT_SET) {
			info.m_time_s = true;
			time /= moves2go;
			time -= 50;
			info.m_stop_t = info.m_start_t + time + inc;
		}

		if (depth == Enums::NOT_SET) {
			info.m_depth = Cnst::MAX_DEPTH;
		}

		std::cout << "time: " << time << " start: " << info.m_start_t << " stop: " << info.m_stop_t << " depth: "
			<< info.m_depth << " timeset: " << info.m_time_s << '\n';
		Search::start_search(pos, info);
	}

	void position_parser(const std::string& str, Boards::Board& pos) noexcept {
		if (str.find("startpos") != std::string::npos) {
			pos.parse_fen(START_POS);
		}
		else {
			auto found{ str.find("fen") };
			if (found == std::string::npos) {
				pos.parse_fen(START_POS);
			}
			else {
				pos.parse_fen(std::string(str.begin() + 13, str.end()).c_str());
			}
		}

		if (str.find("moves") != std::string::npos) {
			std::string temp(str.begin() + str.find("moves") + 6, str.end());
			for (int index{}; index < temp.size(); ++index) {
				int move{ Print::parse_move(std::string(temp.begin() + index, temp.end()), pos) };
				if (move == Enums::INCORRECT_MOVE) break;
				pos.make_move(move);
				pos.set_ply(0);
				while (temp[index] != ' ') ++index;
			}
		}
		pos.print_board();
	}

	void uci() noexcept {
		std::string line{};
		std::cout << "id name Dark Knight\n";
		std::cout << "uciok\n";

		Boards::Board position;
		Search::SearchInformations si;
		position.transp_table_init(10);

		while (!si.m_quit) {
			if (!std::getline(std::cin, line) || line.empty()) {
				continue;
			}
			if (line == "isready") {
				std::cout << "readyok\n";
				continue;
			}
			else if (line.find("position") != std::string::npos) {
				position_parser(line, position);
			}
			else if (line == "ucinewgame") {
				std::string s{ "position startpos\\n" };
				position_parser(s, position);
			}
			else if (line.find("go") != std::string::npos) {
				go_parser(line, si, position);
			}
			else if (line == "quit") {
				si.m_quit = true;
				break;
			}
			else if (line == "uci") {
				std::cout << "id name Dark Knight\n";
				std::cout << "id author SoWeBegin\n";
				std::cout << "uciok\n";
			}

			else if (line == "evaluate") {
				auto score = Eval::evaluate_pos(position);
				std::cout << score << '\n';
			}

		}
	}
}