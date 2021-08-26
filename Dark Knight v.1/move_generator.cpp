#include "move_generator.h"
#include "bitboard.h"

void MovesList::gen_blackpawnmoves(const Board& position, bool quiet) noexcept {
	constexpr int forward{ -10 };
	constexpr int diag_right{ -11 };
	constexpr int diag_left{ -9 };
	constexpr int color{ Enums::B_PAWN };
	constexpr int initial_rank{ Enums::RANK7 };

	for (int currpiece{}; currpiece < position.pieces_left_index(color); ++currpiece) {
		int square{ position.pieces_list_index(color, currpiece) };

		if (position.pieces_index(square + forward) == Enums::NO_PIECE) { // pawn can march forward
			set_blackpawn_move(position, MoveUtils::gen_singlemove(square, square + forward), quiet);
			auto doublepawnmove = square + (forward * 2);
			if (position.get_rank[square] == initial_rank && position.pieces_index(doublepawnmove) == Enums::NO_PIECE) { // Two-moves forward (init pawn jump)
				set_quietmove(position, MoveUtils::gen_singlemove(square, doublepawnmove, Enums::NO_PIECE, Enums::NO_PIECE, MoveUtils::pawnfirst));
			}
		}

		// Captures diagonally
		if (Board::square_exists(square + diag_left) &&
			PieceInfo::get_piece_color(position.pieces_index(square + diag_left)) == Enums::WHITE) {
			set_blackpawn_move(position, MoveUtils::gen_singlemove(square, square + diag_left, position.pieces_index(square + diag_left)));
		}

		if (Board::square_exists(square + diag_right) &&
			PieceInfo::get_piece_color(position.pieces_index(square + diag_right)) == Enums::WHITE) {
			set_blackpawn_move(position, MoveUtils::gen_singlemove(square, square + diag_right, position.pieces_index(square + diag_right)));
		}

		auto ep_square{ position.enPassant_square() };
		// Enpassant captures
		if (ep_square != Enums::NO_SQUARE) {
			if (square + diag_left == ep_square) {
				set_epmove(position, MoveUtils::gen_singlemove(square, square + diag_left, Enums::NO_PIECE, Enums::NO_PIECE, MoveUtils::enpassant));
			}

			if (square + diag_right == ep_square) {
				set_epmove(position, MoveUtils::gen_singlemove(square, square + diag_right, Enums::NO_PIECE, Enums::NO_PIECE, MoveUtils::enpassant));
			}
		}
	}
}

void MovesList::gen_blackpawnmoves_capt(const Board& position) noexcept {
	constexpr int diag_right{ -11 };
	constexpr int diag_left{ -9 };
	constexpr int color{ Enums::B_PAWN };

	for (int currpiece{}; currpiece < position.pieces_left_index(color); ++currpiece) {
		int square{ position.pieces_list_index(color, currpiece) };

		if (Board::square_exists(square + diag_left) &&
			PieceInfo::get_piece_color(position.pieces_index(square + diag_left)) == Enums::WHITE) {
			set_blackpawn_move(position, MoveUtils::gen_singlemove(square, square + diag_left, position.pieces_index(square + diag_left)));
		}

		if (Board::square_exists(square + diag_right) &&
			PieceInfo::get_piece_color(position.pieces_index(square + diag_right)) == Enums::WHITE) {
			set_blackpawn_move(position, MoveUtils::gen_singlemove(square, square + diag_right, position.pieces_index(square + diag_right)));
		}

		auto ep_square{ position.enPassant_square() };
		if (ep_square != Enums::NO_SQUARE) {
			if (square + diag_left == ep_square) {
				set_epmove(position, MoveUtils::gen_singlemove(square, square + diag_left, Enums::NO_PIECE, Enums::NO_PIECE, MoveUtils::enpassant));
			}

			if (square + diag_right == ep_square) {
				set_epmove(position, MoveUtils::gen_singlemove(square, square + diag_right, Enums::NO_PIECE, Enums::NO_PIECE, MoveUtils::enpassant));
			}
		}
	}
}

