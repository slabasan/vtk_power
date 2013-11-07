#include <vtkDataSet.h>
#include <vtkRectilinearGrid.h>
#include <vtkContourFilter.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataNormals.h>

#include <unistd.h>
#include <sys/time.h>
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int rank, size;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */

	struct timeval startProg, endProg; 
	struct timeval startSleep;
	struct timeval startData; 
	struct timeval startIso; 
	struct timeval startNorm; 

	gettimeofday(&startProg, NULL);

	cerr <<"Phase 1: Creating data set" << endl;
	vtkRectilinearGrid *rg = vtkRectilinearGrid::New();
	int ndims = 200;
	int dims[3] = { ndims, ndims, ndims };
	rg->SetDimensions(dims);

	vtkFloatArray *coord = vtkFloatArray::New();
	coord->SetNumberOfTuples(ndims);
	int i;
	for (i = 0 ; i < ndims ; i++)
		coord->SetTuple1(i, i/((float)ndims-1.0));

	rg->SetXCoordinates(coord);
	rg->SetYCoordinates(coord);
	rg->SetZCoordinates(coord);

	vtkFloatArray *var = vtkFloatArray::New();
	int nvars = ndims*ndims*ndims;
	var->SetNumberOfTuples(nvars);
    
	srand(0);
	for (i = 0 ; i < nvars ; i++)
		var->SetTuple1(i, rand()%100000/(100000.0-1.0));
	var->SetName("rand");

	rg->GetPointData()->AddArray(var);
	rg->GetPointData()->SetActiveScalars("rand");
	var->Delete();
	cerr <<"Done creating data set" << endl;

	gettimeofday(&startIso, NULL);

	vtkContourFilter *cf = vtkContourFilter::New();
	cf->SetNumberOfContours(1);
	cf->SetValue(0, 0.5);	//2nd parameter (isovalue) ranges from 0 to 1 (see ln 52)
				//alter 2nd param to change output of num triangles
				//isovalue of 0.2 and 0.5 is more likely than value of 0.01
				//larger # of triangles = more work = higher power wattage
	cf->SetInput(rg);
	cerr << "Phase 2: Isosurfacing!" << endl;
	cf->Update();
	cerr << "Done isosurfacing!, num triangles in output is " << cf->GetOutput()->GetNumberOfCells() << endl;

	gettimeofday(&startNorm, NULL);

	vtkPolyDataNormals *pdn = vtkPolyDataNormals::New();
	pdn->SetInput(cf->GetOutput());
	cerr << "Phase 3: Normals!" << endl;
	pdn->Update();
	cerr << "Done with normals" << endl;

	gettimeofday(&endProg, NULL);

	unsigned long long elapsedProg = (endProg.tv_sec-startProg.tv_sec) + (endProg.tv_usec-startProg.tv_usec)/1000000 ;
	unsigned long long elapsedData = (startIso.tv_sec-startProg.tv_sec) + (startIso.tv_usec-startProg.tv_usec)/1000000 ;
	unsigned long long elapsedIso = (startNorm.tv_sec-startIso.tv_sec) + (startNorm.tv_usec-startIso.tv_usec)/1000000 ;
	unsigned long long elapsedNorm= (endProg.tv_sec-startNorm.tv_sec) + (endProg.tv_usec-startNorm.tv_usec)/1000000 ;

	/*
	if (rank == 0) {
		printf("\n");
		printf("Start time data set: 0 sec\n");
		printf("Start time isosurfacing: %llu sec\n", (startIso.tv_sec-startProg.tv_sec) + (startIso.tv_usec-startProg.tv_usec)/1000000);
		printf("Start time normals: %llu sec\n", (startNorm.tv_sec-startProg.tv_sec) + (startNorm.tv_usec-startProg.tv_usec)/1000000 );
		printf("End time of program: %llu sec\n", (endProg.tv_sec-startProg.tv_sec) + (endProg.tv_usec-startProg.tv_usec)/1000000);
		printf("\n");
		printf("Elapsed time of data: %llu sec\n", elapsedData);
		printf("Elapsed time of isosurfacing: %llu sec\n", elapsedIso);
		printf("Elapsed time of normals: %llu sec\n", elapsedNorm);
		printf("Elapsed time of program: %llu sec\n", elapsedProg);
	}
	*/
	
	MPI_Finalize();
	return 0;
}


