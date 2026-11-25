#include "Scor.h"

// rlutil poate colora textul in doua feluri: prin functii Windows sau prin coduri ANSI
// (niste caractere speciale pe care consola le transforma in culori).
// Alegem ANSI, pentru ca il intelege si consola din CLion, si terminalele de pe Linux/macOS.
#define RLUTIL_USE_ANSI
#include <rlutil.h>

Scor::Scor(double valoare) : valoare_(valoare) {}

bool Scor::esteFoarteBun() const {
    return valoare_ >= PRAG_FOARTE_BUN;
}

int Scor::culoare() const {
    if (esteFoarteBun()) {
        return rlutil::LIGHTGREEN;
    }
    if (valoare_ >= PRAG_BUN) {
        return rlutil::YELLOW;
    }
    return rlutil::LIGHTRED;
}

std::ostream& operator<<(std::ostream& os, const Scor& scor) {
    // rlutil scrie culorile direct in std::cout. Daca scriem in alt loc
    // (de exemplu intr-un fisier sau intr-un test), afisam doar numarul, fara culori.
    if (&os != &std::cout) {
        os << scor.valoare_;
        return os;
    }
    rlutil::setColor(scor.culoare());
    os << scor.valoare_;
    rlutil::resetColor();
    return os;
}
