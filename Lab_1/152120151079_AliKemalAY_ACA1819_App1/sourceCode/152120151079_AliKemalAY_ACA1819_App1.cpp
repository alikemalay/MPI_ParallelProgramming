#include "pch.h"
#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

/** \brief Makine basina tutulan rakam miktarini tanimlar.
*/
#define numCount 8 

/** \brief Gonderilen vectorun, sifirlar(0) haric tutularak, elemanlarinin ortalamasini hesaplar.
* \param v Ortalamasi hesaplanacak vectoru tanimlar.
* \return Hesaplanan ortalamayi dondurur.
*/
float vectorAvarage(vector<int> v) {
	float sum = 0, cnt = 0;
	for (int i = 0; i < v.size(); i++) {
		if (v[i] != 0) {
			cnt++;
			sum += v[i];
		}
	}
	return sum / cnt;
}

int main(int argc, char*argv[]) {
	int rank, macCount,tag=15,master=0;

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Comm_size(MPI_COMM_WORLD, &macCount);

	vector<int> numVector(numCount*(macCount));
	vector<float> avarage(macCount - 1);

	if (rank == master) {
		cout << "\nNumber(Quantity) = " << numCount << "\t Machine(Quantity) = " << macCount << endl;

		for (int i = 0; i < numCount*(macCount); i++) {
			numVector[i] = i + 1;
		}

		MPI_Send(&numVector[numCount], ((macCount - (rank + 1)) * numCount), MPI_INT, rank + 1, tag, MPI_COMM_WORLD);


		for (int i = 1; i < macCount; i++)
		{
			MPI_Recv(&avarage[i - 1], 1, MPI_FLOAT, i, tag, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
			cout << "\nRank(" << rank << "): Avarage received from Rank(" << i << ")\n\n\t Avarage: " << avarage[i - 1] << "\n";
		}
	}
	else {

		MPI_Recv(&numVector[(rank * numCount)], (numCount * (macCount - rank)), MPI_INT, (rank - 1), tag, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		cout << "\nRank(" << rank << "): first value=" << numVector[(rank * numCount)]
			<< "\t last value=" << numVector[(macCount * numCount) - 1] << endl;

		avarage[rank - 1] = vectorAvarage(numVector);

		MPI_Send(&avarage[rank - 1], 1, MPI_FLOAT, master, tag, MPI_COMM_WORLD);

		if (rank != (macCount - 1)) {
			MPI_Send(&numVector[(((rank + 1) * numCount))], ((macCount - (rank + 1)) * numCount), MPI_INT, rank + 1, tag, MPI_COMM_WORLD);
		}
	}
	MPI_Finalize();
	return 0;
}