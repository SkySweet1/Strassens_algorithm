/*
роль 1              разработка модификации Штрассена (формулы)              strassen_mul
роль 2              проверка корректности, спецификации                     naive_mul
роль 3              реализация операций в поле                              field_add, sub, mul
роль 4              сборка всего вместе (ввод-вывод, память)                mat_create, free main и др.
роль 5              проверка корректности и скорости                        test_perf
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define THRESHOLD 64
/*
наша задача как у проэкта это решить проблему алгоритма Штрассена
короче у нас есть формула, которая быстро умножает две квадратные матрицы(n x n), но
для маленьких матриц она работает в 1000 раз медленнее и поэтому мы вводим такое число,
с которого будет применяться этот алгоритм (еще раз -> если меньше, то просто бессмысленно(долго))

THRESHOLD это порог вхождения матрицы, при котором еще умжножение будет происхлдить по алгоритму Штрассена
*/

#define MIN_MAT_SIZE 1
#define MAX_MAT_SIZE 1024
/*
вывожу здесь #define чтобы можно было менять при необходимости
*/

typedef struct {
    size_t rows;
    size_t cols;
    int mod;
    int* data;
    int is_dynamic;
} Matrix;
/*
size_t это тип хранения размеров (строк столбцов)
вмещает любой размер массива в памяти (а так же неотрицательное)

не int потому что слишком мало (там вроде максимум 32к байт что мало)

typedef struct {} Matrix; чтобы не писать каждый раз struct Matrix, а прсото писать Matrix

вообщем мы создали тип матрицы                      typedef struct {} Matrix;
размеры (неотрицательные большие)                   size_t rows, cols;
модуль поля                                         int mod;
указатель на числа матрицы                          int* data;
флаг что матрица большая (освободить память)        int is_dynamic;
*/

int field_add(int a, int b, int mod) // роль 3
{
    int result = a + b;

    if(result >= mod) result -= mod;

    return result;
}
/*
эта функция будет заварачивать числа в модуль
ну типо 5 (mod 4) = 1 остаток

ну только у нас будет field_add(a, b, mod) -> a+b(mod), то есть field_add(5, 6, 10) -> 1 
*/

int field_sub(int a, int b, int mod) // роль 3
{
    int result = a - b;

    if(result < 0) result += mod;

    return result;
}
/*
тоже самое с вычитанием 
field_sub(a, b, mod) a-b(mod)

*/

int field_mul(int a, int b, int mod) // роль 3
{
    long long result = (long long)a * b;

    return (int)(result % mod);
}
/*
тоже самое с умножением
field_sub(a, b, mod) a*b(mod)

long long используеться для безумно огромных чисел
int не подойдет так как его диапазон очень ограничен, а тип long long может вмещать в себя примерно 9 000 000 000 000 000 000

(long long)a * b                        превращает тип int в тим long long чтобы засунуть в long long result
(int)(result % mod)                     делит result на mod и превращает обратно в int
*/

Matrix mat_create(size_t n, int mod) // роль 4
{
    Matrix new_matrix;

    new_matrix.rows = n;
    new_matrix.cols = n;
    new_matrix.mod = mod;

    size_t size = n * n * sizeof(int);

    new_matrix.data = (int*)malloc(size);

    if(new_matrix.data == NULL){
        new_matrix.rows = 0;
        new_matrix.cols = 0;
        new_matrix.is_dynamic = 0;
        
        return new_matrix;
    }

    for(size_t i = 0; i < n * n; i++){
        new_matrix.data[i] = 0;

    }

    new_matrix.is_dynamic = (size > 16 * 1024) ? 1 : 0;

    return new_matrix;
}
/*
эта функция создает матрицу 5x5 или 100x100 (квадратная короче) и возвращает ее

Matrix new_matrix;                                      создаем матрицу

new_matrix.rows = n;                                    заполняем поля
new_matrix.cols = n;                                    ну тут вроде нет ничего непонятного
new_matrix.mod = mod;

size_t size = n * n * sizeof(int);                      считаем размер выделения памяти для матрицы NxN
new_matrix.data = (int*)malloc(size);                   выделяем рассчитаную память для нашей матрицы

(new_matrix.data == NULL)                               лучше всегда делать проверку на выделение памяти

for(){new_matrix.data[i] = 0;}                          заполняем нулями (т.к. создаем матрицу из ничего)

new_matrix.is_dynamic = (size > 16 * 1024) ? 1 : 0;     проверка на размер матрицы - больше 16 Кб или нет

? :  - это тренарный оператор (сокращенный if),
т.е. если size > 16*1024 истинна то is_dynamic = 1, если ложь, то is_dynamic = 0

return new_matrix;                                      ну и возвращаем матрицу которую мы создали
*/

