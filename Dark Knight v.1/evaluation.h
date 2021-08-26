#ifndef EVALUATION_H
#define EVALUATION_H

#include "utilities.h"

#include <array>

namespace Boards {
	class Board;
}

namespace Eval {
	[[nodiscard]] int evaluate_pos(const Boards::Board& board) noexcept;
}

#endif