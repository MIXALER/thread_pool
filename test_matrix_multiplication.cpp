#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include "ThreadPool.h"

typedef struct matrix
{
    int **a;
    int **b;
    int **c;
} NMatrix;


typedef struct param
{
    NMatrix *p_matrix;
    int row;
    int col;
    int size;
} Param;


void *func(void *args)
{
    Param *param = (Param *) args;
    int row = param->row;
    int col = param->col;
    int ans = 0;

    for (int i = 0; i < param->size; ++i)
    {
        ans += param->p_matrix->a[row][i] * param->p_matrix->b[i][col];
    }
    param->p_matrix->c[row][col] = ans;
    return nullptr;
}

#define MALLOC(n, type)     \
        ((type *)malloc( (n) * sizeof(type)))

#define FREE(p)             \
        if(p!=NULL)         \
        {                   \
              free(p);      \
              p = NULL;     \
        }

void print(const int **a, int size)
{
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            printf("%d ", a[i][j]);
        }
        printf("\n");
    }
}


int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);
    if (argc != 4)
    {
        printf("Usage: ./a.out max_thread_nums matrix_size measurement_times \n");
        exit(0);
    }
    double total_time;
    int max_threads = atoi(argv[1]);
    int matrix_size = atoi(argv[2]);
    int n_times = atoi(argv[3]);

    ThreadPool *pool = new ThreadPool(4, 8);
    pool->Run();

    for (int size = 2; size <= matrix_size; ++size)
    {
        srand(time(NULL));   // 通过时间初始化随机种子

        for (int n_thread = 1; n_thread < max_threads && n_thread <= size * size; ++n_thread)
        {
            NMatrix matrixs;
            matrixs.a = MALLOC(size, int *);
            matrixs.b = MALLOC(size, int *);
            matrixs.c = MALLOC(size, int *);
            Param **param = MALLOC(size, Param *);
            for (int i = 0; i < size; ++i)
            {
                matrixs.a[i] = MALLOC(size, int);
                matrixs.b[i] = MALLOC(size, int);
                matrixs.c[i] = MALLOC(size, int);
                param[i] = MALLOC(size, Param);
            }
            for (int time = 0; time < n_times; ++time)
            {
                for (int i = 0; i < size; i++)
                {
                    for (int j = 0; j < size; j++)
                    {
                        matrixs.a[i][j] = rand() % 10;
                        matrixs.b[i][j] = rand() % 10;
                        matrixs.c[i][j] = 0;
                    }
                }
                clock_t start = clock();
                for (int i = 0; i < size; ++i)
                {
                    for (int j = 0; j < size; ++j)
                    {
                        param[i][j].row = i;
                        param[i][j].col = j;
                        param[i][j].size = size;
                        param[i][j].p_matrix = &matrixs;
                        TaskFunc fun = static_cast<TaskFunc>(func);
                        std::shared_ptr<ThreadTask> thread_task(new ThreadTask);
                        thread_task->taskFunc_ = fun;
                        thread_task->taskArgs_ = (void *) &param[i][j];
                        pool->TaskAdd(thread_task);
                    }
                }
                clock_t finish = clock();
                total_time += (double) (finish - start);
            }
            print((const int **) matrixs.a, size);
            print((const int **) matrixs.b, size);
            print((const int **) matrixs.c, size);
            for (int i = 0; i < size; i++)
            {
                FREE(matrixs.a[i]);
                FREE(matrixs.b[i]);
                FREE(matrixs.c[i]);
                FREE(param[i]);
            }
            FREE(matrixs.a);
            FREE(matrixs.b);
            FREE(matrixs.c);
            FREE(param);
            FILE *fd;
            fd = fopen("./pool_log.txt", "a");
            fprintf(fd, "Matrix Size = %d  -----Average Runtime of %d times = %.10f\n", size, n_times,
                    (double) (total_time / n_times) / CLOCKS_PER_SEC);
            fclose(fd);
            total_time = 0;
        }
    }
    return 0;
}