#ifndef RECENZIE_H
#define RECENZIE_H

#include <iostream>
#include <string>

// Parerea unui utilizator despre un titlu: o nota si un text scurt.
class Recenzie {
public:
    // Limitele unei note; orice nota primita e adusa in acest interval.
    static constexpr int NOTA_MINIMA = 1;
    static constexpr int NOTA_MAXIMA = 10;

    Recenzie(const std::string& utilizator, int nota, const std::string& text);

    const std::string& getUtilizator() const;
    int getNota() const;

    friend std::ostream& operator<<(std::ostream& os, const Recenzie& recenzie);

private:
    // Aduce o nota in intervalul [NOTA_MINIMA, NOTA_MAXIMA] (de exemplu 15 devine 10).
    static int limiteazaNota(int nota);

    std::string utilizator_;
    int nota_;
    std::string text_;
};

#endif // RECENZIE_H
