#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/mman.h>
#include <time.h>
#define SIZE 102400
#define RESULT "result.txt"
#define MAX_LEVELS 1280
#define stack_size 1024*1024*1024


static ucontext_t *uctxs, uctx_main;
static ucontext_t uctx_start;
int *flags;
double *times;
static void *
allocate_stack_mprot()
{
	void *stack = malloc(stack_size);
	mprotect(stack, stack_size, PROT_READ | PROT_WRITE | PROT_EXEC);
	return stack;
}

static void quickSort(int *arr, int elements, int id, int size) {
    clock_t begin = clock();
    int beg[MAX_LEVELS], end[MAX_LEVELS], L, R;
    int i = 0;

    beg[0] = 0;
    end[0] = elements;
    while (i >= 0) {
        L = beg[i];
        R = end[i];
        if (L + 1 < R--) {
            int piv = arr[L];
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
        
        if (findWorkingCorutine(size, id)!=-1){
            if(swapcontext(&uctxs[id], &uctxs[findGreaterWorkingCorutine(size, id)]) == -1) {
                printf("some problems");
            }
        }
    }
    clock_t end_ = clock();
    double time_spent = (double)(end_ - begin) / (CLOCKS_PER_SEC / 1000);
    times[id] = time_spent;
    flags[id] = 1;
    
}
int findGreaterWorkingCorutine(size, id){
    int id_;
    id_ = id % size;
    for(int i = 0; i < 2 * size; i++){
        if ((flags[i%size] == 0) && (i%size!=id) && ((i%size > id_) || (i>=size))){
            return i%size;
        }
    }
    return -1;
}

int indexOfMin(int **array, int *pivots, int size, int *tmp) {
    int min = 2147483647;
    int index = -1;
    for (int k = 0; k < size; k++){
        if((tmp[k] < min) && (tmp[k]!=-1)){
            min = tmp[k];
            index = k;
        }
    }
    return index;
};
int findWorkingCorutine(int size, int id){
    for(int i = 0; i < size; i++){
        if ((flags[i] == 0) && (i!=id)){
            return i;
        }
    }
    return -1;
    
}
int checkFlagsForEnd(int size){
    for (int l; l<size; l++){
        if (flags[l]==0){
            return 0;
        }
    }
    return 1;
}
void start_end(int size){
    while(1){
        if (findWorkingCorutine(size, -1) == -1){
            swapcontext(&uctx_start, &uctx_main);
            break;
        } //все корутины закончились
        else{
            swapcontext(&uctx_start, &uctxs[findWorkingCorutine(size, -1)]);
        }
    }

}
void finalMerging(int **a, int size, char *filename, int size_of_last_one, int *sizes){   //передаем массив указалелей на массивы отсортированные и размер данного массива 
    
    int *pivots = malloc(size*sizeof(pivots));
    int *tmp = (int*)malloc(size * sizeof(tmp));                                          //указатели на текущие элементы массивов
    for(int k = 0; k < size; k++){
        pivots[k] = 0;
    }
    for (int k = 0; k < size; k++){
        tmp[k] = a[k][pivots[k]];
    }
    FILE *finalfp = fopen(filename, "w");
    int actual_min;
    int n = 0;
    while (n<size){
        
        actual_min = indexOfMin(a, pivots, size, tmp);
        fprintf(finalfp, "%d ", a[actual_min][pivots[actual_min]]);
        
        pivots[actual_min]++;
            
        if (pivots[actual_min] == sizes[actual_min]){
            n++;
            tmp[actual_min] = -1;
        }
        else{
            tmp[actual_min] = a[actual_min][pivots[actual_min]];
        }
    }
    fclose(finalfp);
    free(pivots);
    free(tmp);
}

int main(int argc, char **argv) 
{
    clock_t begin = clock();
    flags = (unsigned int *)malloc(sizeof(flags)*(argc-1));
    times = (double *)malloc(sizeof(double)*(argc-1));
    for (int i = 0; i < argc-1; i++){
        flags[i] = 0;
    }
    int *sizes = (int*)malloc(sizeof(int) * (argc-1));
    FILE *fp;
    int a[SIZE];
    int i = 0;
    int size = 0;

    int  **ps = (int **) malloc((argc-1) * sizeof(ps));

    uctxs = malloc((argc-1) * sizeof(uctxs));
    
    //читаем файлы в память
    for (int j = 0; j < argc-1; j++){
        size = 0;
        i = 0;
        if ((fp = fopen(argv[j+1], "r")) < 0){
            printf("some problems\n");
            return -1;
        }; 
        while(fscanf(fp, " %d ", &a[i]) > 0){
            size++;
            i++;
        }
        ps[j] = (int *)malloc((size) * sizeof(int));
        for (int q = 0; q<size; q++){
            ps[j][q] = a[q];
        }
        sizes[j] = size;
        
    }
    fclose(fp);

    
    getcontext(&uctx_start);
    uctx_start.uc_stack.ss_sp = allocate_stack_mprot();
    uctx_start.uc_stack.ss_size = stack_size;
    uctx_start.uc_link = &uctx_main;
    makecontext(&uctx_start, start_end, 1, (argc-1));


    for(int j = 0; j < argc-1; j++){
        if(getcontext(&uctxs[j]) == -1){
            printf("some problems");
        }
        uctxs[j].uc_stack.ss_sp = allocate_stack_mprot();
        uctxs[j].uc_stack.ss_size = stack_size;
        uctxs[j].uc_link = &uctx_start;
        makecontext(&uctxs[j], quickSort, 4, ps[j], sizes[j], j, argc-1);
    }
    swapcontext(&uctx_main, &uctx_start);
    int size_of_last_one = 0;
    for (int k = 0; k < argc-1; k++){
        size_of_last_one = size_of_last_one + sizes[k];
    }
    //финальное слияние в один массив
    finalMerging(ps, argc-1, RESULT, size_of_last_one, sizes);
    clock_t end = clock();
    double time_spent = (double)(end - begin)/(CLOCKS_PER_SEC/1000);
    printf("All over in %f ms\n", time_spent);
    for(int i = 0; i < argc - 1; i++){
        printf("Coroutine %d worked %f ms\n", i, times[i]);
    }
    free(sizes);
    free(uctxs);
    free(flags);
    free(ps);
    free(times);
    _Exit(EXIT_SUCCESS);
    return 0;
}