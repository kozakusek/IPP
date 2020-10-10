/** @file
 * Plik main dla części drugiej dużego zadania IPP-2020
 *
 * @author Bartłomiej Kozaryna <bk______@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 08.05.2020
 */

#include <stdint.h>
#include <stdio.h>
#include "gamma.h"
#include "batch_mode.h"
#include "interactive_mode.h"


/** @brief Uruchamia i przeprowaddza rozgrywkę w grze gamma.
 * @return Zero jeżeli program zakończył działanie pomyślnie,
 * jeden jeżeli wystąpił błąd w trybie interaktywnym.
 */
int main() {
    uint32_t line = 1;
    parameter p;
    /* Odpowiada za przechowywanie (width height players areas) (0 - 3) */
    uint32_t instruct[4];
    gamma_t *g = NULL;
    int i_mode_res = 0; /* Wynik wykonania trybu interaktywnego */

    do {
        p = get_first_line(&line, instruct);

        if (p != E) {
            g = gamma_new(instruct[0], instruct[1], instruct[2], instruct[3]);
            if (g == NULL)
                fprintf(stderr, "ERROR %d\n", line - 1);
        }

    } while (p != E && g == NULL);

    if (g != NULL) {
        if (p == B) {
            printf("OK %d\n", line - 1);
            run_batch_mode(g, &line);
        } else if (p == I) {
            i_mode_res = run_interactive_mode(g, instruct[0],
                                              instruct[1], instruct[2]);
        }
    }

    gamma_delete(g);
    return i_mode_res == 0 ? 0 : 1;
}
