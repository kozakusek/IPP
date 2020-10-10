/** @file
* Moduł z procedurami pomocniczymi dla batch_mode.
*
* @author Bartłomiej Kozaryna <bk______@students.mimuw.edu.pl>
* @copyright Uniwersytet Warszawski
* @date 16.05.2020
*/


#ifndef GAMMA_BATCH_AUX_H
#define GAMMA_BATCH_AUX_H

#include <stdint.h>
#include <stdbool.h>
#include "parameter_gamma.h"


/**
 * Największa akcpetowalna na wejsciu długość liczby.
 */
#define MAX_GAME_PARAM_LEN 10


/**
 * Maksymalny indeks istrukcji w tablicy instrukcji.
 */
#define MAX_INSTRUCTIONS_INDEX 3


/** @brief Wypisuje wiadomość o błędzie na stderr.
 * @param[in] line – numer linii, w której występił błąd.
 */
void print_err(uint32_t line);


/** @brief Wykonuje reakcje na biały znak różny od końca linii.
 *  Jeżeli jest to pierwszy biały znak po ciągu cyfr to przepisuje liczbę
 *  do tablicy instrukcji sprawdzając jej poprawność.
 *  Po znaku nie będącym cyfrą, a poprawnym parametrem instrukcji jedynie
 *  przesuwa wskaźnik @p g_iter.
 * @param[in] line                 – numer bieżącej linii,
 * @param[in, out]  instructions   – tablica instrukcji,
 * @param[in, out]  ignore         – wskazuje czy należy analizować znak,
 * @param[in, out]  p              – parametr wczytywanej instrukcji,
 * @param[in, out]  game_params    – string trzymający wczytywany parametr,
 * @param[in, out]  g_iter         – iterator po tablicy z parametrem,
 * @param[in, out]  i_iter         – iterator po tablicy instrukcji.
 */
void whitespace(uint32_t line, uint32_t instructions[],
                bool *ignore, parameter *p, char game_params[],
                int *g_iter, int *i_iter);


/** @brief Wykonuje operacje związane z natrafieniem na znak nowej linii.
 * Sprawdza poprawność wczytanej do tej pory isntrukcji, ustawiając @p processed
 * na @p true jeżeli była poprawna. W przeciwnym przypadku jedynie resetuje
 * wszystkie parametry instruckcji.
 * Zawsze zwiększa @p line.
 * @param[in, out]  line           – numer bieżącej linii,
 * @param[in, out]  processed      – wskazuje czy instrukcja jest poprawna,
 * @param[in, out]  instructions   – tablica instrukcji,
 * @param[in, out]  ignore         – wskazuje czy należy analizować znak,
 * @param[in, out]  p              – parametr wczytywanej instrukcji,
 * @param[in, out]  game_params    – string trzymający wczytywany parametr,
 * @param[in, out]  g_iter         – iterator po tablicy z parametrem,
 * @param[in, out]  i_iter         – iterator po tablicy instrukcji,
 * @param[in] empty_line           - wskazuje czy linie była do tej pory pusta.
 */
void new_line(uint32_t *line, bool *processed, uint32_t instructions[],
              bool *ignore, parameter *p, char game_params[],
              int *g_iter, int *i_iter, bool empty_line);


/** @brief Wykonuje operacje związane z natrafieniem na znak komentarza.
 * Jeżeli @p # jest pierwszym znakiem linii, ustawia @p ignore na @p true,
 * w przeciwnym wypadku dodatkowo wypisuje komunikat o błędzie.
 * @param[in] line                 – numer bieżącej linii,
 * @param[in, out]  ignore         – wskazuje czy należy analizować znak,
 * @param[in, out]  g_iter         – iterator po tablicy z parametrem,
 * @param[in, out]  i_iter         – iterator po tablicy instrukcji,
 * @param[in, out]  p              – parametr wczytywanej instrukcji.
 */
void hash(uint32_t line, bool *ignore, int g_iter,
          int i_iter, parameter *p);


/** @brief Wykonuje operacje związane z natrafieniem na cyfrę.
 * Dopisuje wczytaną cyfrę do wczytywanego parametru. Jeżeli liczba
 * przekroczyłaby @ref MAX_GAME_PARAM_LEN ustawia @p ignore na true
 * i wypisuje komuniakt o błędzie.
 * @param[in] line                 – numer bieżącej linii,
 * @param[in] input                – aktualnie przetważany znak,
 * @param[in, out]  ignore         – wskazuje czy należy analizować znak,
 * @param[in, out]  p              – parametr wczytywanej instrukcji,
 * @param[in, out]  game_params    – string trzymający wczytywany parametr,
 * @param[in, out]  g_iter         – iterator po tablicy z parametrem,
 * @param[in, out]  i_iter         – iterator po tablicy instrukcji.
 */
void digit(uint32_t line, int input, bool *ignore, parameter *p,
           char game_params[], int *g_iter, int i_iter);


/** @brief Wykonuje operacje związane z natrafieniem na niebiałą nie cyfrę.
 * Jeżeli podany znak jest poprawnym parametrem instrukcji to ustwia nań
 * wartość @p p, w przeciwnym wypadku ustawia @p ignore na true, ustawia
 * @p p na @p E oraz wypisuje komunikat o błędzie.
 * @param[in] line                 – numer bieżącej linii,
 * @param[in] input                – aktualnie przetważany znak,
 * @param[in, out]  ignore         – wskazuje czy należy analizować znak,
 * @param[in, out]  p              – parametr wczytywanej instrukcji,
 * @param[in, out]  g_iter         – iterator po tablicy z parametrem,
 * @param[in, out]  i_iter         – iterator po tablicy instrukcji.
 */
void non_digit(uint32_t line, int input, bool *ignore,
               parameter *p, int g_iter, int *i_iter);


#endif /* GAMMA_BATCH_AUX_H */
