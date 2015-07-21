/*
Sprite.h
Written by John McGonagle on May 11, 2010.
*/
#ifndef _SPRITE_H
#define _SPRITE_H

#include <utility>
#include <vector>
#include <map>

using std::pair;
using std::vector;
using std::map;

enum Heading {LEFT, RIGHT, DOWN, UP};
enum Mode {NORMAL, ANGRY, SCARED, DEAD};

class Sprite
{
	public:
		Sprite(pair<int,int> cur_loc, int color, char symbol, Heading direction);
		virtual ~Sprite() {};
		pair<int,int> GetCurLocation() const;
		void SetCurLocation(pair<int,int> new_loc);
		void SetCurDirection(Heading new_direction);
		int GetCurColor() const;
		char GetSymbol() const;
		void Reset();
		Heading GetDirection() const;

		virtual void Move(vector<Heading>& valid_directions) {};
		virtual void Move(vector<Heading>& valid_directions, pair<int,int> asciiman_loc, long timer) {};
		virtual void MakeFeel(Mode mood) {};
		virtual Mode GetMood() const {return NORMAL;};
		virtual long GetNumberOfLives() const {return 0;};
		virtual void LoseALife() {};
		virtual void GainALife() {};

		
		static map<Heading,pair<int,int>> direction_offsets;
		static map<Heading,Heading> opposite_direction;

		static float FindDistance(pair<int,int> first_loc, pair<int,int> second_loc);

	protected:
		pair<int,int> m_start_loc;
		pair<int,int> m_cur_loc;
		int m_start_color;
		int m_cur_color;
		char m_symbol;
		Heading m_start_direction;
		Heading m_cur_direction;
};

class Ghost:public Sprite
{
	public:
		Ghost(pair<int,int> cur_loc, int color, char symbol, Heading direction, Mode mood, pair<int,int> home);
		void Move(vector<Heading>& valid_directions, pair<int,int> asciiman_loc, long timer);
		void MakeFeel(Mode mood);
		Mode GetMood() const;
		void Reset();

	private:
		Mode m_mood;
		int m_mood_full;
		int m_mood_count;
		pair<int,int> m_home_loc;

		int FindBestPath(const vector<Heading>& good_directions, pair<int,int> some_loc, bool closer) const;
};

class ASCIIMan:public Sprite
{
	public:
		ASCIIMan(pair<int,int> cur_loc, int color, char symbol, Heading direction, long lives);
		void Move(vector<Heading>& valid_directions);
		long GetNumberOfLives() const;
		void LoseALife();
		void GainALife();

		static bool KeyPressed(int key);

	private:
		long m_lives;
};

#endif