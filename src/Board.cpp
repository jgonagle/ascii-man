#include "Board.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <utility>
#include <map>
#include <ctype.h>

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::vector;
using std::pair;
using std::find;
using std::max;
using std::map;
using std::sort;

vector<Heading> Board::m_directions;
map<Heading,pair<int,int>> Board::m_direction_offsets;

bool Board::CompareHighScores(pair<long,string> score_one, pair<long,string> score_two)
{ 
	return (score_one.first > score_two.first);
}

Board::Board(const string& file_name)
{
	m_board_name = file_name;
	m_score = 0;
	m_level = 0;
	m_start_tokens_left = 0;
	m_cur_tokens_left = 0;
	m_start_coins_left = 0;
	m_cur_coins_left = 0;

	m_new_life = false;

	ReadLogoBoardHighScore(file_name);
	FindKeysDoorsPortals();
	CountTokensAndCoins();
	FindGhostZone();
	FindSpritesStart();

	static bool map_init = false;

	if (!map_init)
	{
		map_init = true;	

		m_directions.push_back(LEFT);
		m_directions.push_back(RIGHT);
		m_directions.push_back(DOWN);
		m_directions.push_back(UP);

		m_direction_offsets.insert(pair<Heading,pair<int,int>> (LEFT, pair<int,int> (-1, 0)));
		m_direction_offsets.insert(pair<Heading,pair<int,int>> (RIGHT, pair<int,int> (1, 0)));
		m_direction_offsets.insert(pair<Heading,pair<int,int>> (DOWN, pair<int,int> (0, 1)));
		m_direction_offsets.insert(pair<Heading,pair<int,int>> (UP, pair<int,int> (0, -1)));
	}
}

void Board::SetDifficulty(bool hard_mode)
{
	m_hard_mode = hard_mode;

	AssociateKeysAndDoors();
	CreateSprites();
}

const string& Board::GetBoardName() const
{
	return m_board_name;
}

void Board::ReadLogoBoardHighScore(const string& board_name)
{
	string logo_name = "ASCIIManLogo.txt";

	m_max_board_dim.first = 0;

	ifstream InBoard;

	InBoard.open(board_name.c_str());

	string part;
	string total = "";

	if (InBoard.is_open())
	{
		while (! InBoard.eof())
		{
			getline(InBoard, part);

			vector<char> line_chars;

			for (string::const_iterator string_iter = part.begin(); string_iter != part.end(); string_iter++)
			{
				line_chars.push_back(*string_iter);
			}

			m_max_board_dim.first = max<int>(m_max_board_dim.first, part.length());

			m_new_board.push_back(line_chars);
		}
	}

	InBoard.close();
	
	m_cur_board = m_new_board;
	m_max_board_dim.second = m_cur_board.size();
	
	m_max_logo_dim.first = 0;
	m_max_logo_dim.second = 0;

	ifstream InHighScores;

	InHighScores.open(("HighScores" + board_name).c_str());

	if (InHighScores.is_open())
	{
		while (!InHighScores.eof())
		{
			getline(InHighScores, part);
			m_high_scores.push_back(pair<long, string> (atol((part.substr(0, part.find_first_of('|'))).c_str()),
															  part.substr(part.find_first_of('|') + 1)));
		}
	}

	InHighScores.close();

	ifstream InLogo;

	InLogo.open(logo_name.c_str());

	if (InLogo.is_open())
	{
		while (!InLogo.eof())
		{
			getline(InLogo, part);
			cout << part << endl;

			m_max_logo_dim.first = max<int>(m_max_logo_dim.first, part.length());
			m_max_logo_dim.second++;
		}
	}

	InLogo.close();
}

vector<pair<long, string>>& Board::SortHighScores()
{
	for (vector<pair<long,string>>::const_iterator score_iter = m_high_scores.begin();
		 score_iter != m_high_scores.end();
		 score_iter++)
	{
		sort(m_high_scores.begin(), m_high_scores.end(), CompareHighScores);
	}

	return m_high_scores;
}

void Board::Reset(bool same_level)
{
	for (vector<Sprite*>::const_iterator sprite_iterator = m_sprites.begin();
		 sprite_iterator != m_sprites.end();
		 sprite_iterator++)
	{
		(*(*sprite_iterator)).Reset();
		
		if (same_level && (*(*sprite_iterator)).GetSymbol() == '@')
		{
			(*(*sprite_iterator)).LoseALife();
			m_info_updates.push_back(pair<int,int> (0, (*(*sprite_iterator)).GetNumberOfLives()));
		}
	}

	if (! same_level)
	{
		m_cur_board = m_new_board;
		m_level++;
		m_cur_tokens_left = m_start_tokens_left;
		m_cur_coins_left = m_start_coins_left;
		m_new_life = false;

		m_info_updates.push_back(pair<int,int> (2, m_level));
		m_board_updates.clear();
	}
}

