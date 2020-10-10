/** @file
 * Interfejs służący do uruchamiania gry gamma w trybie wsadowym
 *
 * @author Bartłomiej Kozaryna <bk______@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 08.05.2020
 */

#ifndef GAMMA_BATCH_MODE_H
#define GAMMA_BATCH_MODE_H

#include <stdint.h>
#include "gamma.h"
#include "parameter_gamma.h"


/** @brief Zczytuje linię uruchamiającą grę.
 * Czyta dane ze standardeowego wejścia do znalezienia
 * spełniającej wzór (B | I) i 4x uint32_t
 * @param[in, out] line          – licznik linii na wejściu,
 * @param[in, out] instructions  – tablica przekazująca
 *                                 parametry gry.
 * @return Parametr (B,I,E) opisujący wybrany tryb gry.
 */
parameter get_first_line(uint32_t *line, uint32_t *instructions);


/** @brief Przeprowadza rozgrywkę w trybie wsadowym.
 * @param[in, out] line – licznik linii na wejściu,
 * @param[in, out] g    – wskaźnik na strukturę przechowywującą
 *                        stan gry, gamma_t.
 */
void run_batch_mode(gamma_t *g, uint32_t *line);


#endif /* GAMMA_BATCH_MODE_H */
