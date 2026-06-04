/*
роль 1              Разработка модификации Штрассена (формулы)              strassen_mul
роль 2              Проверка корректности, спецификации                     naive_mul
роль 3              Реализация операций в поле                              field_add, sub, mul
роль 4              Сборка всего вместе (ввод-вывод, память)                mat_create, free main и др.
роль 5              Проверка корректности и скорости                        test_perf
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define THRESHOLD 64
/*
Наша задача заключается в решении проблемы алгоритма Штрассена
У нас есть формула, которая быстро умножает две квадратные матрицы(n x n), но
для маленьких матриц она работает намного медленнее, поэтому мы вводим такое число,
с которого будет применяться этот алгоритм (еще раз -> если меньше, то просто бессмысленно(долго))

THRESHOLD-это порог вхождения матрицы, при котором умножение ещё будет происходить по алгоритму Штрассена
*/

#define MIN_MAT_SIZE 1
#define MAX_MAT_SIZE 1024
/*
Выводим здесь #define, для того чтобы можно было менять при необходимости
*/

typedef struct {
    size_t rows;
    size_t cols;
    int mod;
    int* data;
    int is_dynamic;
} Matrix;
/*
size_t-это тип хранения размеров (строк столбцов)
Вмещает любой размер массива в памяти (а так же неотрицательное)

Используем не int, потому что слишком мало байт (там вроде максимум 32к байт, что мало)

typedef struct {} Matrix;-для того чтобы не писать каждый раз struct Matrix, а просто Matrix

Мы создали тип матрицы                      typedef struct {} Matrix;
Размеры (неотрицательные большие)                   size_t rows, cols;
Модуль поля                                         int mod;
Указатель на числа матрицы                          int* data;
Флаг, что матрица большая (освободить память)        int is_dynamic;
*/

int field_add(int a, int b, int mod) // роль 3
{
    int sum = a + b;
    int corrected;

    if(sum >= mod){
        corrected = sum - mod;
    } else{
        corrected = sum;
    }

    return corrected;

}
/*
Данная функция будет заворачивать числа в модуль
Например: 5 (mod 4) = 1 остаток
Если получившаяся сумма больше либо равна модулю, то происходит вычитание sum - mod, 
иначе просто присваиваем переменной corrected значение sum, после проверки возвращаем получившееся значение переменной corrected

Только в нашем случае, у нас будет field_add(a, b, mod) -> a+b(mod), то есть field_add(5, 6, 10) -> 1 
*/

int field_sub(int a, int b, int mod) // роль 3
{

    int difference = a - b;

    if(difference >= 0){
        return difference;
    } else{
        if(mod > 0){
            difference += mod;
        }
        return difference;
    }

    
}
/*
Та же функция, но уже с вычитанием 
field_sub(a, b, mod) a-b(mod)
Если получившаяся разность больше либо равна 0, то сразу возвращаем значение difference,
иначе (делаем дополнительную проверку на неотрицательность модуля) добавляем mod к difference и уже после возвращаем значение difference


*/

int field_mul(int a, int b, int mod) // роль 3
{
    long long result = (long long)a * b;

    return (int)(result % mod);
}
/*
Тоже самое с умножением
field_sub(a, b, mod) a*b(mod)

long long используется для безумно больших чисел
int не подойдет, так как его диапазон очень ограничен, а тип long long может вмещать в себя примерно 9 000 000 000 000 000 000

(long long)a * b                        Превращает тип int в тип long long, чтобы положить в long long result
(int)(result % mod)                     Делит result на mod и превращает обратно в int
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
Данная функция создает матрицу 5x5 или 100x100 (то есть квадратная) и возвращает ее

Matrix new_matrix;                                      Создаем матрицу

new_matrix.rows = n;                                    Заполняем поля
new_matrix.cols = n;                                    
new_matrix.mod = mod;

size_t size = n * n * sizeof(int);                      Считаем размер выделения памяти для матрицы NxN
new_matrix.data = (int*)malloc(size);                   Выделяем рассчитанную память для нашей матрицы

(new_matrix.data == NULL)                               Всегда лучше делать проверку на выделение памяти

for(){new_matrix.data[i] = 0;}                          Заполняем нулями (т.к. создаем матрицу из ничего)

new_matrix.is_dynamic = (size > 16 * 1024) ? 1 : 0;     Проверка на размер матрицы - больше 16 Кб или нет

? :  - это тренарный оператор (сокращенный if),
т.е. если size > 16*1024 истинна то is_dynamic = 1, если ложь, то is_dynamic = 0

return new_matrix;                                      Возвращаем матрицу которую мы создали
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
Данная функция освобождает память, которую занимала матрица

Matrix* new_matrix                      Передаем указатель, так как изменяем оригинальную матрицу, а не ее копию

new_matrix->data != NULL                Проверяем есть ли у матрицы выделенная память

free(new_matrix->data);                 Освобождаем память

new_matrix->data = NULL;                Флаг

Без очистки памяти, так как она будет утекать и не будет освобождаться, что приведет к падению программы 
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
Данная функция заполняет матрицу случайными числами

new_matrix->data == NULL                                        Проверка на существование матрицы

size_t i = 0; i < new_matrix->rows * new_matrix->cols; i++      Проходимся по всем элементам
new_matrix->data[i] = rand() % new_matrix->mod;                 Каждый элемент -> случайное число

Если матрица заполнилась успешно, то вернется 0, если нет, то 1. 
Поэтому мы используем int, а не void перед функцией
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
Данная функция для вывода матрицы
Пример:
    матрица A:
    1   2   
    4   5   

new_matrix->data == NULL                    Проверка на существование данных в матрице

Вывод через вложенный цикл и после каждой строки "\n"

*/