void MovesList::gen_whitepawnmove(const Board& position, bool quiet) noexcept {
	constexpr int forward{ 10 };
	constexpr int diag_right{ 11 };
	constexpr int diag_left{ 9 };
	constexpr int color{ Enums::W_PAWN };
	constexpr int initial_rank{ Enums::RANK2 };

	for (int currpiece{}; currpiece < position.pieces_left_index(color); ++currpiece) {
		int square{ position.pieces_list_index(color, currpiece) };
		if (position.pieces_index(square + forward) == Enums::NO_PIECE) { // pawn can march forward
			set_whitepawn_move(position, MoveUtils::gen_singlemove(square, square + forward), quiet);
			if (position.get_rank[square] == initial_rank && position.pieces_index(square + (forward + forward)) == Enums::NO_PIECE) { // Two-moves forward (init pawn jump)
				set_quietmove(position, MoveUtils::gen_singlemove(square, square + (forward + forward), Enums::NO_PIECE, Enums::NO_PIECE, MoveUtils::pawnfirst));
			}
		}

		// Captures diagonally
		if (Board::square_exists(square + diag_left) &&
			PieceInfo::get_piece_color(position.pieces_index(square + diag_left)) == Enums::BLACK) {
			set_whitepawn_move(position, MoveUtils::gen_singlemove(square, square + diag_left, position.pieces_index(square + diag_left)));
		}

		if (Board::square_exists(square + diag_right) &&
			PieceInfo::get_piece_color(position.pieces_index(square + diag_right)) == Enums::BLACK) {
			set_whitepawn_move(position, MoveUtils::gen_singlemove(square, square + diag_right, position.pieces_index(square + diag_right)));
		}

		// Enpassant captures
		auto ep{ position.enPassant_square() };
		if (ep != Enums::NO_SQUARE) {
			if (square + diag_left == ep) {
				set_epmove(position, MoveUtils::gen_singlemove(square, square + diag_left, Enums::NO_PIECE, Enums::NO_PIECE, MoveUtils::enpassant));
			}
			if (square + diag_right != 99 && square + diag_right == ep) {
				set_epmove(position, MoveUtils::gen_singlemove(square, square + diag_right, Enums::NO_PIECE, Enums::NO_PIECE, MoveUtils::enpassant));
			}
		}
	}
}

void MovesList::gen_whitepawnmoves_capt(const Board& position) noexcept {
	constexpr int diag_right{ 11 };
	constexpr int diag_left{ 9 };
	constexpr int color{ Enums::W_PAWN };

	for (int currpiece{}; currpiece < position.pieces_left_index(color); ++currpiece) {
		int square{ position.pieces_list_index(color, currpiece) };

		if (Board::square_exists(square + diag_left) &&
			PieceInfo::get_piece_color(position.pieces_index(square + diag_left)) == Enums::BLACK) {
			set_whitepawn_move(position, MoveUtils::gen_singlemove(square, square + diag_left, position.pieces_index(square + diag_left)));
		}

		if (Board::square_exists(square + diag_right) &&
			PieceInfo::get_piece_color(position.pieces_index(square + diag_right)) == Enums::BLACK) {
			set_whitepawn_move(position, MoveUtils::gen_singlemove(square, square + diag_right, position.pieces_index(square + diag_right)));
		}

		auto ep{ position.enPassant_square() };
		if (ep != Enums::NO_SQUARE) {
			if (square + diag_left == ep) {
				set_epmove(position, MoveUtils::gen_singlemove(square, square + diag_left, Enums::NO_PIECE, Enums::NO_PIECE, MoveUtils::enpassant));
			}
			if (square + diag_right != 99 && square + diag_right == ep) {
				set_epmove(position, MoveUtils::gen_singlemove(square, square + diag_right, Enums::NO_PIECE, Enums::NO_PIECE, MoveUtils::enpassant));
			}
		}
	}
}

void MovesList::gen_black_castle_moves(const Board& position) noexcept {
	if ((position.castle_rights() & Enums::KING_SIDE_B) && position.pieces_index(Enums::F8) == Enums::NO_PIECE && position.pieces_index(Enums::G8) == Enums::NO_PIECE
		&& !position.is_square_attacked(Enums::E8, Enums::WHITE) && !position.is_square_attacked(Enums::F8, Enums::WHITE)) {
		set_quietmove(position, MoveUtils::gen_singlemove(Enums::E8, Enums::G8, Enums::NO_PIECE, Enums::NO_PIECE, MoveUtils::castle));
	}

	if ((position.castle_rights() & Enums::QUEEN_SIDE_B) && position.pieces_index(Enums::D8) == Enums::NO_PIECE && position.pieces_index(Enums::C8) == Enums::NO_PIECE && position.pieces_index(Enums::B8) == Enums::NO_PIECE
		&& !position.is_square_attacked(Enums::E8, Enums::WHITE) && !position.is_square_attacked(Enums::D8, Enums::WHITE)) {
		set_quietmove(position, MoveUtils::gen_singlemove(Enums::E8, Enums::C8, Enums::NO_PIECE, Enums::NO_PIECE, MoveUtils::castle));
	}
}

