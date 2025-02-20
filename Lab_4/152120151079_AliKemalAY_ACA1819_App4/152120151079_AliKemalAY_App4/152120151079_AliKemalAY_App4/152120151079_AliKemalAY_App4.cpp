// 152120151079_AliKemalAY_ACA1819_App4.cpp


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
#define STRLEN 100
#define BUFFSIZE 100

void printMatrix1D(int *vector, int length, char name); //prints the vector
void printMatrix2D(int **matrix, int countRow, int countColumn, char name); //prints the matrix

int main(int argc, char **argv) {
	int rank, size, countRow, countColumn, tag = 0, position,
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

	
	// A = (int**)malloc(0); //Only MASTER uses this matrix
	//B = (int**)malloc(0); //Only MASTER uses this matrix
	C = (int**)malloc(0); //Only MASTER uses this matrix


	/*myArray = (int*)malloc((size*countColumn)*sizeof(int));*/

	rowA = (int*)malloc(countColumn * sizeof(int));
	rowB = (int*)malloc(countColumn * sizeof(int));
	rowC = (int*)malloc(countColumn * sizeof(int));
	statsA = (int*)malloc(countColumn * sizeof(int));
	statsB = (int*)malloc(countColumn * sizeof(int));


	char myBuffer[BUFSIZ];         

	/* now let's pack all those values into a single message */

	A = (int**)malloc(countRow * sizeof(int));
	A[0] = (int*)malloc(countRow*countColumn * sizeof(int));


	B = (int**)malloc(countRow * sizeof(int));
	B[0] = (int*)malloc(countRow*countColumn * sizeof(int));

	cout << "------------ - rank:" << rank << "-------------\n";
	if (MASTER == rank) //MASTER
	{
		/*A = (int**)malloc(countRow * sizeof(int));
		A[0] = (int*)malloc(countRow*countColumn * sizeof(int));*/
		for (int i = 1; i < countRow; i++)
			A[i] = A[0] + (i * countColumn);

		//B = (int**)malloc(countRow * sizeof(int));
		//B[0] = (int*)malloc(countRow*countColumn * sizeof(int));
		for (int i = 1; i < countRow; i++)
			B[i] = B[0] + (i * countColumn);


		//random initializations
		srand((unsigned)time(NULL));
		for (int i = 0; i < countRow; i++)
			for (int j = 0; j < countColumn; j++)
			{
				A[i][j] = rand() % 10;
				B[i][j] = rand() % 10;
			}

		//printMatrix2D(A, countRow, countColumn, 'A');
		//printMatrix2D(B, countRow, countColumn, 'B');

		
		

		


		for (int i = 1; i < size; i++) {

			position = 0;

			rowA = A[i-1];
			rowB = B[i-1];
/*
			cout << " RANK (MASTER)" << endl;

			cout << "ROW - A : " << *rowA;
			cout << " ara ";
			cout << "ROW - B : " << *rowB;*/

			MPI_Pack(&rowA[0], countColumn, MPI_INT, myBuffer, BUFFSIZE,
				&position, MPI_COMM_WORLD);

			MPI_Pack(&rowB[0], countColumn, MPI_INT, myBuffer, BUFFSIZE,
				&position, MPI_COMM_WORLD);

			MPI_Send(myBuffer,BUFFSIZE, MPI_PACKED, i, tag, MPI_COMM_WORLD);
		}

		

	}

	else {

		MPI_Recv(myBuffer, BUFFSIZE, MPI_PACKED, MASTER, tag, MPI_COMM_WORLD, &status);

		position = 0;

		MPI_Unpack(myBuffer, BUFFSIZE, &position, &rowA[0], countColumn, MPI_INT, MPI_COMM_WORLD);

		MPI_Unpack(myBuffer, BUFFSIZE, &position, &rowB[0], countColumn, MPI_INT, MPI_COMM_WORLD);
		
		cout << " RANK (slave)" << endl;
		cout << "ROW - A : " << *rowA;
		cout << " ara ";
		cout << "ROW - B : " << *rowB;


	}

	

	MPI_Finalize();
}

void printMatrix1D(int *vector, int length, char name) //prints the vector
{
	cout << name << "(" << 1 << "x" << length << "):";
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