int naive_mul(const Matrix* A, const Matrix* B, Matrix* C) // роль 2
{
    if(A->data == NULL || B->data == NULL || C->data == NULL){
        return 1;
    }

    if(A->cols != B->rows){
        return 2;
    }

    if(A->rows != C->rows || B->cols != C->cols){
        return 3;
    }

    if(A->mod != B->mod || A->mod != C->mod){
        return 4;
    }

    size_t n = A->rows;
    size_t m = A->cols;
    size_t p = B->cols;

    for(size_t i = 0; i < n; i++){
        for(size_t j = 0; j < p; j++){
            int summ = 0;

            for(size_t k = 0; k < m; k++){
                int a = A->data[i * m + k];
                int b = B->data[k * p + j];

                summ = field_add(summ, field_mul(a, b, A->mod), A->mod);

            }

            C->data[i * p + j] = summ;
        }
    }

    return 0;

};
/*
Данная функция умножает две матрицы стандартным способом (три вложенных цикла)

if(A->data == NULL || B->data == NULL || C->data == NULL)                   Проверяем, что все три матрицы имеют выделенную память
if(A->cols != B->rows)                                                      умножать можно только если ширина А равна высоте B - правило умножения матриц
if(A->rows != C->rows || B->cols != C->cols)                                результат С должен иметь размер (сторки А)х(столбцы В)
if(A->mod != B->mod || A->mod != C->mod)                                    все три матрицы должны работать в одном поле (с одинаковым модулем)

size_t n = A->rows; size_t m = A->cols; size_t p = B->cols;                 для матрицы А размером n x m, B размером m x p, C результат будет n x p

for(size_t i = 0; i < n; i++)                                               цикл по строкам А
for(size_t j = 0; j < p; j++)                                               цикл по столбцам В
int sum = 0;                                                                начало с нуля
for(size_t k = 0; k < m; k++)                                               цикл по общему измерению

int a = A->data[i * m + k];                                                 А[i][k]
int b = B->data[k * p + j];                                                 B[k][j]

summ = field_add(summ, field_mul(a, b, A->mod), A->mod);
field_mul(a, b, A->mod)                                                     умножает а и b
field_add(summ, ... , A->mod);                                              прибавляет к накопленной сумме

каждое действие должно заворачиваться по модулю, иначе было бы summ = summ + a * b (что неправильно)
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
Создание кусочка матрицы - 4 блока (по алгоритму Штрассена)

const Matrix* mat_origin                    Исходная матрица
size_t row_begin                            Строка начала
size_t col_begin                            Столбец начала
size_t block_size                           Размер блока, который будем вырезать (4 -> 4x4)

Matrix block = mat_create(block_size, mat_origin->mod);
Создание матрицы размера block_size x block_size

block.data == NULL                          Проверка выделения памяти

block.data[i * block_size + j] = mat_origin->data[(row_begin + i) * mat_origin->cols + (col_begin + j)];
Это самая основная и самая сложная строчка. Мы копируем данные из оригинальной матрицы block.data[] = mat_origin->data[]

Возвращаем получившуюся матрицу (блок)
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
Обратное действие к mat_submatrix() 
Вставляет маленький блок обратно в большую матрицу

Matrix* insert_mat                      Матрица, в которую будем вставлять
size_t col_begin                        С какой строки начинать вставку
size_t row_begin                        С какого столбца начинать вставку
const Martix* block                     Мини-блок, который будем вставлять

size_t n = block->rows                  Обьявляем размер блока (квадрат, поэтому rows=cols)

insert_mat->data[(row_begin + i) * insert_mat->cols + (col_begin + j)] = block->data[i * n + j];

Вычисляем индекс в матрице, которую будем вставлять, чтобы положить туда нужную матрицу
(row_start + i)                         Номер строки
insert_mat->cols                        Умножаем на ширину, чтобы перейки к нужной строке
(col_begin + j)                         Прибавляем номер столбца

block->data[i * n + j]                  Вычисляем откуда берем
i * n + j                               Это индекс элемента в маленьком блоке

*/

Matrix mat_add(const Matrix* A, const Matrix* B) // роль 4
{
    Matrix C = mat_create(A->rows, A->mod);

    if(C.data == NULL) return C;

    for(size_t i = 0; i < A->rows * A->cols; i++){
        C.data[i] = field_add(A->data[i], B->data[i], A->mod);
    }

    return C;
}
/*
Данная функция для сложения двух матриц поэлементно

const Matrix* A, const Matrix* B                        Принимаем две матрицы

Matrix C = mat_create(A->rows, A->mod);                 Создаем пустую матрицы такого же размера, как А и с таким же модулем

(size_t i = 0; i < A->rows * A->cols; i++)              Делаем один цикл, так как матрица хранится, как плоский массив

C.data[i] = field_add(A->data[i], B->data[i], A->mod);  Сложение двух чисел из матриц А, В, далее кладет результат в матрицы С, но с учетом модуля

Возвращаем матрицу С
*/

Matrix mat_sub(const Matrix* A, const Matrix* B) // роль 4
{
    Matrix C = mat_create(A->rows, A->mod);

    if(C.data == NULL) return C;

    for(size_t i = 0; i < A->rows * A->cols; i++){
        C.data[i] = field_sub(A->data[i], B->data[i], A->mod);
    }

    return C;

}
/*
Данная функция для вычитания двух матриц поэлементно

const Matrix* A, const Matrix* B                        Принимаем две матрицы

Matrix C = mat_create(A->rows, A->mod);                 Создаем пустую матрицы такого же размера, как А и с таким же модулем

(size_t i = 0; i < A->rows * A->cols; i++)              Делаем один цикл, так как матрица хранится, как плоский массив

C.data[i] = field_sub(A->data[i], B->data[i], A->mod);  Вычитание двух чисел из матриц А, В, далее кладет результат в матрицы С, но с учетом модуля

Возвращаем матрицу С
*/


