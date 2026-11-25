#include "Catalog.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <set>

#include <csv.hpp>
#include <random.hpp>

#include "Scor.h"

Catalog::Catalog(int voturiMinime) : voturiMinime_(voturiMinime) {}

bool Catalog::incarcaDinFolder(const std::string& folder) {
    const std::string titluri = folder + "/titluri.csv";
    const std::string distributie = folder + "/distributie.csv";
    const std::string recenzii = folder + "/recenzii.csv";

    if (!existaFisier(titluri) || !existaFisier(distributie) || !existaFisier(recenzii)) {
        return false;
    }

    // Ordinea conteaza: actorii si recenziile se leaga de titluri deja citite.
    citesteTitluri(titluri);
    citesteDistributie(distributie);
    citesteRecenzii(recenzii);
    return true;
}

// Fiecare rand: titlu,an,gen,durata_minute,regizor,an_nastere_regizor
void Catalog::citesteTitluri(const std::string& cale) {
    csv::CSVReader fisier(cale);
    for (csv::CSVRow& rand : fisier) {
        const Persoana regizor(rand["regizor"].get<std::string>(),
                               rand["an_nastere_regizor"].get<int>());
        adaugaTitlu(Titlu(rand["titlu"].get<std::string>(),
                          rand["an"].get<int>(),
                          rand["gen"].get<std::string>(),
                          rand["durata_minute"].get<int>(),
                          regizor));
    }
}

// Fiecare rand: titlu,actor,an_nastere
void Catalog::citesteDistributie(const std::string& cale) {
    csv::CSVReader fisier(cale);
    for (csv::CSVRow& rand : fisier) {
        Titlu* titlu = gaseste(rand["titlu"].get<std::string>());
        if (titlu != nullptr) {   // ignoram actorii unor titluri care nu exista
            titlu->adaugaActor(Persoana(rand["actor"].get<std::string>(),
                                        rand["an_nastere"].get<int>()));
        }
    }
}

// Fiecare rand: titlu,utilizator,nota,text
void Catalog::citesteRecenzii(const std::string& cale) {
    csv::CSVReader fisier(cale);
    for (csv::CSVRow& rand : fisier) {
        adaugaRecenzie(rand["titlu"].get<std::string>(),
                       Recenzie(rand["utilizator"].get<std::string>(),
                                rand["nota"].get<int>(),
                                rand["text"].get<std::string>()));
    }
}

void Catalog::adaugaTitlu(const Titlu& titlu) {
    titluri_.push_back(titlu);
}

bool Catalog::adaugaRecenzie(const std::string& numeTitlu, const Recenzie& recenzie) {
    Titlu* titlu = gaseste(numeTitlu);
    if (titlu == nullptr) {
        return false;
    }
    titlu->adaugaRecenzie(recenzie);
    return true;
}

int Catalog::numarTitluri() const {
    return static_cast<int>(titluri_.size());
}

// Scor = (v / (v + m)) * R + (m / (v + m)) * C
//   R = nota medie a titlului, v = cate voturi are
//   C = media tuturor notelor din catalog, m = voturiMinime_
// Un titlu cu putine voturi este "tras" spre media generala, ca un singur 10
// sa nu il urce direct pe primul loc.
double Catalog::scorPonderat(const Titlu& titlu) const {
    const int voturi = titlu.numarVoturi();
    if (voturi == 0) {
        return 0.0;
    }
    const double v = static_cast<double>(voturi);
    const double m = static_cast<double>(voturiMinime_);
    return (v / (v + m)) * titlu.notaMedie() + (m / (v + m)) * mediaGlobala();
}

std::vector<Titlu> Catalog::cauta(const std::string& text) const {
    std::vector<Titlu> rezultat;
    const std::string cautat = litereMici(text);
    for (const Titlu& titlu : titluri_) {
        if (litereMici(titlu.getNume()).find(cautat) != std::string::npos) {
            rezultat.push_back(titlu);
        }
    }
    return rezultat;
}

std::vector<Titlu> Catalog::topDupaGen(const std::string& gen, std::size_t n) const {
    std::vector<Titlu> rezultat;
    for (const Titlu& titlu : titluri_) {
        if (litereMici(titlu.getGen()) == litereMici(gen)) {
            rezultat.push_back(titlu);
        }
    }
    ordoneazaTop(rezultat, n);
    return rezultat;
}

