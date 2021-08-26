#include "search.h"
#include "board.h"
#include "transposition_table.h"
#include "print.h"
#include "evaluation.h"
#include "move_generator.h"
#include "input_handle.h"

#include <chrono>

namespace Search {

	void stop(SearchInformations& info) noexcept {
		using namespace std::chrono;
		if (info.m_time_s && static_cast<int>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()) > info.m_stop_t) {
			info.m_stopped = true;
		}

		InputHandle::read_input(info);
	}

	void reset_searchinfo(Boards::Board& position, SearchInformations& info) noexcept {
		position.reset_search_heuristics();
		position.reset_killer_moves();
		position.reset_transp_table();
		position.set_ply(0);
		info.m_stopped = false;
		info.m_nodes = 0;
		info.m_fail_high = info.m_fail_high_first = 0.0;
	}

	void start_search(Boards::Board& position, SearchInformations& info) noexcept {
		reset_searchinfo(position, info);
		using namespace std::chrono;

		int bestmove{ Enums::INCORRECT_MOVE };
		for (int cdepth{ 1 }; cdepth <= info.m_depth; ++cdepth) {
			int max_score{ alpha_beta(position, info, -Enums::INF, Enums::INF, cdepth) };
			if (info.m_stopped) {
				break;
			}
			int pv{ TT::get_best_line(cdepth, position) };
			bestmove = position.pv_moves_index(0);
			std::cout << "info score cp " << max_score << " depth " << cdepth << " nodes " << info.m_nodes << " time "
				<< static_cast<int>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count()) - info.m_start_t << ' ';

			std::cout << "pv ";
			for (int index{}; index < pv; ++index) {
				std::cout << Print::get_stringmove(position.pv_moves()[index]) << "  ";
			}
			std::cout << std::endl;
			//std::cout << "Move ordering: " << info.m_fail_high_first / info.m_fail_high << std::endl;
		}
		std::cout << "bestmove " << Print::get_stringmove(bestmove) << '\n';
	}

	[[nodiscard]] int alpha_beta(Boards::Board& position, SearchInformations& info, int alpha, int beta, int depthleft, bool null_move_possible) noexcept {
		assert(position.board_check());
		if (depthleft == 0) {
			return quiescence(position, info, alpha, beta);
		}
		if ((info.m_nodes % 2053) == 0) {
			stop(info);
		}

		++info.m_nodes;
		if ((position.is_repetition() || position.move50_rule() >= 100) && position.ply()) {
			return Enums::DRAW;
		}
		if (position.ply() > Cnst::MAX_DEPTH - 1) {
			return Eval::evaluate_pos(position);
		}

		bool square_attacked{ position.is_square_attacked(position.king_location_index(position.turn()), position.turn() ^ 1) };
		if (square_attacked) ++depthleft;

		int score{ -Enums::INF };
		if (null_move_possible && !square_attacked && position.ply() && depthleft >= 4 && (position.big_pieces_index(position.turn()) > 0)) {
			position.make_nullmove();
			score = -alpha_beta(position, info, -beta, -beta + 1, depthleft - 4, false);
			position.unmake_nullmove();
			if (info.m_stopped) return 0;
			if (score >= beta) return beta;
		}

		MovesList ml;
		ml.generate_moves(position);
		int temp_alpha{ alpha };
		int tot_legals{};
		int best_move{ Enums::INCORRECT_MOVE };

		int transpmove{ position.get_transposition_move() };
		if (transpmove != Enums::INCORRECT_MOVE) {
			for (int cmove{}; cmove < ml.get_tot_moves(); ++cmove) {
				if (ml[cmove].move == transpmove) {
					ml.set_score(cmove, Enums::TRANSP_MOVE); // principal variation move
					break;
				}
			}
		}

		int bestscore{ -Enums::INF };
		for (int cmove{}; cmove < ml.get_tot_moves(); ++cmove) {
			choose_bestmove(cmove, ml);
			if (!position.make_move(ml[cmove].move)) continue;
			++tot_legals;
			int curr_score{ -alpha_beta(position, info, -beta, -alpha, depthleft - 1) };
			position.unmake_move();
			if (info.m_stopped) {
				return 0; // Stop right away and ignore everything in the current search
			}
			if (curr_score > bestscore) {
				bestscore = curr_score;
				best_move = ml[cmove].move;

				if (curr_score > alpha) {
					if (curr_score >= beta) {
						if (tot_legals == 1) ++info.m_fail_high_first;
						++info.m_fail_high;
						if (!(ml[cmove].move & MoveUtils::capture)) {
							position.set_killers(1, position.ply(), position.killer_moves_index(0, position.ply()));
							position.set_killers(0, position.ply(), ml[cmove].move);

						}
						position.set_transposition_move(best_move);
						return beta;
					}
					alpha = curr_score;
					best_move = ml[cmove].move;
					if (!(ml[cmove].move & MoveUtils::capture)) {
						position.incr_search_heuristic_val(position.pieces_index(MoveUtils::get_from(best_move)), MoveUtils::get_to(best_move), depthleft);
					}
				}
			}
		}

		if (tot_legals == 0) { // No legal moves made: King is in check 
			if (square_attacked) {
				return (-Enums::MATE) + position.ply();
			}
			else return Enums::DRAW; // Stalemate
		}

		// Alpha is now better
		if (alpha != temp_alpha) {
			position.set_transposition_move(best_move);
		}
		else {
			position.set_transposition_move(best_move);
		}

		return alpha;
	}

	// order moves in the best-move table
	constexpr void choose_bestmove(int moven, MovesList& ml) noexcept {
		int best_score{};
		int moven_best{ moven };
		for (int curr{ moven + 1 }; curr < ml.get_tot_moves(); ++curr) {
			if (ml[curr].score > best_score) {
				best_score = ml[curr].score;
				moven_best = curr;
			}
		}
		MovesList::Move tmp_move = ml[moven];
		ml[moven] = ml[moven_best];
		ml[moven_best] = tmp_move;
	}

	[[nodiscard]] int quiescence(Boards::Board& position, SearchInformations& info, int alpha, int beta) noexcept {
		if ((info.m_nodes % 2053) == 0) {
			stop(info);
		}
		++info.m_nodes;
		if ((position.is_repetition() || position.move50_rule() >= 100) && position.ply()) {
			return Enums::DRAW;
		}

		if (position.ply() > Cnst::MAX_DEPTH - 1) {
			return Eval::evaluate_pos(position);
		}

		int score{ Eval::evaluate_pos(position) };
		if (score >= beta) {
			return beta;
		}
		if (score > alpha) {
			alpha = score;
		}

		MovesList ml;
		ml.gen_captures_only(position);
		int temp_alpha{ alpha };
		int tot_legals{};
		int best_move{ Enums::INCORRECT_MOVE };
		score = -Enums::INF;

		for (int cmove{}; cmove < ml.get_tot_moves(); ++cmove) {
			choose_bestmove(cmove, ml);
			if (!position.make_move(ml[cmove].move)) continue;
			++tot_legals;
			score = -quiescence(position, info, -beta, -alpha);
			position.unmake_move();
			if (info.m_stopped) {
				return 0; // Stop right away and ignore everything in the current search
			}
			if (score > alpha) {
				if (score >= beta) {
					if (tot_legals == 1) ++info.m_fail_high_first;
					++info.m_fail_high;
					return beta;
				}
				alpha = score;
				best_move = ml[cmove].move;
			}
		}

		if (alpha != temp_alpha) {
			position.set_transposition_move(best_move);
		}

		return alpha;
	}
}