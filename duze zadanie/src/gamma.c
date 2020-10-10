/** @file
 * Implementacja interfejsu klasy przechowującej stan gry gamma
 *
 * @author Bartłomiej Kozaryna <bk______@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 09.06.2020
 */

#include "gamma.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Alias dla typu unsigned __int128.
 */
typedef unsigned __int128 uint128_t;


/**
 * Struktura reprezentująca stan pola w planszy.
 */
typedef struct field {
    uint32_t taken; /**< numer gracza zajmującego pole - 1 */
    uint32_t area; /**< numer obszaru do którego pole należy */
    bool free; /**< @p true jeżeli pole jest wolne, startowo @p true */
    bool visited; /**< znacznik odwedzenia pola, początkowo @p false*/
} field_t;


/**
 * Struktura reprezentująca stan gracza w grze.
 */
typedef struct player {
    bool golden_move; /**< czy nie zrobił złotego ruchu, startowo @p true */
    uint64_t *area_fields_count;  /**< tablica ilości pól w obszarach */
    uint32_t lowest_free_id; /**< pierwszy wolny obszar, startowo @p UINT32_MAX */
    uint32_t highest_freed_id; /**< ostatni zwolniony obszar, startowo @p 0 */
    uint64_t adjacent_free_count; /**< ilość przylegająych pól, startowo @p 0 */
    uint32_t fields_count; /**< liczba zajmowanych pól, startowo @p 0 */
    uint32_t areas_count; /**< liczba posiadanych obszrów, startowo @p 0 */
} player_t;


/**
 * Struktura przechowywująca stan gry gamma.
 */
typedef struct gamma {
    field_t **board; /**< tablica reprezentują planszę z polami @ref field_t */
    player_t *players; /**< tablica graczy @ref field_t */
    uint32_t players_count; /**< liczba graczy, zadana przy inicjalizacji */
    uint64_t free_fields_count; /**< liczba wolnych pól, początkowo @p 0 */
    uint32_t width; /**< szerokość planszy, zadana przy inicjalizacji */
    uint32_t height; /**< wysokość planszy, zadana przy inicjalizacji */
    uint32_t max_areas; /**< maksimum obszarów, zadane przy inicjalizacji */
} gamma_t;


/** @brief Sprawdza czy gracz ma max obszaróww.
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @return Wartość @p true jeżeli @p player ma co najmniej max obszarów
 * określony w @p g, w przeciwnym wypadku @p false.
 */
static inline bool player_has_max_areas(gamma_t *g, uint32_t player) {
    return (g->players[player - 1].areas_count >= g->max_areas);
}


/** @brief Bada czy lewe pole należy do aracza.
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 * @return Wartość @p true jeżeli lewe pole od (@p x, @p y) należy
 * do gracza @p player, @p false jeżeli nie.
 */
static inline bool left_is_players(gamma_t *g, uint32_t player,
                                   uint32_t x, uint32_t y) {
    if (x == 0) {
        return false;
    } else {
        return (!(g->board[x - 1][y].free)
                && g->board[x - 1][y].taken == player - 1);
    }
}


/** @brief Bada czy prawe pole należy do aracza.
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 * @return Wartość @p true jeżeli prawe pole od (@p x, @p y) należy
 * do gracza @p player, @p false jeżeli nie.
 */
static inline bool right_is_players(gamma_t *g, uint32_t player,
                                    uint32_t x, uint32_t y) {
    if (x == g->width - 1) {
        return false;
    } else {
        return (!(g->board[x + 1][y].free)
                && g->board[x + 1][y].taken == player - 1);
    }
}


/** @brief Bada czy górne pole należy do aracza.
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 * @return Wartość @p true jeżeli górne pole od (@p x, @p y) należy
 * do gracza @p player, @p false jeżeli nie.
 */
static inline bool up_is_players(gamma_t *g, uint32_t player,
                                 uint32_t x, uint32_t y) {
    if (y == g->height - 1) {
        return false;
    } else {
        return (!(g->board[x][y + 1].free)
                && g->board[x][y + 1].taken == player - 1);
    }
}


/** @brief Bada czy dolne pole należy do aracza.
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 * @return Wartość @p true jeżeli dolne pole od (@p x, @p y) należy
 * do gracza @p player, @p false jeżeli nie.
 */
static inline bool down_is_players(gamma_t *g, uint32_t player,
                                   uint32_t x, uint32_t y) {
    if (y == 0) {
        return false;
    } else {
        return (!(g->board[x][y - 1].free)
                && g->board[x][y - 1].taken == player - 1);
    }
}


/** @brief Zlicza sąsiadów pola naleążcych do gracza.
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 * @return Iloś pól gracza @p player sąsiadujących z polem (@p x, @p y);
 * liczba z przedziału [0, 4].
 */
static inline char how_many_neighbours_owns(gamma_t *g, uint32_t player,
                                            uint32_t x, uint32_t y) {
    char count = 0;

    if (left_is_players(g, player, x, y))
        count++;

    if (right_is_players(g, player, x, y))
        count++;

    if (up_is_players(g, player, x, y))
        count++;

    if (down_is_players(g, player, x, y))
        count++;

    return count;
}


/** @brief Sprawdza czy gracz nie sąsiadował z lewym polem.
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 * @return @p True jeżeli lewe pole z sąsiedztwa (@p x, @p y) nie należało
 * do gracza @p player, w przeciwnym wypadku zwraca @p false.
 */
static inline bool adj_from_left(gamma_t *g, uint32_t player,
                                 uint32_t x, uint32_t y) {
    if (x == 0) {
        return false;
    } else {
        return (g->board[x - 1][y].free
                && !left_is_players(g, player, x - 1, y)
                && !up_is_players(g, player, x - 1, y)
                && !down_is_players(g, player, x - 1, y));
    }
}


/** @brief Sprawdza czy gracz nie sąsiadował z górnym polem.
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 * @return @p True jeżeli górne pole z sąsiedztwa (@p x, @p y) nie należało
 * do gracza @p player, w przeciwnym wypadku zwraca @p false.
 */
static inline bool adj_from_up(gamma_t *g, uint32_t player,
                               uint32_t x, uint32_t y) {
    if (y == g->height - 1) {
        return false;
    } else {
        return (g->board[x][y + 1].free
                && !left_is_players(g, player, x, y + 1)
                && !up_is_players(g, player, x, y + 1)
                && !right_is_players(g, player, x, y + 1));
    }
}


/** @brief Sprawdza czy gracz nie sąsiadował z prawym polem.
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 * @return @p True jeżeli prawe pole z sąsiedztwa (@p x, @p y) nie należało
 * do gracza @p player, w przeciwnym wypadku zwraca @p false.
 */
static inline bool adj_from_right(gamma_t *g, uint32_t player,
                                  uint32_t x, uint32_t y) {
    if (x == g->width - 1) {
        return false;
    } else {
        return (g->board[x + 1][y].free
                && !right_is_players(g, player, x + 1, y)
                && !up_is_players(g, player, x + 1, y)
                && !down_is_players(g, player, x + 1, y));
    }
}


/** @brief Sprawdza czy gracz nie sąsiadował z dolnym polem.
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 * @return @p True jeżeli dolne pole z sąsiedztwa (@p x, @p y) nie należało
 * do gracza @p player, w przeciwnym wypadku zwraca @p false.
 */
