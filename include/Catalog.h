#ifndef CATALOG_H
#define CATALOG_H

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "Titlu.h"

// Catalogul CineDB: toate titlurile, plus functiile de cautare, clasament si recomandare.
class Catalog {
public:
    // Cate voturi trebuie sa aiba un titlu ca nota lui sa conteze "pe deplin" in clasament.
    static constexpr int VOTURI_MINIME_IMPLICIT = 3;

    explicit Catalog(int voturiMinime = VOTURI_MINIME_IMPLICIT);

    // Citeste titluri.csv, distributie.csv si recenzii.csv din folderul dat.
    // Intoarce false daca vreunul dintre fisiere lipseste.
    bool incarcaDinFolder(const std::string& folder);

    void adaugaTitlu(const Titlu& titlu);

    // Intoarce false daca nu exista niciun titlu cu acest nume.
    bool adaugaRecenzie(const std::string& numeTitlu, const Recenzie& recenzie);

    int numarTitluri() const;

    // Scorul ponderat (formula folosita de IMDb pentru Top 250).
    double scorPonderat(const Titlu& titlu) const;

    // Titlurile al caror nume contine textul cautat (fara sa conteze literele mari/mici).
    std::vector<Titlu> cauta(const std::string& text) const;

    // Cele mai bune n titluri dintr-un gen, dupa scorul ponderat.
    std::vector<Titlu> topDupaGen(const std::string& gen, std::size_t n) const;

    // Titlurile la care a lucrat o persoana (actor sau regizor), in ordine cronologica.
    std::vector<Titlu> filmografie(const std::string& numePersoana) const;

    // Titluri nevotate inca, din genurile la care utilizatorul a dat note mari.
    std::vector<Titlu> recomandari(const std::string& utilizator, std::size_t n) const;

    friend std::ostream& operator<<(std::ostream& os, const Catalog& catalog);

private:
    // De la ce nota consideram ca unui utilizator i-a placut un titlu.
    static constexpr int NOTA_FILM_PLACUT = 8;

    // Cele trei etape ale incarcarii, cate una pentru fiecare fisier CSV.
    void citesteTitluri(const std::string& cale);
    void citesteDistributie(const std::string& cale);
    void citesteRecenzii(const std::string& cale);

    // Titlul cu acest nume (fara sa conteze literele mari/mici), sau nullptr daca nu exista.
    Titlu* gaseste(const std::string& numeTitlu);

    // Media tuturor notelor din catalog.
    double mediaGlobala() const;

    // Sorteaza lista descrescator dupa scor si pastreaza doar primele n titluri.
    void ordoneazaTop(std::vector<Titlu>& lista, std::size_t n) const;

    static bool existaFisier(const std::string& cale);
    static std::string litereMici(const std::string& text);

    std::vector<Titlu> titluri_;
    int voturiMinime_;
};

#endif // CATALOG_H
