#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/mman.h>
#include <time.h>                                                
#define RESULT "result.txt"                                             
#define MAX_SIZE 128
#define stack_size 64*1024                                            


static ucontext_t *uctxs = NULL, uctx_main;             
static ucontext_t uctx_start;
int *flags = NULL;
double *times = NULL;

static void *
allocate_stack_mprot()                                                      // создание стека для корутин
{
	void *stack = malloc(stack_size);
	mprotect(stack, stack_size, PROT_READ | PROT_WRITE | PROT_EXEC);
	return stack;
}

static void quickSort(int *arr, int elements, int id, int size) {           // алгоритм быстрой сортировки 
    clock_t begin = clock();
    int beg[MAX_SIZE], end[MAX_SIZE], L, R;
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
        
        if (findWorkingCorutine(size, id)!=-1){                             // переключание контекста
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

int findGreaterWorkingCorutine(int size, int id){                           // поиск следующей рабочей корутины 
    int id_;
    id_ = id % size;
    for(int i = 0; i < 2 * size; i++){
        if ((flags[i%size] == 0) && (i%size!=id) && ((i%size > id_) || (i>=size))){
            return i%size;
        }
    }
    return -1;
}

int indexOfMin(int **array, int *pivots, int size, int *tmp) {              // поиск индекса минимального 
    int min = 2147483647;
    int index = -1;
    for (int k = 0; k < size; k++){
        if((tmp[k] < min) && (tmp[k]!=-1)){
            min = tmp[k];
            index = k;
        }
    }
    return index;
}

int findWorkingCorutine(int size, int id){                                  // поиск просто работающий корутины, либо -1 (если нет работающих корутин)
    for(int i = 0; i < size; i++){
        if ((flags[i] == 0) && (i!=id)){
            return i;
        }
    }
    return -1;
    
}

static void startEnd(int size){                                            // функция для корутины, которая будет обрабатывать все сортирующие корутины
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

void finalMerging(int **a, int size, char *filename, int size_of_last_one, int *sizes){   // финальное слияние
    
    int *pivots = (int *)malloc(size*sizeof(int));                                        
    int *tmp = (int*)malloc(size * sizeof(int));                                       
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
            tmp[actual_min] = -1;                                                           // небольшая эвристика, ставлю -1, как знак того,
        }                                                                                   // что не нужно дальше рассматривать данный массив (он закончился)
        else{                                                                               // *в предположении, что все элементы файлов неотрицательные
            tmp[actual_min] = a[actual_min][pivots[actual_min]];                            // могу переписать по-другому, для обобщения на отрицательные элементы
        }
    }
    fclose(finalfp);
    free(pivots);
    free(tmp);
}

int main(int argc, char **argv) 
{
    clock_t begin = clock();
    flags = (unsigned int *)malloc(sizeof(unsigned int)*(argc-1));
    times = (double *)malloc(sizeof(double)*(argc-1));
    char **stacks = (char **)malloc((argc-1)*sizeof(char));   

    for (int i = 0; i < argc-1; i++){
        flags[i] = 0;
    }

    int *sizes = NULL;
    sizes = (int*)malloc(sizeof(int) * (argc-1));
    FILE *fp = NULL;
    int i = 0;
    int size = 0;
    int num;
    int  **ps;
    ps = (int **) malloc((argc-1) * sizeof(ps));

    for(int i = 0; i < argc - 1; i++){
        ps[i] = NULL;
    }

    uctxs = (ucontext_t *)malloc((argc) * sizeof(ucontext_t));
    
    
    for (int j = 0; j < argc-1; j++){                                                       //читаем файлы в память
        size = 0;
        if ((fp = fopen(argv[j+1], "r")) < 0){
            printf("some problems\n");
            return -1;
        }; 
        while (fscanf(fp, "%d", &num) == 1 ){
            if ((ps[j] = (int*)realloc(ps[j], sizeof(int) * (size + 1)) ) == NULL){
                perror("realloc");
                exit(EXIT_FAILURE);
            }
            ps[j][size++] = num;
        }
        sizes[j] = size;
        
    }
    fclose(fp);

    
    getcontext(&uctx_start);                                                        // обрабатываем корутину-обработчика
    uctx_start.uc_stack.ss_sp = allocate_stack_mprot();
    uctx_start.uc_stack.ss_size = stack_size;
    uctx_start.uc_link = &uctx_main;
    makecontext(&uctx_start, startEnd, 1, (argc-1));


    for(int j = 0; j < argc-1; j++){                                                // обрабатывем сортирующие корутины
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
    
    finalMerging(ps, argc-1, RESULT, size_of_last_one, sizes);                         //финальное слияние в один массив
    clock_t end = clock();
    double time_spent = (double)(end - begin)/(CLOCKS_PER_SEC/1000);
    printf("All over in %f ms\n", time_spent);
    for(int i = 0; i < argc - 1; i++){
        printf("Coroutine %d worked %f ms\n", i, times[i]);
    }
    for(int i = 0; i < argc - 1; i++){
        free(uctxs[i].uc_stack.ss_sp);
        free(ps[i]);
    }

    free(stacks);
    free(uctxs);
    free(sizes);
    free(flags);
    free(ps);
    free(times);
    return 0;
}