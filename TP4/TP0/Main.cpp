#include "ilcplex/ilocplex.h";  
#include <iostream>
#include<fstream>;
#include<chrono>;
#include<random>;
#include "RandGen.h"
#include"TP_Data.h"


using namespace std;
typedef IloArray<IloNumVarArray> NumVar2D;
typedef IloArray<NumVar2D> NumVar3D;
#define all(x) (x).begin(), (x).end()

int buracos(vector<vector<int>> SH) {

	int contador = 0;
	for (auto s : SH) {
		for (int i = 0; i < s.size() - 1; i++) {
			if (s[i] == -1 && s[i + 1] != -1) {
				contador++;
			}
		}
	}
	
	return contador;
}



int main()
{	
	// Criando o ambiente
	IloEnv env;
	IloModel Model(env, "Pasa");
	auto start = chrono::high_resolution_clock::now();

	try
	{

#pragma region Definição  dos Dados

		
		ProblemData::nT = 5;
		ProblemData::nS = 6;
		ProblemData::nH = 10;
		

		ProblemData::seed = 39;
		RandLib RL(ProblemData::seed);
		ProblemData::Populate_Parameters(RL);

#pragma endregion

#pragma region Recepção dos Dados

		int nS = ProblemData::nS;
		int nT = ProblemData::nT;
		int nH = ProblemData::nH;
		int seed = 39;
		// create and object of RandLib
		
		int** HT = ProblemData::HT;
		std::vector<std::vector<int>>CH = ProblemData::CH;
		
		for (int h = 0; h < nH; h++)
		{
			for (int t = 0; t < nT; t++)
			{
				printf("%d ", HT[h][t]);
			}
			printf("\n");
		}
		printf("\n");
		for (int h = 0; h < nH; h++)
		{
			for (int t = 0; t < CH[h].size(); t++)
			{
				printf("%d ", CH[h][t]);
			}
			printf("\n");
		}

		cout << endl << "qnt CH: "<< CH.size() << endl;
		
		for (auto v : CH) {
			cout << v.size() << endl;
		}
#pragma endregion

#pragma region Variável de decisão
		NumVar2D x(env, nS);
		NumVar2D y(env, nS);
		NumVar2D z(env, nS);

		for (int s = 0; s < nS; s++) {
			x[s] = IloNumVarArray(env, nT, 0, 1, ILOINT);
			y[s] = IloNumVarArray(env, nH-1, 0, 1, ILOINT);
			z[s] = IloNumVarArray(env, nH, 0, 1, ILOINT);
		}
		/*
		for (int s = 0; s < nS; s++)
		{
			for (int t = 0; t < nT; t++)
			{
				char nome[10];
				sprintf_s(nome, "x[%d][%d]", s, t);
				x[s][t].setName(nome);
			}
		}

		for (int s = 0; s < nS; s++)
		{
			for (int h = 0; h < nH; h++)
			{
				char nome[10];
				sprintf_s(nome, "z[%d][%d]", s, h);
				z[s][h].setName(nome);

			}
		}
		for (int s = 0; s < nS; s++)
		{
			for (int h = 0; h < nH-1; h++)
			{
				char nome[10];
				sprintf_s(nome, "y[%d][%d]", s, h);
				y[s][h].setName(nome);

			}
		}
		*/
#pragma endregion

#pragma region Função Objetivo

		IloExpr exp0(env);

		for (int s = 0; s < nS; s++)
		{
			for (int h = 0; h < nH - 1; h++)
			{
				exp0 += y[s][h];
			}

		}
		Model.add(IloMinimize(env, exp0));

#pragma endregion

#pragma region Restrições
		
		//toda turma deve ser alocada à sala de aula
		for (int t = 0; t < nT; t++)
		{
			IloExpr exp1(env);
			for (int s = 0; s < nS; s++)
			{
				exp1 += x[s][t];
			}

			Model.add(exp1 == 1);
		}
		/*
		// não pode haver choque de salas
		for (int s = 0; s < nS; s++)
		{
			for (int h = 1; h < nH; h++)
			{
				IloExpr exp2(env); // somatório de xsi das turmas que acontecem no horario i, sala s fixa
				for (int t = 0; t < CH[h].size(); t++)
				{
					exp2 += x[s][CH[h][t]];
				}
				if(CH[h].size()>=1 && CH[h][0] != -1) Model.add(exp2 <= 1);
			}

		}
		*/
		
		// zsh, diz se a sala está sendo usada no horario i
		for (int s = 0; s < nS; s++)
		{	
			for (int h = 1; h < nH; h++) // dado um horario i, se tem um conjunto de aulas 
			{
				IloExpr exp3(env);
				for (int t = 0; t < CH[h].size(); t++)
				{
					exp3 += x[s][ CH[h][t] ];
				}
				// -1 é a flag pra quando não existe nenhuma aula naquele horário 
				if (CH[h].size() >= 1 && CH[h][0] != -1) Model.add(exp3 == z[s][h]);

			}
		}
		
		// horario artificial (h = 0) não tem nenhuma aula alocada.
		for (int s = 0; s < nS; s++)
		{
			Model.add(0 == z[s][0]);
		}
		
		for (int s = 0; s < nS; s++)
		{
			for (int h = 0; h < nH - 1; h++)
			{
				IloExpr exp4(env);
				int _h = h + 1;
				exp4 = z[s][_h] - z[s][h];
				Model.add(exp4 <= y[s][h]);
			}
		}
		
#pragma endregion
		
#pragma region solver
		IloCplex cplex(Model);
		cplex.setOut(env.getNullStream());
		cplex.setParam(IloCplex::TiLim, 120);

		//cplex.exportModel("tabela_horarios.lp");
		if (!cplex.solve()) {
			env.error() << "Falhou ao otmizar o problema" << endl;
			throw(-1);
			//cout << cplex.getStatus() << endl;
			
		}

		double obj = cplex.getObjValue();
		auto end = chrono::high_resolution_clock::now();
		auto Elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
		cout << "\t Elapsed Time(ms): " << Elapsed.count() << endl;
		cout << "\n\n\tCusto otimo " << obj << endl;

#pragma endregion

#pragma region Arquivo de Saída

		ofstream fid;
		string FileName = "Pasa_" + std::to_string(nT) + "_" + std::to_string(nS) + "_" + std::to_string(nH-1) + ".txt";

		fid.open(FileName);
		fid<< "\n\n\n\tProblem Size (nT x nS x nH):" << nT << "x" << nS << "x" << nH-1 << endl;
		fid << "\tCPU Time(sec): " << Elapsed.count() / 100 << endl;
		fid << "\tObj Value: " << obj << endl << endl;

	
		for (int h = 0; h < nH; h++)
		{
			for (int s = 0; s < nS; s++)
			{
				double zval = cplex.getValue(z[s][h]);
				fid << "\t\t\t" << zval << " ";

			}
			fid << endl;
		}
		fid << endl << endl;
		for (int s = 0; s < nS; s++)
		{
			for (int t = 0; t < nT; t++)
			{
				double Xval = cplex.getValue(x[s][t]);
				if (Xval > 0)
				{
					fid << "\t\t\t X[" << s << "][" << t << "] = " << Xval << endl;
				}
			}
		}
		// matris salas x horarios
		vector<vector<int>> HS;
		for (int h = 0; h < nH; h++)
		{
			vector<int> v(nS, 0);
			HS.push_back(v);
		}

		vector<vector<int>> SH;
		for (int s = 0; s < nS; s++)
		{
			vector<int> v(nH, -1);
			SH.push_back(v);
		}

		for (int h = 1; h < nH; h++)
		{
			for (auto t : CH[h]) {
				for (int s = 0; s < nS; s++) {
					double Xval = cplex.getValue(x[s][t]);
					if (Xval > 0) {
						HS[h][s] = t;
						SH[s][h] = t;
					}
				}
				
			}
		}

		fid << endl << endl;

		for (auto h : HS) 
		{
			for (auto t : h)
			{
				fid << "\t\t\t" << t << " ";

			}
			fid << endl;
		}

		int contador = buracos(SH);
		fid << endl << "\t\t\t" << "Buracos: " << contador << " ";
		


		fid.close();

		ofstream fid2;
		fid2.open("TP_Results.txt", std::ios::app);
		fid2 << "\n\t" << nS;
		fid2 << "-" << nT;
		fid2 << "-" << nH-1;
		fid2 << "-" << ProblemData::seed;

		fid2 << "\t\t" << obj;
		fid2 << "\t" << Elapsed.count() / 100;
		fid2.close();

#pragma endregion

		
	}
	catch (const IloException& e)
	{
		cerr << "Exception caught: " << e << endl;
	}
	catch (...)
	{
		cerr << "Unknown exception caught!" << endl;
	}

	env.end();
	return 0;
}

