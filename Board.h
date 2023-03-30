#ifndef BOARD_H_
#define BOARD_H_

#include "GameConstants.h"
#include <iostream>
#include <fstream>
#include <string>

class Board
{
public:
	enum GridEntry {
		empty, player, blue_coin_square, red_coin_square, up_dir_square,
		down_dir_square, left_dir_square, right_dir_square, event_square,
		bank_square, star_square, bowser, boo
	};
	enum LoadResult {
		load_success, load_fail_file_not_found, load_fail_bad_format
	};

	Board()
	{
		for (int gy = 0; gy < BOARD_HEIGHT; gy++)
			for (int gx = 0; gx < BOARD_WIDTH; gx++)
				m_grid[gy][gx] = empty;
	}

	LoadResult loadBoard(const std::string &filename)
	{
		std::ifstream boardFile(filename);
		if (!boardFile)
			return load_fail_file_not_found;

		  // get the grid

		std::string line;
		int numPlayerLocations = 0;

		for (int gy = BOARD_HEIGHT-1; std::getline(boardFile, line); gy--)
		{
			if (gy < 0)  // too many grid lines?
			{
				if (line.find_first_not_of(" \t\r") != std::string::npos)
					return load_fail_bad_format;  // non-blank line
				char dummy;
				if (boardFile >> dummy)  // non-blank rest of file
					return load_fail_bad_format;
				break;
			}
			if (line.size() < BOARD_WIDTH  ||
					line.find_first_not_of(" \t\r", BOARD_WIDTH) != std::string::npos)
				return load_fail_bad_format;

			for (int gx = 0; gx < BOARD_WIDTH; gx++)
			{
				GridEntry ge;
				switch (line[gx])
				{
					default:  return load_fail_bad_format;
					case ' ': ge = empty; break;
					case '@': ge = player; numPlayerLocations++; break;
					case '+': ge = blue_coin_square; break;
					case '-': ge = red_coin_square; break;
					case '<': ge = left_dir_square; break;
					case '>': ge = right_dir_square; break;
					case '^': ge = up_dir_square; break;
					case 'v': ge = down_dir_square; break;
					case '!': ge = event_square; break;
					case '$': ge = bank_square; break;
					case '*': ge = star_square; break;
					case 'B': ge = bowser; break;
					case 'b': ge = boo; break;
				}
				m_grid[gy][gx] = ge;
			}
		}
		if (numPlayerLocations != 1)
			return load_fail_bad_format;

		return load_success;
	}

	GridEntry getContentsOf(int gx, int gy)
	{
		if (gx < 0  ||  gx >= BOARD_WIDTH  ||  gy < 0  ||  gy >= BOARD_HEIGHT)
			return empty;

		return m_grid[gy][gx];
	}

private:
	GridEntry m_grid[BOARD_HEIGHT][BOARD_WIDTH];  // indexed by [gy][gx]
};

#endif // #ifndef BOARD_H_

