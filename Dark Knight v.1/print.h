#ifndef PRINT_H
#define PRINT_H
#include "board.h"
#include "utilities.h"
#include "piece_info.h"
#include "move_generator.h"

#include <iostream>
#include <string>


//////////////////////////////////////////////////////////////////////////////////////////
// This file contains utilities to print various stuff to the console for testing purposes
// Separate file to avoid including iostream and causing a messy structure of the program
// can be removed
//////////////////////////////////////////////////////////////////////////////////////////


namespace Print {
	// Bitboard
	constexpr void print_bitboard(FAST_64UI bb) noexcept {
		FAST_64UI to_shift{ 1ULL };
		for (int curr_rank{ Enums::RANK1 }; curr_rank <= Enums::RANK8; ++curr_rank) {
			for (int curr_file{ Enums::A_FILE }; curr_file <= Enums::H_FILE; ++curr_file) {
				int square = Utils::getSquare64(curr_file, 7 - curr_rank);
				int square64 = Boards::Board::squares120[square];
				if ((to_shift << square64) & bb) std::cout << '+';
				else std::cout << '-';
			}
			std::cout << '\n';
		}
	}

	[[nodiscard]] inline std::string get_stringsquare(int square) noexcept {
		return std::string{ static_cast<char>('a' + Boards::Board::get_file.at(square)),  static_cast<char>('1' + Boards::Board::get_rank.at(square)) };
	}

	[[nodiscard]] inline std::string get_stringmove(int m) noexcept {
		int filefrom{ Boards::Board::get_file[MoveUtils::get_from(m)] };
		int fileto{ Boards::Board::get_file[MoveUtils::get_to(m)] };
		int rankfrom{ Boards::Board::get_rank[MoveUtils::get_from(m)] };
		int rankto{ Boards::Board::get_rank[MoveUtils::get_to(m)] };

		if (MoveUtils::is_promotion(m)) {
			// TODO: check if NRVO is applied
			char promoted_piece{ PieceInfo::get_promoted_piece(MoveUtils::get_prom(m)) };
			return std::string{ static_cast<char>('a' + filefrom), static_cast<char>('1' + rankfrom), static_cast<char>('a' + fileto), static_cast<char>('1' + rankto), promoted_piece };
		}
		else {
			return std::string{ static_cast<char>('a' + filefrom), static_cast<char>('1' + rankfrom), static_cast<char>('a' + fileto), static_cast<char>('1' + rankto) };
		}
	}

	inline void print_attacked(Enums::Color side, const Boards::Board& pos) noexcept {
		std::cout << "\n\nSquares attacked by: " << Cnst::side_str[side];
		for (int rank{ Enums::RANK8 }; rank >= Enums::RANK1; --rank) {
			for (int file = Enums::A_FILE; file <= Enums::H_FILE; ++file) {
				int square = Utils::getSquare64(file, rank);
				if (pos.is_square_attacked(square, side)) std::cout << 'X';
				else std::cout << '-';
			}
			std::cout << '\n';
		}
		std::cout << "\n\n";
	}

	inline void print_moves(const MovesList& ml) noexcept {
		std::cout << "Total moves: " << ml.get_tot_moves() << '\n';
		for (int index{}; const auto & cmove : ml.get_all_moves()) {
			int currentmove{ cmove.move };
			int score{ cmove.score };
			std::cout << "Current move [index " << index++ << "]: " << get_stringmove(currentmove) << '\n' << "Score: " << score << '\n';
		}
		std::cout << "Total moves so far: " << ml.get_tot_moves() << '\n';
	}

	[[nodiscard]] inline int parse_move(const std::string& move_str, Boards::Board& position) noexcept {
		int from_sq64{ Utils::getSquare64(move_str[0] - 'a', move_str[1] - '1') };
		int to_sq64{ Utils::getSquare64(move_str[2] - 'a', move_str[3] - '1') };

		MovesList moves;
		moves.generate_moves(position);
		for (auto& current_move : moves.get_all_moves()) {
			int move{ current_move.move };
			if (MoveUtils::get_from(move) == from_sq64 && MoveUtils::get_to(move) == to_sq64) {
				int promotion{ MoveUtils::get_prom(move) };
				if (promotion != Enums::NO_PIECE) {
					char promoted_piece{ PieceInfo::get_promoted_piece(promotion) };
					if ((promoted_piece == 'r' && move_str[4] == 'r') || (promoted_piece == 'b' && move_str[4] == 'b')
						|| (promoted_piece == 'q' && move_str[4] == 'q') || (promoted_piece == 'n' && move_str[4] == 'n')) {
						return move;
					}
					else continue;
				}
				return move;
			}
		}
		return Enums::INCORRECT_MOVE;
	}
}

#endif