void Board::WipeBoard()
{
	for (vector<Sprite*>::const_iterator sprite_iterator = m_sprites.begin();
		 sprite_iterator != m_sprites.end();
		 sprite_iterator++)
	{
		(*(*sprite_iterator)).Reset();
	}

	m_score = 0;
	m_level = 0;
	
	m_cur_board = m_new_board;
	m_cur_tokens_left = m_start_tokens_left;
	m_cur_coins_left = m_start_coins_left;
	m_new_life = false;

	m_board_updates.clear();
	m_info_updates.clear();
	m_sprites.clear();
	m_key_door_assocs.clear();
}

pair<int,int> Board::GetMaxAttributes() const
{
	return pair<int,int> (max<int>(m_max_board_dim.first, m_max_logo_dim.first),
						  m_max_board_dim.second + m_max_logo_dim.second + 5);
}

pair<int,int> Board::GetBoardDim() const
{
	return m_max_board_dim;
}

pair<int,int> Board::GetBoardOffset() const
{
	return pair<int,int> (max<int>((m_max_logo_dim.first - m_max_board_dim.first) / 2, 0),
						  m_max_logo_dim.second + 5);
}

long Board::GetASCIIManLives() const
{
	return m_sprites[0]->GetNumberOfLives();
}

long Board::GetBoardScore() const
{
	return m_score;
}

int Board::GetBoardLevel() const
{
	return m_level;
}

vector<vector<char>> Board::GetBoardState() const
{
	return m_cur_board;
}

void Board::FindKeysDoorsPortals()
{
	for (size_t i = 0; i < m_new_board.size(); i++)
	{
		for (size_t j = 0; j < m_new_board[i].size(); j++)
		{
			if (m_new_board[i][j] == '-' || m_new_board[i][j] == '|')
			{
				m_door_locs.push_back(pair<int,int> (i,j));
			}
			else if (m_new_board[i][j] == '%')
			{
				m_key_locs.push_back(pair<int,int> (i,j));
			}
			else if (m_new_board[i][j] == '[' || m_new_board[i][j] == ']')
			{
				if (m_new_board[i][j] == '[')
				{
					m_portal_locs.first = pair<int,int> (i, j);
				}
				else
				{
					m_portal_locs.second = pair<int,int> (i,j);
				}
			}
		}
	}
}

void Board::FindGhostZone()
{
	for (size_t i = 0; i < m_new_board.size(); i++)
	{
		for (size_t j = 0; j < m_new_board[i].size(); j++)
		{
			if (isalpha(m_new_board[i][j]) || m_new_board[i][j] == '^' ||
				m_new_board[i][j] == '=' || m_new_board[i][j] == '~')
			{
				if (m_new_board[i][j] == '~')
				{
					m_ghost_home = pair<int,int> (i,j);
				}

				m_ghost_zone.push_back(pair<int,int> (i,j));
			}
		}
	}
}

void Board::FindSpritesStart()
{
	for (size_t i = 0; i < m_new_board.size(); i++)
	{
		for (size_t j = 0; j < m_new_board[i].size(); j++)
		{
			if (m_new_board[i][j] == '!')
			{
				m_asciiman_start = pair<int,int>(i,j);
			}
			else if (isalpha(m_new_board[i][j]))
			{
				m_ghost_starts.push_back(pair<int,int> (i,j));
			}
		}
	}
}

void Board::CountTokensAndCoins()
{
	for (size_t i = 0; i < m_new_board.size(); i++)
	{
		for (size_t j = 0; j < m_new_board[i].size(); j++)
		{
			if (m_new_board[i][j] == '$')
			{
				m_start_tokens_left++;
			}
			else if (m_new_board[i][j] == '*')
			{
				m_start_coins_left++;
			}
		}
	}

	m_cur_tokens_left = m_start_tokens_left;
	m_cur_coins_left = m_start_coins_left;
}

void Board::AssociateKeysAndDoors()
{
	for (size_t i = 0; i < m_key_locs.size(); i++)
	{
		if (m_hard_mode)
		{
			pair<pair<int,int>,pair<int,int>> new_key_door_assoc (m_key_locs[i],m_door_locs[m_key_locs.size() - i-1]);
			m_key_door_assocs.insert(new_key_door_assoc);
		}
		else
		{
			pair<pair<int,int>,pair<int,int>> new_key_door_assoc (m_key_locs[i],m_door_locs[i]);
			m_key_door_assocs.insert(new_key_door_assoc);
		}
	}
}

