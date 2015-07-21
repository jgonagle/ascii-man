#include "GameGraphics.h"
#include <utility>
#include <vector>
#include <map>
#include <iostream>
#include <windows.h>
#include <sstream>
#include <ctype.h>
#include <time.h>

/*  COLORS (text mode)
 ====================|Back-|Fore-
  Constant     |Value|grnd?|grnd?
 --------------+-----+-----+-----
  BLACK        |  0  | Yes | Yes
  BLUE         |  1  | Yes | Yes
  GREEN        |  2  | Yes | Yes
  CYAN         |  3  | Yes | Yes
  RED          |  4  | Yes | Yes
  MAGENTA      |  5  | Yes | Yes
  BROWN        |  6  | Yes | Yes
  LIGHTGRAY    |  7  | Yes | Yes
  DARKGRAY     |  8  | No  | Yes
  LIGHTBLUE    |  9  | No  | Yes
  LIGHTGREEN   | 10  | No  | Yes
  LIGHTCYAN    | 11  | No  | Yes
  LIGHTRED     | 12  | No  | Yes
  LIGHTMAGENTA | 13  | No  | Yes
  YELLOW       | 14  | No  | Yes
  WHITE        | 15  | No  | Yes
*/

using std::vector;
using std::pair;
using std::cout;
using std::stringstream;

