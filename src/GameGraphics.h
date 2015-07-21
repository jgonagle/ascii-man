/*
GameGraphics.h
Written by John McGonagle on May 10, 2010.
*/
#ifndef _GAMEGRAPHICS_H
#define _GAMEGRAPHICS_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <windows.h>
#include "Sprite.h"

using std::string;
using std::vector;
using std::pair;
using std::map;

class GameGraphics
{
public:
	GameGraphics(HANDLE& display_h, pair<int,int> max_butes, pair<int,int> board_dims, pair<int,int> board_offset);
	GameGraphics() {};
	~GameGraphics(){};

	void PrintCurBoard(const vector<vector<char>>& cur_board, bool invert);
	void PrintBoardInfo(int level, long lives, long score);
	void UpdateBoard(const vector<pair<int,int>>& board_updates, const vector<vector<char>>& board_state);
	void UpdateSprites(const vector<Sprite*>& sprites);
	void UpdateInformation(const vector<pair<int,long>>& info_updates);
	void SetCursor(pair<int,int> cursor_loc);
	void SetColor(int color);
	void PrintMenu();
	void UpdateMenu(int old_loc, int cur_loc);
	void ClearBoard(bool everything = false);
	void PrintCenterMessage(string message, int color, unsigned int display_time, unsigned int rest_time, int vert_offset = 0, bool inverse = false, bool border = true);

	static void Sleep(unsigned int mseconds);

private:
	map<char,int> symbol_colors;
	void PrintGraphic(pair<int,int> square_loc, int color, string symbols, bool invert = false);
	pair<int,int> m_board_offset;
	pair<int,int> m_board_dims;
	HANDLE m_display_handler;
};

#endif