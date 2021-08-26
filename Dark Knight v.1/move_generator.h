
#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "utilities.h"
#include "piece_info.h"
#include "board.h"
#include <array>

///////////////////////////////////////////////////////////////////////////////////////////
// This file contains all move related generations (pawns, sliding and non sliding pieces) 
// Move generation type: legal moves only
///////////////////////////////////////////////////////////////////////////////////////////

/* TODO
* Switch from castle flags. Create an int castle_move and set up the bitwise logic for generating a castle move, instead of using a castle flag
* ...
*/


//namespace Boards { class Board; }

namespace MoveUtils {
	// flags
	constexpr inline int enpassant{ 0x40000 };
	constexpr inline int pawnfirst{ 0x80000 };
	constexpr inline int castle{ 0x1000000 };
	constexpr inline int capture{ 0x7C000 };
	constexpr inline int promotion{ 0xF00000 };

	[[nodiscard]] static constexpr __forceinline int get_from(int move) noexcept { return move & 0x7F; }
	[[nodiscard]] static constexpr __forceinline int get_to(int move)   noexcept { return (move >> 7) & 0x7F; }
	[[nodiscard]] static constexpr __forceinline int get_capt(int move) noexcept { return (move >> 14) & 0xF; }
	[[nodiscard]] static constexpr __forceinline int get_prom(int move) noexcept { return (move >> 20) & 0xF; }

	[[nodiscard]] static constexpr int __forceinline gen_singlemove(int from, int to, int capt = Enums::NO_PIECE, int prom = Enums::NO_PIECE, int flags = 0) noexcept {
		// flags = enpassant, pawnfirst
		return (from | to << 7 | capt << 14 | prom << 20 | flags);
	}

	[[nodiscard]] static constexpr __forceinline bool is_capture(int move)   noexcept { return move & capture; }
	[[nodiscard]] static constexpr __forceinline bool is_promotion(int move) noexcept { return move & promotion; }
	[[nodiscard]] static constexpr __forceinline bool is_pawnfirst(int move) noexcept { return move & pawnfirst; }
	[[nodiscard]] static constexpr __forceinline bool is_castle(int move)    noexcept { return move & castle; }
	[[nodiscard]] static constexpr __forceinline bool is_enpassant(int move) noexcept { return move & enpassant; }

	[[nodiscard]] constexpr int get_castle_perm(int castle_index) noexcept {
		switch (castle_index) {
			case Enums::CastlePermissions::BROOK_QUEENSIDE: return Enums::BLACK_NOCASTLE_QUEENSIDE;
			case Enums::CastlePermissions::BKING:           return Enums::BLACK_NOCASTLE;
			case Enums::CastlePermissions::BROOK_KINGSIDE:  return Enums::BLACK_NOCASTLE_KINGSIDE;
			case Enums::CastlePermissions::WROOK_QUEENSIDE: return Enums::WHITE_NOCASTLE_QUEENSIDE;
			case Enums::CastlePermissions::WKING:			return Enums::WHITE_NOCASTLE;
			case Enums::CastlePermissions::WROOK_KINGSIDE:  return Enums::WHITE_NOCASTLE_KINGSIDE;
			default: return Enums::CastleBits::ALL_CASTLES_AVAILABLE;
		}
	}
}

class MovesList {
public:
	struct Move {
		int move{};
		int score{};
		/* move consists of multiple stuff as bits, from right to left:
		* 0000 0000 0000 0000 0000 0000 0000
		* FROM (which square) : bits 0 to 6		 => 0x7F
		* TO (which square)   : bits 7 to 13	 => (TO>>7) + 0x7F
		* Captured piece	  : bits 14 to 17	 => (CP>>14) + 0xF
		* enpassant flag	  : bit 18			 => 0x40000
		* pawn first move     : bit 19			 => 0x80000
		* promoted piece	  : bits 20 to 23	 => (PP>>20) + 0xF
		* castle flag		  : 24				 => 0x1000000
		*/
	};

private:
	std::array<Move, Cnst::MAX_POS_MOVES> all_moves{};
	std::size_t tot_moves{};


public:
	using Board = Boards::Board;
	constexpr __forceinline const auto& operator[](std::size_t index) const noexcept { return all_moves[index]; }
	constexpr __forceinline auto& operator[](std::size_t index) noexcept { return all_moves[index]; }

	// Pawns
	void set_whitepawn_move(const Board& position, int move, bool quiet = false) noexcept;
	void set_blackpawn_move(const Board& position, int move, bool quiet = false) noexcept;
	void gen_blackpawnmoves(const Boards::Board& position, bool quiet = true) noexcept;
	void gen_whitepawnmove(const Board& position, bool quiet = true) noexcept;

	// Slide: Bishop, Rook, Queen -- Non slide: Knight, king
	void gen_slide_moves(const Board& position, std::size_t piece_index) noexcept;
	void gen_nonslide_moves(const Board& position, std::size_t piece_index) noexcept;

	// Castle 
	void gen_black_castle_moves(const Board& position) noexcept;
	void gen_white_castle_moves(const Board& position) noexcept;

	// All moves
	void generate_moves(const Board& position) noexcept;

	// Move type
	constexpr void set_quietmove(const Boards::Board& position, int move) noexcept;

	constexpr void set_tacticalmove(const Boards::Board& position, int move) noexcept;

	constexpr void set_epmove(const Boards::Board& position, int move) noexcept {
		all_moves[tot_moves] = { move, 105 + Enums::CAPTURES_FIRST }; // 105 = P x P
		++tot_moves;
	}

	// Getters
	[[nodiscard]] __forceinline constexpr std::size_t get_tot_moves() const noexcept {
		return tot_moves;
	}

	[[nodiscard]] __forceinline constexpr const auto& get_all_moves() const noexcept {
		return all_moves;
	}

	// Sanity checks
	[[nodiscard]] bool move_check(Boards::Board& board, int move) noexcept;

	__forceinline constexpr void set_score(int move_num, int new_score) noexcept {
		all_moves[move_num].score = new_score;
	}

	void gen_captures_only(const Board& pos) noexcept;
	void gen_blackpawnmoves_capt(const Board& position) noexcept;
	void gen_whitepawnmoves_capt(const Board& position) noexcept;
	void gen_slide_moves_capt(const Board& position, std::size_t piece_index) noexcept;
	void gen_nonslide_moves_capt(const Board& position, std::size_t piece_index) noexcept;
};

constexpr void MovesList::set_quietmove(const Boards::Board& position, int move) noexcept {
	all_moves[tot_moves].move = move;
	if (position.killer_moves_index(0, position.ply()) == move) {
		all_moves[tot_moves].score = Enums::KILLER_FIRST;
	}
	else if (position.killer_moves_index(1, position.ply()) == move) {
		all_moves[tot_moves].score = Enums::KILLER_SECOND;
	}
	else all_moves[tot_moves].score = position.search_heuristics_index(position.pieces_index(MoveUtils::get_from(move)), MoveUtils::get_to(move));
	++tot_moves;
}

constexpr void MovesList::set_tacticalmove(const Boards::Board& position, int move) noexcept {
	all_moves[tot_moves] = {
		move, PieceInfo::mvv_lva[MoveUtils::get_capt(move)][position.pieces_index(MoveUtils::get_from(move))] + Enums::CAPTURES_FIRST
	};
	++tot_moves;
}

#endif