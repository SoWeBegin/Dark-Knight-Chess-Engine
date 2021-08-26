#include "board.h"
#include "move_generator.h"
#include "boardKey_generator.h"
#include "piece_info.h"
#include "bitboard.h"

#include <cstdlib>
#include <iostream>


void Boards::Board::initialize_board64() noexcept {
	for (auto& curr_square : squares120) {
		curr_square = Cnst::OUT_OF_RANGE;
	}

	for (auto& curr_square64 : squares64) {
		curr_square64 = Cnst::OUT_OF_RANGE64;
	}

	int square64{};
	for (int rank{ Enums::RANK1 }; rank <= Enums::RANK8; ++rank) {
		for (int file{ Enums::A_FILE }; file <= Enums::H_FILE; ++file) {
			int square = Utils::getSquare64(file, rank);
			squares64[square64] = square;
			squares120[square] = square64++;
		}
	}
}

void Boards::Board::initialize_ranks_files() noexcept {
	int square{};
	while (square < Cnst::MAX_SQUARES) {
		get_rank[square] = get_file[square] = Enums::OFFBOARD;
		++square;
	}
	square = 0;
	for (int rank{ Enums::RANK1 }; rank <= Enums::RANK8; ++rank) {
		for (int file{ Enums::A_FILE }; file <= Enums::H_FILE; ++file) {
			square = Utils::getSquare64(file, rank);
			get_file[square] = file;
			get_rank[square] = rank;
		}
	}
}

void Boards::Board::board_reset() noexcept {
	// Reset king's positions
	m_king_location[Enums::WHITE] = m_king_location[Enums::BLACK] = Enums::NO_SQUARE;

	// Reset miscellaneous members
	m_ply = m_move50_rule = m_total_ply = m_castle_rights = m_total_moves = {};
	m_major_pieces = {};
	m_minor_pieces = {};
	m_big_pieces = {};
	m_turn = Enums::WHITE;
	m_enPassant_square = Enums::NO_SQUARE;
	m_position_key = {};
	m_material_count = {};
	m_pawns = {};
	m_pieces_left = {};

	// Set all initial squares to NO SQUARE, effectively "resets" each square
	for (auto& current_piece : m_pieces) {
		current_piece = Enums::OFFBOARD;
	}

	// Sets the actual (allowed) squares to NO_PIECE, so they can be differentiated from NO_SQUARE. No square means off the board; no piece means a valid 
	// square that contains no pieces.
	for (const auto& current_sq64 : squares64) {
		m_pieces[current_sq64] = Enums::NO_PIECE;
	}
}

