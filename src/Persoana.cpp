#include "Persoana.h"

Persoana::Persoana(const std::string& nume, int anNastere)
    : nume_(nume), anNastere_(anNastere) {}

const std::string& Persoana::getNume() const {
    return nume_;
}

int Persoana::varstaIn(int an) const {
    return an - anNastere_;
}

std::ostream& operator<<(std::ostream& os, const Persoana& persoana) {
    os << persoana.nume_ << " (n. " << persoana.anNastere_ << ")";
    return os;
}
