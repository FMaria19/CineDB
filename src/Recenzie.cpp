#include "Recenzie.h"

#include <algorithm>

Recenzie::Recenzie(const std::string& utilizator, int nota, const std::string& text)
    : utilizator_(utilizator), nota_(limiteazaNota(nota)), text_(text) {}

const std::string& Recenzie::getUtilizator() const {
    return utilizator_;
}

int Recenzie::getNota() const {
    return nota_;
}

int Recenzie::limiteazaNota(int nota) {
    return std::clamp(nota, NOTA_MINIMA, NOTA_MAXIMA);
}

std::ostream& operator<<(std::ostream& os, const Recenzie& recenzie) {
    os << recenzie.utilizator_ << ": " << recenzie.nota_ << "/" << Recenzie::NOTA_MAXIMA;
    if (!recenzie.text_.empty()) {
        os << " \"" << recenzie.text_ << "\"";
    }
    return os;
}