// Todo: switch to std::string & use istringstream
void Boards::Board::parse_fen(const char* fen) {
	assert(fen && "fen's argument was empty");
	board_reset();

	int empty_sq_count{};
	Enums::Piece piece{};
	int file{ Enums::A_FILE };
	int rank{ Enums::RANK8 };

	// FEN example: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 4
	/* 1. Piece position */
	for (; rank >= Enums::RANK1 && *fen;) {
		empty_sq_count = 1;
		switch (*fen) {
		case 'p': piece = Enums::B_PAWN;   break;
		case 'r': piece = Enums::B_ROOK;   break;
		case 'n': piece = Enums::B_KNIGHT; break;
		case 'b': piece = Enums::B_BISHOP; break;
		case 'k': piece = Enums::B_KING;   break;
		case 'q': piece = Enums::B_QUEEN;  break;
		case 'P': piece = Enums::W_PAWN;   break;
		case 'R': piece = Enums::W_ROOK;   break;
		case 'N': piece = Enums::W_KNIGHT; break;
		case 'B': piece = Enums::W_BISHOP; break;
		case 'Q': piece = Enums::W_QUEEN;  break;
		case 'K': piece = Enums::W_KING;   break;

		case '1': [[fallthrough]];
		case '2': case '3': case '4':
		case '5': case '6': case '7': case '8':
			piece = Enums::NO_PIECE;
			empty_sq_count = *fen - '0';
			break;

		case '/': [[fallthrough]];
		case ' ':
			--rank;
			file = Enums::A_FILE;
			++fen;
			continue;

		default: return;
		}
		for (int index{}; index < empty_sq_count; ++index) {
			int square64 = rank * 8 + file;
			int square120 = squares64[square64];
			if (piece != Enums::NO_PIECE) {
				m_pieces[square120] = piece;
			}
			++file;
		}
		++fen;
	}

	/* 2. Side to move */
	assert(*fen == 'w' || *fen == 'b' && "FEN must be either w or b");
	m_turn = ((*fen == 'w') ? Enums::WHITE : Enums::BLACK);
	fen += 2;

	/* 3. Castling permissions */
	for (int current{}; current < 4; ++current) {
		if (*fen == ' ') break;
		switch (*fen) {
			// If FEN contains either Q, K, q or k, this updates the castle rights by setting the int's bits  
		case 'Q': m_castle_rights |= Enums::QUEEN_SIDE_W; break;
		case 'K': m_castle_rights |= Enums::KING_SIDE_W;  break;
		case 'q': m_castle_rights |= Enums::QUEEN_SIDE_B; break;
		case 'k': m_castle_rights |= Enums::KING_SIDE_B;  break;
		default: break;
		}
		++fen;
	}
	++fen;
	assert(m_castle_rights >= 0 && m_castle_rights <= 15 && "castle rights was greater than 0 or less than 15");

	/* 4. Enpassant squares */
	if (*fen != '-') { // - in FEN means no enpassant square
		file = fen[0] - 'a'; // gets number through the char
		rank = fen[1] - '1';
		assert((file >= Enums::A_FILE && file <= Enums::H_FILE) && "invalid file or rank");
		assert((rank >= Enums::RANK1 && rank <= Enums::RANK8) && "invalid rank");
		m_enPassant_square = Utils::getSquare64(file, rank);
		fen += 3;
	}
	else fen += 2;

	//5. Halfmove clock, useful to check whether the 50 move rule was reached */
	if (*fen == '-') {
		m_move50_rule = 0;
		fen += 2;
	}
	else {
		if (*(fen + 1) != ' ') {
			int firstchar = fen[0] - '0';
			int secondchar = fen[1] - '0';
			m_move50_rule = (firstchar * 10) + secondchar;
			fen += 3;
		}
		else {
			m_move50_rule = fen[0] - '0';
			fen += 2;
		}
	}

	/* 6. Total moves played so far since the beginning */
	if (*fen == '-') m_total_moves = 0;
	else m_total_moves = std::atoi(fen);

	m_position_key = HashGenerator::key_generator(*this);
	update_material();
}

void Boards::Board::update_material() noexcept {
	int square{};
	while (square < Cnst::MAX_SQUARES) {
		int piece = m_pieces[square];
		if (piece != Enums::OFFBOARD && piece != Enums::NO_PIECE) {
			int p_color = PieceInfo::get_piece_color(piece);
			if (PieceInfo::is_piece_big(piece)) { ++m_big_pieces[p_color]; }
			if (PieceInfo::is_piece_major(piece)) { ++m_major_pieces[p_color]; }
			if (PieceInfo::is_piece_minor(piece)) { ++m_minor_pieces[p_color]; }
			m_material_count[p_color] += PieceInfo::get_piece_value(piece);

			m_pieces_list[piece][m_pieces_left[piece]] = square;
			++m_pieces_left[piece];
			switch (piece) {
			case Enums::W_KING: m_king_location[Enums::WHITE] = square; break;
			case Enums::B_KING: m_king_location[Enums::BLACK] = square; break;
			case Enums::W_PAWN:
				Bitboard::set_bit(m_pawns[Enums::WHITE], squares120[square]);
				Bitboard::set_bit(m_pawns[Enums::NEITHER], squares120[square]);
				break;
			case Enums::B_PAWN:
				Bitboard::set_bit(m_pawns[Enums::BLACK], squares120[square]);
				Bitboard::set_bit(m_pawns[Enums::NEITHER], squares120[square]);
				break;
			default: break;
			}
		}
		++square;
	}
}