int strassen_mul(const Matrix* A, const Matrix* B, Matrix* C) // роль 1
{
    // Ошибка, если не выделится память
    if (A->data == NULL || B->data == NULL || C->data == NULL) {
        return 1;
    }
    
    // Ошибка, если матрицы не кввадратные или разных размеров
    if (A->rows != A->cols || B->rows != B->cols || A->rows != B->rows) {
        return 2;
    }
    
    // Ошибка, если размер матрицы С не совпадает с размером А
    if (A->rows != C->rows || A->cols != C->cols) {
        return 2;
    }
    
    // Ошибка, если модули не совпадают
    if (A->mod != B->mod || A->mod != C->mod) {
        return 4;
    }
    
    size_t n = A->rows; 
    
    if (n <= THRESHOLD) {
        return naive_mul(A, B, C);
    }
    
    size_t m = 1;

    while (m < n) m <<= 1;
    
    Matrix A_ext = mat_create(m, A->mod);
    Matrix B_ext = mat_create(m, B->mod);
    Matrix C_ext = mat_create(m, C->mod);
    
    if (A_ext.data == NULL || B_ext.data == NULL || C_ext.data == NULL) {
        if (A_ext.data != NULL) mat_free(&A_ext);
        if (B_ext.data != NULL) mat_free(&B_ext);
        if (C_ext.data != NULL) mat_free(&C_ext);

        // Ошибка, если память на матрицы не выделится
        return 1;
    }
    
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            A_ext.data[i * m + j] = A->data[i * n + j];
            B_ext.data[i * m + j] = B->data[i * n + j];
        }
    }
    
    size_t k = m / 2;
    
    Matrix A11 = mat_submatrix(&A_ext, 0,   0,   k);
    Matrix A12 = mat_submatrix(&A_ext, 0,   k,   k);
    Matrix A21 = mat_submatrix(&A_ext, k,   0,   k);
    Matrix A22 = mat_submatrix(&A_ext, k,   k,   k);
    
    Matrix B11 = mat_submatrix(&B_ext, 0,   0,   k);
    Matrix B12 = mat_submatrix(&B_ext, 0,   k,   k);
    Matrix B21 = mat_submatrix(&B_ext, k,   0,   k);
    Matrix B22 = mat_submatrix(&B_ext, k,   k,   k);
    
    if (A11.data == NULL || A12.data == NULL || A21.data == NULL || A22.data == NULL || B11.data == NULL || B12.data == NULL || B21.data == NULL || B22.data == NULL)
    {

        if (A11.data != NULL) mat_free(&A11);
        if (A12.data != NULL) mat_free(&A12);
        if (A21.data != NULL) mat_free(&A21);
        if (A22.data != NULL) mat_free(&A22);
        if (B11.data != NULL) mat_free(&B11);
        if (B12.data != NULL) mat_free(&B12);
        if (B21.data != NULL) mat_free(&B21);
        if (B22.data != NULL) mat_free(&B22);

        mat_free(&A_ext);
        mat_free(&B_ext);
        mat_free(&C_ext);

        // Ошибка, если память не выделится
        return 1;
    }
    
    Matrix S1 = mat_sub(&B12, &B22); 
    Matrix S2 = mat_add(&A11, &A12);
    Matrix S3 = mat_add(&A21, &A22);
    Matrix S4 = mat_sub(&B21, &B11);
    Matrix S5 = mat_add(&A11, &A22);
    Matrix S6 = mat_add(&B11, &B22);
    Matrix S7 = mat_sub(&A12, &A22);
    Matrix S8 = mat_add(&B21, &B22);
    Matrix S9 = mat_sub(&A11, &A21);
    Matrix S10 = mat_add(&B11, &B12);
    
    if (S1.data == NULL || S2.data == NULL || S3.data == NULL || S4.data == NULL || S5.data == NULL || S6.data == NULL || S7.data == NULL || S8.data == NULL || S9.data == NULL || S10.data == NULL)
    {

        if (S1.data != NULL) mat_free(&S1);
        if (S2.data != NULL) mat_free(&S2);
        if (S3.data != NULL) mat_free(&S3);
        if (S4.data != NULL) mat_free(&S4);
        if (S5.data != NULL) mat_free(&S5);
        if (S6.data != NULL) mat_free(&S6);
        if (S7.data != NULL) mat_free(&S7);
        if (S8.data != NULL) mat_free(&S8);
        if (S9.data != NULL) mat_free(&S9);
        if (S10.data != NULL) mat_free(&S10);
        if (A11.data != NULL) mat_free(&A11);
        if (A12.data != NULL) mat_free(&A12);
        if (A21.data != NULL) mat_free(&A21);
        if (A22.data != NULL) mat_free(&A22);
        if (B11.data != NULL) mat_free(&B11);
        if (B12.data != NULL) mat_free(&B12);
        if (B21.data != NULL) mat_free(&B21);
        if (B22.data != NULL) mat_free(&B22);

        mat_free(&A_ext);
        mat_free(&B_ext);
        mat_free(&C_ext);

        // Ошибка, если память не выделится
        return 1;
    }
    
    Matrix P1 = mat_create(k, A->mod);
    Matrix P2 = mat_create(k, A->mod);
    Matrix P3 = mat_create(k, A->mod);
    Matrix P4 = mat_create(k, A->mod);
    Matrix P5 = mat_create(k, A->mod);
    Matrix P6 = mat_create(k, A->mod);
    Matrix P7 = mat_create(k, A->mod);
    
    if (P1.data == NULL || P2.data == NULL || P3.data == NULL || P4.data == NULL || P5.data == NULL || P6.data == NULL || P7.data == NULL)
    {

        if (P1.data != NULL) mat_free(&P1);
        if (P2.data != NULL) mat_free(&P2);
        if (P3.data != NULL) mat_free(&P3);
        if (P4.data != NULL) mat_free(&P4);
        if (P5.data != NULL) mat_free(&P5);
        if (P6.data != NULL) mat_free(&P6);
        if (P7.data != NULL) mat_free(&P7);
        if (S1.data != NULL) mat_free(&S1);
        if (S2.data != NULL) mat_free(&S2);
        if (S3.data != NULL) mat_free(&S3);
        if (S4.data != NULL) mat_free(&S4);
        if (S5.data != NULL) mat_free(&S5);
        if (S6.data != NULL) mat_free(&S6);
        if (S7.data != NULL) mat_free(&S7);
        if (S8.data != NULL) mat_free(&S8);
        if (S9.data != NULL) mat_free(&S9);
        if (S10.data != NULL) mat_free(&S10);
        if (A11.data != NULL) mat_free(&A11);
        if (A12.data != NULL) mat_free(&A12);
        if (A21.data != NULL) mat_free(&A21);
        if (A22.data != NULL) mat_free(&A22);
        if (B11.data != NULL) mat_free(&B11);
        if (B12.data != NULL) mat_free(&B12);
        if (B21.data != NULL) mat_free(&B21);
        if (B22.data != NULL) mat_free(&B22);

        mat_free(&A_ext);
        mat_free(&B_ext);
        mat_free(&C_ext);

        // Ошибка, если память не выделится
        return 1;
    }
    
    int err;
    
    err = strassen_mul(&A11, &S1, &P1);

    if (err != 0)
    {
        mat_free(&P1); mat_free(&P2); 
        mat_free(&P3); mat_free(&P4);
        mat_free(&P5); mat_free(&P6); 
        mat_free(&P7); mat_free(&S1); 
        mat_free(&S2); mat_free(&S3); 
        mat_free(&S4); mat_free(&S5); 
        mat_free(&S6); mat_free(&S7); 
        mat_free(&S8); mat_free(&S9); 
        mat_free(&S10); mat_free(&A11); 
        mat_free(&A12); mat_free(&A21); 
        mat_free(&A22); mat_free(&B11); 
        mat_free(&B12); mat_free(&B21); 
        mat_free(&B22); mat_free(&A_ext); 
        mat_free(&B_ext); mat_free(&C_ext);

        // Проверка, что умножение по алгоритму
        return err; 
    }
    
    err = strassen_mul(&S2, &B22, &P2);

    if (err != 0)
    {
        mat_free(&P1); mat_free(&P2); 
        mat_free(&P3); mat_free(&P4);
        mat_free(&P5); mat_free(&P6); 
        mat_free(&S1); mat_free(&S2); 
        mat_free(&S3); mat_free(&S4);
        mat_free(&S5); mat_free(&S6); 
        mat_free(&S7); mat_free(&S8);
        mat_free(&S9); mat_free(&S10);
        mat_free(&A11); mat_free(&A12); 
        mat_free(&A21); mat_free(&A22);
        mat_free(&B11); mat_free(&B12); 
        mat_free(&B21); mat_free(&B22);
        mat_free(&A_ext); mat_free(&B_ext); 
        mat_free(&C_ext); mat_free(&P7);

        return err; 
    }
    
    err = strassen_mul(&S3, &B11, &P3);

    if (err != 0)
    {
        mat_free(&P1); mat_free(&P2); 
        mat_free(&P3); mat_free(&P4);
        mat_free(&P5); mat_free(&P6); 
        mat_free(&S1); mat_free(&S2); 
        mat_free(&S3); mat_free(&S4);
        mat_free(&S5); mat_free(&S6); 
        mat_free(&S7); mat_free(&S8);
        mat_free(&S9); mat_free(&S10);
        mat_free(&A11); mat_free(&A12); 
        mat_free(&A21); mat_free(&A22);
        mat_free(&B11); mat_free(&B12); 
        mat_free(&B21); mat_free(&B22);
        mat_free(&A_ext); mat_free(&B_ext); 
        mat_free(&C_ext); mat_free(&P7);

        return err; 
    }
    
    err = strassen_mul(&A22, &S4, &P4);

    if (err != 0)
    {
        mat_free(&P1); mat_free(&P2); 
        mat_free(&P3); mat_free(&P4);
        mat_free(&P5); mat_free(&P6); 
        mat_free(&S1); mat_free(&S2); 
        mat_free(&S3); mat_free(&S4);
        mat_free(&S5); mat_free(&S6); 
        mat_free(&S7); mat_free(&S8);
        mat_free(&S9); mat_free(&S10);
        mat_free(&A11); mat_free(&A12); 
        mat_free(&A21); mat_free(&A22);
        mat_free(&B11); mat_free(&B12); 
        mat_free(&B21); mat_free(&B22);
        mat_free(&A_ext); mat_free(&B_ext); 
        mat_free(&C_ext); mat_free(&P7);

        return err; 
    }
    
    err = strassen_mul(&S5, &S6, &P5);

    if (err != 0)
    {
        mat_free(&P1); mat_free(&P2); 
        mat_free(&P3); mat_free(&P4);
        mat_free(&P5); mat_free(&P6); 
        mat_free(&S1); mat_free(&S2); 
        mat_free(&S3); mat_free(&S4);
        mat_free(&S5); mat_free(&S6); 
        mat_free(&S7); mat_free(&S8);
        mat_free(&S9); mat_free(&S10);
        mat_free(&A11); mat_free(&A12); 
        mat_free(&A21); mat_free(&A22);
        mat_free(&B11); mat_free(&B12); 
        mat_free(&B21); mat_free(&B22);
        mat_free(&A_ext); mat_free(&B_ext); 
        mat_free(&C_ext); mat_free(&P7);
        return err; }
    
    err = strassen_mul(&S7, &S8, &P6);

    if (err != 0)
    {
        mat_free(&P1); mat_free(&P2); 
        mat_free(&P3); mat_free(&P4);
        mat_free(&P5); mat_free(&P6); 
        mat_free(&S1); mat_free(&S2); 
        mat_free(&S3); mat_free(&S4);
        mat_free(&S5); mat_free(&S6); 
        mat_free(&S7); mat_free(&S8);
        mat_free(&S9); mat_free(&S10);
        mat_free(&A11); mat_free(&A12); 
        mat_free(&A21); mat_free(&A22);
        mat_free(&B11); mat_free(&B12); 
        mat_free(&B21); mat_free(&B22);
        mat_free(&A_ext); mat_free(&B_ext); 
        mat_free(&C_ext); mat_free(&P7);

        return err; 
    }
    
    err = strassen_mul(&S9, &S10, &P7);

    if (err != 0)
    {
        mat_free(&P1); mat_free(&P2); 
        mat_free(&P3); mat_free(&P4);
        mat_free(&P5); mat_free(&P6); 
        mat_free(&S1); mat_free(&S2); 
        mat_free(&S3); mat_free(&S4);
        mat_free(&S5); mat_free(&S6); 
        mat_free(&S7); mat_free(&S8);
        mat_free(&S9); mat_free(&S10);
        mat_free(&A11); mat_free(&A12); 
        mat_free(&A21); mat_free(&A22);
        mat_free(&B11); mat_free(&B12); 
        mat_free(&B21); mat_free(&B22);
        mat_free(&A_ext); mat_free(&B_ext); 
        mat_free(&C_ext); mat_free(&P7);

        return err; 
    }
    
    Matrix t1 = mat_add(&P5, &P4);

    Matrix t2 = mat_sub(&t1, &P2);

    Matrix C11 = mat_add(&t2, &P6);

    Matrix C12 = mat_add(&P1, &P2);

    Matrix C21 = mat_add(&P3, &P4);
    
    Matrix t3 = mat_add(&P5, &P1);

    Matrix t4 = mat_sub(&t3, &P3);

    Matrix C22 = mat_sub(&t4, &P7);
    
    if (t1.data == NULL || t2.data == NULL || C11.data == NULL || C12.data == NULL || C21.data == NULL || t3.data == NULL || t4.data == NULL || C22.data == NULL)
    {
        mat_free(&A_ext); mat_free(&B_ext); 
        mat_free(&A11); mat_free(&A12); 
        mat_free(&A21); mat_free(&A22);
        mat_free(&B11); mat_free(&B12); 
        mat_free(&B21); mat_free(&B22);
        mat_free(&S1); mat_free(&S2); 
        mat_free(&S3); mat_free(&S4);
        mat_free(&S5); mat_free(&S6); 
        mat_free(&S7); mat_free(&S8);
        mat_free(&S9); mat_free(&S10);
        mat_free(&P1); mat_free(&P2); 
        mat_free(&P3); mat_free(&P4);
        mat_free(&P5); mat_free(&P6); 
        mat_free(&P7); mat_free(&C_ext);
        mat_free(&t1); mat_free(&t2); 
        mat_free(&t3); mat_free(&t4);
        mat_free(&C11); mat_free(&C12); 
        mat_free(&C21); mat_free(&C22);

        // Ошибка, если не выделили память
        return 1;
    }
    
    mat_set_submatrix(&C_ext, 0,   0,   &C11);
    mat_set_submatrix(&C_ext, 0,   k,   &C12);
    mat_set_submatrix(&C_ext, k,   0,   &C21);
    mat_set_submatrix(&C_ext, k,   k,   &C22);
    
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            C->data[i * n + j] = C_ext.data[i * m + j];
        }
    }
    
    mat_free(&A_ext); mat_free(&B_ext); 
    mat_free(&A11); mat_free(&A12); 
    mat_free(&A21); mat_free(&A22);
    mat_free(&B11); mat_free(&B12); 
    mat_free(&B21); mat_free(&B22);
    mat_free(&S1); mat_free(&S2); 
    mat_free(&S3); mat_free(&S4);
    mat_free(&S5); mat_free(&S6); 
    mat_free(&S7); mat_free(&S8);
    mat_free(&S9); mat_free(&S10);
    mat_free(&P1); mat_free(&P2); 
    mat_free(&P3); mat_free(&P4);
    mat_free(&P5); mat_free(&P6); 
    mat_free(&P7); mat_free(&C_ext);
    mat_free(&t1); mat_free(&t2); 
    mat_free(&t3); mat_free(&t4);
    mat_free(&C11); mat_free(&C12); 
    mat_free(&C21); mat_free(&C22);
    
    return 0;
}
/*
Главная функция умножения двух матриц по алгоритму Штрассена.

    size_t n = A->rows;                                     сохранение размера матрицы в n

    if (n <= THRESHOLD) {                                   если размер у матрицы маленький, то наивное умножение
        return naive_mul(A, B, C);
    }

    size_t m = 1;                                           ищем близжайшую степень 2 (алгоритм штрассена
    while (m < n) m <<= 1;                                  можно применить только к матрицам с размером степени 2)

    Matrix A_ext = mat_create(m, A->mod);                   расширенные матрицы, дополенные нулями
    Matrix B_ext = mat_create(m, B->mod);
    Matrix C_ext = mat_create(m, C->mod);

    for (size_t i = 0; i < n; i++) {                        коприрование матриц А и В в левый верхний
        for (size_t j = 0; j < n; j++) {                    угол
            A_ext.data[i * m + j] = A->data[i * n + j];
            B_ext.data[i * m + j] = B->data[i * n + j];
        }
    }

    size_t k = m / 2;                                       размер половины матрицы

    Matrix A11 = mat_submatrix(&A_ext, 0,   0,   k);        делим матрицы А и В на 4 части
    Matrix A12 = mat_submatrix(&A_ext, 0,   k,   k);
    Matrix A21 = mat_submatrix(&A_ext, k,   0,   k);
    Matrix A22 = mat_submatrix(&A_ext, k,   k,   k);
    
    Matrix B11 = mat_submatrix(&B_ext, 0,   0,   k);
    Matrix B12 = mat_submatrix(&B_ext, 0,   k,   k);
    Matrix B21 = mat_submatrix(&B_ext, k,   0,   k);
    Matrix B22 = mat_submatrix(&B_ext, k,   k,   k);

    Matrix S1 = mat_sub(&B12, &B22);                        вычисляем  формузы для алгоритма
    Matrix S2 = mat_add(&A11, &A12);
    Matrix S3 = mat_add(&A21, &A22);
    Matrix S4 = mat_sub(&B21, &B11);
    Matrix S5 = mat_add(&A11, &A22);
    Matrix S6 = mat_add(&B11, &B22);
    Matrix S7 = mat_sub(&A12, &A22);
    Matrix S8 = mat_add(&B21, &B22);
    Matrix S9 = mat_sub(&A11, &A21);
    Matrix S10 = mat_add(&B11, &B12);


    Matrix P1 = mat_create(k, A->mod);                      создаются матрицы Р и заполняются 0
    Matrix P2 = mat_create(k, A->mod);
    Matrix P3 = mat_create(k, A->mod);
    Matrix P4 = mat_create(k, A->mod);
    Matrix P5 = mat_create(k, A->mod);
    Matrix P6 = mat_create(k, A->mod);
    Matrix P7 = mat_create(k, A->mod);

    err = strassen_mul(&A11, &S1, &P1);                     создается переменная для ошибок и проверяет, 
    err = strassen_mul(&S2, &B22, &P2);                     что можно умножить матрицы для формул для алгоритма штрассена,
    err = strassen_mul(&S3, &B11, &P3);                     параллельно заполняем Р
    err = strassen_mul(&A22, &S4, &P4);
    err = strassen_mul(&S5, &S6, &P5);
    err = strassen_mul(&S7, &S8, &P6);
    err = strassen_mul(&S9, &S10, &P7);


    Matrix t1 = mat_add(&P5, &P4);                          создаем еще дополнительные матрицы для вычисления результата

    Matrix t2 = mat_sub(&t1, &P2);

    Matrix C11 = mat_add(&t2, &P6);
    
    Matrix C12 = mat_add(&P1, &P2);
    
    Matrix C21 = mat_add(&P3, &P4);
    
    Matrix t3 = mat_add(&P5, &P1);

    Matrix t4 = mat_sub(&t3, &P3);

    Matrix C22 = mat_sub(&t4, &P7);

    mat_set_submatrix(&C_ext, 0,   0,   &C11);              вставляем блоки матрицы обратно
    mat_set_submatrix(&C_ext, 0,   k,   &C12);
    mat_set_submatrix(&C_ext, k,   0,   &C21);
    mat_set_submatrix(&C_ext, k,   k,   &C22);
    
    for (size_t i = 0; i < n; i++) {                        копирование результата из C_ext в С
        for (size_t j = 0; j < n; j++) {
            C->data[i * n + j] = C_ext.data[i * m + j];
        }
    }


*/

