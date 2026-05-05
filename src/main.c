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

}
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

int strassen_mul() // роль 1
{

}
/*
Главная функция умножения двух матриц по алгоритму Штрассена

*/

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

    clock_t start = clock();

    int err;

    if(use_strassen){
        err = strassen_mul(&A, &B, &C);

    } else {
        err = naive_mul(&A, &B, &C);

    }

    clock_t end = clock();

    mat_free(&A); 
    mat_free(&B);
    mat_free(&C);

    if(err != 0){
        return -1.0;

    }

    return (double)(end - start)/CLOCKS_PER_SEC;

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

*/

int main(void){    // роль 4
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
    int err_nai_random = naive_mul(&A, &B, &C_naive);
    int err_str_random = strassen_mul(&A, &B, &C_strass);

    if (err_nai == 0 && err_str == 0 && err_nai_random == 0 && err_str_random) {
        mat_print(&C_naive, "Naive result");
        mat_print(&C_strass, "Strassen result");
        mat_print(&C_naive_random, "Naive_random result");
        mat_print(&C_strass_random, "Strassen_random result");
    

        return 0;
    }

    int equal = 1;
    for (size_t i = 0; i < 16; i++) {
        if (C_naive.data[i] != C_strass.data[i]) {
            equal = 0;
            break;
        }
    }
    printf("Results are %s\n", equal ? "IDENTICAL" : "DIFFERENT");

    int equal_random = 1;
    for (size_t i = 0; i < 16; i++) {
        if (C_naive_random.data[i] != C_strass_random.data[i]) {
            equal = 0;
            break;
        }
    }
    printf("Results_random are %s\n", equal_random ? "IDENTICAL" : "DIFFERENT");

    mat_free(&A); 
    mat_free(&B);
    mat_free(&A_random); 
    mat_free(&B_random);
    mat_free(&C_naive);
    mat_free(&C_strass);
    mat_free(&C_naive_random);
    mat_free(&C_strass_random);
    
    // далее будет тест производительности
}
/*
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
*/