void Boards::Board::piecelist_check() const noexcept {
	for (int curr_piece{ Enums::W_PAWN }; curr_piece <= Enums::B_KING; ++curr_piece) {
		for (int piece_type{}; piece_type < m_pieces_left[curr_piece]; ++piece_type) {
			assert(m_pieces[m_pieces_list[curr_piece][piece_type]] == curr_piece && "piece list failed while checking the board");
		}
	}
}

void Boards::Board::counters_check(arr_size2& temp_bigpiece, arr_size2& temp_majpiece, arr_size2& temp_minpiece, arr_size2& temp_material,
	std::array<int, Cnst::TOT_PIECE_TYPE>& temp_pieceleft) const noexcept {

	for (int square64{}; square64 < Cnst::MAX64_SQUARES; ++square64) {
		int curr_square120 = squares64[square64];
		int curr_piece = m_pieces[curr_square120];
		++temp_pieceleft[curr_piece];
		int curr_color = PieceInfo::get_piece_color(curr_piece);

		if (curr_piece != Enums::NO_PIECE) {
			if (PieceInfo::is_piece_big(curr_piece))   ++temp_bigpiece[curr_color];
			if (PieceInfo::is_piece_major(curr_piece)) ++temp_majpiece[curr_color];
			if (PieceInfo::is_piece_minor(curr_piece)) ++temp_minpiece[curr_color];
			temp_material[curr_color] += PieceInfo::get_piece_value(curr_piece);
		}
	}

	for (int curr_piece{ Enums::W_PAWN }; curr_piece <= Enums::B_KING; ++curr_piece) {
		assert(temp_pieceleft[curr_piece] == pieces_left[curr_piece] && "Failed pieces_left while checking board; board was set incorrectly");
	}
}

void Boards::Board::pawns_check(std::array<FAST_64UI, Cnst::MAX_PAWN_COLORS>& temp_pawns) const noexcept {
	int pawns_currWhite   __attribute__((unused)) { Bitboard::bit_count(temp_pawns[Enums::WHITE]) };
	int pawns_currBlack   __attribute__((unused)) { Bitboard::bit_count(temp_pawns[Enums::BLACK]) };
	int pawns_currNeither __attribute__((unused)) { Bitboard::bit_count(temp_pawns[Enums::NEITHER]) };

	assert((pawns_currWhite == pieces_left[Enums::W_PAWN]
		|| pawns_currBlack == pieces_left[Enums::W_PAWN]
		|| pawns_currNeither == pieces_left[Enums::W_PAWN] + pieces_left[Enums::B_PAWN])
		&& "current pawns were not set up correctly");
}

void Boards::Board::bitboard_check(std::array<FAST_64UI, Cnst::MAX_PAWN_COLORS>& temp_pawns) const noexcept {
	int curr_square64{};

	while (temp_pawns[Enums::WHITE]) {
		curr_square64 = Bitboard::pop_bit(temp_pawns[Enums::WHITE]);
		assert(m_pieces[squares64[curr_square64]] == Enums::W_PAWN && "bitboard squares (white) were set incorrectly");
	}

	while (temp_pawns[Enums::BLACK]) {
		curr_square64 = Bitboard::pop_bit(temp_pawns[Enums::BLACK]);
		assert(m_pieces[squares64[curr_square64]] == Enums::B_PAWN && "bitboard squares (black) were set incorrectly");
	}

	while (temp_pawns[Enums::NEITHER]) {
		curr_square64 = Bitboard::pop_bit(temp_pawns[Enums::NEITHER]);
		assert(m_pieces[squares64[curr_square64]] == Enums::W_PAWN || m_pieces[squares64[curr_square64]] == Enums::B_PAWN
			&& "bitboard squares (neither) were set incorrectly");
	}
}

