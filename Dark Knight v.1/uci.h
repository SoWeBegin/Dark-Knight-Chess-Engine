#ifndef UCI_PROTOCOL_H
#define UCI_PROTOCOL_H

#include "utilities.h"
#include <string>


namespace Search {
	struct SearchInformations;
}

namespace Boards {
	class Board;
}

namespace UCI {
	void go_parser(const std::string& str, Search::SearchInformations& info, Boards::Board& pos) noexcept;
	void position_parser(const std::string& str, Boards::Board& pos) noexcept;

	void uci() noexcept;
}

#endif