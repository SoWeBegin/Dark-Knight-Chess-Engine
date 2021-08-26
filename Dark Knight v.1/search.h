#ifndef SEARCH_H
#define SEARCH_H
#include "utilities.h"
#include <cstdint>

namespace Boards {
	class Board;
}

class MovesList;

namespace Search {
	struct SearchInformations {
		double m_fail_high{};
		double m_fail_high_first{};
		int64_t m_nodes{};
		int m_start_t{};
		int m_stop_t{};
		int m_depth{};
		int m_depth_s{};
		int m_moves_togo{};
		bool m_time_s{};
		bool m_quit{};
		bool m_stopped{};
		bool m_infinite{};

		constexpr void reset() noexcept {
			m_start_t = m_stop_t =/* m_depth =*/ m_depth_s = m_time_s = m_moves_togo = 0;
			m_fail_high = m_fail_high_first = 0.0;
			m_quit = m_stopped = m_infinite = false;
		}
	};

	void start_search(Boards::Board& position, SearchInformations& info) noexcept;

	void reset_searchinfo(Boards::Board& position, SearchInformations& info) noexcept;

	[[nodiscard]] int alpha_beta(Boards::Board& position, SearchInformations& info, int alpha, int beta, int depthleft, bool null_move_possible = true) noexcept;

	void stop(SearchInformations& info) noexcept;

	[[nodiscard]] int quiescence(Boards::Board& position, SearchInformations& info, int alpha, int beta) noexcept;

	constexpr void choose_bestmove(int moven, MovesList& ml) noexcept;
}


#endif