void Board::CreateSprites()
{
	long lives;

	if (m_hard_mode)
	{
		lives = 2;
	}
	else
	{
		lives = 4;
	}

	m_sprites.push_back(new ASCIIMan (m_asciiman_start, 14, '@', UP, lives));

	for (vector<pair<int,int>>::const_iterator ghost_start_iter = m_ghost_starts.begin();
		 ghost_start_iter != m_ghost_starts.end();
		 ghost_start_iter++)
	{
		m_sprites.push_back(new Ghost (*ghost_start_iter, 15,'&', LEFT, NORMAL, m_ghost_home));
	}
}

int Board::Go(long timer)
{
	m_board_updates.clear();
	m_info_updates.clear();

	int update_status = 0;

	for(vector<Sprite*>::const_iterator sprite_go_iter = m_sprites.begin();
		sprite_go_iter != m_sprites.end();
		sprite_go_iter++)
	{
		pair<int,int> old_sprite_location = (*(*sprite_go_iter)).GetCurLocation();

		if ((*(*sprite_go_iter)).GetSymbol() == '@')
		{
			(*(*sprite_go_iter)).Move(ValidMoves(false, 
												 (*(*sprite_go_iter)).GetCurLocation()));
		}
		else
		{
			(*(*sprite_go_iter)).Move(ValidMoves(true, 
												(*(*sprite_go_iter)).GetCurLocation()),
									  (*(*m_sprites.begin())).GetCurLocation(), timer);
		}

		pair<int,int> new_sprite_location = (*(*sprite_go_iter)).GetCurLocation();

		update_status = UpdateBoard(old_sprite_location, 
									new_sprite_location, (*(*sprite_go_iter)).GetSymbol(),
									sprite_go_iter - m_sprites.begin(),
									timer);

		if (update_status != 0)
		{
			return update_status;
		}
	}

	return update_status;
}

int Board::UpdateBoard(pair<int,int> old_loc, pair<int,int> new_loc, char symbol, int index, int timer)
{
	if (old_loc != new_loc)
	{
		m_board_updates.push_back(old_loc);
	}
	
	if (symbol == '@')
	{
		if (NoGhostHere(new_loc) || 
			GhostAt(new_loc)->GetMood() == SCARED || 
			GhostAt(new_loc)->GetMood() == DEAD)
		{
			bool clear_it = true;

			if (m_cur_board[new_loc.first][new_loc.second] == '*')
			{
				AddScore();
				m_info_updates.push_back(pair<int,long> (1, m_score));

				m_cur_coins_left--;

				if (m_cur_coins_left == 0 && m_cur_tokens_left == 0)
				{
					return 1;
				}
			}
			else if (m_cur_board[new_loc.first][new_loc.second] == '%')
			{
				pair<int,int> door_unlocked_loc = FindDoorToKey(new_loc);
				m_cur_board[door_unlocked_loc.first][door_unlocked_loc.second] = ' ';
				m_board_updates.push_back(pair<int,int> (door_unlocked_loc.first, door_unlocked_loc.second));
			}
			else if (m_cur_board[new_loc.first][new_loc.second] == '$')
			{
				AddScore();
				m_info_updates.push_back(pair<int,long> (1, m_score));

				MakeSpritesFeel(SCARED);
				m_cur_tokens_left--;

				if (m_cur_coins_left == 0 && m_cur_tokens_left == 0)
				{
					return 1;
				}
			}
			else if (m_cur_board[new_loc.first][new_loc.second] == '[' ||
					 m_cur_board[new_loc.first][new_loc.second] == ']')
			{
				clear_it = false;	
			}

			if (m_level % 5 == 4 && (! m_new_life))
			{
				(*(m_sprites[index])).GainALife();
				m_info_updates.push_back(pair<int,long> (0, (*(m_sprites[index])).GetNumberOfLives()));
				m_new_life = true;
			}

			if (clear_it)
			{
				m_cur_board[new_loc.first][new_loc.second] = ' ';
			}
		}
		else if (GhostAt(new_loc)->GetMood() == SCARED)
		{
			GhostAt(new_loc)->MakeFeel(DEAD);
			AddScore();
		}
		else if (GhostAt(new_loc)->GetMood() != DEAD)
		{
			return 2;
		}
	}
	else if (InGhostZone(new_loc) && (*m_sprites[index]).GetMood() != NORMAL)
	{
		(*m_sprites[index]).MakeFeel(NORMAL);
	}
	else if (new_loc == (*m_sprites[0]).GetCurLocation())
	{
		if (m_sprites[index]->GetMood() == SCARED)
		{
			m_sprites[index]->MakeFeel(DEAD);
		}
		else if (m_sprites[index]->GetMood() != DEAD)
		{
			return 2;
		}
	}

	if (m_cur_board[new_loc.first][new_loc.second] == '[' ||
		m_cur_board[new_loc.first][new_loc.second] == ']')
	{
		if (m_cur_board[new_loc.first][new_loc.second] == '[')
		{
			(*(m_sprites[index])).SetCurLocation(m_portal_locs.second);
		}
		else
		{
			(*(m_sprites[index])).SetCurLocation(m_portal_locs.first);
		}

		m_board_updates.push_back(new_loc);
	}

	
	if (timer % 120 == 119)
	{
		MakeSpritesFeel(ANGRY);
	}

	return 0;
}

