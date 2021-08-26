#ifndef PIECE_INFO_H
#define PIECE_INFO_H

#include "utilities.h"
#include <array>

///////////////////////////////////////////////////////////////////////////
// This file contains utilities to find out informations related to a piece
///////////////////////////////////////////////////////////////////////////


namespace PieceInfo {
	enum GeneralDirections {
		left = -1, up = -10, right = 1, down = 10, upright = -9,
		upleft = -11, downright = 11, downleft = 9
	};

	// Lookup for slide/non slide pieces
	constexpr inline std::array<int, 8> loop_slide{
	Enums::W_BISHOP, Enums::W_ROOK, Enums::W_QUEEN, Cnst::ZERO_PIECES, Enums::B_BISHOP, Enums::B_ROOK, Enums::B_QUEEN, Cnst::ZERO_PIECES
	};

	constexpr inline std::array<int, 6> loop_nonslide{
	 Enums::W_KNIGHT, Enums::W_KING, Cnst::ZERO_PIECES, Enums::B_KNIGHT, Enums::B_KING, Cnst::ZERO_PIECES
	};

	constexpr inline std::array<int, 8> get_slide_piece{
		Enums::W_BISHOP, Enums::W_ROOK, Enums::W_QUEEN, Cnst::ZERO_PIECES, Enums::B_BISHOP, Enums::B_ROOK, Enums::B_QUEEN, Cnst::ZERO_PIECES
	};

	constexpr inline std::array<int, 6> get_notslide_piece{
		Enums::W_KNIGHT, Enums::W_KING, Cnst::ZERO_PIECES, Enums::B_KNIGHT, Enums::B_KING, Cnst::ZERO_PIECES
	};


	// Lookup tables 
	constexpr inline std::array<int, 8> kingqueen_attacks{ left, up, right, down, upright, upleft, downright, downleft };
	constexpr inline std::array<int, 4> rook_attacks{ left, up, right, down };
	constexpr inline std::array<int, 4> bishop_attacks{ upright, upleft, downright, downleft };
	constexpr inline std::array<int, 8> knight_attacks{ -8, -19, -21, -12, 8, 19, 21, 12 };

	// Most valuable victim, least valuable aggressor
	// 0 to account for Enums::NO_PIECE
	constexpr inline std::array<std::array<int, 13>, 13> mvv_lva{ {
		{ 0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0 },
		{ 0, 105, 104, 103, 102, 101, 100,	105, 104, 103, 102, 101, 100 },
		{ 0, 205, 204, 203, 202, 201, 200,	205, 204, 203, 202, 201, 200 },
		{ 0, 305, 304, 303, 302, 301, 300,	305, 304, 303, 302, 301, 300 },
		{ 0, 405, 404, 403, 402, 401, 400,	405, 404, 403, 402, 401, 400 },
		{ 0, 505, 504, 503, 502, 501, 500,	505, 504, 503, 502, 501, 500 },
		{ 0, 605, 604, 603, 602, 601, 600,	605, 604, 603, 602, 601, 600 },

		{ 0, 105, 104, 103, 102, 101, 100,	105, 104, 103, 102, 101, 100 },
		{ 0, 205, 204, 203, 202, 201, 200,	205, 204, 203, 202, 201, 200 },
		{ 0, 305, 304, 303, 302, 301, 300,	305, 304, 303, 302, 301, 300 },
		{ 0, 405, 404, 403, 402, 401, 400,	405, 404, 403, 402, 401, 400 },
		{ 0, 505, 504, 503, 502, 501, 500,	505, 504, 503, 502, 501, 500 },
		{ 0, 605, 604, 603, 602, 601, 600,	605, 604, 603, 602, 601, 600 },
	} };

	[[nodiscard]] static constexpr __forceinline int get_tot_direction(int piece) noexcept {
		switch (piece) {
			case Enums::NO_PIECE: case Enums::W_PAWN: case Enums::B_PAWN: return 0;
			case Enums::W_ROOK: case Enums::B_ROOK: case Enums::W_BISHOP: case Enums::B_BISHOP: return 4;
			default: return 8;
		}
	}

