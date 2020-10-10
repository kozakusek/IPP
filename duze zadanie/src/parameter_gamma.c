#include <stdbool.h>
#include "parameter_gamma.h"

bool correct_parameter(int c) {
    return (c == 'B' || c == 'I' ||
            c == 'm' || c == 'g' ||
            c == 'b' || c == 'f' ||
            c == 'q' || c == 'p');
}

parameter char_to_param(int c) {
    if (correct_parameter(c)) {
        if (c == 'B')
            return B;
        else if (c == 'I')
            return I;
        else if (c == 'm')
            return m;
        else if (c == 'g')
            return g;
        else if (c == 'b')
            return b;
        else if (c == 'f')
            return f;
        else if (c == 'q')
            return q;
        else
            return p;
    } else {
        return E;
    }
}