#ifndef SCOR_H
#define SCOR_H

#include <iostream>

// Scorul CineDB al unui titlu, cu o "eticheta" de calitate:
// foarte bun (verde), bun (galben) sau slab (rosu).
// Cand e afisat pe ecran (std::cout), scorul apare colorat.
class Scor {
public:
    explicit Scor(double valoare);

    // true daca scorul ajunge la pragul de "foarte bun" (cel afisat cu verde)
    bool esteFoarteBun() const;

    friend std::ostream& operator<<(std::ostream& os, const Scor& scor);

private:
    static constexpr double PRAG_FOARTE_BUN = 8.5;
    static constexpr double PRAG_BUN = 7.0;

    // Culoarea (din biblioteca rlutil) potrivita pentru acest scor.
    int culoare() const;

    double valoare_;
};

#endif // SCOR_H