static inline bool adj_from_down(gamma_t *g, uint32_t player,
                                 uint32_t x, uint32_t y) {
    if (y == 0) {
        return false;
    } else {
        return (g->board[x][y - 1].free
                && !right_is_players(g, player, x, y - 1)
                && !left_is_players(g, player, x, y - 1)
                && !down_is_players(g, player, x, y - 1));
    }
}


/** @brief Sprawdza czy gracz sąsiadował z polem.
 * @param[in] g  – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 * @return @p True jeżeli jakieś pole z sąsiedztwa (@p x, @p y) należy
 * do gracza @p player, jeżeli nie, to @p false.
 */
static inline bool was_adjacent(gamma_t *g, uint32_t player,
                                uint32_t x, uint32_t y) {
    return (how_many_neighbours_owns(g, player, x, y) > 0);
}


/** @brief Zwiększa adjacent_file_count dla @ref place_pawn.
 * Procedura kontroluje liczbę pól sąsiadujących, gracza @p player,
 * którego pionek jest stawiany na polu (@p x, @p y).
 * @param[in,out] g  – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 */
static void afc_expand(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (adj_from_left(g, player, x, y))
        g->players[player - 1].adjacent_free_count++;

    if (adj_from_up(g, player, x, y))
        g->players[player - 1].adjacent_free_count++;

    if (adj_from_right(g, player, x, y))
        g->players[player - 1].adjacent_free_count++;

    if (adj_from_down(g, player, x, y))
        g->players[player - 1].adjacent_free_count++;

    if (was_adjacent(g, player, x, y))
        g->players[player - 1].adjacent_free_count--;

}


/** @brief Zwmniejsza adjacent_file_count dla @ref place_pawn.
 * Procedura kontroluje liczbę pól sąsiadujących, dla graczy sąsiadującyh
 * z polem (@p x, @p y) nie będących graczem @p player.
 * @param[in,out] g  – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 */
static void afc_dimnish_others(gamma_t *g, uint32_t player,
                               uint32_t x, uint32_t y) {
    uint32_t p1 = UINT32_MAX;
    uint32_t p2 = UINT32_MAX;
    uint32_t p3 = UINT32_MAX;

    if (x > 0) {
        if (!(g->board[x - 1][y].free) && g->board[x - 1][y].taken != player - 1) {
            p1 = g->board[x - 1][y].taken;
            g->players[p1].adjacent_free_count--;
        }
    }

    if (x < g->width - 1) {
        if (!(g->board[x + 1][y].free) && g->board[x + 1][y].taken != player - 1
            && g->board[x + 1][y].taken != p1) {
            p2 = g->board[x + 1][y].taken;
            g->players[g->board[x + 1][y].taken].adjacent_free_count--;
        }
    }

    if (y > 0) {
        if (!(g->board[x][y - 1].free) && g->board[x][y - 1].taken != player - 1
            && g->board[x][y - 1].taken != p1 && g->board[x][y - 1].taken != p2) {
            p3 = g->board[x][y - 1].taken;
            g->players[g->board[x][y - 1].taken].adjacent_free_count--;
        }
    }

    if (y < g->height - 1) {
        if (!(g->board[x][y + 1].free) && g->board[x][y + 1].taken != player - 1
            && g->board[x][y + 1].taken != p1 && g->board[x][y + 1].taken != p2
            && g->board[x][y + 1].taken != p3) {
            g->players[g->board[x][y + 1].taken].adjacent_free_count--;
        }
    }
}


/** @brief Stawia pionek gracza na danym polu.
 * Zmienia stan gry @p g, stawiając w miejsce (@p x, @p y) pionek
 * gracza @p player.
 * @param[in] g       – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] x       – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y       – rzędna bieżącego pola, liczba nieujemna.
 */
static inline void place(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    g->board[x][y].taken = player - 1;
    g->board[x][y].free = false;
    g->players[player - 1].fields_count++;
    afc_expand(g, player, x, y);
    afc_dimnish_others(g, player, x, y);
    g->free_fields_count--;
}


/** @brief Ustawia parametr lowest_free_id.
 * @param[in,out] p   – wskaźnik na gracza, @ref player_t,
 */
static inline void set_lowest(player_t *p) {
    while (p->area_fields_count[p->lowest_free_id] != 0) {
        p->lowest_free_id++;
    }
}


/** @brief Szuka numeru dla nowego obszaru dla gracza.
 * Korzystając informacji o ilości obszarów gracza @p player, a także
 * wartości pól lowest_free_id, highest_freed_id, ustala
 * wolny numer pola dla gracza oraz aktaulizuje wymienione pola.
 * @param[in,out] g   – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia.
 * @return Najmniejszy numer wolnego obszaru gracza @p player.
 */
static uint32_t get_next_area(gamma_t *g, uint32_t player) {
    player_t *tmp_p = &(g->players[player - 1]);
    uint32_t area = 0;

    if (tmp_p->lowest_free_id == tmp_p->highest_freed_id) {
        area = tmp_p->lowest_free_id;
        tmp_p->highest_freed_id = 0;
        tmp_p->lowest_free_id = UINT32_MAX;
    } else if (tmp_p->lowest_free_id > tmp_p->highest_freed_id) {
        area = tmp_p->areas_count - 1;
    } else {
        area = tmp_p->lowest_free_id;
        set_lowest(tmp_p);
    }

    g->players[player - 1].areas_count++;

    return area;
}


/** @brief  Dodaje nowy obszar dla @ref check_areas.
 * Zmienia dane w @p g tak, żeby został dodany nowy obszar graczowi
 * @p player, znajdujący się w polu (@p x, @p y).
 * @param[in,out] g   – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] x       – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y       – rzędna bieżącego pola, liczba nieujemna.
 */
static inline void new_area(gamma_t *g, uint32_t player,
                            uint32_t x, uint32_t y) {

    g->players[player - 1].areas_count++;
    g->board[x][y].area = get_next_area(g, player);
    g->players[player - 1].areas_count--;
    g->players[player - 1].area_fields_count[g->board[x][y].area]++;
}


/** @brief Znając ilość pól w obszarach podaje id największego.
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 * @param[in] a      – liczba pól obszaru górnego,
 * @param[in] b      – liczba pól obszaru prawego,
 * @param[in] c      – liczba pól obszaru dolnego,
 * @param[in] d      – liczba pól obszaru lewego.
 * @return Numer obszaru, który zajmuje najwięcej pól z pośród czterech:
 * @p a, @p b, @p c, @p d; liczba nieujemna.
 */
static inline uint32_t area_of_max_fields_id(gamma_t *g,
                                             uint32_t x, uint32_t y,
                                             uint64_t a, uint64_t b,
                                             uint64_t c, uint64_t d) {
    if (a >= b && a >= c && a >= d) /* up */
        return g->board[x][y + 1].area;
    else if (b >= a && b >= c && b >= d) /* right */
        return g->board[x + 1][y].area;
    else if (c >= a && c >= b && c >= d) /* down */
        return g->board[x][y - 1].area;
    else /* left */
        return g->board[x - 1][y].area;
}


