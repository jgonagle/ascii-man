#include "Sprite.h"
#include <vector>
#include <map>
#include <utility>
#include <cstdlib>
#include <algorithm>
#include <windows.h>
#include <math.h>

using std::pair;
using std::vector;
using std::map;
using std::find;

map<Heading,pair<int,int>> Sprite::direction_offsets;
map<Heading,Heading> Sprite::opposite_direction;

float Sprite::FindDistance(pair<int,int> first_loc, pair<int,int> second_loc)
{
	return sqrt(pow((float)(first_loc.first - second_loc.first), 2) + pow((float) (first_loc.second - second_loc.second), 2));
}

Sprite::Sprite(pair<int,int> cur_loc, int color, char symbol, Heading direction)
{
	m_start_loc = cur_loc;
	m_cur_loc = cur_loc;
	m_start_color = color;
	m_cur_color = color;
	m_symbol = symbol;
	m_start_direction = direction;
	m_cur_direction = direction;

	static bool map_init = false;

	if (!map_init)
	{
		map_init = true;

		direction_offsets.insert(pair<Heading, pair<int,int>> (LEFT, pair<int,int> (-1, 0)));
		direction_offsets.insert(pair<Heading, pair<int,int>> (RIGHT, pair<int,int> (1, 0)));
		direction_offsets.insert(pair<Heading, pair<int,int>> (DOWN, pair<int,int> (0, 1)));
		direction_offsets.insert(pair<Heading, pair<int,int>> (UP, pair<int,int> (0, -1)));
		
		opposite_direction.insert(pair<Heading,Heading> (LEFT, RIGHT));
		opposite_direction.insert(pair<Heading,Heading> (RIGHT, LEFT));
		opposite_direction.insert(pair<Heading,Heading> (DOWN, UP));
		opposite_direction.insert(pair<Heading,Heading> (UP, DOWN));
	}
}

pair<int,int> Sprite::GetCurLocation() const
{
	return m_cur_loc;
}

void Sprite::SetCurLocation(pair<int,int> new_loc)
{
	m_cur_loc = new_loc;
}

void Sprite::SetCurDirection(Heading new_diriection)
{
	m_cur_direction = new_diriection;
}

void Sprite::Reset()
{
	m_cur_loc = m_start_loc;
	m_cur_direction = m_start_direction;
	m_cur_color = m_start_color;
}

int Sprite::GetCurColor() const
{
	return m_cur_color;
}

char Sprite::GetSymbol() const
{
	return m_symbol;
}

Heading Sprite::GetDirection() const
{
	return m_cur_direction;
}

Ghost::Ghost(pair<int,int> cur_loc, int color, char symbol, Heading direction, Mode mood, pair<int,int> home)
:Sprite(cur_loc, color, symbol, direction)
{
	m_mood = mood;
	m_home_loc = home;
	m_mood_full = 25;
}

void Ghost::Reset()
{
	Sprite::Reset();
	m_mood = NORMAL;
}

void Ghost::MakeFeel(Mode mood)
{
	switch (mood)
	{
		case NORMAL:
		{
			m_mood = NORMAL;
			m_cur_color = m_start_color;
			m_mood_count = 0;
			break;
		}
		case SCARED:
		{
			if (m_mood != DEAD)
			{
				m_mood = SCARED;
				m_cur_color = 10;
				m_mood_count = m_mood_full / 2;
			}
			break;
		}
		case ANGRY:
		{
			if (m_mood == NORMAL)
			{
				m_mood = ANGRY;
				m_cur_color = 12;
				m_mood_count = m_mood_full;
			}
			break;
		}
		default:
		{
			m_mood = DEAD;
			m_cur_color = 8;
			m_mood_count = 0;
			break;
		}
	}
}

Mode Ghost::GetMood() const
{
	return m_mood;
}

