#include "transposition_table.h"
#include "board.h"
#include "move_generator.h"
#include "bitboard.h"

namespace TT {
	[[nodiscard]] int get_best_line(int depth, Boards::Board& pos) noexcept {
		int index{};
		MovesList movelist;
		int currmove{ pos.get_transposition_move() };


		while (currmove != Enums::INCORRECT_MOVE && index < depth) {
			if (movelist.move_check(pos, currmove)) { // Move's legal
				pos.make_move(currmove);
				pos.set_pv_moves(index++, currmove);
			}
			else break;

			currmove = pos.get_transposition_move();
		}

		while (pos.ply() > 0) {
			pos.unmake_move();
		}
		return index;
	}

	TranspositionTable::TranspositionTable(int size) noexcept {
		tot_entries_cap = size * (1048576) / sizeof(InfoTT); // conversion to megabytes + how many entries can fit in that size
		std::size_t tot_cap{ tot_entries_cap * sizeof(InfoTT) };
		transposition_table.reserve(tot_cap);
		transposition_table.resize(tot_cap);
	}

	void TranspositionTable::init(int new_size) noexcept {
		tot_entries_cap = new_size * 1048576 / sizeof(InfoTT);
		std::size_t tot_cap{ tot_entries_cap * sizeof(InfoTT) };
		transposition_table.clear();
		transposition_table.reserve(tot_cap);
		transposition_table.resize(tot_cap);
	}

	void TranspositionTable::set(int hashed_index, int best_move, FAST_64UI key, int depth) noexcept {
		transposition_table[hashed_index].best_move = best_move;
		transposition_table[hashed_index].key = key;
		transposition_table[hashed_index].depth = depth;
	}

}