vector<Heading> Board::ValidMoves(bool intangible, pair<int,int> cur_loc) const
{
	vector<Heading> valid_directions;

	for(vector<Heading>::const_iterator direction_iter = m_directions.begin(); 
		direction_iter != m_directions.end();
		direction_iter++)
	{
		char over_here = m_cur_board[m_direction_offsets[(*direction_iter)].first + cur_loc.first]
									[m_direction_offsets[(*direction_iter)].second + cur_loc.second];

		if (over_here != '|' && over_here != '-' && over_here != '#')
		{
			if (intangible)
			{
				if (NoGhostHere(pair<int,int> (m_direction_offsets[(*direction_iter)].first + cur_loc.first,
											   m_direction_offsets[(*direction_iter)].second + cur_loc.second)))
				{
					valid_directions.push_back(*direction_iter);
				}
			}
			else
			{
				if (over_here != '=' && over_here != '~')
				{
					valid_directions.push_back(*direction_iter);
				}
			}
		}
	}

	return valid_directions;
}

bool Board::NoGhostHere(pair<int,int> pos_loc) const
{
	vector<pair<int,int>> ghost_locs;

	for (vector<Sprite*>::const_iterator sprites_loc_iter = m_sprites.begin();
		 sprites_loc_iter != m_sprites.end();
		 sprites_loc_iter++)
	{
		if ((*(*sprites_loc_iter)).GetSymbol() != '@')
		{
			ghost_locs.push_back((*(*sprites_loc_iter)).GetCurLocation());
		}
	}

	return (ghost_locs.end() == find(ghost_locs.begin(), ghost_locs.end(), pos_loc));
}

Sprite* Board::GhostAt(pair<int,int> pos_loc) const
{
	for (vector<Sprite*>::const_iterator sprites_loc_iter = m_sprites.begin();
		 sprites_loc_iter != m_sprites.end();
		 sprites_loc_iter++)
	{
		if ((*(*sprites_loc_iter)).GetSymbol() != '@' &&
			(*(*sprites_loc_iter)).GetCurLocation() == pos_loc)
		{
			return (*sprites_loc_iter);
		}
	}

	return (*(m_sprites.begin()));
}


void Board::AddScore()
{
	m_score += (((m_level / 5) + 1) * 10);
}

void Board::MakeSpritesFeel(Mode mood)
{
	for(vector<Sprite*>::const_iterator sprite_scared_iter = m_sprites.begin();
		sprite_scared_iter != m_sprites.end();
		sprite_scared_iter++)
	{
		if ((*(*sprite_scared_iter)).GetSymbol() != '@' && 
			(! InGhostZone((*(*sprite_scared_iter)).GetCurLocation())))
		{
			(*(*sprite_scared_iter)).MakeFeel(mood);
		}
	}
}

pair<int,int> Board::FindDoorToKey(pair<int,int> key_loc)
{
	return m_key_door_assocs[key_loc];
}

bool Board::InGhostZone(pair<int,int> new_loc) const
{
	return (m_ghost_zone.end() != find(m_ghost_zone.begin(), m_ghost_zone.end(), new_loc));
}

vector<pair<int,int>> Board::GetBoardUpdates() const
{
	return m_board_updates;
}

vector<Sprite*> Board::GetSprites() const
{
	return m_sprites;
}

vector<pair<int,long>> Board::GetInfoUpdates() const
{
	return m_info_updates;
}

void Board::PrintBoard() const
{
	for (size_t i = 0; i < m_cur_board.size(); i++)
	{
		for (size_t j = 0; j < m_cur_board[i].size(); j++)
		{
			cout << m_cur_board[i][j];
		}

	cout << endl;
	}
}
