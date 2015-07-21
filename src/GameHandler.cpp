#include "GameHandler.h"
#include "Board.h"
#include "GameGraphics.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <time.h>
#include <sstream>
#include <math.h>
#include <ctype.h>

using std::max;
using std::min;
using std::stringstream;
using std::cin;
using std::cout;
using std::endl;
using std::ofstream;
using std::ios;
using std::stringstream;

bool GameHandler::KeyPressed(int key) 
{
   return ((GetAsyncKeyState(key) & 0x8000) != 0);
}

string GameHandler::ExtractInitials(string name)
{
	string name_alphas = "";

	for(string::const_iterator name_iter = name.begin();
		name_iter != name.end();
		name_iter++)
	{
		if (isalpha(*name_iter))
		{
			name_alphas += toupper(*name_iter);
		}
	}

	
	name_alphas += "AAA";

	return name_alphas.substr(0, 3);
}

GameHandler::GameHandler(const Board& board, const GameGraphics& game_graphics)
: m_board(board), m_game_graphics(game_graphics)
{	
	m_max_scores = 15;
	m_entered_name = false;
	m_player_inits = "";
}

void GameHandler::Run()
{
	int cur_vert_count = 0;
	int old_vert_count = 1;
	bool no_exit_menu = true;
	bool update_menu = false;

	m_game_graphics.PrintMenu();

	while (no_exit_menu)
	{
		if (KeyPressed(VK_DOWN))
		{
			old_vert_count = cur_vert_count;
			cur_vert_count = ++cur_vert_count % 4;
			update_menu = true;
		}
		else if (KeyPressed(VK_UP))
		{
			old_vert_count = cur_vert_count;
			cur_vert_count = max<int>(--cur_vert_count % 4, (--cur_vert_count + 4) % 4);
			update_menu = true;
		}
		else if (KeyPressed(VK_RETURN))
		{			
			switch (cur_vert_count)
			{
				case 0:
				{
					StartNewGame(false);
					break;
				}
				case 1:
				{
					StartNewGame(true);
					break;
				}
				case 2:
				{
					HighScores(m_board.SortHighScores());
					break;
				}
				case 3:
				{
					no_exit_menu = false;
					break;
				}
				default:
				{
					break;
				}
			}


			if (no_exit_menu)
			{
				m_game_graphics.ClearBoard();
				m_game_graphics.PrintMenu();
				cur_vert_count = 0;
				old_vert_count = 1;
				
			}
		}

		if (update_menu)
		{
			m_game_graphics.UpdateMenu(old_vert_count, cur_vert_count);
			update_menu = ! update_menu;
			GameGraphics::Sleep(200);
		}
	}
}

void GameHandler::StartNewGame(bool hard_mode)
{
	bool game_not_over = true;
	int game_status = 1;
	long count = -1;

	m_board.SetDifficulty(hard_mode);

	m_game_graphics.PrintCurBoard(m_board.GetBoardState(),
								  false);
	m_game_graphics.UpdateSprites(m_board.GetSprites());
	m_game_graphics.PrintBoardInfo(m_board.GetBoardLevel() + 1,	
									m_board.GetASCIIManLives(),
									m_board.GetBoardScore());

	while(game_not_over)
	{		
		if (count != -1)
		{
			game_status = m_board.Go(count);
		}

		switch (game_status)
		{
			case 0:
			{
				m_game_graphics.UpdateBoard(m_board.GetBoardUpdates(), m_board.GetBoardState());
				m_game_graphics.UpdateSprites(m_board.GetSprites());
				m_game_graphics.UpdateInformation(m_board.GetInfoUpdates());
				break;
			}
			case 1:
			{
				count = 0;
				stringstream ss;
				
				m_game_graphics.UpdateSprites(m_board.GetSprites());
				m_game_graphics.UpdateBoard(m_board.GetBoardUpdates(), m_board.GetBoardState());
				
				if (count != -1)
				{
					GameGraphics::Sleep(1000);
				}

				m_board.Reset(false);

				if (count != 1)
				{
					m_game_graphics.PrintCurBoard(m_board.GetBoardState(),
												  false);
					m_game_graphics.UpdateSprites(m_board.GetSprites());
					m_game_graphics.UpdateInformation(m_board.GetInfoUpdates());
				}

				ss << m_board.GetBoardLevel();

				GameGraphics::Sleep(1000);

				m_game_graphics.PrintCenterMessage("-LEVEL: " + ss.str() + "-", 14, 1000, 400);
				m_game_graphics.PrintCenterMessage("-READY-", 14, 900, 300);
				m_game_graphics.PrintCenterMessage("-SET-", 14, 900, 300);
				m_game_graphics.PrintCenterMessage("-GO-", 14, 900, 300);

				m_game_graphics.PrintCurBoard(m_board.GetBoardState(),
											   false);
				m_game_graphics.UpdateSprites(m_board.GetSprites());
				
				break;
			}
			case 2:
			{
				bool inversed = true;

				m_game_graphics.UpdateSprites(m_board.GetSprites());
				m_game_graphics.UpdateBoard(m_board.GetBoardUpdates(), m_board.GetBoardState());

				GameGraphics::Sleep(500);

				for (int i = 0; i < 14; i++)
				{
					m_game_graphics.PrintCurBoard(m_board.GetBoardState(),
												   inversed);
					inversed = !inversed;
				}

				m_board.Reset(true);

				if (m_board.GetASCIIManLives() == 0)
				{	
					m_game_graphics.UpdateSprites(m_board.GetSprites());		
					game_not_over = false;
				}
				else
				{	
					m_game_graphics.PrintCurBoard(m_board.GetBoardState(),
												   false);
					m_game_graphics.UpdateSprites(m_board.GetSprites());				
					m_game_graphics.UpdateInformation(m_board.GetInfoUpdates());	

					GameGraphics::Sleep(1000);

					m_game_graphics.PrintCenterMessage("-READY-", 14, 900, 300);
					m_game_graphics.PrintCenterMessage("-SET-", 14, 900, 300);
					m_game_graphics.PrintCenterMessage("-GO-", 14, 900, 300);

					m_game_graphics.PrintCurBoard(m_board.GetBoardState(),
												   false);
					m_game_graphics.UpdateSprites(m_board.GetSprites());
				}
				break;
			}
			default:
			{
				break;
			}
		}
		
		m_game_graphics.UpdateInformation(m_board.GetInfoUpdates());
		GameGraphics::Sleep(max<int>(100, 150 - 2 * m_board.GetBoardLevel()));
		count++;
	}
	
	m_game_graphics.PrintCenterMessage("--GAME OVER--", 12, 2500, 500);

	UpdateHighScores(m_board.SortHighScores());
	m_board.WipeBoard();
}

