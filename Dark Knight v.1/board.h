#ifndef BOARD_H
#define BOARD_H

#include "utilities.h"
#include "transposition_table.h"
#include "nnue.h"

#include <array>
#include "nnue.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////
// This file contains anything related to the board (mainly). There are some other utilities to work
// through 64-square boards and 120 square boards, since the 10x12 squares method is used for the engine.
////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Boards {
	// Contains only stuff that cannot be recovered from BoardState
	struct PreviousBoardState {
		FAST_64UI m_position_key{};
		int m_enPassant_square{};
		int m_move50_rule{};
		int m_total_moves{};
		int m_turn{};
		int m_move{};
		int m_castle_rights{};
	};

	class Board {
	private:
		TT::TranspositionTable m_transp_table{};
		std::array<int, Cnst::MAX_DEPTH> m_pv_moves{};
		std::array<PreviousBoardState, Cnst::MAX_TOTAL_MOVES> m_history{};
		std::array<int, Cnst::MAX_SQUARES> m_pieces{};
		std::array<int, Cnst::TOT_PIECE_TYPE> m_pieces_left{}; // Contains numbers of each piece-type left (ie 2 knights, 1 bishop, etc)
		std::array<std::array<int, Cnst::MAX_SQUARES>, Cnst::TOT_PIECE_TYPE> m_search_heuristics{}; // Updated when alpha is beaten by move or piece
		std::array<std::array<int, Cnst::MAX_DEPTH>, 2> m_killer_moves{}; // Moves that caused B-cutoff

		// Contains square of each piece (faster for move generation)
		std::array<std::array<int, Cnst::MAX_SINGLE_PIECE>, Cnst::TOT_PIECE_TYPE> m_pieces_list{};

		std::array<int, Cnst::MAX_COLORS> m_big_pieces{};		 // Bishops, knights, rooks, queen
		std::array<int, Cnst::MAX_COLORS> m_minor_pieces{};    // Bishops/Knights
		std::array<int, Cnst::MAX_COLORS> m_major_pieces{};	 // Rooks/Queen
		std::array<int, Cnst::MAX_KING_COLORS> m_king_location{};
		std::array<int, Cnst::MAX_COLORS> m_material_count{};

		// Bitwise representations
		using bitwise_array = std::array<FAST_64UI, Cnst::MAX_PAWN_COLORS>;
		bitwise_array m_pawns{};

		// Miscellaneous
		FAST_64UI m_position_key{};
		int m_ply{};		 // total ply in the current search
		int m_total_moves{}; // same as ply*2
		int m_total_ply{};   // total ply since the beginning of the match (hisPly)
		int m_move50_rule{};
		int m_castle_rights{};
		int m_enPassant_square{};
		int m_turn{};

	public:
		inline static std::array<int, Cnst::MAX_SQUARES> get_rank{};
		inline static std::array<int, Cnst::MAX_SQUARES> get_file{};
		inline static std::array<int, Cnst::MAX_SQUARES> squares120{};
		inline static std::array<int, Cnst::MAX64_SQUARES> squares64{};

		// Hash related - these arrays will hold random generated numbers [See: Zobrist Hashing for info]
		using piece_per_square = std::array<std::array<FAST_64UI, Cnst::MAX_SQUARES>, Cnst::TOT_PIECE_TYPE>;
		inline static piece_per_square pieces_key{};		   // Key representing all pieces in the board
		inline static FAST_64UI side_tomove_key{};			   // Key representing the side to move (the key represents that the side to move is black)
		inline static std::array<FAST_64UI, 16> castle_key{};  // Key representing castle rights. 16 is preferred since castle rights is a bitset of 4 specific bits (which derive from 1, 2, 
															   // 4 and 8, and which in total is 15)

		using arr_size2 = std::array<int, Cnst::MAX_COLORS>;

		Board() = default;
		Board(Board&) = delete;

		// Getters
		[[nodiscard]] __forceinline constexpr FAST_64UI position_key() const noexcept { return m_position_key; }
		[[nodiscard]] __forceinline constexpr int ply() const noexcept { return m_ply; }
		[[nodiscard]] __forceinline constexpr int total_ply() const noexcept { return m_total_ply; }
		[[nodiscard]] __forceinline constexpr int total_moves() const noexcept { return m_total_moves; }
		[[nodiscard]] __forceinline constexpr int move50_rule() const noexcept { return m_move50_rule; }
		[[nodiscard]] __forceinline constexpr int castle_rights() const noexcept { return m_castle_rights; }
		[[nodiscard]] __forceinline constexpr int enPassant_square() const noexcept { return m_enPassant_square; }
		[[nodiscard]] __forceinline constexpr int turn() const noexcept { return m_turn; }
		[[nodiscard]] __forceinline constexpr const auto& pieces_list() const noexcept { return m_pieces_list; }
		[[nodiscard]] __forceinline constexpr int pieces_list_index(std::size_t piece_type, std::size_t piece_num) const noexcept { return m_pieces_list[piece_type][piece_num]; }
		[[nodiscard]] __forceinline constexpr const auto& big_pieces() const noexcept { return m_big_pieces; }
		[[nodiscard]] __forceinline constexpr int big_pieces_index(std::size_t index) const noexcept { return m_big_pieces[index]; }
		[[nodiscard]] __forceinline constexpr const auto& minor_pieces() const noexcept { return m_minor_pieces; }
		[[nodiscard]] __forceinline constexpr int minor_pieces_index(std::size_t index) const noexcept { return m_minor_pieces[index]; }
		[[nodiscard]] __forceinline constexpr const auto& major_pieces() const noexcept { return m_major_pieces; }
		[[nodiscard]] __forceinline constexpr int major_pieces_index(std::size_t index) const noexcept { return m_major_pieces[index]; }
		[[nodiscard]] __forceinline constexpr const auto& king_location() const noexcept { return m_king_location; }
		[[nodiscard]] __forceinline constexpr int king_location_index(std::size_t index) const noexcept { return m_king_location[index]; }
		[[nodiscard]] __forceinline constexpr const auto& material_count() const noexcept { return m_material_count; }
		[[nodiscard]] __forceinline constexpr int material_count_index(std::size_t index) const noexcept { return m_material_count[index]; }
		[[nodiscard]] __forceinline constexpr const auto& pawns() const noexcept { return m_pawns; }
		[[nodiscard]] __forceinline constexpr auto pawns_index(std::size_t index) const noexcept { return m_pawns[index]; }
		[[nodiscard]] __forceinline constexpr const auto& history() const noexcept { return m_history; }
		[[nodiscard]] __forceinline constexpr const auto& history_index(std::size_t index) const noexcept { return m_history[index]; }
		[[nodiscard]] __forceinline constexpr const auto& pieces() const noexcept { return m_pieces; }
		[[nodiscard]] __forceinline constexpr int pieces_index(std::size_t index) const noexcept { return m_pieces[index]; }
		[[nodiscard]] __forceinline constexpr const auto& pieces_left() const noexcept { return m_pieces_left; }
		[[nodiscard]] __forceinline constexpr int pieces_left_index(std::size_t index) const noexcept { return m_pieces_left[index]; }
		[[nodiscard]] __forceinline constexpr const auto& transp_table() const noexcept { return m_transp_table; }
		[[nodiscard]] __forceinline constexpr const auto& pv_moves() const noexcept { return m_pv_moves; }
		[[nodiscard]] __forceinline constexpr int pv_moves_index(std::size_t index) const noexcept { return m_pv_moves[index]; }
		[[nodiscard]] __forceinline constexpr const auto& search_heuristics() const noexcept { return m_search_heuristics; }
		[[nodiscard]] __forceinline constexpr int search_heuristics_index(int piece_type, int square) const noexcept { return m_search_heuristics[piece_type][square]; }
		[[nodiscard]] __forceinline constexpr int killer_moves_index(std::size_t index, int depth) const noexcept { return m_killer_moves[index][depth]; }


		// Miscellaneous
		constexpr void set_pv_moves(std::size_t index, int move);
		constexpr void incr_search_heuristics(int piece_type, int square) noexcept;
		constexpr void reset_search_heuristics() noexcept;
		constexpr void reset_killer_moves() noexcept;
		void reset_transp_table() noexcept;
		constexpr void set_ply(int ply) noexcept;
		constexpr void transp_table_init(std::size_t index) noexcept;
		constexpr void set_killers(std::size_t index, int ply, int new_val) noexcept;
		constexpr void incr_search_heuristic_val(int from, int to, int new_val) noexcept;

		// Initialization
		static void initialize_board64() noexcept;
		static void initialize_ranks_files() noexcept;
		static void initialize() noexcept {
			Board::initialize_board64();
			Board::initialize_ranks_files();
			nnue_init("nn-eba324f53044.nnue");
		}

		// Position related
		void board_reset() noexcept;
		void parse_fen(const char* fen);
		void update_material() noexcept;
		void print_board() const noexcept;
		[[nodiscard]] bool is_square_attacked(int square, char side) const noexcept;

		// Board checks
		void piecelist_check() const noexcept;
		void counters_check(arr_size2&, arr_size2&, arr_size2&, arr_size2&, std::array<int, 13>&) const noexcept;
		void pawns_check(std::array<FAST_64UI, Cnst::MAX_PAWN_COLORS>& temp_pawns) const noexcept;
		void bitboard_check(std::array<FAST_64UI, Cnst::MAX_PAWN_COLORS>& temp_pawns) const noexcept;
		bool board_check() const noexcept;

		// Sanity checks
		[[nodiscard]] __forceinline static constexpr bool square_exists(int square) noexcept {
			return get_file[square] != Enums::OFFBOARD;
		}

		[[nodiscard]] __forceinline static constexpr bool side_exists(char side) noexcept {
			return (side == Enums::WHITE || side == Enums::BLACK);
		}

		[[nodiscard]] __forceinline static constexpr bool filerank_exists(int filerank) noexcept {
			return (filerank >= 0 && filerank <= 7);
		}

		[[nodiscard]] __forceinline static constexpr bool piece_exists(int piece) noexcept {
			return (piece >= Enums::NO_PIECE && piece <= Enums::B_KING);
		}

		[[nodiscard]] __forceinline static constexpr bool piece_exists_noempty(int piece) noexcept {
			return (piece >= Enums::W_PAWN && piece <= Enums::B_KING);
		}

		__forceinline constexpr void hash_piece(int piece, int square) noexcept {
			m_position_key ^= pieces_key[piece][square];
		}

		__forceinline void hash_side() noexcept {
			m_position_key ^= side_tomove_key;
		}

		__forceinline constexpr void hash_castle() noexcept {
			m_position_key ^= castle_key[m_castle_rights];
		}

		__forceinline constexpr void hash_epsquare() noexcept {
			m_position_key ^= pieces_key[Enums::NO_PIECE][m_enPassant_square];
		}


		// Move related
		bool make_move(int move) noexcept;
		void make_nullmove() noexcept;
		void unmake_nullmove();
		void unmake_move() noexcept;
		void remove_piece(int square) noexcept;
		void add_piece(int square, int piece) noexcept;
		void move_piece(unsigned int from, unsigned int to) noexcept;
		[[nodiscard]] constexpr bool is_repetition() const noexcept;

		// Transposition table related (best moves in the current position)
		void set_transposition_move(int move) noexcept;
		[[nodiscard]] int get_transposition_move() const noexcept;
	};

	constexpr void Board::set_pv_moves(std::size_t index, int move) { 
		m_pv_moves[index] = move; 
	}

	constexpr void Board::incr_search_heuristics(int piece_type, int square) noexcept { 
		++m_search_heuristics[piece_type][square]; 
	}

	constexpr void Board::reset_search_heuristics() noexcept { 
		m_search_heuristics = {}; }
	constexpr void Board::reset_killer_moves() noexcept { m_killer_moves = {};
	}

	inline void Board::reset_transp_table() noexcept { 
		m_transp_table.reset_internal();
	}

	constexpr void Board::set_ply(int ply) noexcept {
		m_ply = ply; 
	}

	constexpr void Board::transp_table_init(std::size_t index) noexcept { 
		m_transp_table.init(index); 
	}

	constexpr void Board::set_killers(std::size_t index, int ply, int new_val) noexcept {
		m_killer_moves[index][ply] = new_val;
	}

	constexpr void Board::incr_search_heuristic_val(int from, int to, int new_val) noexcept {
		m_search_heuristics[from][to] += new_val;
	}

	constexpr bool Board::is_repetition() const noexcept {
		// Consider only reversible moves (pawn moves & captures not considered, those cannot be reversed)
		for (int current{ 4 }; current <= m_move50_rule; current += 2) {
			if (m_position_key == m_history[m_total_ply - current].m_position_key) {
				return true;
			}
		}
		return false;
	}

	// Transposition table related (best moves in the current position)
	inline void Board::set_transposition_move(int move) noexcept {
		int hashed_index{ static_cast<int>(m_position_key % m_transp_table.get_size()) };
		m_transp_table.set(hashed_index, move, m_position_key, m_ply);
	}

	[[nodiscard]] inline int Board::get_transposition_move() const noexcept {
		int hashed_index{ static_cast<int>(m_position_key % m_transp_table.get_size()) };

		if (m_transp_table.get_key(hashed_index) == m_position_key) {
			return m_transp_table.get_bestmove(hashed_index);
		}

		return static_cast<int>(Enums::INCORRECT_MOVE);
	}

}
#endif