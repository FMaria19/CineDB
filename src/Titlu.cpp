#include "Titlu.h"

Titlu::Titlu(const std::string& nume, int an, const std::string& gen, int durataMinute, const Persoana& regizor)
    : nume_(nume), an_(an), gen_(gen), durataMinute_(durataMinute), regizor_(regizor) {}

// Constructorul de copiere: noul obiect primeste o copie a fiecarui atribut.
Titlu::Titlu(const Titlu& other)
    : nume_(other.nume_),
      an_(other.an_),
      gen_(other.gen_),
      durataMinute_(other.durataMinute_),
      regizor_(other.regizor_),
      distributie_(other.distributie_),
      recenzii_(other.recenzii_) {}

// operator=: un obiect care exista deja primeste valorile altuia.
Titlu& Titlu::operator=(const Titlu& other) {
    if (this != &other) {   // protectie la auto-atribuire (t = t)
        nume_ = other.nume_;
        an_ = other.an_;
        gen_ = other.gen_;
        durataMinute_ = other.durataMinute_;
        regizor_ = other.regizor_;
        distributie_ = other.distributie_;
        recenzii_ = other.recenzii_;
    }
    return *this;
}

// Destructorul: nu avem memorie alocata manual (string si vector se elibereaza singure),
// deci nu este nimic de facut aici.
Titlu::~Titlu() {}

void Titlu::adaugaActor(const Persoana& actor) {
    distributie_.push_back(actor);
}

void Titlu::adaugaRecenzie(const Recenzie& recenzie) {
    for (Recenzie& existenta : recenzii_) {
        if (existenta.getUtilizator() == recenzie.getUtilizator()) {
            existenta = recenzie;
            return;
        }
    }
    recenzii_.push_back(recenzie);
}

const std::string& Titlu::getNume() const {
    return nume_;
}

const std::string& Titlu::getGen() const {
    return gen_;
}

int Titlu::getAn() const {
    return an_;
}

double Titlu::notaMedie() const {
    if (recenzii_.empty()) {
        return 0.0;
    }
    int suma = 0;
    for (const Recenzie& recenzie : recenzii_) {
        suma += recenzie.getNota();
    }
    return static_cast<double>(suma) / static_cast<double>(recenzii_.size());
}

int Titlu::numarVoturi() const {
    return static_cast<int>(recenzii_.size());
}

bool Titlu::areInEchipa(const std::string& nume) const {
    if (regizor_.getNume() == nume) {
        return true;
    }
    for (const Persoana& actor : distributie_) {
        if (actor.getNume() == nume) {
            return true;
        }
    }
    return false;
}

int Titlu::notaDeLa(const std::string& utilizator) const {
    for (const Recenzie& recenzie : recenzii_) {
        if (recenzie.getUtilizator() == utilizator) {
            return recenzie.getNota();
        }
    }
    return 0;
}

// operator<< foloseste operator<< din Persoana si din Recenzie (compunere).
std::ostream& operator<<(std::ostream& os, const Titlu& titlu) {
    os << titlu.nume_ << " (" << titlu.an_ << ") | " << titlu.gen_ << " | "
       << titlu.durataMinute_ << " min\n";
    os << "  Regia: " << titlu.regizor_ << "\n";
    os << "  Distributie:\n";
    for (const Persoana& actor : titlu.distributie_) {
        os << "    - " << actor << ", " << actor.varstaIn(titlu.an_) << " ani la lansare\n";
    }
    os << "  Nota medie: " << titlu.notaMedie() << " (voturi: " << titlu.numarVoturi() << ")\n";
    if (!titlu.recenzii_.empty()) {
        os << "  Recenzii:\n";
        for (const Recenzie& recenzie : titlu.recenzii_) {
            os << "    - " << recenzie << "\n";
        }
    }
    return os;
}
