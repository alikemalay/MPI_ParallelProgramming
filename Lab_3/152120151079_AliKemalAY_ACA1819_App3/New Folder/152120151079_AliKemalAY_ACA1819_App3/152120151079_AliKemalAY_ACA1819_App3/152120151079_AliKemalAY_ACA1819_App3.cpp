// 152120151079_AliKemalAY_ACA1819_App3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include "mpi.h"
#include <time.h>
#include <Windows.h>
#include <string>

using namespace std;

#define PERPROCESS 5
#define MASTER 0

void printMatrix1D(int *vector, int length, string name); //prints the vector
void printMatrix2D(int **matrix, int countRow, int countColumn, string name); //prints the matrix

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

	//A = (int**)malloc(0); //Only MASTER uses this matrix
	//B = (int**)malloc(0); //Only MASTER uses this matrix
	//C = (int**)malloc(0); //Only MASTER uses this matrix
	/*rowA = (int*)malloc(countColumn * sizeof(int));
	rowB = (int*)malloc(countColumn * sizeof(int));
	rowC = (int*)malloc(countColumn * sizeof(int));
	statsA = (int*)malloc(countColumn * sizeof(int));
	statsB = (int*)malloc(countColumn * sizeof(int));*/

	A = new int*[0];
	B = new int*[0];
	C = new int*[0];
	
	rowA = new int[countColumn];
	rowB = new int[countColumn];
	rowC = new int[countColumn];

	statsA = new int[countColumn];
	statsB = new int[countColumn];

	

	/*cout << "------------ - rank:" << rank << "-------------\n";*/
	if (MASTER == rank) //MASTER
	{
		/*A = (int**)malloc(countRow * sizeof(int));*/
		A = new int*[countRow];
		
		/*A[0] = (int*)malloc(countRow*countColumn * sizeof(int));*/
		A[0] = new int[countColumn*countRow];

		for (int i = 1; i < countRow; i++) {
			/*A[i] = new int[countColumn];
			A[i] = A[0] + (i * countColumn);*/
			A[i] = A[0] + (i*countColumn);
			/*A[i] = new int[countColumn];*/
		}
		
		/*B = (int**)malloc(countRow * sizeof(int));*/
		B = new int*[countRow];
		/*B[0] = (int*)malloc(countRow*countColumn * sizeof(int));*/
		B[0] = new int[countColumn*countRow];
		
		for (int i = 1; i < countRow; i++){
			/*B[i] = new int[countColumn];
			B[i] = B[0] + (i * countColumn);*/
			B[i]= B[0] + (i*countColumn);
			/*B[i] = new int[countColumn];*/
		}
		
		C = new int*[countRow];
		/*C = (int**)malloc(countRow * sizeof(int));*/
		
		C[0] = new int[countColumn*countRow];
		
		/*C[0] = (int*)malloc(countRow*countColumn * sizeof(int));*/

		for (int i = 1; i < countRow; i++) {
			C[i] = C[0] + (i*countColumn);
		}
			

		// random initializations
		srand((unsigned)time(NULL));
		for (int i = 0; i < countRow; i++)
			for (int j = 0; j < countColumn; j++)
			{
				A[i][j] = rand() % 10;
				B[i][j] = rand() % 10;
			}

		printMatrix2D(A, countRow, countColumn, "A");
		printMatrix2D(B, countRow, countColumn, "B");

		rowA = A[0];
		

		for (int i = 1; i < size; i++) {
			MPI_Send(&A[i][0], countColumn, MPI_INT, i, tag, MPI_COMM_WORLD);
		}
	}

	else {
		MPI_Recv(&rowA[0], countColumn, MPI_INT, MASTER, tag, MPI_COMM_WORLD,&status);
	}
	
	
	MPI_Scatter(&B[0][0], countColumn, MPI_INT,
		&rowB[0], countColumn, MPI_INT, MASTER, MPI_COMM_WORLD);

	
	cout << "Rank(" << rank << ") ";
	printMatrix1D(rowA, countColumn, "RowA");

	cout << "Rank(" << rank << ") ";
	printMatrix1D(rowB, countColumn, "RowB");
	
	MPI_Allreduce(&rowA[0], &statsA[0],countColumn,MPI_INT,MPI_SUM,MPI_COMM_WORLD);

	cout << "Rank(" << rank << ") ";
	printMatrix1D(statsA, PERPROCESS, "Sta-A");
	cout << "Rank(" << rank << ") ";
	printMatrix1D(statsB, PERPROCESS, "Sta-B");

	MPI_Barrier(MPI_COMM_WORLD);
	cout << " - - - - - BARRIER 1- - - - - " << endl;
	MPI_Reduce(&rowB[0],&statsB[0],countColumn,MPI_INT,MPI_SUM,MASTER,MPI_COMM_WORLD);

	MPI_Bcast(&statsB[0],countColumn,MPI_INT,MASTER,MPI_COMM_WORLD);

	cout << "Rank(" << rank << ") ";
	printMatrix1D(statsA, countColumn, "Sta-A");
	cout << "Rank(" << rank << ") ";
	printMatrix1D(statsB, countColumn, "Sta-B");

	for (int i = 0; i < countColumn; i++) {
		rowC[i] = rowA[i] + rowB[i];
	}


	MPI_Barrier(MPI_COMM_WORLD);
	cout << " - - - - - BARRIER 2- - - - - " << endl;

	MPI_Gather(&rowC[0], countColumn, MPI_INT, &C[0][0], countColumn, MPI_INT, MASTER, MPI_COMM_WORLD);
	
	if (rank == MASTER) {
		cout << "IS GATHER ?" << endl;
		printMatrix2D(C, countRow, countColumn, "C");
	}
	
	/*free(A);
	free(B);
	free(C);
	free(rowA);
	free(rowB);
	free(rowC);
	free(statsA);
	free(statsB);*/
	delete[] A;
	delete[] B;
	delete[] C;

	MPI_Finalize();
}

void printMatrix1D(int *vector, int length, string name) //prints the vector
{
	cout  << name << " ( " << 1 << " x " << length  << " ):" ;
	for (int i = 0; i < length; i++)
		cout << vector[i] << " ";
	cout << "\n";
}

void printMatrix2D(int **matrix, int countRow, int countColumn, string name) //prints the matrix
{
	cout << " - - - - - - - - - PRINT2DMATRIX - - - - - - - - - " << endl;
	cout << "\t" << name << " (" << countRow << " x " << countColumn << " )\n";
	for (int i = 0; i < countRow; i++)
	{
		cout << "\t| ";
		for (int j = 0; j < countColumn; j++)
			cout << matrix[i][j] << " ";
		cout << "|\n";
	}
	cout << "\n";
	cout << " - - - - - - - - - - - - - - - - - - - - - - - - " << endl << endl;
}