void mat_free(Matrix* new_matrix) // роль 4
{
    if(new_matrix->data != NULL){
        free(new_matrix->data);
        new_matrix->data = NULL;
    }

    new_matrix->rows = 0;
    new_matrix->cols = 0;
}
/*
функция освобождает память, которую занимала матрица

Matrix* new_matrix                      передаем указатель так как изменяем оригинальную матрицу, а не ее копию

new_matrix->data != NULL                проверяем есть ли у матрицы выдленная память

free(new_matrix->data);                 освобождаем память

new_matrix->data = NULL;                флаг

без очистки памяти, она будет утекать и не освобождаться, что привело бы к падению программы 
*/

int mat_fill_random(Matrix* new_matrix) // роль 4
{
    if(new_matrix->data == NULL) return 1;

    for(size_t i = 0; i < new_matrix->rows * new_matrix->cols; i++){
        new_matrix->data[i] = rand() % new_matrix->mod;
    }

    return 0;
}
/*
функция заполняет матрицу рандомными числами

new_matrix->data == NULL                                        проверка на существование матрицы

size_t i = 0; i < new_matrix->rows * new_matrix->cols; i++      идем по всем элементам
new_matrix->data[i] = rand() % new_matrix->mod;                 каждый элемент -> рандомное число

если матрица заполнилась успешно, то вернеться 0, если нет, то 1. 
поэтому ты используем int, а не void перед функцией
*/

int mat_print(const Matrix* new_matrix, const char* name_data) // роль 4
{
    if(new_matrix->data == NULL) return 1;

    printf("%s:\n", name_data);

    for(size_t i = 0; i < new_matrix->rows; i++){
        for(size_t j = 0; j < new_matrix->cols; j++){
            printf("%4d ", new_matrix->data[i * new_matrix->cols + j]);
        }

        printf("\n");
    }

    return 0;
}
/*
функция вывода матрицы
пример:
    матрица A:
    1   2   
    4   5   

new_matrix->data == NULL                    проверка на существование данных в матрице

вывод через вложеный цикл и после каждой строки "\n"

*/

int naive_mul() // роль 2
{

}
/*
функция умножает две матрицы обычным способом (три вложенных цикла)

*/

Matrix mat_submatrix(const Matrix* mat_origin, size_t row_begin, size_t col_begin, size_t block_size) // роль 4
{
    Matrix block = mat_create(block_size, mat_origin->mod);

    if(block.data == NULL) return block;

    for(size_t i = 0; i < block_size; i++){
        for(size_t j = 0; j < block_size; j++){
            block.data[i * block_size + j] = mat_origin->data[(row_begin + i) * mat_origin->cols + (col_begin + j)];
        }
    }

    return block;
}
/*
создание кусочка матрицы - 4 блока (по алгоритму Штрассена)

const Matrix* mat_origin                    исходная матрица
size_t row_begin                            строка начала
size_t col_begin                            столбец начала
size_t block_size                           размер блока который будем вырезать (4 -> 4x4)

Matrix block = mat_create(block_size, mat_origin->mod);
создание матрицы размера block_size x block_size

block.data == NULL                          проверка выделения памяти

block.data[i * block_size + j] = mat_origin->data[(row_begin + i) * mat_origin->cols + (col_begin + j)];
это самая основная и самая сложная строчка. мы копируем данные из оригинальной матрицы block.data[] = mat_origin->data[]

возвращаем получившуюся матрицу (блок)
*/

void mat_set_submatrix(Matrix* insert_mat, size_t row_begin, size_t col_begin, const Matrix* block) // роль 4
{
    size_t n = block->rows;

    for(size_t i = 0; i < n; i++){
        for(size_t j = 0; j < n; j++){
            insert_mat->data[(row_begin + i) * insert_mat->cols + (col_begin + j)] = block->data[i * n + j];
        }
    }
}
/*
обратное действие к mat_submatrix() 
вставляет маленький блок обратно в большую матрицу

Matrix* insert_mat                      матрица, в которую будем вставлять
size_t col_begin                        с какой строки начинать вставку
size_t row_begin                        с какого столбца начинать вставку
const Martix* block                     мини-блок который будем вставлять

size_t n = block->rows                  обьявляем размер блока (квадрат поэтому rows=cols)

insert_mat->data[(row_begin + i) * insert_mat->cols + (col_begin + j)] = block->data[i * n + j];

вычисляем индекс в матрице которую будем вставлять чтобы положить туда нужную матрицу
(row_start + i)                         номер строки
insert_mat->cols                        умножаем на ширину, чтобы перейки к нужной строке
(col_begin + j)                         прибавляем номер столбца

block->data[i * n + j]                  вычисляем откуда берем
i * n + j                               это индекс элемента в маленьком блоке

*/

Matrix mat_add() // роль 4
{

}
/*
функция сложения двух матриц поэлементно

*/

Matrix mat_sub() // роль 4
{

}
/*
функция вычитания двух матриц поэлементно

*/

int strassen_mul() // роль 1
{

}
/*
глаыная функция умножения двух матриц по алгоритму Штрассена

*/

double test_perf() // роль 5
{

}
/*
замер времени умножения матриц

*/

int main(void){    // роль 4


    return 0;
}