std::vector<Titlu> Catalog::filmografie(const std::string& numePersoana) const {
    std::vector<Titlu> rezultat;
    for (const Titlu& titlu : titluri_) {
        if (titlu.areInEchipa(numePersoana)) {
            rezultat.push_back(titlu);
        }
    }
    std::sort(rezultat.begin(), rezultat.end(), [](const Titlu& a, const Titlu& b) {
        return a.getAn() < b.getAn();
    });
    return rezultat;
}

std::vector<Titlu> Catalog::recomandari(const std::string& utilizator, std::size_t n) const {
    // Pasul 1: genurile care i-au placut utilizatorului.
    std::set<std::string> genuriPreferate;
    for (const Titlu& titlu : titluri_) {
        if (titlu.notaDeLa(utilizator) >= NOTA_FILM_PLACUT) {
            genuriPreferate.insert(titlu.getGen());
        }
    }

    // Pasul 2: titlurile din acele genuri pe care nu le-a votat inca.
    std::vector<Titlu> rezultat;
    for (const Titlu& titlu : titluri_) {
        const bool genPreferat = genuriPreferate.count(titlu.getGen()) > 0;
        if (genPreferat && !aVotat(titlu, utilizator)) {
            rezultat.push_back(titlu);
        }
    }

    // Pasul 3: cele mai bune n dintre ele.
    ordoneazaTop(rezultat, n);
    return rezultat;
}

std::optional<Titlu> Catalog::filmSurpriza(const std::string& utilizator) const {
    // Pasul 1: candidatii = titlurile foarte bune pe care utilizatorul nu le-a votat.
    std::vector<Titlu> candidati;
    for (const Titlu& titlu : titluri_) {
        if (!aVotat(titlu, utilizator) && Scor(scorPonderat(titlu)).esteFoarteBun()) {
            candidati.push_back(titlu);
        }
    }
    if (candidati.empty()) {
        return std::nullopt;
    }

    // Pasul 2: biblioteca random alege unul la intamplare.
    // get(candidati) intoarce un iterator ("un deget" care arata spre un element), iar * il ia.
    return *effolkronium::random_static::get(candidati);
}

bool Catalog::aVotat(const Titlu& titlu, const std::string& utilizator) {
    // notaDeLa intoarce 0 (o nota sub minim) cand utilizatorul nu a votat.
    return titlu.notaDeLa(utilizator) >= Recenzie::NOTA_MINIMA;
}

Titlu* Catalog::gaseste(const std::string& numeTitlu) {
    for (Titlu& titlu : titluri_) {
        if (litereMici(titlu.getNume()) == litereMici(numeTitlu)) {
            return &titlu;
        }
    }
    return nullptr;
}

double Catalog::mediaGlobala() const {
    double sumaNotelor = 0.0;
    int totalVoturi = 0;
    for (const Titlu& titlu : titluri_) {
        sumaNotelor += titlu.notaMedie() * titlu.numarVoturi();
        totalVoturi += titlu.numarVoturi();
    }
    if (totalVoturi == 0) {
        return 0.0;
    }
    return sumaNotelor / totalVoturi;
}

void Catalog::ordoneazaTop(std::vector<Titlu>& lista, std::size_t n) const {
    std::sort(lista.begin(), lista.end(), [this](const Titlu& a, const Titlu& b) {
        return scorPonderat(a) > scorPonderat(b);
    });
    if (lista.size() > n) {
        lista.erase(lista.begin() + static_cast<std::ptrdiff_t>(n), lista.end());
    }
}

bool Catalog::existaFisier(const std::string& cale) {
    const std::ifstream fisier(cale);
    return fisier.good();
}

std::string Catalog::litereMici(const std::string& text) {
    std::string rezultat = text;
    for (char& c : rezultat) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return rezultat;
}

// operator<< foloseste operator<< din Titlu (compunere).
std::ostream& operator<<(std::ostream& os, const Catalog& catalog) {
    os << "=== CineDB: " << catalog.numarTitluri() << " titluri ===\n\n";
    for (const Titlu& titlu : catalog.titluri_) {
        os << titlu << "  Scor CineDB: " << Scor(catalog.scorPonderat(titlu)) << "\n\n";
    }
    return os;
}