	[[nodiscard]] static constexpr __forceinline bool is_piece_big(int piece) noexcept {
		return (1 << piece) & 0b1111101111100;
	}

	[[nodiscard]] static constexpr __forceinline bool is_piece_major(int piece) noexcept {
		return (1 << piece) & 0b1110001110000;
	}

	[[nodiscard]] static constexpr __forceinline bool is_piece_minor(int piece) noexcept {
		return (1 << piece) & 0b0001100001100;
	}

	[[nodiscard]] static constexpr __forceinline bool is_piece_pawn(int piece) noexcept {
		return (1 << piece) & 0b0000010000010;
	}

	[[nodiscard]] static constexpr __forceinline bool is_piece_knight(int piece) noexcept {
		return (1 << piece) & 0b0000100000100;
	}

	[[nodiscard]] static constexpr __forceinline bool is_piece_king(int piece) noexcept {
		return (1 << piece) & 0b1000001000000;
	}

	[[nodiscard]] static constexpr __forceinline bool rook_or_queen(int piece) noexcept {
		return (1 << piece) & 0b0110000110000;
	}

	[[nodiscard]] static constexpr __forceinline bool is_piece_rook(int piece) noexcept {
		return (1 << piece) & 0b0010000010000;
	}

	[[nodiscard]] static constexpr __forceinline bool is_piece_queen(int piece) noexcept {
		return (1 << piece) & 0b0100000100000;
	}

	[[nodiscard]] static constexpr __forceinline bool bishop_or_queen(int piece) noexcept {
		return (1 << piece) & 0b0101000101000;
	}

	[[nodiscard]] static constexpr __forceinline bool is_piece_bishop(int piece) noexcept {
		return (1 << piece) & 0b0001000001000;
	}

	[[nodiscard]] static constexpr __forceinline bool piece_slides(int piece) noexcept {
		return (1 << piece) & 0b0111000111000;
	}

	[[nodiscard]] static constexpr int get_direction(int piece, std::size_t index) noexcept {
		assert(piece >= 0 && piece <= 13 && (index >= 0 && index <= 8));

		switch (piece) {
			case Enums::NO_PIECE: case Enums::W_PAWN: case Enums::B_PAWN: return 0;
			case Enums::B_KING: case Enums::W_KING:
			case Enums::B_QUEEN: case Enums::W_QUEEN:
				return kingqueen_attacks[index];
			case Enums::B_ROOK: case Enums::W_ROOK:
				return rook_attacks[index];
			case Enums::B_KNIGHT: case Enums::W_KNIGHT:
				return knight_attacks[index];
			default:
				return bishop_attacks[index];
			}
	}

	[[nodiscard]] static constexpr char get_promoted_piece(int piece) noexcept {
		if (is_piece_rook(piece)) return 'r';
		else if (is_piece_queen(piece)) return 'q';
		else if (is_piece_bishop(piece)) return 'b';
		else return 'n';
	}

	[[nodiscard]] static constexpr int get_piece_value(int piece) noexcept {
		assert(piece >= 0 && piece <= 12 && "piece does not exist");
		switch (piece) {
			case 0: return 0; // NO_PIECE
			case 1: case 7: return 100; // pawns
			case 2: case 8: return 325; // knight
			case 3: case 9: return 350; // bishop
			case 4: case 10: return 550; // rook
			case 5: case 11: return 1000; // queen
			default: return 50000; // king
		}
	}

	[[nodiscard]] static constexpr __forceinline char get_piece_color(int piece) noexcept {
		assert(piece >= 0 && piece <= 12 && "piece does not exist");
		if (piece == 0) return Enums::NEITHER;
		else if (piece >= 1 && piece <= 6) return Enums::WHITE;
		else return Enums::BLACK;
	}
}

#endif