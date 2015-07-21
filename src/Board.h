/*
Board.h
Written by John McGonagle on May 8, 2010.
*/
#ifndef _BOARD_H
#define _BOARD_H

#include <string>
#include <vector>
#include <utility>
#include <map>
#include "Sprite.h"

using std::string;
using std::vector;
using std::pair;
using std::map;

class Board
{
	static vector<Heading> m_directions;
	static map<Heading,pair<int,int>> m_direction_offsets;
public:
	Board(const string& filename);
	Board() {};
	~Board() {};
	void Reset(bool same_level);
	void WipeBoard();
	void SetDifficulty(bool hard_mode);

	int Go(long timer);

	long GetASCIIManLives() const;
	long GetBoardScore() const;
	int GetBoardLevel() const;
	const string& GetBoardName() const;
	vector<vector<char>> GetBoardState() const;

	vector<pair<long,string>>& SortHighScores();
	pair<int,int> GetMaxAttributes() const;
	pair<int,int> GetBoardDim() const;
	pair<int,int> GetBoardOffset() const;

	vector<pair<int,int>> GetBoardUpdates() const;
	vector<Sprite*> GetSprites() const;
	vector<pair<int,long>> GetInfoUpdates() const;

	void PrintBoard() const;

	static bool CompareHighScores(pair<long,string> score_one, pair<long,string> score_two);

private:
	string m_board_name;
    long m_score;
    int m_level;

	bool m_hard_mode;
	bool m_new_life;

	int m_start_tokens_left;
	int m_cur_tokens_left;
	int m_start_coins_left;
	int m_cur_coins_left;
	
	vector<pair<long,string>> m_high_scores;

	pair<int,int> m_max_board_dim;
	pair<int,int> m_max_logo_dim;

    vector<vector<char>> m_new_board;
	vector<vector<char>> m_cur_board;
	vector<pair<int,int>> m_key_locs;
	vector<pair<int,int>> m_door_locs;
	vector<pair<int,int>> m_ghost_zone;
	pair<pair<int,int>,pair<int,int>> m_portal_locs;
	map<pair<int,int>,pair<int,int>> m_key_door_assocs;

	vector<pair<int,int>> m_board_updates;
	vector<pair<int,long>> m_info_updates;

	pair<int,int> m_asciiman_start;
	pair<int,int> m_ghost_home;
	vector<pair<int,int>> m_ghost_starts;
	vector<Sprite*> m_sprites;

    void ReadLogoBoardHighScore(const string& filename);
	void FindKeysDoorsPortals();
	void FindGhostZone();
	void CountTokensAndCoins();
	void AssociateKeysAndDoors();
	void FindSpritesStart();
	void CreateSprites();

	int UpdateBoard(pair<int,int> old_loc, pair<int,int> new_loc, char symbol, int index, int timer);
	vector<Heading> ValidMoves(bool intangible, pair<int,int> cur_loc) const;
	bool NoGhostHere(pair<int,int> pos_loc) const;
	Sprite* GhostAt(pair<int,int> pos_loc) const;
	void AddScore();
	void MakeSpritesFeel(Mode mood);
	pair<int,int> FindDoorToKey(pair<int,int> key_loc);
	bool InGhostZone(pair<int,int> new_loc) const;
};

#endif
