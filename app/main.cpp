#include <cstddef>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Catalog.h"

namespace {

// Afiseaza un mesaj si citeste o linie intreaga de la tastatura (poate contine spatii).
std::string citesteLinie(const std::string& mesaj) {
    std::cout << mesaj;
    std::string linie;
    std::getline(std::cin, linie);
    if (!linie.empty() && linie.back() == '\r') {   // Enter-ul de pe Windows lasa un '\r'
        linie.pop_back();
    }
    return linie;
}

// Transforma textul in numar. Intoarce 0 daca textul nu este un numar.
int laNumar(const std::string& text) {
    std::istringstream sin(text);
    int numar = 0;
    if (!(sin >> numar)) {
        return 0;
    }
    return numar;
}

// Citeste o nota si verifica daca e intre NOTA_MINIMA si NOTA_MAXIMA.
// Intoarce 0 (si afiseaza un mesaj) daca nota nu e valida.
int citesteNota(const std::string& mesaj) {
    const int nota = laNumar(citesteLinie(mesaj));
    if (nota < Recenzie::NOTA_MINIMA || nota > Recenzie::NOTA_MAXIMA) {
        std::cout << "Nota trebuie sa fie intre " << Recenzie::NOTA_MINIMA << " si "
                  << Recenzie::NOTA_MAXIMA << ".\n";
        return 0;
    }
    return nota;
}

void afiseazaLista(const std::vector<Titlu>& lista, const Catalog& catalog) {
    if (lista.empty()) {
        std::cout << "  (niciun rezultat)\n";
        return;
    }
    int pozitie = 1;
    for (const Titlu& titlu : lista) {
        std::cout << "  " << pozitie << ". " << titlu.getNume() << " (" << titlu.getAn()
                  << ", " << titlu.getGen() << ") - scor " << catalog.scorPonderat(titlu) << "\n";
        ++pozitie;
    }
}

void afiseazaMeniu() {
    std::cout << "\n========== CineDB ==========\n"
              << "1. Toate titlurile\n"
              << "2. Cauta un titlu\n"
              << "3. Top pe gen\n"
              << "4. Celebritati: filmografie\n"
              << "5. Adauga o recenzie\n"
              << "6. Simuleaza un vot (fara salvare)\n"
              << "7. Recomandari pentru mine\n"
              << "0. Iesire\n";
}

void meniuCautare(const Catalog& catalog) {
    const std::string text = citesteLinie("Cauta: ");
    const std::vector<Titlu> rezultate = catalog.cauta(text);
    if (rezultate.empty()) {
        std::cout << "Nu am gasit niciun titlu care sa contina \"" << text << "\".\n";
        return;
    }
    for (const Titlu& titlu : rezultate) {
        std::cout << titlu << "\n";
    }
}

void meniuTop(const Catalog& catalog) {
    constexpr int TOP_IMPLICIT = 3;   // folosit cand utilizatorul nu da un numar valid

    const std::string gen = citesteLinie("Gen (SF, Drama, Comedie, Actiune, ...): ");
    int cate = laNumar(citesteLinie("Cate titluri? "));
    if (cate <= 0) {
        cate = TOP_IMPLICIT;
    }
    std::cout << "Top " << cate << " " << gen << ":\n";
    afiseazaLista(catalog.topDupaGen(gen, static_cast<std::size_t>(cate)), catalog);
}

void meniuFilmografie(const Catalog& catalog) {
    const std::string nume = citesteLinie("Numele actorului sau regizorului: ");
    std::cout << "Filmografia lui " << nume << ":\n";
    afiseazaLista(catalog.filmografie(nume), catalog);
}

void meniuRecenzie(Catalog& catalog) {
    const std::string titlu = citesteLinie("Titlul exact: ");
    const std::string utilizator = citesteLinie("Utilizator: ");
    const int nota = citesteNota("Nota: ");
    if (nota == 0) {
        return;
    }
    const std::string text = citesteLinie("Parerea ta: ");

    const Recenzie recenzie(utilizator, nota, text);
    if (catalog.adaugaRecenzie(titlu, recenzie)) {
        std::cout << "Am salvat recenzia: " << recenzie << "\n";
    } else {
        std::cout << "Nu exista titlul \"" << titlu << "\".\n";
    }
}

// Arata cum s-ar schimba nota unui titlu, lucrand pe o COPIE.
// Aici se vad constructorul de copiere si operator= din clasa Titlu.
void meniuSimulare(const Catalog& catalog) {
    const std::vector<Titlu> rezultate = catalog.cauta(citesteLinie("Titlu: "));
    if (rezultate.empty()) {
        std::cout << "Nu am gasit titlul.\n";
        return;
    }
    const int nota = citesteNota("Ce nota ai da? ");
    if (nota == 0) {
        return;
    }

    Titlu copie(rezultate[0]);                                // constructor de copiere
    copie.adaugaRecenzie(Recenzie("vot simulat", nota, ""));
    std::cout << copie.getNume() << ": nota medie ar trece de la "
              << rezultate[0].notaMedie() << " la " << copie.notaMedie() << "\n";

    copie = rezultate[0];                                     // operator=: anulam votul
    std::cout << "Votul simulat a fost anulat. Nota ramane " << copie.notaMedie()
              << " (voturi: " << copie.numarVoturi() << ").\n";
}

void meniuRecomandari(const Catalog& catalog) {
    constexpr std::size_t NUMAR_RECOMANDARI = 3;

    const std::string utilizator = citesteLinie("Utilizator: ");
    std::cout << "Recomandari pentru " << utilizator << ":\n";
    afiseazaLista(catalog.recomandari(utilizator, NUMAR_RECOMANDARI), catalog);
}

} // namespace

int main() {
    /////////////////////////////////////////////////////////////////////////
    /// Datele de intrare pentru meniu sunt in tastatura.txt (folosit de bife pe GitHub).
    /// Filmele sunt citite din alte fisiere: assets/titluri.csv, distributie.csv, recenzii.csv.
    /////////////////////////////////////////////////////////////////////////
    std::cout << std::fixed << std::setprecision(2);   // notele se afiseaza cu 2 zecimale

    Catalog catalog;
    if (!catalog.incarcaDinFolder("assets")) {
        std::cout << "Nu pot citi fisierele CSV din folderul assets\n";
        return 1;
    }
    std::cout << "Am incarcat " << catalog.numarTitluri() << " titluri.\n";

    while (true) {
        afiseazaMeniu();
        const std::string optiune = citesteLinie("Optiune: ");
        if (!std::cin || optiune == "0") {   // !std::cin = s-au terminat datele de intrare
            break;
        }

        if (optiune == "1") {
            std::cout << catalog;
        } else if (optiune == "2") {
            meniuCautare(catalog);
        } else if (optiune == "3") {
            meniuTop(catalog);
        } else if (optiune == "4") {
            meniuFilmografie(catalog);
        } else if (optiune == "5") {
            meniuRecenzie(catalog);
        } else if (optiune == "6") {
            meniuSimulare(catalog);
        } else if (optiune == "7") {
            meniuRecomandari(catalog);
        } else {
            std::cout << "Optiune invalida.\n";
        }
    }

    std::cout << "La revedere!\n";
    return 0;
}