void GameGraphics::Sleep(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

GameGraphics::GameGraphics(HANDLE& display_h, pair<int,int> max_butes, pair<int,int> board_dims, pair<int,int> board_offset)
{
	m_display_handler = display_h;
	m_board_offset = board_offset;
	m_board_dims = board_dims;

	SetConsoleTitle("ASCII-MAN");
	SMALL_RECT windowSize = {0, 0, max_butes.first - 1, max_butes.second - 1 + 4};
	COORD bufferSize = {max_butes.first, max_butes.second + 4};

	SetConsoleWindowInfo(m_display_handler, TRUE, &windowSize);
	SetConsoleScreenBufferSize(m_display_handler, bufferSize);

	symbol_colors.insert(pair<int,int> ('#', 9));
	symbol_colors.insert(pair<int,int> ('*', 13));
	symbol_colors.insert(pair<int,int> ('-', 15));
	symbol_colors.insert(pair<int,int> ('|', 15));
	symbol_colors.insert(pair<int,int> ('^', 0));
	symbol_colors.insert(pair<int,int> ('$', 10));
	symbol_colors.insert(pair<int,int> ('%', 14));
	symbol_colors.insert(pair<int,int> ('=', 15));
	symbol_colors.insert(pair<int,int> ('~', 15));
	symbol_colors.insert(pair<int,int> ('[', 8));
	symbol_colors.insert(pair<int,int> (']', 8));
}

void GameGraphics::PrintCurBoard(const vector<vector<char>>& cur_board, bool invert)
{
	for (size_t i = 0; i < cur_board.size(); i++)
	{
		for (size_t j = 0; j < cur_board[i].size(); j++)
		{			
			string char_string = string(1, cur_board[i][j]);
			PrintGraphic(pair<int,int> (i, j), -1, char_string, invert);
		}
	}
}

void GameGraphics::PrintBoardInfo(int level, long lives, long score)
{
	stringstream ss;
	ss << level;
	PrintGraphic(pair<int,int> (-4, 0), 15, "LEVEL:", false);
	PrintGraphic(pair<int,int> (-4, 6), 0, "               ", false);
	PrintGraphic(pair<int,int> (-4, 6), 15, ss.str(), false);
	ss.str("");

	string char_string = string(lives, '@');
	PrintGraphic(pair<int,int> (-3, 0), 15, "LIVES:", false);
	PrintGraphic(pair<int,int> (-3, 6), 0, "               ", false);
	PrintGraphic(pair<int,int> (-3, 6), 14, char_string, false);

	ss << score;
	PrintGraphic(pair<int,int> (-2, 0), 15, "SCORE:", false);
	PrintGraphic(pair<int,int> (-2, 6), 0, "               ", false);
	PrintGraphic(pair<int,int> (-2, 6), 15, ss.str(), false);
	ss.str("");
}

void GameGraphics::UpdateBoard(const vector<pair<int,int>>& board_updates, const vector<vector<char>>& board_state)
{
	for(vector<pair<int,int>>::const_iterator board_update_iter = board_updates.begin();
		board_update_iter != board_updates.end();
		board_update_iter++)
	{
		string char_string = string(1, board_state[board_update_iter->first][board_update_iter->second]);
		PrintGraphic(pair<int,int> (board_update_iter->first, board_update_iter->second), -1, char_string);
	}					 
}

void GameGraphics::UpdateSprites(const vector<Sprite*>& sprites)
{
	for(vector<Sprite*>::const_iterator sprite_update_iter = sprites.begin();
		sprite_update_iter != sprites.end();
		sprite_update_iter++)
	{
		string char_string = string(1,(*(*sprite_update_iter)).GetSymbol());
		if ((*(*sprite_update_iter)).GetMood() != DEAD || 
			(*(*sprite_update_iter)).GetCurLocation() != (*(*sprites.begin())).GetCurLocation())
		{
			PrintGraphic((*(*sprite_update_iter)).GetCurLocation(),
						 (*(*sprite_update_iter)).GetCurColor(),
						 char_string);
		}
	}
}

void GameGraphics::UpdateInformation(const vector<pair<int,long>>& info_updates)
{
	for(vector<pair<int,long>>::const_iterator info_update_iter = info_updates.begin();
		info_update_iter != info_updates.end();
		info_update_iter++)
	{
		if (info_update_iter->first == 2)
		{
			stringstream ss;
			ss << info_update_iter->second;
			PrintGraphic(pair<int,int> (-4, 6), 15, ss.str());
		}
		else if (info_update_iter->first == 0)
		{
			string char_string = string(info_update_iter->second, '@');
			string empty_string = string(info_update_iter->second + 1, ' ');
			PrintGraphic(pair<int,int> (-3, 6), 0, empty_string);
			PrintGraphic(pair<int,int> (-3, 6), 14, char_string);
		}
		else if (info_update_iter->first == 1)
		{
			stringstream ss;
			ss << info_update_iter->second;
			PrintGraphic(pair<int,int> (-2, 6), 15, ss.str());
		}
	}
}

void GameGraphics::SetCursor(pair<int,int> cursor_loc)
{
	COORD cursor_coord;
	cursor_coord.X = cursor_loc.second + m_board_offset.first;
	cursor_coord.Y = cursor_loc.first + m_board_offset.second;


	SetConsoleCursorPosition(m_display_handler, cursor_coord);
}

void GameGraphics::SetColor(int color)
{
	SetConsoleTextAttribute(m_display_handler, color);
}

void GameGraphics::PrintCenterMessage(string message, int color, unsigned int display_time, unsigned int rest_time, int vert_offset, bool inverse, bool border)
{
	int message_offset = (m_board_dims.first - message.length()) / 2;
	string message_clear = string(message.length(), ' ');
	
	if (border)
	{
		PrintGraphic(pair<int,int> (m_board_dims.second / 2 + vert_offset - 1, message_offset), 0, message_clear, inverse);
	}
		PrintGraphic(pair<int,int> (m_board_dims.second / 2 + vert_offset, message_offset), color, message, inverse);
	if (border)
	{
		PrintGraphic(pair<int,int> (m_board_dims.second / 2 + vert_offset + 1, message_offset), 0, message_clear, inverse);
	}

	if (display_time != 0)
	{
		Sleep(display_time);

		string message_clear = string(message.length(), ' ');
		PrintGraphic(pair<int,int> (m_board_dims.second / 2 + vert_offset, message_offset), color, message_clear, inverse);
		
		Sleep(rest_time);
	}	
}

void GameGraphics::PrintMenu()
{
	PrintCenterMessage("--START EASY GAME--", 14, 0, 0, -6, true);
	PrintCenterMessage("--START HARD GAME--", 14, 0, 0, -3);
	PrintCenterMessage("--HIGH SCORES--", 14, 0, 0, 0);
	PrintCenterMessage("--EXIT--", 14, 0, 0, 3);
}

void GameGraphics::UpdateMenu(int old_sel, int cur_sel)
{
	switch (old_sel)
	{
		case 0:
		{
			PrintCenterMessage("--START EASY GAME--", 14, 0, 0, -6);
			break;
		}
		case 1:
		{
			PrintCenterMessage("--START HARD GAME--", 14, 0, 0, -3);
			break;
		}
		case 2:
		{
			PrintCenterMessage("--HIGH SCORES--", 14, 0, 0, 0);
			break;
		}
		case 3:
		{
			PrintCenterMessage("--EXIT--", 14, 0, 0, 3);
			break;
		}
		default:
		{
			break;
		}
	}
	switch (cur_sel)
	{
		case 0:
		{
			PrintCenterMessage("--START EASY GAME--", 14, 0, 0, -6, true);
			break;
		}
		case 1:
		{
			PrintCenterMessage("--START HARD GAME--", 14, 0, 0, -3, true);
			break;
		}
		case 2:
		{
			PrintCenterMessage("--HIGH SCORES--", 14, 0, 0, 0, true);
			break;
		}
		case 3:
		{
			PrintCenterMessage("--EXIT--", 14, 0, 0, 3, true);
			break;
		}
		default:
		{
			break;
		}
	}
}

void GameGraphics::ClearBoard(bool everything)
{
	int horiz_offset = 0;

	if (everything)
	{
		horiz_offset = m_board_offset.first;
	}

	for (int i = -4; i < m_board_dims.second; i++)
	{
		string char_string(horiz_offset * 2 + m_board_dims.first, ' ');
		PrintGraphic(pair<int,int> (i , 0 - horiz_offset), 0, char_string, false);
	}
}

void GameGraphics::PrintGraphic(pair<int,int> square_loc, int color, string chars_to_print, bool invert)
{
	if (color == -1)
	{
		if (isalpha(chars_to_print[0]))
		{
			color = 0;
		}
		else
		{
			color = symbol_colors[chars_to_print[0]];
		}
	}

	COORD print_here;
	print_here.X = square_loc.second + m_board_offset.first;
	print_here.Y = square_loc.first + m_board_offset.second;

	if (invert)
	{
		SetConsoleTextAttribute(m_display_handler, 255 - color);
	}
	else
	{
		SetConsoleTextAttribute(m_display_handler, color);
	}

	SetConsoleCursorPosition(m_display_handler, print_here);
	cout << chars_to_print;
}
