// 152120151079_AliKemalAY_ACA1819_App2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "mpi.h"
#include <vector>
#include <time.h>

using namespace std;

// PERPROCESS NO
#define N 5

float minVector(float *arr, int length) {
	float min = arr[0];
	for (int i = 0; i < length; i++) {
		if (min > arr[i])
			min = arr[i];
	}
	return min;
}

float maxVector(float *arr, int length) {
	float max = arr[0];
	for (int i = 0; i < length; i++) {
		if (max < arr[i])
			max = arr[i];
	}
	return max;
}

float sumVector(float *arr, int length) {
	float sum = 0;
	for (int i = 0; i < length; i++)
		sum += arr[i];
	return sum;
}

float avgVector(float *arr, int length) {
	float sum = sumVector(arr, length);
	return sum / length;
}

void whichOperation(int opID, float &Result, float *operationVector, int length) {
	if (opID == 0) {
		Result = sumVector(operationVector, length);
	}
	else if (opID == 1) {
		Result = minVector(operationVector, length);
	}
	else if (opID == 2) {
		Result = avgVector(operationVector, length);
	}
	else if (opID == 3) {
		Result = maxVector(operationVector, length);
	}
}

void printVector(float *arr, int length, int rank) {
	cout << " Rank(" << rank << "): LOCAL ARRAY\t: [ ";

	for (int i = 0; i < length; i++)
		cout << arr[i] << " ";

	cout << "]\n";
}

int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
	{
		srand(time(NULL));

		int opID, world_size, rankNo, master = 0, tag = 123;

		float *numberVector, *localVector, *localResults, localResult, globalResult;

		MPI_Comm_size(MPI_COMM_WORLD, &world_size);

		MPI_Comm_rank(MPI_COMM_WORLD, &rankNo);

		// ALLOCATE(numVct,locVct,locRslts) for ALL * * * * * * * * * * * * START * * * * * * * * * * * *
		{
			numberVector = (float*)malloc(0 * sizeof(float));
			localVector = (float*)malloc(N * sizeof(float));
			localResults = (float*)malloc(0 * sizeof(float));
		}
		// ALLOCATE(numVct,locVct,locRslts) for ALL * * * * * * * * * * * * END * * * * * * * * * * * * *

		if (rankNo == master) {
			{
				cout << " - - - - - - - - - - - - - - - - - - - -\n\n"
					<< " |\t\tACA_APP2\t\t|\n\n"
					<< " - - - - - - - - - - - - - - - - - - - -\n";
			}
			
			// ASSIGN OPID and ALLOCATE(numVct,locRslts) for MASTER * * * * * * * * * * * * START * * * * * * * * * * * *
			{
				numberVector = (float*)malloc((N*world_size) * sizeof(float));
				localResults = (float*)malloc(world_size * sizeof(float));
				opID = 3;
			}
			// ASSIGN OPID and ALLOCATE(numVct,locRslts) for MASTER * * * * * * * * * * * * END * * * * * * * * * * * *
			

			// SOME INFO PRINTING (PERPROCESS,MACHINES..) * * * * * * * * * * * * START * * * * * * * * * * * *
			{
				cout << " PERPROCESS\t: " << N << endl
					<< "\n MACHINES\t: " << world_size << endl
					<< "\n STARTING ARRAY\t: [ ";

				for (int i = 0; i < N * world_size; i++)
				{
					numberVector[i] = rand() % 50 + 5;
					if (i < 3) {
						cout << numberVector[i] << " ";
					}
					else if (i > (N * world_size) - (4)) {
						cout << numberVector[i] << " ";
					}
					else if (i == 5) {
						cout << ". . . ";
					}
				}
				cout << "]\n\n OPERATIONS\t- [0]:Sum , [1]:Min , [2]:Avg , [3]:Max\n\n"
					<< " SELECTED OPERATION ID\t: " << opID << endl << endl;
			}
			// SOME INFO PRINTING (PERPROCESS,MACHINES..) * * * * * * * * * * * * END * * * * * * * * * * * * 


			// MASTER SEND OPID * * * * * * * * * * * * START * * * * * * * * * * * *
			{
				for (int dest = 1; dest < (world_size); dest++)
				{
					MPI_Send(&opID, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
				}
			}
			// MASTER SEND OPID * * * * * * * * * * * * END * * * * * * * * * * * *
		}
		else {
			// SLAVES RECV OPID * * * * * * * * * * * * START * * * * * * * * * * * *
			{
				MPI_Recv(&opID, 1, MPI_INT, master, tag, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
			}
			// SLAVES RECV OPID * * * * * * * * * * * * END * * * * * * * * * * * *
		}

		// MASTER SCATTER NUMBERVECTOR * * * * * * * * * * * * START * * * * * * * * * * * *
		{
			MPI_Scatter(&numberVector[0], N, MPI_INT, &localVector[0], N, MPI_INT, master, MPI_COMM_WORLD);
		}
		// MASTER SCATTER NUMBERVECTOR * * * * * * * * * * * * END * * * * * * * * * * * * *


		// CALCULATE LOCAL RESULT * * * * * * * * * * * * START * * * * * * * * * * * *
		{
			whichOperation(opID, localResult, localVector, N);
			printVector(localVector, N, rankNo);
			cout << "\t  LOCAL RESULT \t: " << localResult << endl;
		}
		// CALCULATE LOCAL RESULT * * * * * * * * * * * * END * * * * * * * * * * * * *


		// GATHER OPERATION WITH INT? OR FLOAT? * * * * * * * * * * * * START * * * * * * * * * * * *
		{
			if (opID == 2)
				MPI_Gather(&localResult, 1, MPI_FLOAT, &localResults[rankNo], 1, MPI_FLOAT, master, MPI_COMM_WORLD);
			else
				MPI_Gather(&localResult, 1, MPI_INT, &localResults[rankNo], 1, MPI_INT, master, MPI_COMM_WORLD);
		}
		// GATHER OPERATION WITH INT? OR FLOAT? * * * * * * * * * * * * END * * * * * * * * * * * * *


		// CALC GLOBAL RESULT WITH LOCALRESULT * * * * * * * * * * * * START * * * * * * * * * * * *
		{
			if (rankNo == master) {
				cout << "\n Local results were gathered from ALL to MASTER . [ ";
				for (int i = 0; i < world_size; i++)
					cout << localResults[i] << " ";

				cout << "]" << endl << endl;

				whichOperation(opID, globalResult, localResults, world_size);

				cout << " Rank(" << rankNo << "): Global Operation = " << globalResult << endl;
			}
		}
		// CALC GLOBAL RESULT WITH LOCALRESULT * * * * * * * * * * * * END * * * * * * * * * * * * *

	}
	MPI_Finalize();
	return 0;
}