/** @brief Szuka numeru największego sąsiedniego obszaru.
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y      – rzędna bieżącego pola, liczba nieujemna.
 * @return Numer obszaru należącego do gracza @p player, sąsiadującego z polem
 * (@p x, @p y), który zajmuje najwięcej pól, liczba nieujemna.
 */
static uint32_t biggest_neighbouring_area(gamma_t *g, uint32_t player,
                                          uint32_t x, uint32_t y) {
    uint64_t up = 0;
    uint64_t right = 0;
    uint64_t down = 0;
    uint64_t left = 0;

    if (up_is_players(g, player, x, y))
        up = g->players[player - 1].area_fields_count[g->board[x][y + 1].area];

    if (right_is_players(g, player, x, y))
        right = g->players[player - 1].area_fields_count[g->board[x + 1][y].area];

    if (down_is_players(g, player, x, y))
        down = g->players[player - 1].area_fields_count[g->board[x][y - 1].area];

    if (left_is_players(g, player, x, y))
        left = g->players[player - 1].area_fields_count[g->board[x - 1][y].area];

    return area_of_max_fields_id(g, x, y, up, right, down, left);
}


/** @brief Sprawdza czy prawe pole należy do największego sąsiedniego obszaru.
 * @note Wykorzystuje fakt, że informacja o numerze największego
 * sąsiedniego obszaru została zakodowana w polu (@p x, @p y).
 * @param[in] g       – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] x       – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y       – rzędna bieżącego pola, liczba nieujemna.
 * @return Wartość @p true gdy pole na prawo od (@p x, @p y) należy do gracza
 * @p player i jego największego obszaru z okolicy tego pola,
 * a w przeciwnym wypadku wartość @p false.
 */
static inline bool is_right_area_biggest_area(gamma_t *g, uint32_t player,
                                              uint32_t x, uint32_t y) {
    if (!right_is_players(g, player, x, y))
        return true;
    else
        return (g->board[x + 1][y].area == g->board[x][y].area);
}


/** @brief Sprawdza czy lewe pole należy do największego sąsiedniego obszaru.
 * @note Wykorzystuje fakt, że informacja o numerze największego
 * sąsiedniego obszaru została zakodowana w polu (@p x, @p y).
 * @param[in] g       – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] x       – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y       – rzędna bieżącego pola, liczba nieujemna.
 * @return Wartość @p true gdy pole na lewo od (@p x, @p y) należy do gracza
 * @p player i jego największego obszaru z okolicy tego pola,
 * a w przeciwnym wypadku wartość @p false.
 */
static inline bool is_left_area_biggest_area(gamma_t *g, uint32_t player,
                                             uint32_t x, uint32_t y) {
    if (!left_is_players(g, player, x, y))
        return true;
    else
        return (g->board[x - 1][y].area == g->board[x][y].area);

}


/** @brief Sprawdza czy górne pole należy do największego sąsiedniego obszaru.
 * @note Wykorzystuje fakt, że informacja o numerze największego
 * sąsiedniego obszaru została zakodowana w polu (@p x, @p y).
 * @param[in] g       – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] x       – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y       – rzędna bieżącego pola, liczba nieujemna.
 * @return Wartość @p true gdy pole nad (@p x, @p y) należy do gracza
 * @p player i jego największego obszaru z okolicy tego pola,
 * a w przeciwnym wypadku wartość @p false.
 */
static inline bool is_up_area_biggest_area(gamma_t *g, uint32_t player,
                                           uint32_t x, uint32_t y) {
    if (!up_is_players(g, player, x, y))
        return true;
    else
        return (g->board[x][y + 1].area == g->board[x][y].area);

}


/** @brief Sprawdza czy dolne pole należy do największego sąsiedniego obszaru.
 * @note Wykorzystuje fakt, że informacja o numerze największego
 * sąsiedniego obszaru została zakodowana w polu (@p x, @p y).
 * @param[in] g       – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] x       – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y       – rzędna bieżącego pola, liczba nieujemna.
 * @return Wartość @p true gdy pole pod (@p x, @p y) należy do gracza
 * @p player i jego największego obszaru z okolicy tego pola,
 * a w przeciwnym wypadku wartość @p false.
 */
static inline bool is_down_area_biggest_area(gamma_t *g, uint32_t player,
                                             uint32_t x, uint32_t y) {
    if (!down_is_players(g, player, x, y))
        return true;
    else
        return (g->board[x][y - 1].area == g->board[x][y].area);
}


/** @brief Sprawdza czy górne pole należy do obszaru.
 * @param[in] g       – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] xi      – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] yi      – rzędna bieżącego pola, liczba nieujemna.
 * @param[in] from    – numer obszaru, który ma ulec zmianie.
 * @return Wartość @p true gdy pole nad (@p xi, @p yi) należy do gracza
 * @p player i jego obszaru @p from. W przeciwnym wypadku @p false.
 */
static inline bool up_is_in_area(gamma_t *g, uint32_t player, uint32_t xi,
                                 uint32_t yi, uint32_t from) {
    if (yi == g->height - 1) {
        return false;
    } else {
        return (g->board[xi][yi + 1].area == from
                && g->board[xi][yi + 1].taken == player - 1);
    }
}


/** @brief Sprawdza czy prawe pole należy do obszaru.
 * @param[in] g       – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] xi      – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] yi      – rzędna bieżącego pola, liczba nieujemna.
 * @param[in] from    – numer obszaru, który ma ulec zmianie.
 * @return Wartość @p true gdy pole na prawo od (@p xi, @p yi) należy do gracza
 * @p player i jego obszaru @p from. W przeciwnym wypadku @p false.
 */
static inline bool right_is_in_area(gamma_t *g, uint32_t player, uint32_t xi,
                                    uint32_t yi, uint32_t from) {
    if (xi == g->width - 1) {
        return false;
    } else {
        return (g->board[xi + 1][yi].area == from
                && g->board[xi + 1][yi].taken == player - 1);
    }
}


/** @brief Sprawdza czy dolne pole należy do obszaru.
 * @param[in] g       – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] xi      – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] yi      – rzędna bieżącego pola, liczba nieujemna.
 * @param[in] from    – numer obszaru, który ma ulec zmianie.
 * @return Wartość @p true gdy pole pod (@p xi, @p yi) należy do gracza
 * @p player i jego obszaru @p from. W przeciwnym wypadku @p false.
 */
static inline bool down_is_in_area(gamma_t *g, uint32_t player, uint32_t xi,
                                   uint32_t yi, uint32_t from) {
    if (yi == 0) {
        return false;
    } else {
        return (g->board[xi][yi - 1].area == from
                && g->board[xi][yi - 1].taken == player - 1);
    }
}


/** @brief Sprawdza czy lewe pole należy do obszaru.
 * @param[in] g       – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] xi      – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] yi      – rzędna bieżącego pola, liczba nieujemna.
 * @param[in] from    – numer obszaru, który ma ulec zmianie.
 * @return Wartość @p true gdy pole na lewo od (@p xi, @p yi) należy do gracza
 * @p player i jego obszaru @p from. W przeciwnym wypadku @p false.
 */
