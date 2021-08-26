#include "perf_test.h"
#include "utilities.h"
#include "transposition_table.h"
#include "search.h"
#include "evaluation.h"
#include "uci.h"
#include "boardKey_generator.h"
#include "bitboard.h"

int main()
{
	Boards::Board::initialize();
	Bitboard::initialize();
	HashGenerator::initialize();
	UCI::uci();
}

