#pragma once

#include"RandGen.h"

class ProblemData
{
public:
	static int nS;
	static int nT;
	static int nH;
	static int seed;

	static std::vector<std::vector<int>>  CH;
	static int** HT;
	static void Populate_Parameters(RandLib RL);
};