bool Boards::Board::board_check() const noexcept { // returns bool so it can be inserted into assert and later on disabled completely
	std::cout << "test" << std::endl;
	std::array<int, Cnst::TOT_PIECE_TYPE> temp_pieceleft{};
	std::array<FAST_64UI, Cnst::MAX_PAWN_COLORS> temp_pawns{ m_pawns[Enums::WHITE], m_pawns[Enums::BLACK], m_pawns[Enums::NEITHER] };
	arr_size2 temp_bigpiece{};
	arr_size2 temp_majpiece{};
	arr_size2 temp_minpiece{};
	arr_size2 temp_material{};

	piecelist_check();
	counters_check(temp_bigpiece, temp_majpiece, temp_minpiece, temp_material, temp_pieceleft);
	pawns_check(temp_pawns);
	bitboard_check(temp_pawns);

	// Sanity checks
	assert(temp_material[Enums::WHITE] == m_material_count[Enums::WHITE] && temp_material[Enums::BLACK] == m_material_count[Enums::BLACK]);
	assert(temp_minpiece[Enums::WHITE] == m_minor_pieces[Enums::WHITE] && temp_minpiece[Enums::BLACK] == m_minor_pieces[Enums::BLACK]);
	assert(temp_majpiece[Enums::WHITE] == m_major_pieces[Enums::WHITE] && temp_majpiece[Enums::BLACK] == m_major_pieces[Enums::BLACK]);
	assert(temp_bigpiece[Enums::WHITE] == m_big_pieces[Enums::WHITE] && temp_bigpiece[Enums::BLACK] == m_big_pieces[Enums::BLACK]);
	assert(m_turn == Enums::WHITE || m_turn == Enums::BLACK);
	assert(HashGenerator::key_generator(*this) == position_key);
	assert(m_enPassant_square == Enums::NO_SQUARE || (get_rank[m_enPassant_square] == Enums::RANK6 && m_turn == Enums::WHITE)
		|| get_rank[m_enPassant_square] == Enums::RANK3 && m_turn == Enums::BLACK);
	assert(m_pieces[m_king_location[Enums::WHITE]] == Enums::W_KING);
	assert(m_pieces[m_king_location[Enums::BLACK]] == Enums::B_KING);

	return true;
}

[[nodiscard]] bool Boards::Board::is_square_attacked(int square, char side) const noexcept {
	assert((square_exists(square) && side_exists(side) && board_check() && "sanity checks failed"));

	if (side == Enums::WHITE && (m_pieces[square + PieceInfo::upright] == Enums::W_PAWN
		|| m_pieces[square + PieceInfo::upleft] == Enums::W_PAWN)) return true;
	else if (side == Enums::BLACK && (m_pieces[square + PieceInfo::downright] == Enums::B_PAWN
		|| m_pieces[square + PieceInfo::downleft] == Enums::B_PAWN)) return true;

	int piece{};
	for (auto& curr_king : PieceInfo::kingqueen_attacks) {
		piece = m_pieces[square + curr_king];
		if (piece != Enums::OFFBOARD && PieceInfo::is_piece_king(piece) && PieceInfo::get_piece_color(piece) == side) {
			return true;
		}
	}

	for (auto& current : PieceInfo::knight_attacks) {
		piece = m_pieces[square + current];
		if (piece != Enums::OFFBOARD && PieceInfo::is_piece_knight(piece) && PieceInfo::get_piece_color(piece) == side) {
			return true;
		}
	}

	for (auto& curr_rook : PieceInfo::rook_attacks) {
		int temp_square{ square + curr_rook };
		piece = m_pieces[temp_square];
		while (piece != Enums::OFFBOARD) {
			if (piece != Enums::NO_PIECE) {
				if (PieceInfo::rook_or_queen(piece) && PieceInfo::get_piece_color(piece) == side) {
					return true;
				} break; // The existing piece is neither a rook or a queen, so the current square in X direction is not attacked 
			}
			temp_square += curr_rook;
			piece = m_pieces[temp_square];
		}
	}

	for (auto& curr_bishop : PieceInfo::bishop_attacks) { // curr_bishop = direction 
		int temp_square{ square + curr_bishop };
		piece = m_pieces[temp_square];
		while (piece != Enums::OFFBOARD) {
			if (piece != Enums::NO_PIECE) {
				if (PieceInfo::bishop_or_queen(piece) && PieceInfo::get_piece_color(piece) == side) {
					return true;
				} break;
			}
			temp_square += curr_bishop;
			piece = m_pieces[temp_square];
		}
	}
	return false;
}

