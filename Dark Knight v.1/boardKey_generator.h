#ifndef BOARDKEY_GENERATOR_H
#define BOARDKEY_GENERATOR_H
#include "utilities.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// This file contains stuff related to the generation of a hash - see Zobrist Hashing Method
/////////////////////////////////////////////////////////////////////////////////////////////


namespace Boards {
	class Board;
}

namespace HashGenerator {
	void initialize_hash();

	FAST_64UI key_generator(const Boards::Board& current_board) noexcept;

	void initialize();
}
#endif