static inline bool left_is_in_area(gamma_t *g, uint32_t player, uint32_t xi,
                                   uint32_t yi, uint32_t from) {
    if (xi == 0) {
        return false;
    } else {
        return (g->board[xi - 1][yi].area == from
                && g->board[xi - 1][yi].taken == player - 1);
    }
}


/** @brief Rekurencyjnie zmienia numer obszarowi.
 * Wywoływana przez @ref transfer_area.
 * Przechodząc kolejno po polach (@p xi, @p yi) gracza @p player
 * z obszaru @p from zmiennia ich numer na @p to.
 * @param[in,out] g   – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] xi      – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] yi      – rzędna bieżącego pola, liczba nieujemna.
 * @param[in] from    – numer obszaru, który ma ulec zmianie.
 * @param[in] to      – numer obszaru docelowego.
 */
static void transfer_area(gamma_t *g, uint32_t player,
                          uint32_t xi, uint32_t yi,
                          uint32_t from, uint32_t to) {
    g->board[xi][yi].area = to;

    if (up_is_in_area(g, player, xi, yi, from))
        transfer_area(g, player, xi, yi + 1, from, to);

    if (right_is_in_area(g, player, xi, yi, from))
        transfer_area(g, player, xi + 1, yi, from, to);

    if (down_is_in_area(g, player, xi, yi, from))
        transfer_area(g, player, xi, yi - 1, from, to);

    if (left_is_in_area(g, player, xi, yi, from))
        transfer_area(g, player, xi - 1, yi, from, to);

}


/** @brief Zmienia numer obszarowi.
 * Wywoływana przez @ref no_new_areas.
 * Wykorzystując numer nowego obszaru, zakodowany w polu (@p x, @p y),
 * do którego obszar @p from należący go @p player ma zostać dołączony,
 * wywołuje @ref transfer_area.
 * Kontroluje następujace pola z @ref player_t
 * lowest_free_id
 * highest_freed_id
 * areas_count oraz area_fields_count.
 * @param[in,out] g   – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] x       – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y       – rzędna bieżącego pola, liczba nieujemna.
 * @param[in] from    – numer obszaru, który ma ulec zmianie.
 */
static void set_area_id(gamma_t *g, uint32_t player, uint32_t x,
                        uint32_t y, uint32_t from) {
    uint32_t to = g->board[x][y].area;
    player_t *tmp_p = &(g->players[player - 1]);

    if (tmp_p->lowest_free_id > from)
        tmp_p->lowest_free_id = from;

    if (tmp_p->highest_freed_id < from)
        tmp_p->highest_freed_id = from;

    transfer_area(g, player, x, y, from, to);
    tmp_p->area_fields_count[to] += tmp_p->area_fields_count[from];
    tmp_p->area_fields_count[from] = 0;
    tmp_p->areas_count--;
}

/** @brief Dołącza pole do istniejących obszarów.
 * Wywoływana przez @ref check_areas.
 * Dołącza nowo przejęte pole (@p x, @p y) do największego
 * z sąsiadujaych obszarów gracza @p player. Pzyłącza do tego
 * obszaru inne obszary sąsiadujace z (@p x, @p y) jeżeli istnieją.
 * @param[in,out] g   – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] x       – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y       – rzędna bieżącego pola, liczba nieujemna.
 */
static void no_new_areas(gamma_t *g, uint32_t player,
                         uint32_t x, uint32_t y) {
    uint32_t biggest_area = biggest_neighbouring_area(g, player, x, y);
    g->board[x][y].area = biggest_area;

    if (!is_right_area_biggest_area(g, player, x, y))
        set_area_id(g, player, x, y, g->board[x + 1][y].area);

    if (!is_left_area_biggest_area(g, player, x, y))
        set_area_id(g, player, x, y, g->board[x - 1][y].area);

    if (!is_up_area_biggest_area(g, player, x, y))
        set_area_id(g, player, x, y, g->board[x][y + 1].area);

    if (!is_down_area_biggest_area(g, player, x, y))
        set_area_id(g, player, x, y, g->board[x][y - 1].area);

}


/** @brief Bada ilość sąsiadujących obszarów.
 * @param[in] g   – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] x       – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y       – rzędna bieżącego pola, liczba nieujemna.
 * @return ilość obszarów sąsiadujących z polem (@p x, @p y), należących do gracza
 * @p player,liczba nieujemna.
 */
static inline char how_many_adjacent_areas(gamma_t *g, uint32_t player,
                                           uint32_t x, uint32_t y) {
    uint32_t a1 = UINT32_MAX;
    uint32_t a2 = UINT32_MAX;
    uint32_t a3 = UINT32_MAX;
    uint32_t a4 = UINT32_MAX;
    char counter = 0;

    if (left_is_players(g, player, x, y)) {
        a1 = g->board[x - 1][y].area;
        counter++;
    }

    if (up_is_players(g, player, x, y)) {
        a2 = g->board[x][y + 1].area;
        if (a2 != a1)
            counter++;

    }

    if (right_is_players(g, player, x, y)) {
        a3 = g->board[x + 1][y].area;
        if (a3 != a1 && a3 != a2)
            counter++;

    }

    if (down_is_players(g, player, x, y)) {
        a4 = g->board[x][y - 1].area;
        if (a4 != a1 && a4 != a2 && a4 != a3)
            counter++;
    }

    return counter;;
}


/** @brief Zwraca numer jedynego sąsiedniego obszaru.
 * Zakładając, że pole (@p x, @p y), ma dokładnie jeden sąsiadujący obszar
 * zawraca jego numer.
 * @param[in] g   – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] x       – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y       – rzędna bieżącego pola, liczba nieujemna.
 * @return numer jedynego sąsiedniego obszaru, liczba nieujemna.
 */
static inline uint32_t get_only_area(gamma_t *g, uint32_t player,
                                     uint32_t x, uint32_t y) {
    if (left_is_players(g, player, x, y))
        return g->board[x - 1][y].area;
    else if (up_is_players(g, player, x, y))
        return g->board[x][y + 1].area;
    else if (right_is_players(g, player, x, y))
        return g->board[x + 1][y].area;
    else if (down_is_players(g, player, x, y))
        return g->board[x][y - 1].area;
    else
        return 0;
}


/** @brief  Aktualizuje dane dotyczące obszarów po ruchu.
 * Wywoływana przez @ref place_pawn.
 * Dzieli się na przypadki:
 * @ref new_area      – pojawia się nowy obszar,
 * @ref no_new_areas  – pole dołączane jest do istniejącego obszaru.
 * @param[in,out] g   – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] x       – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y       – rzędna bieżącego pola, liczba nieujemna.
 */
static void check_areas(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    char how_many = how_many_adjacent_areas(g, player, x, y);
    uint32_t area = get_only_area(g, player, x, y);

    if (how_many == 0) {
        new_area(g, player, x, y);
    } else if (how_many == 1) {
        g->players[player - 1].area_fields_count[area]++;
        g->board[x][y].area = area;
    } else {
        no_new_areas(g, player, x, y);
    }
}


/** @brief Stawia pionek.
 * Zakładającc poprawność danych stawia w grze @p g,
 * na polu (@p x, @p y) pionek gracza @p player.
 * @param[in,out] g   – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] x       – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y       – rzędna bieżącego pola, liczba nieujemna.
 * @return Wartość @p true. (Dla zapisu innych metod)
 */
