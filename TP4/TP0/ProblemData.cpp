#include"TP_Data.h"



int ProblemData::nS;
int ProblemData::nT;
int ProblemData::nH;
int ProblemData::seed;

std::vector<std::vector<int>>  ProblemData::CH;
int** ProblemData::HT;

void ProblemData::Populate_Parameters(RandLib RL)
{


	HT = new int* [nH]();
	nH++;

	HT[0] = new int[nT];
	for (int t = 0; t < nT; t++)
	{
		HT[0][t] = 0;
	}


	std::vector<int> v = { -1 };
	CH.push_back(v);

	for (int h = 1; h < nH; h++)
	{
		HT[h] = new int[nT];
		std::vector<int> v;
		for (int t = 0; t < nT; t++)
		{
			HT[h][t] = RL.randint(0, 1);
			if (HT[h][t] == 1)
			{
				v.push_back(t);
			}
		}
		if (!v.empty()) {
			CH.push_back(v);
		}	
		else {
			v.push_back(-1);
			CH.push_back(v);
		}
	}

}