#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>

/**
 * You can compile and run this example using the commands:
 *
 * $> gcc example_swap.c
 * $> ./a.out
 */

/**
 * Here you have 3 contexts - one for each coroutine. First
 * belongs to main coroutine - the one, who starts all others.
 * Next two are worker coroutines - they do real work. In your
 * code you will have more than 2.
 */
static ucontext_t uctx_main, *uctxs;

#define handle_error(msg) \
   do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define stack_size 1024 * 1024
#define MAX_LEVELS 64
/**
 * Coroutine body. This code is executed by all the corutines at
 * the same time and with different stacks. So you can store any
 * local variables and arguments right here. Here you implement
 * your solution.
 */

static void quickSort(int *arr, int elements, int id, int size) {
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
        printf("swapping\n");
        if(swapcontext(&uctxs[id], &uctxs[(id+1)%size]) == -1) {
            printf("some problems");
        }
    }

}

static void
my_coroutine(int id, int *array){

	printf("func%d: started\n", id);
    for(int i = 0; i < 5; i++){
        array[i] = i;
        printf("%d ", array[i]);
    }
    printf("\n");
	if (id == 1) {
	        printf("coroutine1: swapcontext(&uctx_func1, &uctx_func2)\n");
		if (swapcontext(&uctxs[0], &uctxs[1]) == -1)
	        	handle_error("swapcontext");
	} else {
		printf("coroutine2: swapcontext(&uctx_func2, &uctx_func1)\n");
		if (swapcontext(&uctxs[1], &uctxs[0]) == -1)
			handle_error("swapcontext");
	}
	printf("func%d: returning\n", id);
}

/**
 * Below you can see 3 different ways of how to allocate stack.
 * You can choose any. All of them do in fact the same.
 */

static void *
allocate_stack_sig()
{
	void *stack = malloc(stack_size);
	stack_t ss;
	ss.ss_sp = stack;
	ss.ss_size = stack_size;
	ss.ss_flags = 0;
	sigaltstack(&ss, NULL);
	return stack;
}

static void *
allocate_stack_mmap()
{
	return mmap(NULL, stack_size, PROT_READ | PROT_WRITE | PROT_EXEC,
		    MAP_ANON | MAP_PRIVATE, -1, 0);
}

static void *
allocate_stack_mprot()
{
	void *stack = malloc(stack_size);
	mprotect(stack, stack_size, PROT_READ | PROT_WRITE | PROT_EXEC);
	return stack;
}

enum stack_type {
	STACK_MMAP,
	STACK_SIG,
	STACK_MPROT
};

/**
 * Use this wrapper to choose your favourite way of stack
 * allocation.
 */
static void *
allocate_stack(enum stack_type t)
{
	switch(t) {
	case STACK_MMAP:
		return allocate_stack_mmap();
	case STACK_SIG:
		return allocate_stack_sig();
	case STACK_MPROT:
		return allocate_stack_mprot();
	}
}

int
main(int argc, char *argv[])
{
    int * array1 = malloc(sizeof(int)*5);
    int * array2 = malloc(sizeof(int)*5);
    for(int is = 0; is < 5; is++){
        array1[is] = 5-is;
        array2[is] = 10-is;
    }
    printf("Before\n");
    printf("array1:");
    for(int q = 0; q < 5; q++){
        printf("%d ", array1[q]);
    }
    printf("\narray2:");
    for(int q = 0; q < 5; q++){
        printf("%d ", array2[q]);
    }

	/* First of all, create a stack for each coroutine. */
	char *func1_stack = allocate_stack(STACK_MPROT);
	char *func2_stack = allocate_stack(STACK_MPROT);
    uctxs = malloc(2*sizeof(uctxs));
	/*
	 * Below is just initialization of coroutine structures.
	 * They are not started yet. Just created.
	 */
	if (getcontext(&uctxs[0]) == -1)
		handle_error("getcontext");
	/*
	 * Here you specify a stack, allocated earlier. Unique for
	 * each coroutine.
	 */
	uctxs[0].uc_stack.ss_sp = allocate_stack(STACK_MPROT);
	uctxs[0].uc_stack.ss_size = stack_size;
    uctxs[0].uc_flags = 0;
	/*
	 * Important - here you specify, to which context to
	 * switch after this coroutine is finished. The code below
	 * says, that when 'uctx_func1' is finished, it should
	 * switch to 'uctx_main'.
	 */
	uctxs[0].uc_link = &uctx_main;
	makecontext(&uctxs[0], quickSort, 4, array1, 5, 0, 2);

	if (getcontext(&uctxs[1]) == -1)
		handle_error("getcontext");
	uctxs[1].uc_stack.ss_sp = allocate_stack(STACK_MPROT);
	uctxs[1].uc_stack.ss_size = stack_size;
	/* Successor context is f1(), unless argc > 1. */
	uctxs[1].uc_link =  &uctxs[0];
	makecontext(&uctxs[1], quickSort, 4, array2, 5, 1, 2);

	/*
	 * And here it starts. The first coroutine to start is
	 * 'uctx_func2'.
	 */
	printf("main: swapcontext(&uctx_main, &uctx_func2)\n");
	if (swapcontext(&uctx_main, &uctxs[0]) == -1)
		handle_error("swapcontext");
    printf("array1:");
    for(int q = 0; q < 5; q++){
        printf("%d ", array1[q]);
    }
    printf("\narray2:");
    for(int q = 0; q < 5; q++){
        printf("%d ", array2[q]);
    }


	printf("\nmain: exiting\n");
	return 0;
}