static bool place_pawn(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    place(g, player, x, y);
    check_areas(g, player, x, y);

    return true;
}


/** @brief Odwiedza pola gracza w obszarze.
 * @param[in,out] g   – wskaźnik na grę, @ref gamma_t,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] xi      – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] yi      – rzędna bieżącego pola, liczba nieujemna.
 */
static void try_to_go_back(gamma_t *g, uint32_t player,
                           uint32_t xi, uint32_t yi) {
    if (!(g->board[xi][yi].visited)) {
        g->board[xi][yi].visited = true;

        if (left_is_players(g, player, xi, yi))
            try_to_go_back(g, player, xi - 1, yi);

        if (up_is_players(g, player, xi, yi))
            try_to_go_back(g, player, xi, yi + 1);

        if (right_is_players(g, player, xi, yi))
            try_to_go_back(g, player, xi + 1, yi);

        if (down_is_players(g, player, xi, yi))
            try_to_go_back(g, player, xi, yi - 1);
    }
}


/** @brief Odznacza znacznik odwiedzenia pól w obszarze.
 * @param[in,out] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] xi      – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] yi     – rzędna bieżącego pola, liczba nieujemna.
 */
static void devisit(gamma_t *g, uint32_t xi, uint32_t yi) {
    if (g->board[xi][yi].visited) {
        g->board[xi][yi].visited = false;

        if (left_is_players(g, g->board[xi][yi].taken + 1, xi, yi))
            devisit(g, xi - 1, yi);

        if (up_is_players(g, g->board[xi][yi].taken + 1, xi, yi))
            devisit(g, xi, yi + 1);

        if (right_is_players(g, g->board[xi][yi].taken + 1, xi, yi))
            devisit(g, xi + 1, yi);

        if (down_is_players(g, g->board[xi][yi].taken + 1, xi, yi))
            devisit(g, xi, yi - 1);
    }
}


/** @brief Bada ilość obszarów bo @ref gamma_golden_move.
 * Liczy ile obszarów miałby gracz @p player, po wykonaniu złotego ruchu
 * przez innego gracza na pole (@p x, @p y).
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y      – rzędna bieżącego pola, liczba nieujemna.
 * @return Liczba obszarów jaką miałby gracz @p player,
 * po wykonaniu złotego ruchu przez innego gracza.
 */
static uint32_t explore_neighbouring_area(gamma_t *g, uint32_t player,
                                          uint32_t x, uint32_t y) {
    uint32_t count = g->players[player - 1].areas_count;
    uint32_t add = 0;
    g->board[x][y].visited = true;

    if (left_is_players(g, player, x, y)) {
        add++;
        try_to_go_back(g, player, x - 1, y);
    }

    if (up_is_players(g, player, x, y)) {
        add++;
        if ((g->board[x][y + 1].visited))
            add--;
        else
            try_to_go_back(g, player, x, y + 1);
    }

    if (right_is_players(g, player, x, y)) {
        add++;
        if ((g->board[x + 1][y].visited))
            add--;
        else
            try_to_go_back(g, player, x + 1, y);
    }

    if (down_is_players(g, player, x, y)) {
        add++;
        if ((g->board[x][y - 1].visited))
            add--;
        else
            try_to_go_back(g, player, x, y - 1);
    }

    devisit(g, x, y);

    if (add > 1)
        count += add - 1;

    return count;
}


/** @brief Bada możliwość zabrania pionka z danego pola.
 * Sprawdza czy w wyniku zabrania pionka z pola (@p x, @p y),
 * gracz który aktualnie zajmuje to pole przekroczyłby maksymalną
 * liczbę obszarów z gry @p g.
 * Aktualnie rozważane pole to (@p xi, @p yi).
 * @param[in] g    – wskaźnik na grę, @ref gamma_t,
 * @param[in] x    – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] y    – rzędna bieżącego pola, liczba nieujemna.
 * @return Wartość @p true jeżeli gracz przekroczyłby limit obszarów,
 * @p false w przeciwnym wypadku.
 */
static bool other_player_would_exceed_area_limit(gamma_t *g, uint32_t x,
                                                 uint32_t y) {
    uint32_t player2 = g->board[x][y].taken + 1;
    uint32_t many = how_many_neighbours_owns(g, player2, x, y);

    if (many > 0)
        many--; /* maksymalny przyrost obszarów to 3 */

    if (g->players[player2 - 1].areas_count + many <= g->max_areas)
        return false; /* na pewno nie przekroczy */
    else if (explore_neighbouring_area(g, player2, x, y) <= g->max_areas)
        return false; /* mógł‚ ale nie przekroczył */
    else
        return true; /* przekroczył */
}


/** @brief Klorouje obszar na dany kolor dla @ref repaint.
 * Rekurencyjnie maluje wszystkie pola przynależące do obszaru @p area,
 * gracza @p player na odpowiedni numer.
 * Aktualnie rozważane pole to (@p xi, @p yi).
 * @param[in,out] g  – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] area   – numer obszaru, liczba naturalna,
 * @param[in] xi     – odcięta bieżącego pola, liczba nieujemna,
 * @param[in] yi     – rzędna bieżącego pola, liczba nieujemna.
 */
static void rec_repaint(gamma_t *g, uint32_t player, uint32_t area,
                        uint32_t xi, uint32_t yi) {
    g->board[xi][yi].visited = true;
    g->board[xi][yi].area = area;
    g->players[player - 1].area_fields_count[area]++;

    if (left_is_players(g, player, xi, yi)) {
        if (!(g->board[xi - 1][yi].visited))
            rec_repaint(g, player, area, xi - 1, yi);
    }

    if (up_is_players(g, player, xi, yi)) {
        if (!(g->board[xi][yi + 1].visited))
            rec_repaint(g, player, area, xi, yi + 1);
    }

    if (right_is_players(g, player, xi, yi)) {
        if (!(g->board[xi + 1][yi].visited))
            rec_repaint(g, player, area, xi + 1, yi);
    }

    if (down_is_players(g, player, xi, yi)) {
        if (!(g->board[xi][yi - 1].visited))
            rec_repaint(g, player, area, xi, yi - 1);
    }
}


/** @brief Przyznaje obszarom nowe id dla @ref manage_areas.
 * @param[in,out] g  – wskaźnik na grę, @ref gamma_t,
 * @param[in] x      – odcięta startowego pola, liczba nieujemna,
 * @param[in] y      – rzędna startowego pola, liczba nieujemna.
 */
static void repaint(gamma_t *g, uint32_t x, uint32_t y) {
    uint32_t player = g->board[x][y].taken + 1;

    if (left_is_players(g, player, x, y)) {
        if (!(g->board[x - 1][y].visited))
            rec_repaint(g, player, get_next_area(g, player), x - 1, y);
    }

    if (up_is_players(g, player, x, y)) {
        if (!(g->board[x][y + 1].visited))
            rec_repaint(g, player, get_next_area(g, player), x, y + 1);
    }

    if (right_is_players(g, player, x, y)) {
        if (!(g->board[x + 1][y].visited))
            rec_repaint(g, player, get_next_area(g, player), x + 1, y);
    }

    if (down_is_players(g, player, x, y)) {
        if (!(g->board[x][y - 1].visited))
            rec_repaint(g, player, get_next_area(g, player), x, y - 1);
    }

}


