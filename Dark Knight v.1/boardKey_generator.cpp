#include "boardKey_generator.h"
#include "board.h"

#include <ctime> 
#include <random>

using U64 = FAST_64UI;
#define RAND_64 	((U64)rand() | \
					(U64)rand() << 15 | \
					(U64)rand() << 30 | \
					(U64)rand() << 45 | \
					((U64)rand() & 0xf) << 60 )  

namespace HashGenerator {
	[[nodiscard]] FAST_64UI key_generator(const Boards::Board& current_board) noexcept {
		FAST_64UI key{};
		int square{};
		int current_piece{ Enums::NO_PIECE };
		// Generate unique key considering all the current pieces, the side to move, and the castle rights
		while (square < Cnst::MAX_SQUARES) {
			current_piece = current_board.pieces_index(square);
			if (current_piece != Enums::NO_SQUARE && current_piece != Enums::NO_PIECE && current_piece != Enums::OFFBOARD) {
				assert((current_piece >= Enums::W_PAWN && current_piece <= Enums::B_KING) && "Piece does not exist");
				key ^= Boards::Board::pieces_key[current_piece][square]; // XOR the random generated values to generate the key
			}
			++square;
		}

		int ep{ current_board.enPassant_square() };
		if (ep != Enums::NO_SQUARE) {
			assert((ep > 0 && ep < Cnst::MAX_SQUARES) && "En passant square is invalid");
			key ^= Boards::Board::pieces_key[Enums::NO_PIECE][ep];
		}

		if (current_board.turn() == Enums::WHITE)
			key ^= Boards::Board::side_tomove_key;

		int castle{ current_board.castle_rights() };
		if (castle >= 0 && castle <= 15)
			key ^= Boards::Board::castle_key[castle];

		else assert(false && "current_board.castle_rights was <0 or >15");

		return key;
	}

	void initialize_hash() {
		std::mt19937_64 gen64;

		// Generate random numbers for the pieces 2d array
		for (auto& outer : Boards::Board::pieces_key) {
			for (auto& inner : outer) {
				inner = gen64();
			}
		}

		// Generate random numbers for the castle rights
		for (auto& current : Boards::Board::castle_key) {
			current = gen64();
		}

		// Generate random number for the current side to move
		Boards::Board::side_tomove_key = gen64();
	}

	void initialize() {
		initialize_hash();
	}
}