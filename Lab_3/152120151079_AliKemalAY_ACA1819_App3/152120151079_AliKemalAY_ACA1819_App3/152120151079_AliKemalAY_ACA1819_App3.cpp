// 152120151079_AliKemalAY_ACA1819_App3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include "mpi.h"
#include <time.h>
#include <Windows.h>
#include <string.h>

using namespace std;

#define PERPROCESS 5
#define MASTER 0

void printMatrix1D(int *vector, int length, char name); //prints the vector
void printMatrix2D(int **matrix, int countRow, int countColumn, char name); //prints the matrix

int main(int argc, char **argv){
	int rank, size, countRow, countColumn, tag = 0,
		**A,		//operand 1 - Master creates randomly and sends each row, for only MASTER processor
		**B,		//operand 2 - Master creates randomly and scatters, for only MASTER processor
		**C,		//Gathered result for the operation, for only MASTER processor (C = A + B)
		*rowA,		//Received row from MASTER
		*rowB,		//Scatterred row, root is MASTER
		*rowC,		//will be allgathered
		*statsA,	//Allreduceded sum stats for A
		*statsB;	//Reduceded&Broadcested sum stats for B	

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	
	countRow = size;
	countColumn = PERPROCESS;

	A = (int**)malloc(0); //Only MASTER uses this matrix
	B = (int**)malloc(0); //Only MASTER uses this matrix
	C = (int**)malloc(0); //Only MASTER uses this matrix
	rowA = (int*)malloc(countColumn * sizeof(int));
	rowB = (int*)malloc(countColumn * sizeof(int));
	rowC = (int*)malloc(countColumn * sizeof(int));
	statsA = (int*)malloc(countColumn * sizeof(int));
	statsB = (int*)malloc(countColumn * sizeof(int));

	cout << "------------ - rank:" << rank << "-------------\n";
	if (MASTER == rank) //MASTER
	{
		A = (int**)malloc(countRow * sizeof(int));
		A[0] = (int*)malloc(countRow*countColumn * sizeof(int));
		for (int i = 1; i < countRow; i++)
			A[i] = A[0] + (i * countColumn);

		B = (int**)malloc(countRow * sizeof(int));
		B[0] = (int*)malloc(countRow*countColumn * sizeof(int));
		for (int i = 1; i < countRow; i++)
			B[i] = B[0] + (i * countColumn);

		C = (int**)malloc(countRow * sizeof(int));
		C[0] = (int*)malloc(countRow*countColumn * sizeof(int));
		for (int i = 1; i < countRow; i++)
			C[i] = C[0] + (i * countColumn);

		//random initializations
		srand((unsigned)time(NULL));
		for (int i = 0; i < countRow; i++)
			for (int j = 0; j < countColumn; j++)
			{
				A[i][j] = rand() % 10;
				B[i][j] = rand() % 10;
			}

		printMatrix2D(A, countRow, countColumn, 'A');
		printMatrix2D(B, countRow, countColumn, 'B');

		rowA = A[0];
		

		for (int i = 1; i < size; i++) {
			MPI_Send(&A[i][0], PERPROCESS, MPI_INT, i, tag, MPI_COMM_WORLD);
		}

	}

	else {
		MPI_Recv(&rowA[0], PERPROCESS, MPI_INT, MASTER, tag, MPI_COMM_WORLD,&status);	
	}
	
	MPI_Scatter(&B[0][0], PERPROCESS, MPI_INT,
		&rowB[0], PERPROCESS, MPI_INT, MASTER, MPI_COMM_WORLD);

	cout << "ROWS" << endl;
	printMatrix1D(rowA, PERPROCESS, 'rowA');
	printMatrix1D(rowB, PERPROCESS, 'rowB');
	
	MPI_Allreduce(&rowA[0], &statsA[0],PERPROCESS,MPI_INT,MPI_SUM,MPI_COMM_WORLD);

	
	cout << "STATS" << endl;
	printMatrix1D(statsA, PERPROCESS, 'staA');
	printMatrix1D(statsB, PERPROCESS, 'staB');


	MPI_Reduce(&rowB[0], &statsB[0], PERPROCESS, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);

	MPI_Bcast(&statsB[0], PERPROCESS, MPI_INT, MASTER, MPI_COMM_WORLD);

	cout << "AFTER GATHER STATS" << endl;
	printMatrix1D(statsA, PERPROCESS, 'staA');
	printMatrix1D(statsB, PERPROCESS, 'staB');

	
	for (int i = 0; i < PERPROCESS; i++) {
		rowC[i] = rowA[i] + rowB[i];
	}
	
	MPI_Gather(&rowC[0],PERPROCESS,MPI_INT,&C[0][0],PERPROCESS,MPI_INT,MASTER,MPI_COMM_WORLD);

	if (rank == MASTER) {
		cout << "AFTER SUM A+B " << endl;
		printMatrix2D(C, countRow, countColumn, 'C');
	}
	//printMatrix1D(rowB, PERPROCESS, 'rowB');
	/*free(A);
	free(B);
	free(C);
	free(rowA);
	free(rowB);
	free(rowC);
	free(statsA);
	free(statsB);*/

	MPI_Finalize();
}

void printMatrix1D(int *vector, int length, char name) //prints the vector
{
	cout  << name << "(" << 1 << "x" << length  << "):" ;
	for (int i = 0; i < length; i++)
		cout << vector[i] << " ";
	cout << "\n";
}

void printMatrix2D(int **matrix, int countRow, int countColumn, char name) //prints the matrix
{
	cout << name << "(" << countRow << "x" << countColumn << ")\n";
	for (int i = 0; i < countRow; i++)
	{
		cout << "| ";
		for (int j = 0; j < countColumn; j++)
			cout << matrix[i][j] << " ";
		cout << "|\n";
	}
	cout << "\n";
}