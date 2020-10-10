/** @file
 * Interfejs parametru wejścia gry gamma w trybie wsadowym
 *
 * @author Bartłomiej Kozaryna <bk______@students.mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 08.05.2020
 */

#ifndef GAMMA_PARAMETER_GAMMA_H
#define GAMMA_PARAMETER_GAMMA_H


/**
 * Typ służący do przekazywania parametrów trybu wsadowego
 */
typedef enum parameter {
    B, /**< Włącza tryb wsadowy             */
    I, /**< Włącza tryb interaktywny        */
    E, /**< Wystąpił błąd przy uruchamianiu */
    m, /**< Wywołanie gamma_move            */
    g, /**< Wywołanie gamma_golden_move     */
    b, /**< Wywołanie gamma_busy_fields     */
    f, /**< Wywołanie gamma_free_fields     */
    q, /**< Wywołanie gamma_golden_possible */
    p  /**< Wywołanie gamma_board           */
} parameter;


/** @brief Sprawdza czy znak jest poprawnym parametrem.
 *
 * @param[in] c – znak do sprawdzenia.
 * @return @p True jeżeli @p c jest poprawnym parametrem,
 * @p false w przeciwnym wypadku.
 */
bool correct_parameter(int c);


/** @brief Konwertuje znak do parametru.
 *
 * @param[in] c – znak do konwersji.
 * @return Jeżeli znak jest poprawną wartością parametru to
 * odpowiednia wartość parametru, w przeciwnym wypadku @p E.
 */
parameter char_to_param(int c);

#endif /* GAMMA_PARAMETER_GAMMA_H */