/** @brief Zmienia dane mówiące o istnieniu obszaru.
 * @param[in,out] g  – wskaźnik na grę, @ref gamma_t,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 */
static inline void delete_area(gamma_t *g, uint32_t x, uint32_t y) {
    field_t field = g->board[x][y];

    g->players[field.taken].area_fields_count[field.area] = 0;
    g->players[field.taken].areas_count--;
    if (g->players[field.taken].lowest_free_id > field.area)
        g->players[field.taken].lowest_free_id = field.area;

    if (g->players[field.taken].highest_freed_id < field.area)
        g->players[field.taken].highest_freed_id = field.area;

}


/** @brief Zarządza obszarami dla @ref switch_pawns.
 * Kontroluje liczbę obszarów gracza z pola (@p x, @p y)
 * po zabraniu jego pionka. Usuwa obszar, a potem przydziela je na nowo.
 * @param[in,out] g  – wskaźnik na grę, @ref gamma_t,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 */
static void manage_areas(gamma_t *g, uint32_t x, uint32_t y) {
    delete_area(g, x, y);
    repaint(g, x, y);
    g->board[x][y].visited = true;
    devisit(g, x, y);
}


/** @brief Zwiększa adjacent_file_count dla @ref delete_pawn.
 * Procedura kontroluje liczbę pól sąsiadujących, dla graczy sąsiadującyh
 * z polem (@p x, @p y).
 * @param[in,out] g  – wskaźnik na grę, @ref gamma_t,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 */
static void afc_expand_others(gamma_t *g, uint32_t x, uint32_t y) {
    uint32_t p1 = UINT32_MAX;
    uint32_t p2 = UINT32_MAX;
    uint32_t p3 = UINT32_MAX;

    if (x > 0) {
        if (!(g->board[x - 1][y].free)) {
            p1 = g->board[x - 1][y].taken;
            g->players[p1].adjacent_free_count++;
        }
    }

    if (x < g->width - 1) {
        if (!(g->board[x + 1][y].free) && g->board[x + 1][y].taken != p1) {
            p2 = g->board[x + 1][y].taken;
            g->players[p2].adjacent_free_count++;
        }
    }

    if (y > 0) {
        if (!(g->board[x][y - 1].free) && g->board[x][y - 1].taken != p1
            && g->board[x][y - 1].taken != p2) {
            p3 = g->board[x][y - 1].taken;
            g->players[p3].adjacent_free_count++;
        }
    }

    if (y < g->height - 1) {
        if (!(g->board[x][y + 1].free) && g->board[x][y + 1].taken != p1
            && g->board[x][y + 1].taken != p2 && g->board[x][y + 1].taken != p3) {
            g->players[g->board[x][y + 1].taken].adjacent_free_count++;
        }
    }

}

/** @brief Zmiejsza adjacent_file_count dla @ref delete_pawn.
 * Procedura kontroluje liczbę pól sąsiadujących, gracza z pola (@p x, @p y).
 * @param[in,out] g  – wskaźnik na grę, @ref gamma_t,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 */
static inline void afc_dimnish(gamma_t *g, uint32_t x, uint32_t y) {
    uint64_t how_many = 0;
    uint32_t p = g->board[x][y].taken + 1;

    if (x > 0) {
        if (g->board[x - 1][y].free && how_many_neighbours_owns(g, p, x - 1, y) < 1)
            how_many++;
    }

    if (y > 0) {
        if (g->board[x][y - 1].free && how_many_neighbours_owns(g, p, x, y - 1) < 1)
            how_many++;
    }

    if (x < g->width - 1) {
        if (g->board[x + 1][y].free && how_many_neighbours_owns(g, p, x + 1, y) < 1)
            how_many++;
    }

    if (y < g->height - 1) {
        if (g->board[x][y + 1].free && how_many_neighbours_owns(g, p, x, y + 1) < 1)
            how_many++;
    }

    g->players[p - 1].adjacent_free_count -= how_many;
}


/** @brief Usuwa pionek stojący danym polu.
 * Zmienia dane w grze @p g, tak jakby zabierając pionek z (@p x, @p y).
 * @param[in,out] g  – wskaźnik na grę, @ref gamma_t,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 */
static inline void delete_pawn(gamma_t *g, uint32_t x, uint32_t y) {
    g->free_fields_count++;
    g->players[g->board[x][y].taken].fields_count--;
    g->board[x][y].free = true;
    afc_dimnish(g, x, y);
    afc_expand_others(g, x, y);
}


/** @brief Podmienia pionek na danym polu.
 * Główna funckja wykonująca @ref gamma_golden_move. Sprawdza, czy zamiana
 * jest dozwolona. Jeżeli tak, to usuwa pionek gracza zajmującego (@p x, @p y),
 * poprawia dane dotyczące obszarów, a następnie stawia tam pionek gracz @p player.
 * @param[in,out] g  – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia,
 * @param[in] x      – odcięta rozważanego pola, liczba nieujemna,
 * @param[in] y      – rzędna rozważanego pola, liczba nieujemna.
 * @return Wartość @p true jeżeli zamiana była dozwolona i przebiegła pomyślnie,
 * w przeciwnym wypadku @p false.
 */
static bool switch_pawns(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (player_has_max_areas(g, player)
        && how_many_neighbours_owns(g, player, x, y) == 0) {
        return false;
    } else if (other_player_would_exceed_area_limit(g, x, y)) {
        return false;
    } else {
        g->players[player - 1].golden_move = false;
        delete_pawn(g, x, y);
        manage_areas(g, x, y);
        return place_pawn(g, player, x, y);
    }
}


/** @brief Sprawdza czy jakikolwiek inny gracz zajmuje pole.
 * Bada czy gracz inny od @p player zajmuje jakiekolwiek pole w @p g.
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia.
 * @return Wartość @p true jeżeli w @p g istnieje pole zajmowane przez gracza
 * różnego od @p player, w przeciwnym wypadku @p false.
 */
static bool another_player_has_a_field(gamma_t *g, uint32_t player) {
    return (g->players[player - 1].fields_count
            + g->free_fields_count < g->width * g->height);
}


/** @brief Sprawdza poprawność danych game i player.
 * @param[in] g      – wskaźnik na grę, @ref gamma_t,
 * @param[in] player – numer gracza, liczba dodatnia.
 * @return Wartość @p false jeżeli @p g to NULL lub @p player poza zakresem
 * określonym w @p g albo równy 0, w przeciwnym wypadku @p true.
 */
static inline bool correct_game_and_player(gamma_t *g, uint32_t player) {
    if (g == NULL)
        return false;
    else
        return !(player < 1 || g->players_count < player);
}


/** @brief Sprawdza czy punkt należy do planszy.
 * @param[in] g     – wskaźnik na grę, @ref gamma_t,
 * @param[in] x     – odcięta badanego punktu,, liczba nieujemna,
 * @param[in] y     – rzędna badanego punktu, liczba nieujemna.
 * @return Wartość @p true jeżeli @p x mniejszy od szerokości określonej
 * w @p g oraz @p y od wysokości, w przeciwnym wypadku @p false.
 */
