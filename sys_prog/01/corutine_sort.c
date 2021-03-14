#include <stdio.h>
#include <stdlib.h>
#define SIZE 10240
#define RESULT "result.txt"
#define MAX_LEVELS 64

int quickSort(int *arr, int elements) {
    int beg[MAX_LEVELS], end[MAX_LEVELS], L, R;
    int i = 0;

    beg[0] = 0;
    end[0] = elements;
    while (i >= 0) {
        L = beg[i];
        R = end[i];
        if (L + 1 < R--) {
            int piv = arr[L];
            if (i == MAX_LEVELS - 1)
                return -1;
            while (L < R) {
                while (arr[R] >= piv && L < R)
                    R--;
                if (L < R)
                    arr[L++] = arr[R];
                while (arr[L] <= piv && L < R)
                    L++;
                if (L < R)
                    arr[R--] = arr[L];
            }
            arr[L] = piv;
            if (L - beg[i] > end[i] - R) { 
                beg[i + 1] = L + 1;
                end[i + 1] = end[i];
                end[i++] = L;
            } else {
                beg[i + 1] = beg[i];
                end[i + 1] = L;
                beg[i++] = L + 1;
            }
        } else {
            i--;
        }
    }
    return 0;
}
int indexOfMax(int **array, int *pivots, int size, int *tmp) {
    int min = 2147483647;
    int index = -1;
    //printf("[%d]\n", size);
    for (int k = 0; k < size; k++){
        if(tmp[k] < min){
            min = tmp[k];
            index = k;
        }
    }
    return index;
};

void finalMerging(int **a, int size, char *filename, int size_of_last_one, int *sizes){   //передаем массив указалелей на массивы отсортированные и размер данного массива 
    //printf("Katya\n");
    int *pivots = malloc(size*sizeof(pivots));
    int *tmp = (int*)malloc(size * sizeof(tmp));                                          //указатели на текущие элементы массивов
    for(int k = 0; k < size; k++){
        pivots[k] = 0;
        //printf("%d\n", pivots[k]);
    }
    for (int k = 0; k < size; k++){
        tmp[k] = a[k][pivots[k]];
    }

    //printf("%d\n", pivots[size-1]);
    FILE *finalfp = fopen(filename, "w");
    int actual_max;
    int n = 0;
    while (n<size_of_last_one){
        
        actual_max = indexOfMax(a, pivots, size, tmp);
        //printf("%d", actual_max);
        fprintf(finalfp, "%d ", a[actual_max][pivots[actual_max]]);
        pivots[actual_max]++;
        n++;
        if (pivots[actual_max] == sizes[actual_max]){
            pivots[actual_max] == 0;
            a[actual_max][pivots[actual_max]] = 2147483646;
        }
        else{
            tmp[actual_max] = a[actual_max][pivots[actual_max]];
        }
    }
    //free(pivots);
}

int main(int argc, char **argv) 
{
    FILE **fps;
    int *sizes = (int*)malloc(sizeof(int) * (argc-1) );
    fps = (FILE **)malloc((argc-1) * sizeof(fps));

    for (int j = 0; j < argc-1; j++){
        
        if ((fps[j] = fopen(argv[j+1], "r")) < 0){
            printf("some problems\n");
            return -1;
        }; 
    }
    

    int a[SIZE];
    int i = 0;
    int size = 0;

    int  **ps = NULL;
    ps = (int **) malloc((argc-1) * sizeof(ps));


    for (int j = 0; j < argc-1; j++){
        size = 0;
        i = 0;
        while(fscanf(fps[j], " %d ", &a[i]) > 0){
            size++;
            i++;
        }
        //printf("%d\n", size);
        ps[j] = (int *)malloc(size * sizeof(int));
        for (int q = 0; q<size; q++){
            ps[j][q] = a[q];
        }
        sizes[j] = size;
    }
    
    for (int k = 0; k < argc-1; k++){
        if(quickSort(ps[k], sizes[k])){
            printf("some problems");
        }; 
    }
    /*for(int k = 0; k < argc-1; k++){
        for (int i = 0; i < sizes[k]; i++)
            printf("%d ", ps[k][i]);
        printf("\n");
    }*/
    int size_of_last_one = 0;
    for (int k = 0; k < argc-1; k++){
        size_of_last_one = size_of_last_one + sizes[k];
    }

    finalMerging(ps, argc-1, RESULT, size_of_last_one, sizes);
    //free(ps);
    //free(fps);
    //free(sizes);
    return 0;
}