#ifndef BITBOARD_H
#define BITBOARD_H
#include <intrin.h>
#include "utilities.h"
#include "board.h"
#include <array>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This file contains anything related to the pawns (inside the Board class) which are set up to be bitboards
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace Bitboard {
	// Pawns' bitboard masks
	inline std::array<FAST_64UI, Cnst::MAX64_SQUARES> bitboard_setmask{};
	inline std::array<FAST_64UI, Cnst::MAX64_SQUARES> bitboard_clearmask{};


	// Calls lsb(bitboard), finds the first LSB that's set to 1, sets it to 0 and returns its index
	[[nodiscard]] inline static int pop_bit(FAST_64UI& bitboard) noexcept {
		int least_sbit{ Utils::lsb(bitboard) };
		bitboard &= bitboard - 1;

		return least_sbit;
	}

	// Returns total count of set bits (1). Effectively returns total pawns count.
	[[nodiscard]] inline static int bit_count(FAST_64UI bitboard) noexcept {
		return static_cast<int>(__popcnt64(bitboard));
	}

	static constexpr void initialize_mask() noexcept {
		for (int index{}; auto & current : bitboard_setmask) {
			current |= (1ULL << index);
			bitboard_clearmask[index] = ~current;
			++index;
		}
	}

	__forceinline static constexpr void clear_bit(FAST_64UI& bitboard, int square) noexcept {
		bitboard &= bitboard_clearmask[square];
	}

	__forceinline static constexpr void set_bit(FAST_64UI& bitboard, int square) noexcept {
		bitboard |= bitboard_setmask[square];
	}

	constexpr void initialize() noexcept {
		initialize_mask();
	}
}
#endif