void MovesList::gen_white_castle_moves(const Board& position) noexcept {
	// King side castle available
	// No pieces between king-rook
	// no squares between king-rook attacked
	if ((position.castle_rights() & Enums::KING_SIDE_W) && position.pieces_index(Enums::F1) == Enums::NO_PIECE && position.pieces_index(Enums::G1) == Enums::NO_PIECE
		&& !position.is_square_attacked(Enums::E1, Enums::BLACK) && !position.is_square_attacked(Enums::F1, Enums::BLACK)) {
		set_quietmove(position, MoveUtils::gen_singlemove(Enums::E1, Enums::G1, Enums::NO_PIECE, Enums::NO_PIECE, MoveUtils::castle));
	}

	if ((position.castle_rights() & Enums::QUEEN_SIDE_W) && position.pieces_index(Enums::D1) == Enums::NO_PIECE && position.pieces_index(Enums::C1) == Enums::NO_PIECE
		&& position.pieces_index(Enums::B1) == Enums::NO_PIECE && !position.is_square_attacked(Enums::E1, Enums::BLACK) && !position.is_square_attacked(Enums::D1, Enums::BLACK)) {
		set_quietmove(position, MoveUtils::gen_singlemove(Enums::E1, Enums::C1, Enums::NO_PIECE, Enums::NO_PIECE, MoveUtils::castle));
	}
}

void MovesList::gen_slide_moves(const Board& position, std::size_t piece_index) noexcept {
	int piece{ PieceInfo::loop_slide[piece_index++] };

	while (piece != Cnst::ZERO_PIECES) {
		for (int currpiece{}; currpiece < position.pieces_left_index(piece); ++currpiece) {
			int square{ position.pieces_list_index(piece, currpiece) };

			for (int index{}; index < PieceInfo::get_tot_direction(piece); ++index) {
				int direction{ PieceInfo::get_direction(piece, index) };
				int temp_square{ square + direction };

				while (Board::square_exists(temp_square)) {
					if (position.pieces_index(temp_square) != Enums::NO_PIECE) {
						if (PieceInfo::get_piece_color(position.pieces_index(temp_square)) == (position.turn() ^ 1)) {
							set_tacticalmove(position, MoveUtils::gen_singlemove(square, temp_square, position.pieces_index(temp_square)));
						}
						break;
					}
					set_quietmove(position, MoveUtils::gen_singlemove(square, temp_square));
					temp_square += direction;
				}
			}
		}
		piece = PieceInfo::loop_slide[piece_index++];
	}
}

void MovesList::gen_slide_moves_capt(const Board& position, std::size_t piece_index) noexcept {
	int piece{ PieceInfo::loop_slide[piece_index++] };

	while (piece != Cnst::ZERO_PIECES) {
		for (int currpiece{}; currpiece < position.pieces_left_index(piece); ++currpiece) {
			int square{ position.pieces_list_index(piece, currpiece) };

			for (int index{}; index < PieceInfo::get_tot_direction(piece); ++index) {
				int direction{ PieceInfo::get_direction(piece, index) };
				int temp_square{ square + direction };

				while (Board::square_exists(temp_square)) {
					if (position.pieces_index(temp_square) != Enums::NO_PIECE) {
						if (PieceInfo::get_piece_color(position.pieces_index(temp_square)) == (position.turn() ^ 1)) {
							set_tacticalmove(position, MoveUtils::gen_singlemove(square, temp_square, position.pieces_index(temp_square)));
						}
						break;
					}
					temp_square += direction;
				}
			}
		}
		piece = PieceInfo::loop_slide[piece_index++];
	}
}

void MovesList::gen_nonslide_moves(const Board& position, std::size_t piece_index) noexcept {
	int piece{ PieceInfo::loop_nonslide[piece_index++] };
	auto pieces_left{ position.pieces_left() };
	auto pieces_list{ position.pieces_list() };
	auto pieces{ position.pieces() };

	while (piece != Cnst::ZERO_PIECES) {
		for (int currpiece{}; currpiece < pieces_left[piece]; ++currpiece) {
			int square{ pieces_list[piece][currpiece] };

			for (int index{}; index < PieceInfo::get_tot_direction(piece); ++index) {
				int direction{ PieceInfo::get_direction(piece, index) };
				int temp_square{ square + direction };

				if (!Board::square_exists(temp_square)) continue;
				if (pieces[temp_square] != Enums::NO_PIECE) {
					// There's a piece on the generated move -> capture
					if (PieceInfo::get_piece_color(pieces[temp_square]) == (position.turn() ^ 1)) {
						set_tacticalmove(position, MoveUtils::gen_singlemove(square, temp_square, pieces[temp_square]));
					}
					continue;
				}
				set_quietmove(position, MoveUtils::gen_singlemove(square, temp_square));
			}
		}
		piece = PieceInfo::loop_nonslide[piece_index++];
	}
}

