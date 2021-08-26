
#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H
#include <cstdint>
#include <intrin.h>
#include <string>
#include <sstream>

#ifndef NDEBUG
#  define NDEBUG
#endif
#include <cassert>


///////////////////////////////////////////////////////////////////////////
// General utilities used in the whole program, such as enums and constants
///////////////////////////////////////////////////////////////////////////


using FAST_64UI = unsigned long long;

namespace Utils {
	// Pretty function to calculate the correct square in a 64 board, given file and rank
	[[nodiscard]] __forceinline static constexpr int getSquare64(int file, int rank) noexcept {
		return (file + 21) + (rank * 10);
	}

	// Finds the first least/most significant bit that's set to 1 and returns its index
	[[nodiscard]] __forceinline static int lsb(FAST_64UI bitboard) noexcept {
		return __builtin_ctzll(bitboard);
	}

	// To fix: BitScanReverse64 (Move MSVC -> Clang builtin)
	[[nodiscard]] inline static int msb(FAST_64UI bitboard) noexcept {
		unsigned long ret;
		_BitScanReverse64(&ret, bitboard);
		return static_cast<int>(ret);
	}
}

namespace Cnst {
	// Theoretically, the max total moves in a chess game (Considering 50-moves-rule) is 5898.5. If we wanted though, we could reduce this to 2000, since we probably expect a chess game to never reach such a number.
	constexpr inline int MAX_TOTAL_MOVES{ 5898 };
	constexpr inline int MAX_POS_MOVES{ 256 };
	constexpr inline int MAX_SQUARES{ 120 }; // Keeps track of extra squares out of the board, to check whether a piece is inside the board or not
	constexpr inline int OUT_OF_RANGE64{ 120 };
	constexpr inline int OUT_OF_RANGE{ 65 };
	constexpr inline int MAX64_SQUARES{ 64 };
	constexpr inline int MAX_DEPTH{ 64 };
	constexpr inline int TOT_PIECE_TYPE{ 13 };
	constexpr inline int MAX_SINGLE_PIECE{ 10 };	 // There may be a total of 10 pieces per piece type (e.g 10 rooks, 10 queens, and whatever)
	constexpr inline int MAX_COLORS{ 2 };
	constexpr inline int MAX_PAWN_COLORS{ 3 };
	constexpr inline int MAX_KING_COLORS{ 2 };
	constexpr inline int ZERO_PIECES{};

	const inline std::string piece_str{ ".PNBRQKpnbrqk" };
	const inline std::string side_str{ "wb-" };
	const inline std::string rank_str{ "12345678" };
	const inline std::string file_str{ "abcdefgh" };
}

namespace Enums {
	// 10x12 Board representation = <21 and >98 are "protective walls" to check whether a piece is out of the board.
	enum Square {
		A1 = 21, B1, C1, D1, E1, F1, G1, H1,
		A2 = 31, B2, C2, D2, E2, F2, G2, H2,
		A3 = 41, B3, C3, D3, E3, F3, G3, H3,
		A4 = 51, B4, C4, D4, E4, F4, G4, H4,
		A5 = 61, B5, C5, D5, E5, F5, G5, H5,
		A6 = 71, B6, C6, D6, E6, F6, G6, H6,
		A7 = 81, B7, C7, D7, E7, F7, G7, H7,
		A8 = 91, B8, C8, D8, E8, F8, G8, H8,
		NO_SQUARE, OFFBOARD
	};

	enum Color { WHITE, BLACK, NEITHER };

	enum File {
		A_FILE, B_FILE, C_FILE, D_FILE,
		E_FILE, F_FILE, G_FILE, H_FILE, NO_FILE
	};

	enum Rank {
		RANK1, RANK2, RANK3, RANK4,
		RANK5, RANK6, RANK7, RANK8, NO_RANK
	};

	enum Piece {
		NO_PIECE,
		W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN,
		W_KING, B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK,
		B_QUEEN, B_KING,
	};

	// 1 = 0001 | 2 = 0010 | 4 = 0100 | 8 = 1000
	enum Castle {
		KING_SIDE_W = 1, QUEEN_SIDE_W,
		KING_SIDE_B = 4, QUEEN_SIDE_B = 8
	};

	enum CastlePermissions {
		BROOK_QUEENSIDE = 21, BKING = 25,
		BROOK_KINGSIDE = 28, WROOK_QUEENSIDE = 91,
		WKING = 95, WROOK_KINGSIDE = 98
	};

	// Needed to AND with castle_permission and check whether castling permission is available
	enum CastleBits {
		WHITE_NOCASTLE = 3, WHITE_NOCASTLE_QUEENSIDE = 7,
		WHITE_NOCASTLE_KINGSIDE = 11, BLACK_NOCASTLE,
		BLACK_NOCASTLE_QUEENSIDE, BLACK_NOCASTLE_KINGSIDE,
		ALL_CASTLES_AVAILABLE = 15
	};

	enum CastleMove { NOT_CASTLE, CASTLE };

	enum MoveInfo { INCORRECT_MOVE, ZERO_MOVES, CORRECT_MOVE };

	enum Score { DRAW, MATE = 29000, INF = 30000 };

	enum MoveOrder { CAPTURES_FIRST = 1000000, KILLER_FIRST = 900000, KILLER_SECOND = 800000, TRANSP_MOVE = 2000000 };

	enum DepthInfo { NOT_SET = -1, };

	enum EvalUtils { ISOLATED_PAWN = -5, };
}

#endif