static inline bool coordinates_correct(gamma_t *g, uint32_t x, uint32_t y) {
    return (x < g->width && y < g->height);
}


/** @brief Alokuje pamięć na planszę.
 * Alokuje pamięć potrzebną na jedną grę @p game typu @ref gamma_t
 * oraz na występującą w niej tablicę graczy @p players typu @ref player_t
 * i jej podtablicę zliczającą liczbę pól w obszarach.
 * @note Jeżeli alokacja się nie powiedzie, zwalnia pamięć zalokowaną dotychczas.
 * @param[in,out] game      – wskaźnik na tworzoną grę, @ref gamma_t,
 * @param[in,out] players   – wskaźnik do tablicy graczy, @ref player_t,
 * @param[in] n_players     – liczba graczy, liczba dodatnia,
 * @param[in] areas         – maksymalna liczba obszarów,
 *                            jakie może zająć jeden gracz.
 * @return Wartość @p true jezeli alokacja przebiegła pomyślnie, natomiast
 * @p false jeżeli zabrakło pamięci.
 */
static bool allocate_game_and_players(gamma_t **game, player_t **players,
                                      uint32_t n_players, uint32_t areas) {
    *game = malloc(sizeof(gamma_t));
    if (*game == NULL)
        return false;

    *players = malloc(n_players * sizeof(player_t));
    if (*players == NULL) {
        free(*game);
        return false;
    }

    for (uint32_t i = 0; i < n_players; i++) {
        (*players)[i].area_fields_count = malloc(areas * sizeof(uint64_t));
        if ((*players)[i].area_fields_count == NULL) {
            while (i >= 1) {
                free((*players)[i - 1].area_fields_count);
                i--;
            }
            free(*game);
            free(*players);
            return false;
        }
    }

    return true;
}


/** @brief Zwalnia całą pamięć zalokowaną na graczy.
 * Zwalnia pamięć zalokowaną na tablicę graczy  @p *p oraz jej podtablice.
 * @param[in,out] p     – wskaźnik na tablicę graczy, @ref player_t,
 * @param[in] pc        – liczba graczy w grze, liczba dodatnia.
 */
static inline void free_players(player_t **p, uint32_t pc) {
    for (uint32_t i = 0; i < pc; i++) {
        free((*p)[i].area_fields_count);
    }

    free(*p);
}


/** @brief Alokuje pamięć na planszę.
 * Alokuje pamięć potrzebną na tablicę 2D o wymiarach @p width x @p height
 * Jeżeli alokacja się nie powiedzie, zwalnia również pamięć zalokowaną przez
 * @ref allocate_game_and_players.
 * @param[in,out] board   – wskaźnik na planszę,
 *                          wskaźnik na wskaźnik na @ref field_t,
 * @param[in,out] game    – wskaźnik do tworzonej gry, @ref gamma_t,
 * @param[in,out] players – tablica graczy, @ref player_t,
 * @param[in] width       – szerokość planszy, liczba dodatnia,
 * @param[in] height      – wysokość planszy, liczba dodatnia.
 * @param[in] pc 		  – liczba graczy w tworzonej grze, liczba dodatnia,
 * @return Wartość @p true jeśli alokacja przebiegła pomyślnie, natomiast
 * @p false jeżeli zabrakło pamięci.
 */
static bool allocate_board(field_t ***board, gamma_t **game,
                           player_t **players, uint32_t width,
                           uint32_t height, uint32_t pc) {
    *board = malloc(width * sizeof(field_t *));
    if (*board == NULL) {
        free_players(players, pc);
        free(*game);
        return false;
    }
    for (uint32_t i = 0; i < width; i++) {
        (*board)[i] = malloc(height * sizeof(field_t));
        if ((*board)[i] == NULL) {
            while (i >= 1) {
                free((*board)[i - 1]);
                i--;
            }
            free_players(players, pc);
            free(*game);
            return false;
        }
    }

    return true;
}


/** @brief Inicjalizuje planszę.
 * Ustawia pola planszy utworzonej w @ref gamma_new na startowe;
 * takie jak w @ref field_t.
 * @param[in,out] board  – plansza, tablica 2D typu @ref field_t,
 * @param[in] width      – szerokość planszy, liczba dodatnia,
 * @param[in] height     – wysokość planszy, liczba dodatnia.
 */
static void init_board(field_t **board, uint32_t width, uint32_t height) {
    for (uint32_t x = 0; x < width; x++) {
        for (uint32_t y = 0; y < height; y++) {
            board[x][y].taken = 0;
            board[x][y].area = 0;
            board[x][y].free = true;
            board[x][y].visited = false;
        }
    }
}


/** @brief Inicjalizuje tablicę graczy.
 * Ustawia wartości pól tablicy graczy nowo utworzonej w @ref gamma_new
 * na startowe; takie jak w @ref player_t.
 * @param[in,out] players_arr –  tablica przechowywująca graczy,
 *                               typu @ref player_t,
 * @param[in] n_players       –  ilość graczy, liczba dodatnia,
 * @param[in] max_areas       –  maksymalna liczba obszarów,
 *                               jakie może zająć jeden gracz.
 */
static void init_players(player_t *players_arr, uint32_t n_players,
                         uint32_t max_areas) {
    for (uint32_t i = 0; i < n_players; i++) {
        players_arr[i].golden_move = true;
        for (uint32_t j = 0; j < max_areas; j++) {
            players_arr[i].area_fields_count[j] = 0;
        }
        players_arr[i].lowest_free_id = UINT32_MAX;
        players_arr[i].highest_freed_id = 0;
        players_arr[i].adjacent_free_count = 0;
        players_arr[i].fields_count = 0;
        players_arr[i].areas_count = 0;
    }
}


/** @brief Inicjalizuje pola struktury przechowującej grę.
 * Ustawia wartości w grze nowo utworzonej przez @ref gamma_new
 * na startowe; takie jak w @ref gamma_t.
 * @param[in,out] game    – wskanik na strukturę przechowywującą grę,
 * @param[in] board       – plansza, tablica 2D typu @ref field_t,
 * @param[in] players_arr – tablica przechowywująca graczy, typu @ref player_t,
 * @param[in] width       – szerokość planszy, liczba dodatnia,
 * @param[in] height      – wysokość planszy, liczba dodatnia,
 * @param[in] players     – ilość graczy, liczba dodatnia,
 * @param[in] areas       – maksymalna liczba obszarów,
 *                          jakie może zająć jeden gracz.
 */
static void init_game(gamma_t *game, field_t **board,
                      player_t *players_arr,
                      uint32_t width, uint32_t height,
                      uint32_t players, uint32_t areas) {
    game->board = board;
    game->players = players_arr;
    game->players_count = players;
    game->free_fields_count = width * height;
    game->width = width;
    game->height = height;
    game->max_areas = areas;
}


/** @brief Liczy ilosć cyfr w liczbie.
 * @note Zakłada, że @p n jest większe od 0.
 * @param[in] n   – liczba całkowita z przedziału [0, +inf].
 * @return Ilość cyfry liczby @p n.
 */
static uint32_t get_number_of_digits(uint32_t n) {
    uint32_t counter = 0;

    while (n > 0) {
        n /= 10;
        counter++;
    }

    return counter;
}


