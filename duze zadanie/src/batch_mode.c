#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "batch_mode.h"
#include "parameter_gamma.h"
#include "batch_aux.h"


/* Sprawdza czy w tablicy występuje element równy zero */
static inline bool any_is_zero(const uint32_t tab[], int max) {
    for (int i = 0; i < max; i++) {
        if (tab[i] == 0)
            return true;
    }

    return false;
}


/* Przetwarza pojedyczny znak wejścia */
static bool parse(int input, bool *processed, uint32_t *line,
                  uint32_t instructions[], bool *ignore, parameter *p,
                  char game_params[], int *g_iter, int *i_iter, bool empty_line) {
    if (input == '\n') {
        new_line(line, processed, instructions, ignore, p,
                 game_params, g_iter, i_iter, empty_line);
    } else if (*ignore) {
    } else if (isspace(input) != 0) {
        if (empty_line) { /* wykomenotwanie tego ifa pozwoli na akceptowanie */
            *ignore = true; /* poleceń po serii białych znaków w wierszu*/
            print_err(*line);
        }
        whitespace(*line, instructions, ignore, p, game_params, g_iter, i_iter);
    } else if (input == '#') {
        hash(*line, ignore, *g_iter, *i_iter, p);
    } else if (isdigit(input) != 0) {
        digit(*line, input, ignore, p, game_params, g_iter, *i_iter);
    } else {
        non_digit(*line, input, ignore, p, *g_iter, i_iter);
    }

    return (input == '\n');
}


enum parameter get_first_line(uint32_t *line, uint32_t instructions[4]) {
    int input;
    bool processed = false;
    bool ignore = false;
    bool empty_line = true;
    enum parameter p = E;
    char game_params[MAX_GAME_PARAM_LEN + 1];
    int game_iter = -1;
    int instructions_iter = -1;
    memset(game_params, ' ', MAX_GAME_PARAM_LEN * sizeof(char));
    game_params[MAX_GAME_PARAM_LEN] = '\0';
    memset(instructions, 0, (MAX_INSTRUCTIONS_INDEX + 1) * sizeof(uint32_t));

    while (!processed && (input = getc(stdin)) != EOF) {
        empty_line = parse(input, &processed, line, instructions, &ignore, &p,
                           game_params, &game_iter, &instructions_iter, empty_line);

        if (empty_line) {
            instructions_iter = -1;
            if (processed
                && (any_is_zero(instructions, 4) || !(p == B || p == I))) {
                print_err(*line - 1);
                processed = false;
                p = E;
            }
        }
    }

    /* Jeśli ostatni wiersz nie zakończył się '\n' */
    if (!processed && !ignore && !empty_line) {
        print_err(*line);
        p = E;

    }

    return p;
}


/* Funkcja interpretująca instrukcje dla gry w trybie wsadowym */
static bool interpret(gamma_t *game, parameter par, uint32_t line,
                      uint32_t instr[], int instr_count) {
    bool a;
    uint64_t c;
    char *d;

    if (par == m && instr_count == 3) {
        a = gamma_move(game, instr[0], instr[1], instr[2]);
        printf("%d\n", a);
    } else if (par == g && instr_count == 3) {
        a = gamma_golden_move(game, instr[0], instr[1], instr[2]);
        printf("%d\n", a);
    } else if (par == b && instr_count == 1) {
        c = gamma_busy_fields(game, instr[0]);
        printf("%ld\n", c);
    } else if (par == f && instr_count == 1) {
        c = gamma_free_fields(game, instr[0]);
        printf("%ld\n", c);
    } else if (par == q && instr_count == 1) {
        a = gamma_golden_possible(game, instr[0]);
        printf("%d\n", a);
    } else if (par == p && instr_count == 0) {
        d = gamma_board(game);
        if (d != NULL) {
            printf("%s", d);
            free(d);
        } else {
            print_err(line);
        }
    } else {
        return false;
    }

    return true;
}


void run_batch_mode(gamma_t *g, uint32_t *line) {
    int input;
    bool processed = false;
    bool ignore = false;
    bool empty_line = true;
    enum parameter p = E;
    char game_params[MAX_GAME_PARAM_LEN + 1];
    int game_iter = -1;
    uint32_t instructions[MAX_INSTRUCTIONS_INDEX + 1];
    memset(game_params, ' ', MAX_GAME_PARAM_LEN * sizeof(char));
    game_params[MAX_GAME_PARAM_LEN] = '\0';
    int instructions_iter = -1;
    memset(instructions, 0, (MAX_INSTRUCTIONS_INDEX + 1) * sizeof(uint32_t));
    while ((input = getc(stdin)) != EOF) {
        empty_line = parse(input, &processed, line, instructions, &ignore, &p,
                           game_params, &game_iter, &instructions_iter, empty_line);

        if (empty_line) {
            if (processed) {
                processed = interpret(g, p, *line, instructions,
                                      instructions_iter);
                if (!processed)
                    print_err(*line - 1);
            }
            processed = false;
            instructions_iter = -1;
            p = E;
        }
    }

    /* Jeśli ostatni wiersz nie zakończył się '\n' */
    if (!ignore && !empty_line)
        print_err(*line);

}