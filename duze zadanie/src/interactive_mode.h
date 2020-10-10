/** @file
 * Interfejs służący do uruchamiania gry gamma w trybie interaktywnym
 *
 * @author Bartłomiej Kozaryna <bk______@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 16.05.2020
 */

#ifndef GAMMA_INTERACTIVE_MODE_H
#define GAMMA_INTERACTIVE_MODE_H

#include <stdint.h>
#include "gamma.h"


/** @brief Przeprowadza rozgrywkę w trybie interaktywnym.
 *
 * @param g               – wskaźnik do struktury przechowywującej stan gry,
 * @param width           – szerekość planszy w grze,
 * @param height          – wysokość planszy w grze,
 * @param players_count   – liczba graczy w grze.
 * @return @p -1 jeżeli wystąpił błąd, 0 w przeciwnym przypadku.
 */
int run_interactive_mode(gamma_t *g, uint32_t width, uint32_t height,
                         uint32_t players_count);


#endif /* GAMMA_INTERACTIVE_MODE_H */
