#ifndef SAKAI_HPP
#define SAKAI_HPP

#include "OthelloAI.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

namespace haqt
{
	class SakAI : public OthelloAI
	{
	public:
		virtual std::pair<int, int> chooseMove(const OthelloGameState& state);
		int evaluate(const OthelloGameState& state);
		int search(const OthelloGameState& s, int depth, int color);
		int whatColor(const OthelloGameState& state);
		std::vector<std::pair<int, int>> nexon(const OthelloGameState& state);
		std::pair<int, int> corner(const OthelloGameState& state, std::vector<std::pair<int, int>>& moves);
	};
}

#endif