#include <sys/time.h>

double test_perf(size_t n, int mod, int use_strassen) // роль 5
{
    Matrix A = mat_create(n, mod);
    Matrix B = mat_create(n, mod);
    Matrix C = mat_create(n, mod);

    if(A.data == NULL || B.data == NULL || C.data == NULL){
        mat_free(&A); 
        mat_free(&B);
        mat_free(&C);

        return -1.0;
    }

    mat_fill_random(&A);
    mat_fill_random(&B);

    int iterations = 1;

    if (n <= 4) {
        iterations = 100000;   // 100 тыс.

    } else if (n <= 8) {
        iterations = 10000;

    } else if (n <= 16) {
        iterations = 1000;

    } else if (n <= 32) {
        iterations = 100;

    } else if (n <= 64) {
        iterations = 10;

    }


    // clock_t start = clock();

    // int err;

    // for (int iter = 0; iter < iterations; iter++) {
    //     if (use_strassen) {
    //         err = strassen_mul(&A, &B, &C);
    //     } else {
    //         err = naive_mul(&A, &B, &C);
    //     }
    //     if (err != 0) break;
    // }


    // clock_t end = clock();

    // mat_free(&A); 
    // mat_free(&B);
    // mat_free(&C);

    // if(err != 0){
    //     return -1.0;

    // }

    struct timeval start, end;
    
    gettimeofday(&start, NULL);
    
    int err = 0;

    for (int iter = 0; iter < iterations; iter++) {
        if (use_strassen) {
            err = strassen_mul(&A, &B, &C);

        } else {
            err = naive_mul(&A, &B, &C);

        }
        
        if(err != 0){
            break;
        }
    }
    
    gettimeofday(&end, NULL);

    // double time_per_iter = (double)(end - start) / CLOCKS_PER_SEC / iterations;

    // return time_per_iter;

    if(err != 0){
        return -1.0;
    }

    double total_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    
    return total_time / iterations;

}
/*
Данная функция для замера времени умножения матриц test_performance

Matrix A = mat_create(n, mod);
Matrix B = mat_create(n, mod);
Matrix C = mat_create(n, mod);

создание трех квадратных матриц размера n x n с модулем mod
А и В - будем умножать
С     - будем сохранять

if(A.data == NULL || B.data == NULL || C.data == NULL)
проверка ошибок выделения памяти и если хоть одна матрица не создалась,
то освобождаем, то что успело создаться

return -1.0; - отрицательное время -> сигнал ошибки

mat_fill_random(&A);
mat_fill_random(&B);

заполнение случайными числами
(если заполнить нулями, то умножение даст ноль, что не очень интересно)

clock_t start = clock();

засекаем время до умножения 
функция clock() возвращает количество тактов процессора с момента запуска программы

int err;
if(use_strassen) {err = strassen_mul(&A, &B, &C);} else {err = naive_mul(&A, &B, &C);}

выполняем умножение
если use_strassen == 1 -> вызываем Штрассена
если use_strassen == 0 -> вызываем наивное умножение
err сохраняет код ошибки (0 — всё хорошо, не 0 — что-то пошло не так)

clock_t end = clock();

засекаем время после умножения

mat_free(&A); 
mat_free(&B);
mat_free(&C);

освобождаем память
если не освободить память то будет утечка
каждый вызов mat_create() должен сопровождаться вызовом mat_free()

if(err != 0){return -1.0;}

если умножение вернуло ошибку (допустим неправильные размеры) то возвращаем -1.0

return (double)(end - start)/CLOCKS_PER_SEC;
(end - start)  - разница в тактах процессора
CLOCKS_PER_SEC - сколько тактов в одной секунде
делим и получаем время в секундах

--------------------------------------- UPD ---------------------------------------

добавленно более точное измерение времени с помощью gettimeofday(), так как clock() может не работать корректно для коротких операций

добавленно адаптивное количество итераций для разных размеров матриц, чтобы получить более стабильные измерения времени
В зависимости от размера матрицы n, функция теперь выполняет разное количество повторений умножения
Чтобы для маленьких матриц общее время выполнения всех повторений стало достаточно большим для точного измерения функцией gettimeofday()

for (int iter = 0; iter < iterations; iter++)
Внутри цикла выполняется одно и то же умножение много раз. Затем общее время делится на количество повторений, получая точное время одного умножения


Для измерения времени умножения матриц малого размера (n ≤ 64) применялся метод многократных повторений:
операция выполнялась от 10 до 100 000 раз, после чего общее время делилось на количество повторений
Измерение времени производилось с помощью функции gettimeofday(), обеспечивающей микросекундную точность
Для матриц большего размера (n > 64) достаточно одного измерения.

*/