/** @brief Konwertuje uint32_t na char.
 * @note Zakłada, że @p digit jest cyfrą.
 * @param[in] digit   – liczba całkowita z przedziału [0, 9].
 * @return Wartość char cyfry @p digit.
 */
static inline char get_char_id(uint32_t digit) {
    return '0' + digit;
}


/** @brief Wstawia następne pole bufora.
 * Wstawia do bufora będącego wskaźnikiem na char @p p odpowiednie znaki
 * mające reprezenotować kolejne pole (@p x, @p y) z planszy z gry @p g.
 * Wstawia te dane począwszy od miejsca @p p + @p i.
 * @note Zakłada odpowiednią ilość zalokowanej pamięci na @p p.
 * @param[in,out] p   – wskaźnik na bufor do którego wypisujemy planszę,
 * @param[in,out] i   – wskaźnik indeks pierwszego niezapsianego pola w @p p,
 * @param[in] g       – skaźnik na strukturę przechowującą stan gry,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 * @param[in] m       – nieujemna liczba całkowita opsiująca ile znaków zajmuje
 *                      pojedyncze pole.
 */
static inline void fill_next_field(char *p, uint128_t *i, gamma_t *g,
                                   uint32_t x, uint32_t y, uint32_t m) {
    uint32_t n = 0;
    uint32_t player = g->board[x][y].taken + 1;
    uint32_t digit = 0;
    uint32_t j = 0;

    if (g->board[x][y].free) {
        for (j = 1; j < m; j++) {
            p[*i] = ' ';
            (*i)++;
        }
        p[*i] = '.';
        (*i)++;
    } else {
        n = get_number_of_digits(player);
        for (j = 1; j < m - n + 1; j++) {
            p[*i] = ' ';
            (*i)++;
        }
        (*i) += n;
        for (j = 1; j <= n; j++) {
            digit = player % 10;
            player /= 10;
            p[*i - j] = get_char_id(digit);
        }
    }
}


/** @brief Redukuje rozmiar pola areas w grze.
 * @param[in] width      – szerokość planszy, liczba dodatnia,
 * @param[in] height     – wysokość planszy, liczba dodatnia,
 * @param[in, out] areas – maksymalna liczba obszarów,
 *                         jakie może zająć jeden gracz.
 */
static inline void trim_number_of_areas(uint32_t width, uint32_t height,
                                        uint32_t *areas) {
    uint64_t width64 = (uint64_t) width;
    uint64_t height64 = (uint64_t) height;
    uint64_t areas64 = (uint64_t) *areas;

    if (width64 * height64 < areas64)
        (*areas) = width * height;
}


/** @brief Wstawia następne pole bufora.
 * Wstawia do bufora będącego wskaźnikiem na char @p p odpowiednie znaki
 * mające reprezenotować kolejne pole (@p x, @p y) z planszy z gry @p g.
 * Wstawia te dane począwszy od miejsca @p p + @p i.
 * @note Zakłada odpowiednią ilość zalokowanej pamięci na @p p.
 * @param[in] g       – skaźnik na strukturę przechowującą stan gry,
 * @param[in] player  – numer gracza, liczba dodatnia,
 * @param[in] x       – numer kolumny, liczba nieujemna mniejsza od wartości
 *                      @p width z funkcji @ref gamma_new,
 * @param[in] y       – numer wiersza, liczba nieujemna mniejsza od wartości
 *                      @p height z funkcji @ref gamma_new.
 * @return @p True jeżeli złoty ruch na dane miejsce jest możliwy dla gracza
 * o numerze @p player, @p False w przeciwnym przypadku.
 */
static bool golden_field_possible(gamma_t *g, uint32_t player,
                                  uint32_t x, uint32_t y) {
    if (player_has_max_areas(g, player)
        && how_many_neighbours_owns(g, player, x, y) == 0) {
        return false;
    } else {
        return !other_player_would_exceed_area_limit(g, x, y);
    }
}


gamma_t *gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
    if (players == 0 || areas == 0 || width == 0 || height == 0)
        return NULL;

    trim_number_of_areas(width, height, &areas);

    gamma_t *new_game;
    player_t *players_ar;
    field_t **board;

    if (!allocate_game_and_players(&new_game, &players_ar, players, areas))
        return NULL;

    if (!allocate_board(&board, &new_game, &players_ar, width, height, players))
        return NULL;

    init_board(board, width, height);
    init_players(players_ar, players, areas);
    init_game(new_game, board, players_ar, width, height, players, areas);

    return new_game;
}


void gamma_delete(gamma_t *g) {
    if (g == NULL)
        return;

    for (uint32_t i = 0; i < g->width; i++) {
        free(g->board[i]);
    }

    free(g->board);
    free_players(&(g->players), g->players_count);
    free(g);
}


bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (!correct_game_and_player(g, player)) {
        return false;
    } else if (!coordinates_correct(g, x, y)) {
        return false;
    } else if (!(g->board[x][y].free)) {
        return false;
    } else if (player_has_max_areas(g, player)
               && how_many_neighbours_owns(g, player, x, y) == 0) {
        return false;
    } else {
        return place_pawn(g, player, x, y);
    }
}


bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (!correct_game_and_player(g, player))
        return false;
    else if (!coordinates_correct(g, x, y))
        return false;
    else if (!(g->players[player - 1].golden_move) || g->board[x][y].free)
        return false;
    else if (g->board[x][y].taken == player - 1)
        return false;
    else
        return switch_pawns(g, player, x, y);
}


uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
    if (!correct_game_and_player(g, player))
        return 0;
    else
        return g->players[player - 1].fields_count;
}


uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
    if (!correct_game_and_player(g, player))
        return 0;
    else if (player_has_max_areas(g, player))
        return g->players[player - 1].adjacent_free_count;
    else
        return g->free_fields_count;
}


bool gamma_golden_possible(gamma_t *g, uint32_t player) {
    if (!correct_game_and_player(g, player)) {
        return false;
    } else if (!(g->players[player - 1].golden_move
                 && another_player_has_a_field(g, player))) {
        return false;
    } else {
        for (uint32_t x = 0; x < g->width; x++) {
            for (uint32_t y = 0; y < g->height; y++) {
                if (!(g->board[x][y].free)
                    && g->board[x][y].taken != player - 1) {
                    if (golden_field_possible(g, player, x, y))
                        return true;
                }
            }
        }
        return false;
    }
}


char *gamma_board(gamma_t *g) {
    if (g == NULL)
        return NULL;

    uint32_t multip = get_number_of_digits(g->players_count);
    uint128_t i = (multip * (g->width) + 1) * (g->height);

    char *p = malloc((i + 1) * sizeof(char));
    if (p == NULL)
        return NULL;

    i = 0;
    for (uint32_t y = g->height; y >= 1; y--) {
        for (uint32_t x = 0; x < g->width; x++) {
            fill_next_field(p, &i, g, x, y - 1, multip);
        }
        p[i] = '\n';
        i++;
    }
    p[i] = '\0';

    return p;
}


uint32_t gamma_whose_field(gamma_t *g, uint32_t x, uint32_t y) {
    if (g == NULL || x >= g->width || y >= g->height || g->board[x][y].free)
        return 0;
    else
        return g->board[x][y].taken + 1;
}
