#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "batch_aux.h"


void print_err(uint32_t line) {
    fprintf(stderr, "ERROR %d\n", line);
}


void whitespace(uint32_t line, uint32_t instructions[],
                bool *ignore, parameter *p, char game_params[],
                int *g_iter, int *i_iter) {
    if (*g_iter <= 0) {
        if (*p != E)
            *g_iter = 0;

        return;
    }

    uint64_t temp = strtoull(game_params, NULL, 0);
    memset(game_params, ' ', MAX_GAME_PARAM_LEN * sizeof(char));

    if (temp > UINT32_MAX) {
        *ignore = true;
        *p = E;
        print_err(line);
    } else if (*g_iter > 0) {
        instructions[*i_iter] = (uint32_t) temp;
        (*i_iter)++;
        *g_iter = 0;
    }
}


void new_line(uint32_t *line, bool *processed, uint32_t instructions[],
              bool *ignore, parameter *p, char game_params[],
              int *g_iter, int *i_iter, bool empty_line) {
    whitespace(*line, instructions, ignore, p, game_params,
               g_iter, i_iter);

    if (!(*ignore) && *p != E) {
        *processed = true;
    } else {
        if (!(*ignore) && !empty_line)
            print_err(*line);
        memset(instructions, 0, (MAX_INSTRUCTIONS_INDEX + 1) * sizeof(uint32_t));
        *p = E;
    }

    (*line)++;
    *ignore = false;
    *g_iter = -1;
}


void hash(uint32_t line, bool *ignore, int g_iter,
          int i_iter, parameter *p) {
    if (i_iter != -1 || g_iter > 0)
        print_err(line);

    *p = E;
    *ignore = true;
}


void digit(uint32_t line, int input, bool *ignore, parameter *p,
           char game_params[], int *g_iter, int i_iter) {
    if (0 <= *g_iter && *g_iter < MAX_GAME_PARAM_LEN
        && 0 <= i_iter && i_iter <= MAX_INSTRUCTIONS_INDEX
        && !(*g_iter > 0 && game_params[0] == '0')) {
        game_params[*g_iter] = (char) input;
        (*g_iter)++;
    } else {
        *ignore = true;
        *p = E;
        print_err(line);
    }
}


void non_digit(uint32_t line, int input, bool *ignore,
               parameter *p, int g_iter, int *i_iter) {
    if (g_iter == -1 && *i_iter == -1
        && correct_parameter(input)) {
        *p = char_to_param(input);

        (*i_iter)++;
    } else {
        *ignore = true;
        *p = E;
        print_err(line);
    }
}