void GameHandler::HighScores(const vector<pair<long,string>>& high_scores)
{
	m_game_graphics.ClearBoard();

	size_t num_scores = min<size_t>(high_scores.size(), 15);
	
	m_game_graphics.PrintCenterMessage("HIGH SCORES", 10, 0, 0, 0 - (num_scores / 2) - 3);
	m_game_graphics.PrintCenterMessage("NAME   RANK   SCORE", 14, 0, 0, 0 - (num_scores / 2) - 1);

	for (vector<pair<long,string>>::const_iterator& score_iter = high_scores.begin();
		 score_iter != high_scores.begin() + num_scores;
		 score_iter++)
	{
		int rank = score_iter - high_scores.begin() + 1;

		stringstream ss1;
		ss1 << score_iter->first;
		stringstream ss2;
		ss2 << rank;

		int score_name_dif = (ss1.str()).length() - (score_iter->second).length();
		string name_dif(max<int>(0, score_name_dif), ' ');
		string score_dif(abs(min<int>(0, score_name_dif)), ' ');
		string rank_dif(max<int>(0, 2 - (ss2.str()).length()), '.');

		m_game_graphics.PrintCenterMessage(name_dif + score_iter->second + 
										   "...." +
										   ss2.str() + rank_dif +
										   "..." + ss1.str() + score_dif,
										   15, 0, 0, 0 - (num_scores / 2) + rank, false, false);

	}

	m_game_graphics.PrintCenterMessage("-PRESS ENTER TO RETURN TO MENU-", 12, 0, 0, (num_scores / 2) + 4);

	bool enter_not_pressed = true;

	while (enter_not_pressed)
	{
		if (KeyPressed(VK_RETURN))
		{
			enter_not_pressed = false;
		}
	}

	m_game_graphics.ClearBoard(true);
}

void GameHandler::UpdateHighScores(vector<pair<long,string>>& high_scores)
{
	boolean inserted = false;
	size_t num_scores = min<size_t>(high_scores.size(), m_max_scores);
	vector<pair<long,string>>::iterator& high_score_iter = high_scores.begin();

	while((! inserted) && high_score_iter != (high_scores.begin() + num_scores))
	{
		if (m_board.GetBoardScore() > high_score_iter->first)
		{
			high_scores.insert(high_score_iter, pair<long, string> (m_board.GetBoardScore(), 
																	GetPlayerName()));
			if (num_scores == m_max_scores)
			{
				high_scores.pop_back();
			}

			inserted = true;

			WriteToHighScores(high_scores);
		}
		else
		{
			high_score_iter++;
		}
	}
	
	m_game_graphics.ClearBoard(true);
	HighScores(high_scores);
}

string GameHandler::GetPlayerName()
{
	if (! m_entered_name)
	{
		string name;

		m_entered_name = true;

		m_game_graphics.ClearBoard();

		m_game_graphics.PrintCenterMessage("", 0, 0, 0 - 5);

		cout << endl;

		m_game_graphics.SetColor(10);
		cout << "--YOU GOT A NEW" << endl;
		cout << "--TOP 15 SCORE!!" << endl;

		m_game_graphics.SetColor(14);
		cout << "--PLEASE ENTER YOUR 3 INITIALS BELOW" << endl;
		cout << "--FIRST 3 ALPHAS WILL BE TAKEN" << endl << endl;

		m_game_graphics.SetColor(15);

		cin >> name;
		m_player_inits = ExtractInitials(name);
	}
	
	return m_player_inits;	
}

void GameHandler::WriteToHighScores(const vector<pair<long,string>>& high_scores) const
{
	ofstream OutScores;

	OutScores.open(("HighScores" + m_board.GetBoardName()).c_str(), ios::out);

	if (OutScores.is_open())
	{
		for(vector<pair<long,string>>::const_iterator& write_iter = high_scores.begin();
			write_iter != high_scores.end();
			write_iter++)
		{
			if (write_iter - high_scores.begin() < m_max_scores)
			{
				stringstream ss;
				ss << write_iter->first;
				OutScores << ss.str() + "|" + write_iter->second << endl;
			}
			else
			{
				OutScores << endl;
			}
		}
	}

	OutScores.close();
}

