#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <math.h>
#include "interactive_mode.h"
#include <unistd.h>

/* Globalne parametry dotczące rozgywki */

static uint32_t width; /* szerokość planszy */
static uint32_t height; /* wysokość planszy */
static uint32_t players_count; /* liczba graczy */
static uint32_t x, y; /* współrzędne w grze */
static struct termios default_state, game_state; /* stany terminala */
static const uint32_t banner_height = 5; /* wysokość baneru "GAMMA" */


/* Procedury obsługujące tryb interaktywny */

static bool is_enough_space() {
    uint32_t length = floor(log10(players_count)) + 2;
    uint32_t max_message_height = 4;
    uint32_t banner_width = 42;
    struct winsize w;

    if (ioctl(0, TIOCGWINSZ, &w) != 0)
        return false;

    if (w.ws_col < length * width + 1 || w.ws_col < banner_width
        || w.ws_row < banner_height + height + max_message_height + 3) {
        printf("Error. Size of declared board exceeds size of your window\n");
        printf("Program wil now exit with return code 1.\n");
        return false;
    } else {
        return true;
    }
}

/* Inicjalizajca globalnych parametrów i przygotowanie termianala */
static int prepare_for_game(uint32_t w, uint32_t h, uint32_t pc) {
    width = w;
    height = h;
    x = width / 2;
    y = height / 2 - ((height - 1) % 2);
    players_count = pc;

    if (!is_enough_space())
        return -1;

    if (tcgetattr(STDIN_FILENO, &default_state) != 0) {
        return -1;
    } else {
        game_state = default_state;
        game_state.c_lflag &= ~(ICANON | ECHO);
        return tcsetattr(STDIN_FILENO, TCSANOW, &game_state);
    }
}


static inline void print_banner() {
    printf("\033[95m");
    printf("   ______\n");
    printf("  / ____/___ _____ ___  ____ ___  ____ _\n");
    printf(" / / __/ __ `/ __ `__ \\/ __ `__ \\/ __ `/\n");
    printf("/ /_/ / /_/ / / / / / / / / / / / /_/ / \n");
    printf("\\____/\\__,_/_/ /_/ /_/_/ /_/ /_/\\__,_/\n");
    printf("\033[0m");
}


static inline void print_empty_field() {
    uint32_t length = floor(log10(players_count)) + 2;
    for (uint32_t i = 1; i < length; i++)
        printf(" ");

    printf(".");
}


static inline void print_taken_field(uint32_t player) {
    uint32_t field_length = floor(log10(players_count)) + 1;
    uint32_t player_length = floor(log10(player)) + 1;
    for (uint32_t i = 0; i <= field_length - player_length; i++)
        printf(" ");
    printf("%d", player);
    printf("\033[%dD", field_length + 1);
}


static inline void print_board_border(bool top) {
    uint32_t length = floor(log10(players_count)) + 2;

    if (top)
        printf("╔");
    else
        printf("╚");

    for (uint32_t i = 0; i < length * width; i++) {
        printf("═");
    }

    if (top)
        printf("╗\n");
    else
        printf("╝\n");
}


/* Wypisuje pustą planszę */
static void print_board() {
    print_board_border(true);
    for (uint32_t i = 0; i < height; i++) {
        printf("║");
        for (uint32_t j = 0; j < width; j++) {
            print_empty_field();
        }
        printf("║\n");
    }
    print_board_border(false);
}


static inline void print_encouraging_message(gamma_t *g, uint32_t player) {
    printf("PLAYER: %d\n", player);
    printf("FIELDS TAKEN: %ld\n", gamma_busy_fields(g, player));
    printf("FIELDS TO TAKE: %ld\n", gamma_free_fields(g, player));
    if (gamma_golden_possible(g, player))
        printf("\033[93mGOLDEN MOVE POSSIBLE\033[0m\n");

}


/* Znajduje numer gracza, który zajmuje najwięcej pól.
 * Zwraca 0 jeżeli taki nie istnieje */
static uint32_t find_winner(gamma_t *g) {
    uint32_t leader_nr = 1;
    uint32_t leader_score = gamma_busy_fields(g, 1);
    uint32_t leaders_count = 1;

    for (uint32_t i = 2; i <= players_count; i++) {
        if (leader_score == gamma_busy_fields(g, i)) {
            leaders_count++;
        } else if (leader_score < gamma_busy_fields(g, i)) {
            leader_nr = i;
            leader_score = gamma_busy_fields(g, i);
            leaders_count = 1;
        }
    }

    return leaders_count == 1 ? leader_nr : 0;
}


static void print_start_screen() {
    printf("\033[1;1H\033[2J");
    print_banner();
    print_board();
}


static inline void print_score_board(gamma_t *g) {
    printf("\033[94mSCOREBOARD\033[0m\n");
    for (uint32_t i = 1; i <= players_count; i++) {
        printf("Player %d: %ld\n", i, gamma_busy_fields(g, i));
    }
}


