//Am separat declarația clasei (în header) de implementare (în .cpp). Headerul descrie interfața publică a clasei, iar celelalte fișiere îl includ doar pe el
#ifndef PERSOANA_H
#define PERSOANA_H

#include <iostream>
#include <string>

// O persoana din lumea filmului: un actor sau un regizor. Clasa Persoana este folosită atât pentru regizori, cât și pentru actori
class Persoana {
public:
    Persoana(const std::string& nume, int anNastere);

    const std::string& getNume() const;

    // Cati ani avea persoana intr-un anumit an (anul lansarii unui film).
    int varstaIn(int an) const; //În loc să expun anul nașterii printr-un getter, clasa oferă direct operația de care am nevoie. Datele rămân private, iar clasa oferă ceva

    friend std::ostream& operator<<(std::ostream& os, const Persoana& persoana); //Am folosit functia friend cre îi permite acces la atributele private

private:
    std::string nume_;
    int anNastere_;
};

#endif // PERSOANA_H
