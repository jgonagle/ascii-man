/*
GameHandler.h
Written by John McGonagle on May 8, 2010.
*/
#ifndef _GAMEHANDLER_H
#define _GAMEHANDLER_H

#include <windows.h>
#include "Board.h"
#include <string>
#include "GameGraphics.h"

using std::string;

class GameHandler
{
	public:
		GameHandler(const Board& board, const GameGraphics& game_graphics);
		virtual ~GameHandler() {};
		void Run();

		static bool KeyPressed(int key);
		static string ExtractInitials(string name);

	private:
		string m_map_name;
		HANDLE m_display_handler;
		Board m_board;
		GameGraphics m_game_graphics;

		bool m_entered_name;
		string m_player_inits;

		int m_max_scores;
		int name_count;
		
		void StartNewGame(bool hard_mode);
		void HighScores(const vector<pair<long,string>>& high_scores);
		void UpdateHighScores(vector<pair<long,string>>& high_scores);
		void WriteToHighScores(const vector<pair<long,string>>& high_scores) const;
		string GetPlayerName();
};

#endif