void MovesList::gen_nonslide_moves_capt(const Board& position, std::size_t piece_index) noexcept {
	int piece{ PieceInfo::loop_nonslide[piece_index++] };
	auto pieces_left{ position.pieces_left() };
	auto pieces_list{ position.pieces_list() };
	auto pieces{ position.pieces() };

	while (piece != Cnst::ZERO_PIECES) {
		for (int currpiece{}; currpiece < pieces_left[piece]; ++currpiece) {
			int square{ pieces_list[piece][currpiece] };

			for (int index{}; index < PieceInfo::get_tot_direction(piece); ++index) {
				int direction{ PieceInfo::get_direction(piece, index) };
				int temp_square{ square + direction };

				if (!Board::square_exists(temp_square)) continue;
				if (pieces[temp_square] != Enums::NO_PIECE) {
					// There's a piece on the generated move -> capture
					if (PieceInfo::get_piece_color(pieces[temp_square]) == (position.turn() ^ 1)) {
						set_tacticalmove(position, MoveUtils::gen_singlemove(square, temp_square, pieces[temp_square]));
					}
					continue;
				}
			}
		}
		piece = PieceInfo::loop_nonslide[piece_index++];
	}
}

void MovesList::set_whitepawn_move(const Boards::Board& position, int move, bool quiet) noexcept {
	assert(Board::piece_exists(MoveUtils::get_capt(move)) && Board::square_exists(MoveUtils::get_from(move))
		&& Board::square_exists(MoveUtils::get_to(move)));
	auto cap{ MoveUtils::get_capt(move) };
	auto to{ MoveUtils::get_to(move) };
	auto from{ MoveUtils::get_from(move) };

	if (quiet) {
		if (Board::get_rank[from] == Enums::RANK7) { // Move is promotion
			set_quietmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::W_QUEEN));
			set_quietmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::W_ROOK));
			set_quietmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::W_BISHOP));
			set_quietmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::W_KNIGHT));
		}
		else {
			set_quietmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::NO_PIECE));
		}
	}

	else {
		if (Board::get_rank[from] == Enums::RANK7) { // Move is promotion
			set_tacticalmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::W_QUEEN));
			set_tacticalmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::W_ROOK));
			set_tacticalmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::W_BISHOP));
			set_tacticalmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::W_KNIGHT));
		}
		else {
			set_tacticalmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::NO_PIECE));
		}
	}
}

void MovesList::set_blackpawn_move(const Boards::Board& position, int move, bool quiet) noexcept {
	assert(Board::piece_exists(MoveUtils::get_capt(move)) && Board::square_exists(MoveUtils::get_from(move))
		&& Board::square_exists(MoveUtils::get_to(move)));
	auto cap{ MoveUtils::get_capt(move) };
	auto to{ MoveUtils::get_to(move) };
	auto from{ MoveUtils::get_from(move) };

	if (quiet) {
		if (Board::get_rank[from] == Enums::RANK2) { // Move is promotion
			set_quietmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::B_QUEEN));
			set_quietmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::B_ROOK));
			set_quietmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::B_BISHOP));
			set_quietmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::B_KNIGHT));
		}
		else {
			set_quietmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::NO_PIECE));
		}
	}
	else {
		if (Board::get_rank[from] == Enums::RANK2) { // Move is promotion
			set_tacticalmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::B_QUEEN));
			set_tacticalmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::B_ROOK));
			set_tacticalmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::B_BISHOP));
			set_tacticalmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::B_KNIGHT));
		}
		else {
			set_tacticalmove(position, MoveUtils::gen_singlemove(from, to, cap, Enums::NO_PIECE));
		}
	}
}

void MovesList::generate_moves(const Board& position) noexcept {
	assert(position.board_check());
	tot_moves = 0;
	int turn{ position.turn() };

	if (turn == Enums::WHITE) {
		gen_whitepawnmove(position);
		gen_white_castle_moves(position);
		gen_slide_moves(position, 0);
		gen_nonslide_moves(position, 0);
	}
	else {
		gen_blackpawnmoves(position);
		gen_black_castle_moves(position);
		gen_slide_moves(position, 4);
		gen_nonslide_moves(position, 3);
	}
}

void MovesList::gen_captures_only(const Board& position) noexcept {
	tot_moves = 0;
	if (position.turn() == Enums::WHITE) {
		gen_whitepawnmoves_capt(position);
		gen_slide_moves_capt(position, 0);
		gen_nonslide_moves_capt(position, 0);
	}
	else {
		gen_blackpawnmoves_capt(position);
		gen_slide_moves_capt(position, 4);
		gen_nonslide_moves_capt(position, 3);
	}
}

[[nodiscard]] bool MovesList::move_check(Boards::Board& board, int move) noexcept {
	// Sanity check in case two positions have the same zobrist key
	generate_moves(board);
	for (int curr{}; curr < tot_moves; ++curr) {
		if (!board.make_move(all_moves[curr].move)) continue;
		board.unmake_move();
		if (all_moves[curr].move == move) return true;
	}
	return false;
}