static void print_end_screen(gamma_t *g) {
    uint32_t winner = find_winner(g);

    printf("\033[%d;1H\033[J", height + banner_height + 3);

    if (winner != 0) {
        printf("\033[32;5mCONGRATULATIONS PLAYER %d!\033[0m\n", winner);
        printf("Your score: %ld\n\n", gamma_busy_fields(g, winner));
    }

    print_score_board(g);
}


/* Czyści wszystko pod planszą */
static void clear_message() {
    printf("\033[%d;1H\033[J", height + banner_height + 3);
}


static void highlighting(gamma_t *g, bool turn_on) {
    uint32_t player = gamma_whose_field(g, x, y);
    uint32_t field_length = floor(log10(players_count)) + 1;

    if (turn_on)
        printf("\033[44m");

    if (player == 0) {
        for (uint32_t i = 0; i < field_length; i++)
            printf(" ");

        printf(".");
        printf("\033[%dD", field_length + 1);
    } else {
        print_taken_field(player);
    }

    if (turn_on)
        printf("\033[49m");

}


static inline void move_up(gamma_t *g) {
    highlighting(g, false);
    printf("\033[A");
    y++;
    highlighting(g, true);
}


static inline void move_down(gamma_t *g) {
    highlighting(g, false);
    printf("\033[B");
    y--;
    highlighting(g, true);
}


static inline void move_left(gamma_t *g, uint32_t length) {
    highlighting(g, false);
    printf("\033[%dD", length);
    x--;
    highlighting(g, true);
}


static inline void move_right(gamma_t *g, uint32_t length) {
    highlighting(g, false);
    printf("\033[%dC", length);
    x++;
    highlighting(g, true);
}


static void move_cursor(gamma_t *g, int c) {
    uint32_t length = floor(log10(players_count)) + 2;

    if (c == 'A' && y < height - 1)
        move_up(g);
    else if (c == 'B' && y > 0)
        move_down(g);
    else if (c == 'C' && x < width - 1)
        move_right(g, length);
    else if (c == 'D' && x > 0)
        move_left(g, length);

}


static inline bool make_move(char c, uint32_t player, gamma_t *g) {
    bool success;

    if (c == ' ')
        success = gamma_move(g, player, x, y);
    else
        success = gamma_golden_move(g, player, x, y);


    if (success) {
        printf("\033[44m");
        print_taken_field(player);
        printf("\033[49m");
    } else {
        printf("\a");
    }

    return success;
}


static bool player_input(gamma_t *g, uint32_t player, bool *move) {
    bool succesful_input = false;
    int c = '\0';
    bool char_interpreted = true;

    printf("\033[u");

    while (!succesful_input) {
        if (char_interpreted)
            c = getchar();

        if (c == '\033') {
            if ((c = getchar()) == '[') {
                if ('A' <= (c = getchar()) && c <= 'D')
                    move_cursor(g, c);
                else
                    char_interpreted = false;
            } else {
                char_interpreted = false;
            }
        } else if (c == 'C' || c == 'c') {
            succesful_input = true;
        } else if (c == 'G' || c == 'g' || c == ' ') {
            succesful_input = make_move(c, player, g);
        } else if (c == 4) {
            return true;
        } else {
            char_interpreted = true;
        }
    }

    printf("\033[s");

    if (!(*move)) {
        *move = ((gamma_free_fields(g, player) > 0)
                 || gamma_golden_possible(g, player));
    }

    return false;
}


static void take_turns(gamma_t *g) {
    bool somebody_could_have_moved;
    bool shutdown = false;
    uint32_t mid_column = 2 + (width / 2) * (floor(log10(players_count)) + 2);
    uint32_t mid_row = banner_height + 2 + height / 2;

    printf("\033[%d;%dH\033[s", mid_row, mid_column);
    highlighting(g, true);

    do {
        somebody_could_have_moved = false;
        for (uint32_t player = 1; player <= players_count; player++) {
            if ((gamma_free_fields(g, player) > 0)
                || gamma_golden_possible(g, player)) {
                clear_message();
                print_encouraging_message(g, player);
                shutdown = player_input(g, player, &somebody_could_have_moved);
                if (shutdown)
                    player = UINT32_MAX - 1;
            }
        }
    } while (somebody_could_have_moved && !shutdown);

}


int run_interactive_mode(gamma_t *g, uint32_t width0, uint32_t height0,
                         uint32_t players_count0) {
    if (prepare_for_game(width0, height0, players_count0) != 0)
        return -1;

    print_start_screen();

    printf("\033[?25l");
    take_turns(g);
    printf("\033[?25h");

    print_end_screen(g);

    return tcsetattr(STDIN_FILENO, TCSANOW, &default_state);
}
