#ifndef TITLU_H
#define TITLU_H

#include <iostream>
#include <string>
#include <vector>

#include "Persoana.h"
#include "Recenzie.h"

// Un film din catalog. Este compus din alte clase:
// un regizor (Persoana), o distributie (Persoane) si recenzii (Recenzie).
class Titlu {
public:
    Titlu(const std::string& nume, int an, const std::string& gen, int durataMinute,
          const Persoana& regizor);

    // "Regula celor trei": constructor de copiere, operator= si destructor.
    Titlu(const Titlu& other);
    Titlu& operator=(const Titlu& other);
    ~Titlu();

    void adaugaActor(const Persoana& actor);

    // Fiecare utilizator are o singura recenzie per titlu.
    // Daca voteaza din nou, recenzia veche este inlocuita.
    void adaugaRecenzie(const Recenzie& recenzie);

    const std::string& getNume() const;
    const std::string& getGen() const;
    int getAn() const;

    double notaMedie() const;   // 0 daca titlul nu are inca recenzii
    int numarVoturi() const;

    // true daca persoana a regizat sau a jucat in acest titlu
    bool areInEchipa(const std::string& nume) const;

    // nota data de un utilizator, sau 0 daca utilizatorul nu a votat
    int notaDeLa(const std::string& utilizator) const;

    friend std::ostream& operator<<(std::ostream& os, const Titlu& titlu);

private:
    std::string nume_;
    int an_;
    std::string gen_;
    int durataMinute_;
    Persoana regizor_;
    std::vector<Persoana> distributie_;
    std::vector<Recenzie> recenzii_;
};

#endif // TITLU_H
