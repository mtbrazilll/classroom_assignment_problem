#include "ilcplex/ilocplex.h";  
#include <iostream>

using namespace std;
typedef IloArray<IloNumVarArray> NumVar2D;
typedef IloArray<NumVar2D> NumVar3D;

int main()
{
	// Criando o ambiente
	IloEnv env;
	IloModel Model(env, "Pasa");

	try
	{

#pragma region Dados do Problema

		int nS = 4;
		int nT = 6;
		int nH = 6;

		nH++;
		int** C = new int* [nH];

		C[0] = new int[nT] {0, 0, 0, 0, 0, 0};
		C[1] = new int[nT] {1, 1, 0, 0, 0, 0};
		C[2] = new int[nT] {1, 1, 0, 0, 0, 0};
		C[3] = new int[nT] {0, 0, 1, 1, 0, 0};
		C[4] = new int[nT] {0, 0, 1, 1, 0, 0};
		C[5] = new int[nT] {0, 0, 0, 0, 1, 1};
		C[6] = new int[nT] {0, 0, 0, 0, 1, 1};

		int** M = new int* [7];
		M[1] = new int[nT] {0, 1};
		M[2] = new int[nT] {0, 1};
		M[3] = new int[nT] {2, 3};
		M[4] = new int[nT] {2, 3};
		M[5] = new int[nT] {4, 5};
		M[6] = new int[nT] {4, 5};

#pragma endregion

#pragma region Variável de decisão
		NumVar2D x(env, nS);
		NumVar2D y(env, nS);
		NumVar2D z(env, nS);

		for (int s = 0; s < nS; s++) {
			x[s] = IloNumVarArray(env, nT, 0, 1, ILOINT);
			y[s] = IloNumVarArray(env, nH, 0, 1, ILOINT);
			z[s] = IloNumVarArray(env, nH, 0, 1, ILOINT);
		}

#pragma endregion

#pragma region Função Objetivo

		IloExpr exp0(env);

		for (int s = 0; s < nS; s++)
		{
			for (int h = 0; h < nH-1; h++)
			{
				exp0 += y[s][h];
			}

		}
		Model.add(IloMinimize(env, exp0));

#pragma endregion
		
#pragma region Restrições

		for (int t = 0; t < nT; t++)
		{
			IloExpr exp1(env);
			for (int s = 0; s < nS; s++)
			{
				exp1 += x[s][t];
			}

			Model.add(exp1 == 1);
		}
		
		for (int s = 0; s < nS; s++)
		{
			
			for (int i = 1; i < 7; i++)
			{
				IloExpr exp2(env);
				for (int j = 0; j < 2; j++)
				{
					exp2 += x[s][M[i][j]];
				}
				Model.add(exp2 <= 1);
			}

			for (int s = 0; s < nS; s++)
			{
				
				for (int h = 1; h < nH; h++)
				{
					IloExpr exp3(env);
					for (int i = 0; i < 2; i++)
					{
						exp3 += x[s][M[h][i]];

					}
					Model.add(exp3 <= z[s][h]);
				}
			}

			for (int s = 0; s < nS; s++)
			{
				Model.add(0 == z[s][0]);
			}

		for (int s = 0; s < nS; s++)
			{
				for (int h = 0; h < nH-1; h++)
				{
					IloExpr exp4(env);
					exp4 = z[s][h + 1] - z[s][h];
					Model.add(exp4 <= y[s][h]);
				}
			}
		}
#pragma endregion

		IloCplex cplex(Model);
		//cplex.setOut(env.getNullStream());

		
		if (!cplex.solve()) {
			env.error() << "Falhou ao otmizar o problema" << endl;
			throw(-1);
		}

		double obj = cplex.getObjValue();
		cout << "\n\n\tCusto otimo " << obj << endl; 
		//Obtendo a solução
			for (int s = 0; s < nS; s++)
			{
				for (int t = 0; t < nT; t++)
				{
					double Xval = cplex.getValue(x[s][t]);
					if (Xval > 0)
					{
						cout << "\t\t\t X[" << s << "][" << t << "] = " << Xval << endl;
					}

				}
			}


		//IloNumArray sol(env, n_ing);
		//cplex.getValues(sol, x);

		// Imprimindo a solução
		//for (int i = 0; i < n_ing; i++)
			//cout << "Quantidade do ingrediente " << i + 1 << ": " << sol[i] << endl;

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

