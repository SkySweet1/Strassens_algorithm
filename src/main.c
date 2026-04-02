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

int field_add() // роль 3
{

}
/*
эта функция будет заварачивать числа в модуль
ну типо 5 (mod 4) = 1 остаток

ну только у нас будет field_add(a, b, mod) -> a+b(mod), то есть field_add(5, 6, 10) -> 1 
*/

int field_sub() // роль 3
{

}
/*
тоже самое с вычитанием 
field_sub(a, b, mod) a-b(mod)
*/

int field_mul() // роль 3
{

}
/*
тоже самое с умножением
field_sub(a, b, mod) a*b(mod)
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

void mat_free() // роль 4
{

}
/*
функция освобождает память, которую занимала матрица

*/

int mat_fill_random() // роль 4
{

}
/*
функция заполняет матрицу рандомными числами

*/

int mat_print() // роль 4
{

}
/*
функция вывода матрицы

*/

int naive_mul() // роль 2
{

}
/*
функция умножает две матрицы обычным способом (три вложенных цикла)

*/

Matrix mat_submatrix() // роль 4
{

}
/*
создание кусочка матрицы - 4 блока (по алгоритму Штрассена)

*/

void mat_set_submatrix() // роль 4
{

}
/*
обратное действие к mat_submatrix() 
вставляет маленький блок обратно в большую матрицу

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