bool Boards::Board::make_move(int move) noexcept {
	int from{ MoveUtils::get_from(move) };
	int to{ MoveUtils::get_to(move) };
	int side{ m_turn };
	assert(Board::square_exists(from) && Board::square_exists(from) && board_check()
		&& Board::side_exists(m_turn) && Board::piece_exists(m_pieces[from]));

	// Update position key before move is applied
	m_history[m_total_ply].m_position_key = m_position_key;

	int white_enpassant{ to - 10 };
	int black_enpassant{ to + 10 };
	if (MoveUtils::is_enpassant(move)) { // move is enpassant
		remove_piece(m_turn == Enums::WHITE ? white_enpassant : black_enpassant);
	}

	else if (MoveUtils::is_castle(move)) { // move is castle
		if (to == Enums::C1)	  move_piece(Enums::A1, Enums::D1);
		else if (to == Enums::C8) move_piece(Enums::A8, Enums::D8);
		else if (to == Enums::G1) move_piece(Enums::H1, Enums::F1);
		else if (to == Enums::G8) move_piece(Enums::H8, Enums::F8);
	}

	if (m_enPassant_square != Enums::NO_SQUARE) hash_epsquare();
	hash_castle();

	// Update history
	m_history[m_total_ply].m_move = move;
	m_history[m_total_ply].m_move50_rule = m_move50_rule;
	m_history[m_total_ply].m_castle_rights = m_castle_rights;
	m_history[m_total_ply].m_enPassant_square = m_enPassant_square;

	// Update castle rights + 50 move rule
	m_castle_rights &= MoveUtils::get_castle_perm(from);
	m_castle_rights &= MoveUtils::get_castle_perm(to);
	m_enPassant_square = Enums::NO_SQUARE;
	hash_castle();
	++m_move50_rule;

	// Captured piece exists
	if (MoveUtils::get_capt(move) != Enums::NO_PIECE) {
		remove_piece(to);
		m_move50_rule = 0; // after a capture, the fifty move rule must be reseted
	}
	++m_total_ply;
	++m_ply;

	int white_ep{ from + 10 };
	int black_ep{ from - 10 };
	if (PieceInfo::is_piece_pawn(m_pieces[from])) {
		m_move50_rule = 0;
		if (MoveUtils::is_pawnfirst(move)) {
			m_enPassant_square = (m_turn == Enums::WHITE) ? white_ep : black_ep;
			hash_epsquare();
			// Assert get_rank[enPassant_square]==3 for white and ==6 for black
		}
	}

	move_piece(from, to);
	auto promoted{ MoveUtils::get_prom(move) };
	if (promoted != Enums::NO_PIECE) { // Piece is promotion
		assert(piece_exists(promoted) && !PieceInfo::is_piece_pawn(MoveUtils::get_prom(move)));
		remove_piece(to);
		add_piece(to, promoted);
	}

	if (PieceInfo::is_piece_king(m_pieces[to])) {
		m_king_location[m_turn] = to;
	}
	m_turn ^= 1; // Switch side
	hash_side();

	m_total_moves = m_total_ply;

	assert(board_check());
	if (is_square_attacked(m_king_location[side], m_turn)) {// King of the side before the move was applied is attacked by the current side
		unmake_move();
		return false;
	}
	return true;
}

