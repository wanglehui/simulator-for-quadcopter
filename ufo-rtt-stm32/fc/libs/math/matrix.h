/**
 * @file matrix.h
 * @brief matrix interface
 */

#ifndef MATRIX_H
#define MATRIX_H

/**
 * @addtogroup ufo
 * @{
 */

/**
 * @addtogroup math
 * @{
 */ 

#define rowsof(m)	(sizeof(m) / sizeof(*m))
#define colsof(m)	(sizeof(*m) / sizeof(**m))
#define matrix_get(array)   { rowsof(array), colsof(array), (int **)array }
/**
 * @struct matrix_t matrix.h
 * @brief Describe a matrix
 */
typedef struct matrix_t {
    int rows;       /**< hom many rows of matrix */
    int cols;       /**< how many columns of matrix */
    void **data;    /**< pointer to data's pointer */
} matrix_t;

/**
 * Addition of two matrixes
 * out = l + r
 * @param l the left matrix
 * @param r the right matrix
 * @param out the answer matrix
 * @return none
 */
void matrix_add(matrix_t *l, matrix_t *r, matrix_t *out);

/**
 * Subduction of two matrixes
 * out = l - r
 * @param l the left matrix
 * @param r the right matrix
 * @param out the answer matrix
 * @return none
 */
void matrix_sub(matrix_t *l, matrix_t *r, matrix_t *out);

/**
 * multiplies a m x n matrix by a n x m matrix
 * out = l x r
 * @param l the left matrix
 * @param r the right matrix
 * @param out the answer matrix
 * @return none
 * @note you should give correct parameter of matrix, otherwise errror occured
 */
void matrix_mul(matrix_t *l, matrix_t *r, matrix_t *out);

/**
 * the dot product
 */
void matrix_mul_at(matrix_t *l, matrix_t *r, int row, int col);
void matrix_div(matrix_t *l, matrix_t *r, matrix_t *out);
/**
 * Transpose a matrix
 *
 * @param i The input matrix
 * @param o The output matrix
 * @return None
 */
void matrix_transpose(matrix_t *i, matrix_t *o);

/**
 * print out a matrix
 *
 * @param matrix the matrix printed out
 * @return none
 */
void matrix_print(struct matrix_t *matrix);

/** @} */

/** @} */

#endif  /* MATRIX_H */