void Ghost::Move(vector<Heading>& valid_directions, pair<int,int> asciiman_loc, long timer)
{
	if (m_mood != SCARED || (timer % 2) != 0)
	{
		if (! (valid_directions.empty()))
		{
			int new_dir_index = 0;

			if (valid_directions.size() > 1)
			{
				vector<Heading>::const_iterator opposite_iter = find(valid_directions.begin(),
																	 valid_directions.end(),
																	 opposite_direction[m_cur_direction]);

				if (opposite_iter != valid_directions.end())
				{
					valid_directions.erase(opposite_iter);
				}

				switch (m_mood)
				{
					case ANGRY:
					{
						new_dir_index = FindBestPath(valid_directions, asciiman_loc, true);
						break;
					}
					case SCARED:
					{
						new_dir_index = FindBestPath(valid_directions, asciiman_loc, false);
						break;
					}
					case DEAD:
					{
						new_dir_index = FindBestPath(valid_directions, m_home_loc, true);
						break;
					}
					default:
					{
						new_dir_index = rand() % valid_directions.size();
					}
				}
			}

			m_cur_direction = valid_directions[new_dir_index];
			m_cur_loc.first += direction_offsets[valid_directions[new_dir_index]].first;
			m_cur_loc.second += direction_offsets[valid_directions[new_dir_index]].second;
		}
		
		if (m_mood_count > 1)
		{
			m_mood_count--;
		}
		else if (m_mood_count == 1)
		{
			MakeFeel(NORMAL);
		}
	}
}

int Ghost::FindBestPath(const vector<Heading>& good_directions, pair<int,int> some_loc, bool closer) const
{
	float best_distance = FindDistance(pair<int,int> (m_cur_loc.first + direction_offsets[*(good_directions.begin())].first, 
													   m_cur_loc.second + direction_offsets[*(good_directions.begin())].second),
										some_loc);
	int dir_index = 0;

	for(vector<Heading>::const_iterator good_dir_iter = good_directions.begin() + 1;
		good_dir_iter != good_directions.end();
		good_dir_iter++)
	{
		double distance_some_loc = FindDistance(pair<int,int> (m_cur_loc.first + direction_offsets[*good_dir_iter].first, 
														m_cur_loc.second + direction_offsets[*good_dir_iter].second),
										 some_loc);

		if (closer)
		{
			if (distance_some_loc < best_distance)
			{
				dir_index = good_dir_iter - good_directions.begin();
			}
		}
		else if (distance_some_loc > best_distance)
		{
			dir_index = good_dir_iter - good_directions.begin();
		}
	}

	return dir_index;
}

bool ASCIIMan::KeyPressed(int key) 
{
   return ((GetAsyncKeyState(key) & 0x8000) != 0);
}

ASCIIMan::ASCIIMan(pair<int,int> cur_loc, int color, char symbol, Heading direction, long lives)
:Sprite(cur_loc, color, symbol, direction)
{
	m_lives = lives;
}

long ASCIIMan::GetNumberOfLives() const
{
	return m_lives;
}

void ASCIIMan::LoseALife()
{
	m_lives--;
}

void ASCIIMan::GainALife()
{
	m_lives++;
}

void ASCIIMan::Move(vector<Heading>& valid_directions)
{
	if (KeyPressed(VK_LEFT) && (valid_directions.end() != find(valid_directions.begin(),
															   valid_directions.end(),
															   UP)))
	{
		m_cur_direction = UP;
	}
	else if (KeyPressed(VK_RIGHT) &&(valid_directions.end() != find(valid_directions.begin(),
																	valid_directions.end(),
																	DOWN)))
	{
		m_cur_direction = DOWN;
	}
	else if (KeyPressed(VK_DOWN) && (valid_directions.end() != find(valid_directions.begin(),
																	valid_directions.end(),
																	RIGHT)))
	{
		m_cur_direction = RIGHT;
	}
	else if (KeyPressed(VK_UP) && (valid_directions.end() != find(valid_directions.begin(),
																  valid_directions.end(),
																  LEFT)))
	{
		m_cur_direction = LEFT;
	}
	
	if (valid_directions.end() != find(valid_directions.begin(),
									   valid_directions.end(),
									   m_cur_direction))
	{
		m_cur_loc.first += direction_offsets[m_cur_direction].first;
		m_cur_loc.second += direction_offsets[m_cur_direction].second;
	}
}