void Boards::Board::remove_piece(int square) noexcept {
	int piece_color{ PieceInfo::get_piece_color(m_pieces[square]) };
	assert(piece_color == Enums::WHITE || piece_color == Enums::BLACK);

	int piece{ m_pieces[square] };
	assert(Board::square_exists(square) && Board::piece_exists(piece));
	hash_piece(piece, square);
	m_material_count[piece_color] -= PieceInfo::get_piece_value(piece);
	m_pieces[square] = Enums::NO_PIECE;

	if (PieceInfo::is_piece_big(piece)) {
		--m_big_pieces[piece_color];
		if (PieceInfo::is_piece_major(piece)) --m_major_pieces[piece_color];
		else --m_minor_pieces[piece_color];
	}
	else {
		Bitboard::clear_bit(m_pawns[piece_color], Board::squares120[square]);
		Bitboard::clear_bit(m_pawns[Enums::NEITHER], Board::squares120[square]);
	}

	int index{};
	while (index < m_pieces_left[piece]) {
		if (m_pieces_list[piece][index] == square) {
			--m_pieces_left[piece];
			m_pieces_list[piece][index] = m_pieces_list[piece][m_pieces_left[piece]];
			// ^ last piece in pieces_lift shifted to the removed element, effectively "hides" the piece to be removed
			break;
		}
		++index;
	}
}

void Boards::Board::add_piece(int square, int piece) noexcept {
	int piece_color{ PieceInfo::get_piece_color(piece) };
	assert(Board::square_exists(square) && Board::piece_exists(piece));

	m_pieces[square] = piece;
	hash_piece(piece, square);

	if (PieceInfo::is_piece_big(piece)) {
		++m_big_pieces[piece_color];
		if (PieceInfo::is_piece_major(piece)) ++m_major_pieces[piece_color];
		else ++m_minor_pieces[piece_color];
	}
	else {
		Bitboard::set_bit(m_pawns[piece_color], Board::squares120[square]);
		Bitboard::set_bit(m_pawns[Enums::NEITHER], Board::squares120[square]);
	}

	m_material_count[piece_color] += PieceInfo::get_piece_value(piece);
	m_pieces_list[piece][m_pieces_left[piece]] = square;
	++m_pieces_left[piece];
}

void Boards::Board::move_piece(unsigned int from, unsigned int to) noexcept {
	int piece{ m_pieces[from] };
	int color{ PieceInfo::get_piece_color(piece) };

	hash_piece(piece, from);
	m_pieces[from] = Enums::NO_PIECE;
	hash_piece(piece, to);
	m_pieces[to] = piece;

	if (!PieceInfo::is_piece_big(piece)) {
		Bitboard::clear_bit(m_pawns[color], squares120[from]);
		Bitboard::clear_bit(m_pawns[Enums::NEITHER], squares120[from]);
		Bitboard::set_bit(m_pawns[color], squares120[to]);
		Bitboard::set_bit(m_pawns[Enums::NEITHER], squares120[to]);
	}

	for (int index{}; index < m_pieces_left[piece]; ++index) {
		if (m_pieces_list[piece][index] == from) {
			m_pieces_list[piece][index] = to;
			break;
		}
	}
}

