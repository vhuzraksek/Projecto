#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

/*Print a matrix (of doubles) of size mxn */
void print_double_mat(double** mat, int m, int n){
    int i, j;
    for (i=0; i<m; i++){
        for (j=0; j<n; j++){
            printf("%.2f", mat[i][j]);
            if (j<n-1){
                /*If this is not the last line, print a comma*/
                printf(",");
            }
        }
        printf("\n");
    }
}
/*Free a matrix (of doubles with m rows) memory allocation */
/*TODO: Not sure if this is correct. Maybe I needed to pass a pointer to the arr?*/
int free_double_mat(double** mat, int m){
    /*TODO: Implement verification (check if something went wrong)*/
    int i;
    for (i=0; i<m; i++){
            free(mat[i]);
        }
    free(mat);
    return 1;
}

/* Copy-paste values from one vector to another, in-place function*/
void vecAssign(double *from, double *to, int vecSize) {
    int i;
    for (i=0; i<vecSize; i++){
        to[i] = from[i];
    }
    return;
}

/*Initialize vector with zeros, in-place function*/
void initVectorZeros(double* vector, int vecSize){
    int i;
    for (i = 0; i < vecSize; ++i) {
        vector[i]=0;
    }
}

    /*Calculate the difference between two vectors (of doubles), c=(a-b) of length d,
     * in place into result vector */
void vecDiff(double* a, double* b, double* result, int d){
    int i;
    for (i=0; i<d; i++){
        result[i] = a[i]-b[i];
    }
    return;
}

/*Calculate the difference between two vectors (of doubles), c=(a-b) of length d,
 * in place into result vector */
void vecSum(double* a, double* b, double* result, int d){
    int i;
    for (i=0; i<d; i++){
        result[i] = a[i]+b[i];
    }
    return;
}

void vecDiv(double* vector, int vecSize, int divisor) {
    int i;
    for (i = 0; i < vecSize; ++i) {
        vector[i] = vector[i] / divisor;
    }
}

/* Calculate the euclidean norm of a vector (of doubles) */
double norm2(double* x, int vecSize){
    int i;
    int sumOfSquares = 0;
    for (i=0; i < vecSize; i++){
        sumOfSquares += x[i]*x[i];
    }
    return sqrt(sumOfSquares);
}

/* Initialize the centroids with the K first input vectors */
void init_centroids(double** centroids, double** input, int K, int d){
    int i;
    for (i=0; i<K; i++){
        vecAssign(input[i], centroids[i], d);
    }
    return;
}

/* Init input2CentMapping to a their respective clusters
 * and the others will be assigned to cluster>K, thus non existent clusters,
 * so first time there will be a noticed change for them,
 * while first ones remain unchanged, as expected.
 * In-place function*/
void init_input2CentMapping(int* input2CentMapping, int N){
    int i;
    for (i = 0; i < N; ++i){
        input2CentMapping[i] = i;
    }
    return;
}

/* Logic of the kmeans calculation */
double** kmeans(int K, int N, int d, int MAX_ITER, double** input){
    /*Iteration Variables*/
    int iter, i, j;

    /*Temp vars 2A*/
    double minDistance,currentDistance;
    int minCentroidIndex;
    int changeHappened=TRUE;
    double* diffResult;

    /*Temp vars for 2B*/
    int clusterSize;
    double* newCentroidVec;

    /*Kmeans vars*/
    int* input2CentMapping;
    double** centroids;

    /*1. malloc & init*/
    diffResult = (double *)malloc(N * sizeof(double));
    newCentroidVec = (double *)malloc(d * sizeof(double));
    input2CentMapping = (int *)malloc(N * sizeof(int));
    centroids = (double **)malloc(K * sizeof(double*));
    for(i = 0; i < K; i++) centroids[i] = (double *)malloc(d * sizeof(double));
    init_centroids(centroids, input, K, d);
    init_input2CentMapping(input2CentMapping, N);

    /*2. Iterations*/
    for (iter = 0; iter < MAX_ITER || changeHappened; ++iter) {
        changeHappened=FALSE;

        /*A) Assign inputs to centroids*/
        for (i = 0; i < N; ++i) {
            for (j = 0; j < K; ++j) {
                vecDiff(input[i], centroids[j], diffResult, d);
                currentDistance = norm2(diffResult, d);
                if (j==0 || currentDistance < minDistance){
                    minDistance = currentDistance;
                    minCentroidIndex = j;
                }
            }
            if (input2CentMapping[i]!=minCentroidIndex){
                changeHappened=TRUE;
                input2CentMapping[i]=minCentroidIndex;
            }
        }
        /*B) Update centroids*/
        for (j = 0; j < K; ++j) {
            clusterSize=0;
            initVectorZeros(newCentroidVec, d);
            for (i = 0; i < N; ++i) {
                if (input2CentMapping[i]==j){
                    clusterSize+=1;
                    vecSum(newCentroidVec, input[i], newCentroidVec, d);
                }
            }
            vecDiv(newCentroidVec, d, clusterSize);
            vecAssign(newCentroidVec,centroids[j], d);
        }
    }
    /*Free all local memory*/
    free(diffResult);
    free(newCentroidVec);
    free(input2CentMapping);

    return centroids;
}

/* Verify that args are OK*/
/*TODO: Check things about the input, such as < char*/
/*TODO: Also make sure how to calculate the amount of vars! CLion says 7 but it doesn't really work, terminal says 5.*/
int checkArgs(int argc, char *argv[]){
    int i;
    if (argc!=5) {
        printf("Error, expected 5 arguments but %d were provided:\n\n",argc);
        for (i = 0; i < argc; ++i) {
            printf("%s\n", argv[i]);
        }
        return 0;
    }
    return 1;
}

/* Stream input file into new matrix */
double** parseInputFile(int N, int d){
    int i,j;
    double n1;
    char c;
    double **input = (double **)malloc(N * sizeof(double*));
    for(i = 0; i < N; i++) input[i] = (double *)malloc(d * sizeof(double));

    for(i = 0; i < N; i++){
        for(j = 0; j < d; j++){
            /*Check if next read input is invalid*/
            if (scanf("%lf%c", &n1, &c) != 2){
                printf("Error reading the %d,%d element of the input matrix.\n",i,j);
                free_double_mat(input, N);
                return 0;
            }
            /*Fill in the matrix*/
            input[i][j]=n1;
        }
    }
    if (scanf("%lf%c", &n1, &c) == 2) {
        printf("Warning: Too many numbers in input.\n");
    }
    return input;
}

/* Main, takes care of the execution and system related tasks */
int main(int argc, char *argv[]) {
    int K, N, d, MAX_ITER;
    double** input;
    double** centroids;
    if (checkArgs(argc, argv)==0){
        return 0;
    }
    /*parse vars and input file*/
    K=atoi(argv[1]);
    N=atoi(argv[2]);
    d=atoi(argv[3]);
    MAX_ITER=atoi(argv[4]);
    input = parseInputFile(N, d);
    if (input==0){
        /*Invalid input*/
        return 0;
    }
    /*Actual logic of the algorithm*/
    centroids = kmeans(K, N, d, MAX_ITER, input);

    /*Print and free input/output of the kmeans algorithm*/
    print_double_mat(centroids, K, d);
    free_double_mat(centroids, K);
    free_double_mat(input, N);
    return 1;
}
