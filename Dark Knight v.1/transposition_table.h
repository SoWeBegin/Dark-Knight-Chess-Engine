#ifndef TRANSPOSITION_TABLE
#define TRANSPOSITION_TABLE

#include "utilities.h"
#include <cstdint>
#include <vector>


namespace Boards {
	class Board;
}


namespace TT {
	// Stores all information related to the transposition table
	enum Flags { NONE, ALPHA, BETA, EXACT };
	struct InfoTT {
		FAST_64UI key{};
		int depth{};
		int best_move{};
		int flags{};
		int score{};

		constexpr void reset() noexcept {
			key = {};
			depth = best_move = flags = score = {};
		}
	};


	class TranspositionTable {
	private:
		std::vector<InfoTT> transposition_table{};
		int tot_entries_cap{};

	public:
		TranspositionTable(int size = 16) noexcept;

		void init(int new_size) noexcept;

		void set(int hashed_index, int best_move, FAST_64UI key, int depth) noexcept;

		// Getters
		[[nodiscard]] __forceinline int get_size() const noexcept {
			return tot_entries_cap;
		}

		[[nodiscard]] __forceinline FAST_64UI get_key(int index) const noexcept {
			return transposition_table[index].key;
		}

		[[nodiscard]] __forceinline int get_bestmove(int index) const noexcept {
			return transposition_table[index].best_move;
		}

		void reset_internal() noexcept {
			transposition_table = {};
		}

		constexpr auto& get_transposition_table() const noexcept { return transposition_table; }
	}; // TranspositionTable

	[[nodiscard]] int get_best_line(int depth, Boards::Board& pos) noexcept;

}

#endif