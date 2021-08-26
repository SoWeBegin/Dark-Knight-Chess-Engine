#include "evaluation.h"
#include "board.h"
#include "bitboard.h"
#include "nnue.h"


#include "print.h"
#include <iostream>
namespace Eval {

	// Stockfish NNUE
	constexpr inline std::array<int, 13> nnue_pieces{ 0, 6, 5, 4, 3, 2, 1, 12, 11, 10, 9, 8, 7 };

	[[nodiscard]] int evaluate_pos(const Boards::Board& board) noexcept {
		int pieces[33];
		int squares[33];

		int index{ 2 };
		for (int piece{ Enums::W_PAWN }; piece < Cnst::TOT_PIECE_TYPE; ++piece) {
			for (int cpiece{ 0 }; cpiece < board.pieces_left_index(piece); ++cpiece) {
				if (piece == Enums::W_KING) {
					pieces[0] = nnue_pieces[piece];
					squares[0] = Boards::Board::squares120[board.pieces_list_index(piece, cpiece)];
				}

				else if (piece == Enums::B_KING) {
					pieces[1] = nnue_pieces[piece];
					squares[1] = Boards::Board::squares120[board.pieces_list_index(piece, cpiece)];
				}

				else {
					pieces[index] = nnue_pieces[piece];
					squares[index] = Boards::Board::squares120[board.pieces_list_index(piece, cpiece)];
					++index;
				}
			}
		}

		pieces[index] = 0;
		squares[index] = 0;

		return nnue_evaluate(board.turn(), pieces, squares) * (100 - board.move50_rule()) / 100;
	}
}