void Boards::Board::unmake_move() noexcept {
	assert(board_check());
	--m_total_ply;
	--m_ply;

	int move{ m_history[m_total_ply].m_move };
	int from{ MoveUtils::get_from(move) };
	int to{ MoveUtils::get_to(move) };

	assert(Board::square_exists(from) && Board::square_exists(from)
		&& Board::side_exists(m_turn) && Board::piece_exists(m_pieces[from]));

	if (enPassant_square() != Enums::NO_SQUARE) hash_epsquare();
	hash_castle();

	m_castle_rights = m_history[m_total_ply].m_castle_rights;
	m_move50_rule = m_history[m_total_ply].m_move50_rule;
	m_enPassant_square = m_history[m_total_ply].m_enPassant_square;

	m_total_moves = m_history[m_total_ply].m_total_moves;

	if (enPassant_square() != Enums::NO_SQUARE) hash_epsquare();
	hash_castle();

	m_turn ^= 1;
	hash_side();


	int white_enpassant{ to - 10 };
	int black_enpassant{ to + 10 };
	if (MoveUtils::is_enpassant(move)) { // move was enpassant
		if (m_turn == Enums::WHITE) add_piece(white_enpassant, Enums::B_PAWN);
		else add_piece(black_enpassant, Enums::W_PAWN);
	}

	else if (MoveUtils::is_castle(move)) { // move was castle
		if (to == Enums::C1)	  move_piece(Enums::D1, Enums::A1);
		else if (to == Enums::C8) move_piece(Enums::D8, Enums::A8);
		else if (to == Enums::G1) move_piece(Enums::F1, Enums::H1);
		else move_piece(Enums::F8, Enums::H8);
	}

	move_piece(to, from);
	if (PieceInfo::is_piece_king(m_pieces[from])) m_king_location[m_turn] = from;

	int captured{ MoveUtils::get_capt(move) };
	if (captured != Enums::NO_PIECE) add_piece(to, captured);

	int promoted{ MoveUtils::get_prom(move) };
	if (promoted != Enums::NO_PIECE) {
		remove_piece(from);
		add_piece(from, PieceInfo::get_piece_color(promoted) == Enums::WHITE ? Enums::W_PAWN : Enums::B_PAWN);
	}
	assert(board_check());
}

void Boards::Board::make_nullmove() noexcept {
	++m_ply;
	m_history[m_total_ply].m_position_key = m_position_key;
	if (m_enPassant_square != Enums::NO_SQUARE) hash_epsquare();
	m_history[m_total_ply].m_move = Enums::INCORRECT_MOVE;
	m_history[m_total_ply].m_move50_rule = m_move50_rule;
	m_history[m_total_ply].m_enPassant_square = m_enPassant_square;
	m_history[m_total_ply].m_castle_rights = m_castle_rights;
	m_enPassant_square = Enums::NO_SQUARE;
	m_turn ^= 1;
	++m_total_ply;
	hash_side();
}

void Boards::Board::unmake_nullmove() {
	--m_total_ply;
	--m_ply;
	if (m_enPassant_square != Enums::NO_SQUARE) hash_epsquare();
	m_castle_rights = m_history[m_total_ply].m_castle_rights;
	m_enPassant_square = m_history[m_total_ply].m_enPassant_square;
	m_move50_rule = m_history[m_total_ply].m_move50_rule;
	
	if (m_enPassant_square != Enums::NO_SQUARE) hash_epsquare();
	m_turn ^= 1;
	hash_side();

}



/// change
void Boards::Board::print_board() const noexcept {
	std::cout << "board:\n\n";
	for (int rank{ Enums::RANK1 }; rank <= Enums::RANK8; ++rank) {
		std::cout << 8 - rank;
		for (int file{ Enums::A_FILE }; file <= Enums::H_FILE; ++file) {
			int square = Utils::getSquare64(file, 7 - rank);
			int piece = m_pieces[square];
			std::cout << "   " << Cnst::piece_str[piece];
		}
		std::cout << '\n';
	}

	std::cout << '\n' << " ";
	for (int file{ Enums::A_FILE }; file <= Enums::H_FILE; ++file) {
		std::cout << "   " << static_cast<char>(file + 'a');
	}
	std::cout << "\n\n";
	std::cout << "Side: " << Cnst::side_str[m_turn] << '\n';
	std::cout << "En passant: " << m_enPassant_square << '\n';
	std::cout << "Castle rights: " <<
		(m_castle_rights & Enums::KING_SIDE_W ? 'K' : '-') <<
		(m_castle_rights & Enums::QUEEN_SIDE_W ? 'Q' : '-') <<
		(m_castle_rights & Enums::KING_SIDE_B ? 'k' : '-') <<
		(m_castle_rights & Enums::QUEEN_SIDE_B ? 'q' : '-') << '\n';
	std::cout << "Position key: " << m_position_key << '\n';
	std::cout << "Move 50 rule: " << m_move50_rule << '\n';
	std::cout << "Total moves: " << m_total_moves << '\n';
}
