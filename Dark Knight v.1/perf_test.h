#ifndef PERF_TESTING_H
#define PERF_TESTING_H

#include "utilities.h"
#include "move_generator.h"
#include "board.h"
#include "print.h"

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <windows.h>
#include <chrono> 


inline FAST_64UI perft(int depth, Boards::Board& board) noexcept {
    if (!depth) return 1;

    FAST_64UI leaf_nodes{};
    MovesList movelist;
    movelist.generate_moves(board);

    for (int index{}; index < movelist.get_tot_moves(); ++index) {
        if (!board.make_move(movelist[index].move)) continue;
        leaf_nodes += perft(depth - 1, board);
        board.unmake_move();
    }
    return leaf_nodes;
}

inline FAST_64UI perft_test(int depth, Boards::Board& board) noexcept {
    MovesList movelist;
    FAST_64UI tot_nodes{};

    auto clockbegin{ std::chrono::high_resolution_clock::now() };
    movelist.generate_moves(board);
    for (int index{}; index < movelist.get_tot_moves(); ++index) {
        int move{ movelist[index].move };
        if (!board.make_move(move)) continue;
        FAST_64UI nodes{ perft(depth - 1, board) };
        tot_nodes += nodes;
        board.unmake_move();
    }
    auto clockend{ std::chrono::high_resolution_clock::now() };
    std::cout << "\nTest completed; Total leaf nodes visited: " << tot_nodes << ", time (ms): " << ((std::chrono::duration<double, std::milli>)(clockend - clockbegin)).count() << '\n';
    return tot_nodes;
}

inline bool exec_perft_tests(const Boards::Board& board, int depth) {
    std::string ch_depth{};

    switch (depth) {
        case 1: ch_depth = "D1"; break;
        case 2: ch_depth = "D2"; break;
        case 3: ch_depth = "D3"; break;
        case 4: ch_depth = "D4"; break;
        case 5: ch_depth = "D5"; break;
        case 6: ch_depth = "D6"; break;
        default: ch_depth = "ERROR";
    }

    std::ifstream perft_file{ "perft.txt" };
    if (!perft_file) {
        std::cerr << "File not found";
        return false;
    }

    std::string line{};
    std::unordered_map<std::string, int> perft_cases;
    while (perft_file) {
        std::getline(perft_file, line);
        if (line == "" || ch_depth == "ERROR") break;
        auto iter_depth_found = line.find(ch_depth);
        if (iter_depth_found == std::string::npos) break;

        std::string fen(line.begin(), std::find(line.begin(), line.end(), ';') - 1);
        auto chosen_depth{ std::stoll(std::string(std::next(line.begin(),iter_depth_found + 2), std::find(std::next(line.begin(), iter_depth_found), line.end(), ';') - 1)) };

        perft_cases[fen] = chosen_depth;
    }

    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    Boards::Board b;
    bool all_passed{ true };

    for (std::size_t index{}; const auto & x : perft_cases) {
        b.parse_fen(x.first.c_str());
        auto nodes_found = perft_test(depth, b);
        std::cout << "x second: " << x.second << std::endl;
        if (nodes_found == x.second) {
            SetConsoleTextAttribute(hConsole, 2);
            std::cout << "Perft case [" << index << "] PASSED" << '\n';
        }
        else {
            SetConsoleTextAttribute(hConsole, 4);
            std::cerr << "Perft case [" << index << "] BROKEN" << '\n';
            all_passed = false;
        }
        SetConsoleTextAttribute(hConsole, 15);
        ++index;
    }
    SetConsoleTextAttribute(hConsole, 15);
    return all_passed;
}



#endif