int main(void){    // роль 4
    printf("test correctness\n"); // проверка корректности

    Matrix A = mat_create(4, 7);
    Matrix B = mat_create(4, 7);
    Matrix A_random = mat_create(4, 7);
    Matrix B_random = mat_create(4, 7);
    Matrix C_naive = mat_create(4, 7);
    Matrix C_strass = mat_create(4, 7);
    Matrix C_naive_random = mat_create(4, 7);
    Matrix C_strass_random = mat_create(4, 7);

    if(A.data == NULL || B.data == NULL || C_naive.data == NULL || C_strass.data == NULL || C_naive_random.data == NULL || C_strass_random.data == NULL || A_random.data == NULL || B_random.data == NULL){
        mat_free(&A); 
        mat_free(&B);
        mat_free(&A_random); 
        mat_free(&B_random);
        mat_free(&C_naive);
        mat_free(&C_strass);
        mat_free(&C_naive_random);
        mat_free(&C_strass_random);

        return 1;
    }

    for(size_t i = 0; i < 4; i++){
        for(size_t j = 0; j < 4; j++){
            A.data[i * 4 + j] = (i * 4 + j) % 7;
            B.data[i * 4 + j] = (i * 4 + j) % 7;
        }
    }

    mat_fill_random(&A_random);
    mat_fill_random(&B_random);

    mat_print(&A, "A");
    mat_print(&B, "B");
    mat_print(&A_random, "A_random");
    mat_print(&B_random, "B_random");

    int err_nai = naive_mul(&A, &B, &C_naive);
    int err_str = strassen_mul(&A, &B, &C_strass);
    int err_nai_random = naive_mul(&A_random, &B_random, &C_naive_random);
    int err_str_random = strassen_mul(&A_random, &B_random, &C_strass_random);

    if (err_nai != 0 && err_str != 0 && err_nai_random != 0 && err_str_random != 0) {
        mat_print(&C_naive, "Naive result");
        mat_print(&C_strass, "Strassen result");
        mat_print(&C_naive_random, "Naive_random result");
        mat_print(&C_strass_random, "Strassen_random result");

        return 1;
    
    }

    printf("result for fixed matrix: \n");

    mat_print(&C_naive, "\n"); printf("Naive result\n\n");
    mat_print(&C_strass, "\n"); printf("Strassen result\n\n");

    int equal = 1;
    for (size_t i = 0; i < 16; i++) {
        if (C_naive.data[i] != C_strass.data[i]) {
            equal = 0;
            break;
        }
    }
    printf("Results are %s\n\n", equal ? "IDENTICAL" : "DIFFERENT");

    printf("result for random matrix: \n");

    mat_print(&C_naive_random, "\n"); printf("Naive random result\n\n");
    mat_print(&C_strass_random, "\n"); printf("Strassen random result\n\n");


    int equal_random = 1;
    for (size_t i = 0; i < 16; i++) {
        if (C_naive_random.data[i] != C_strass_random.data[i]) {
            equal = 0;
            break;
        }
    }
    printf("Results_random are %s\n\n", equal_random ? "IDENTICAL" : "DIFFERENT");

    mat_free(&A); 
    mat_free(&B);
    mat_free(&A_random); 
    mat_free(&B_random);
    mat_free(&C_naive);
    mat_free(&C_strass);
    mat_free(&C_naive_random);
    mat_free(&C_strass_random);
    

    printf("test perfomance\n"); // тест производительности

    printf("%-10s %-10s %-15s %-15s %-10s\n", "n", "mod", "Naive(sec)", "Strassen(sec)", "Speedup");

    int mods[] = {7, 10007, 1000003};                                       // int mods[] = {7, 10007, 1000003};
    size_t sizes[] = {1, 2, 3, 4, 5, 8, 16, 32, 64, 128, 256, 512};         // size_t sizes[] = {64, 128, 256, 512};
    
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    for (int mi = 0; mi < 3; mi++) {
        for (int si = 0; si < num_sizes; si++) {
            size_t n = sizes[si];
            int mod = mods[mi];
            
            double time_naive = test_perf(n, mod, 0);
            double time_strassen = test_perf(n, mod, 1);
            
            if (time_naive > 0 && time_strassen > 0) {
                double speedup = time_naive / time_strassen;
                
                if (n <= 512) {
                    printf("%-10zu %-10d %-15.9f %-15.9f %-10.2f\n", n, mod, time_naive, time_strassen, speedup);

                } else {
                    printf("%-10zu %-10d %-15.4f %-15.4f %-10.2f\n", n, mod, time_naive, time_strassen, speedup);

                }
                
            } else {
                printf("%-10zu %-10d %-15s %-15s %-10s\n", n, mod, "ERROR", "ERROR", "ERROR");

            }
        }
        printf("\n");

    }
    
    return 0;

}
/*
-- тест на корректность

надо проверить корректность того что алгоритм Штрассена работает правильно (нужно сравнить с наивным методом)
так же, нужно замерить время для разных размеров и модулей
=======
Нужно проверить корректность, того что алгоритм Штрассена работает правильно (нужно сравнить с наивным методом),
так же нужно замерить время для разных размеров и модулей

будут четыре матрицы - то что будем умножать
две - обычные числа (чтобы можно было проверить результат визуально), две - рандомные (для наглядности),
а так же две матрицы которые будут посчитаны разными способами (обычным и алгоритмом Штрассена)
и две матрицы для результата рандо-заполненных матриц (двумя способами)

далее идет проверка на выделение памяти
if(A.data == NULL || B.data ....

после заполняем каждую матрицу циклом и mat_fill_random()

выводим каждую матрицу с помощью mat_print()

умножение двумя способами
err_nai, err_str ...

если ошибок нет, то выводим оба результата
(err_nai == 0 && err_str == 0 && err_nai_random == 0 && err_str_random)

equal и equal_random - проверяем совпадают ли результаты, если совпадают, то Штрассен работает правильно

после проверки корректности освобождаем память очищением памяти


-- тест производительности

надо оценить производительность кода запуском умножения 
матриц много раз с разными размерами (64,128,256,512) и разными модулями полей (7, 10007, 1000003), замеряя время и выводя таблицу.

int mods[] = {7, 10007, 1000003}                mods        три модуля полей (маленький, средний, большой)
size_t sizes[] = {64, 128, 256, 512}            sizes       четыре размера матриц

(int mi = 0; mi < 3; mi++)      Внешний цикл перебирает модули: 7, 10007, 1000003.
(int si = 0; si < 4; si++)      Внутренний цикл перебирает размеры: 64, 128, 256, 512.

получение текущего размера и модуля:
size_t n = sizes[si]
int mod = mods[mi]

замер времени:
double time_naive = test_perf(n, mod, 0)            замеряет время наивного умножения
double time_strassen = test_perf(n, mod, 1)         замеряет время Штрассена

далее идет проверка и вывод в таблицу
if (time_naive > 0 && time_strassen > 0) {
    double speedup = time_naive / time_strassen;
        printf("%-10zu %-10d %-15.4f %-15.4f %-10.2f\n", n, mod, time_naive, time_strassen, speedup);
    } else {
        printf("%-10zu %-10d %-15s %-15s %-10s\n", n, mod, "ERROR", "ERROR", "ERROR");
    }

почему блочное умножение матриц - (для маленьких возможно )

--------------------------------------- UPD ---------------------------------------

для измерения времени умножения матриц малого размера (n ≤ 64) применялся метод многократных повторений:
операция выполнялась от 10 до 100 000 раз, после чего общее время делилось на количество повторений
Измерение времени производилось с помощью функции gettimeofday(), обеспечивающей микросекундную точность
Для матриц большего размера (n > 64) достаточно одного измерения.

расширились массивы размеров матриц до 1, 2, 3, 4, 5, 8, 16, 32, 64, 128, 256, 512
Чтобы исследовать производительность не только для больших,
но и для маленьких матриц (1×1, 2×2, 3×3, 4×4, 5×5, 8×8, 16×16, 32×32)
для маленьких матриц (n ≤ 4) выполнялось 100 000 повторений

для n ≤ 8 - 10 000 повторений
для n ≤ 16 - 1 000 повторений
для n ≤ 32 - 100 повторений
для n ≤ 64 - 10 повторений
для n > 64 - 1 повторение

добавленно автоматическое вычисление количества размеров в массиве sizes с помощью sizeof:

int num_sizes = sizeof(sizes) / sizeof(sizes[0]);               Вычисляет, сколько элементов в массиве sizes (в данном случае 12)

добавленно форматирование вывода времени в зависимости от размера матрицы:
для n ≤ 4 - 9 знаков после запятой (для точности)
для n > 4 - 4 знака после запятой (для удобства чтения)

if (n <= 4) {
    printf("%-10zu %-10d %-15.9f %-15.9f %-10.2f\n", ...);
} else {
    printf("%-10zu %-10d %-15.4f %-15.4f %-10.2f\n", ...);
}

Для маленьких матриц (n ≤ 4) время очень маленькое (например, 0.000000037 секунды)
Нужно много знаков, чтобы увидеть реальные числа

Для больших матриц (n ≥ 5) время достаточно большое (например, 0.0067 секунды)
Достаточно 4 знаков, чтобы таблица была читаемой и не перегруженной

Для наглядности представления результатов выбран адаптивный формат вывода:
для матриц размером до 4×4 время выводится с девятью знаками после запятой (ввиду его чрезвычайно малых значений), 
для матриц большего размера — с четырьмя знаками, что обеспечивает оптимальную читаемость таблицы


*/