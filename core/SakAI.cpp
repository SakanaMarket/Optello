#include <ics46/factory/DynamicFactory.hpp>
#include "SakAI.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

ICS46_DYNAMIC_FACTORY_REGISTER(OthelloAI, haqt::SakAI, "Sak Othello AI (Required)");

std::pair<int, int> haqt::SakAI::chooseMove(const OthelloGameState& state)
{
	// color: 0 = black, 1 = white
	int color = whatColor(state);
	
	// vector of all valid moves
	std::vector<std::pair<int, int>> nexus = nexon(state);

	// vector of that would score points
	std::vector<std::pair<int, int>> nexus_mkII;


	// comparing possible future scores to the best scores possible

	// defaulted to 0; will be reassigned
	int future_score = 0;

	// defaulted max score earned to 0; will be replaced
	int maximus_future_score = 0;

	for (int m = 0; m < nexus.size(); ++m)
	{
		// create clone to see into future and not mess with original
		std::unique_ptr<OthelloGameState> futuresight = state.clone();
		
		// makes future move on clone
		futuresight->makeMove(nexus[m].first, nexus[m].second);
		future_score = search(*futuresight, 3, color);
		
		// pushes all moves that score > than 0 and greater than previous max
		if (future_score >= maximus_future_score)
		{
			maximus_future_score = future_score;
			nexus_mkII.push_back(nexus[m]);
		}
	}

	// random generator
	// assumes if there are no "best moves"
	// randomly select from original move pool
	static std::default_random_engine gen;
	std::uniform_int_distribution<int> dis(0, nexus.size()-1);
	int p = dis(gen);

	//corners are prioritized
	std::pair <int, int> cornerz = corner(state, nexus_mkII);

	// precautionary step if there are no best moves
	if (nexus_mkII.size() == 0)
	{
		return nexus[p];
	}

	// else make a random best move
	else
	{
		// corners is preferred // make first corner move
		if (cornerz != std::pair<int, int> (1, 1))
		{
			return cornerz;
		}

		// best move is the last in the best move vector
		else
		{
			return nexus_mkII[nexus_mkII.size()-1];
		}
	}
}

int haqt::SakAI::evaluate(const OthelloGameState& state)
{
	// precaution in verifying actually is black
	if (state.isBlackTurn() && (whatColor(state) == 0))
	{
		// number of tiles belonging to black - number of tiles belong to white
		return (state.blackScore() - state.whiteScore());
	}
	// precaution in verifying actually is white
	else if (state.isWhiteTurn() && (whatColor(state) == 1))
	{
		// number of tiles belonging to white - number of tiles belong to black
		return (state.whiteScore() - state.blackScore());
	}
	
	// returns 0 if neither
	else
	{
		return 0;
	}	
}

int haqt::SakAI::search(const OthelloGameState& s, int depth, int color)
//Recursive search
{
	// assuming best case that current score to highets possible score
	int current_score = 0;

	// assuming opponent wins // worst case
	int opposing_score = s.board().width() * s.board().height();

	// possible to make a move that yields 0 score so defaulted to 0; reassigned later
	int possible_score = 0;

	//Find valid next moves
	std::vector<std::pair<int, int>> next = nexon(s);

	// returns current score of the board
	if (depth == 0)
	{
		return evaluate(s);
	}

	// returns maximum score // best interest score
	// if my turn // can't assume a color
	else if ((s.isBlackTurn() && color == 0) || (s.isWhiteTurn() && color == 1))
	{
		// for each valid move 
		for (int a = 0; a < next.size(); ++a)
		{
			// make move on s yielding s'
			// make clone to not mess with original state
			std::unique_ptr<OthelloGameState> theoretical = s.clone();
			theoretical->makeMove(next[a].first, next[a].second);
			// search s'
			possible_score = search(*theoretical, depth - 1, color);
			// compares for max
			// is possible is more than or equal to current
			// make current that possible score
			// possible should be maxed by end of loop
			if (possible_score >= current_score)
			{
				current_score = possible_score;
			}
		}

		return current_score;
	}

	// returns minimum score // opposing's best interest score
	// if not my turn
	else
	{
		// for each valid move opp can make
		for (int b = 0; b < next.size(); ++b)
		{
			// make move on s yielding s'
			// make clone to not mess with original state
			std::unique_ptr<OthelloGameState> theoretical = s.clone();
			theoretical->makeMove(next[b].first, next[b].second);
			// search s'
			possible_score = search(*theoretical, depth - 1, color);
			// compare for min 
			// if possible is less than or equal to opposing
			// make opposing that lower score
			// opposing should be minimum by end of loop
			if (possible_score <= opposing_score)
			{
				opposing_score = possible_score;
			}
		}

		return opposing_score;
	}	
}


int haqt::SakAI::whatColor(const OthelloGameState& state)
{
	// distinguish colors
	// black = 0 ; white = 1
	if (state.isBlackTurn())
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

std::vector<std::pair<int, int>> haqt::SakAI::nexon(const OthelloGameState& state)
{
	// Valid moves // not all are best
	std::vector<std::pair<int, int>> nexus;
	for (int i = 0; i < state.board().width(); ++i)
	{
		for (int x = 0; x < state.board().height(); ++x)
		{
			if (state.isValidMove(i,x))
			{
				nexus.push_back(std::pair<int, int> (i, x));
			}
		}
	}
	return nexus;
}

std::pair<int, int> haqt::SakAI::corner(const OthelloGameState& state, std::vector<std::pair<int, int>>& moves)
{
	// returns first corner pair move
	for (int g = 0; g < moves.size(); ++g)
		{
			if (// upper left
				(moves[g] == std::pair<int, int> (0, 0)) || 
				// lower right
				(moves[g] == std::pair<int, int> (state.board().width() - 1, state.board().height() - 1)) ||
				// lower left
				(moves[g] == std::pair<int, int> (0, state.board().height() - 1)) ||
				// upper right
				(moves[g] == std::pair<int, int> (state.board().width() - 1, 0)))
			{
				return moves[g];
			}

		}
	// random return that isn't a corner
	// used to comparison later
	return std::pair<int, int> (1, 1);
}

// ron-cadillac.ics.uci.edu