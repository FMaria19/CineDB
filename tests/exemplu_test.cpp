// Teste simple, fara framework: doar assert.
// assert() dispare cand NDEBUG este definit (Release), asa ca il anulam.
#undef NDEBUG
#include <cassert>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "Catalog.h"
#include "Scor.h"

namespace {

void testPersoana() {
    const Persoana p("Emma Stone", 1988);
    assert(p.getNume() == "Emma Stone");
    assert(p.varstaIn(2016) == 28);
}

void testRecenzie() {
    const Recenzie normala("ana", 8, "ok");
    assert(normala.getUtilizator() == "ana");
    assert(normala.getNota() == 8);

    // notele din afara intervalului sunt aduse la limite
    const Recenzie preaMare("x", Recenzie::NOTA_MAXIMA + 5, "");
    const Recenzie preaMica("x", Recenzie::NOTA_MINIMA - 4, "");
    assert(preaMare.getNota() == Recenzie::NOTA_MAXIMA);
    assert(preaMica.getNota() == Recenzie::NOTA_MINIMA);
}

void testTitlu() {
    Titlu film("Film", 2020, "SF", 100, Persoana("Regizor", 1970));
    film.adaugaActor(Persoana("Actor", 1990));
    assert(film.getNume() == "Film");
    assert(film.getGen() == "SF");
    assert(film.getAn() == 2020);
    assert(film.notaMedie() == 0.0);

    film.adaugaRecenzie(Recenzie("ana", 10, ""));
    film.adaugaRecenzie(Recenzie("mihai", 6, ""));
    assert(film.numarVoturi() == 2);
    assert(film.notaMedie() == 8.0);

    // acelasi utilizator voteaza din nou: nota veche este inlocuita
    film.adaugaRecenzie(Recenzie("ana", 8, ""));
    assert(film.numarVoturi() == 2);
    assert(film.notaDeLa("ana") == 8);
    assert(film.notaDeLa("necunoscut") == 0);

    assert(film.areInEchipa("Regizor"));
    assert(film.areInEchipa("Actor"));
    assert(!film.areInEchipa("Altcineva"));

    // copia este independenta de original
    Titlu copie(film);
    copie.adaugaRecenzie(Recenzie("ioana", 1, ""));
    assert(copie.numarVoturi() == 3);
    assert(film.numarVoturi() == 2);

    copie = film;
    assert(copie.numarVoturi() == 2);
}

void testCatalog() {
    Catalog catalog(2);
    const bool incarcat = catalog.incarcaDinFolder("folder_care_nu_exista");
    assert(!incarcat);

    Titlu a("Alfa", 2001, "SF", 90, Persoana("R1", 1960));
    a.adaugaRecenzie(Recenzie("ana", 9, ""));
    a.adaugaRecenzie(Recenzie("mihai", 9, ""));
    a.adaugaRecenzie(Recenzie("ioana", 9, ""));

    Titlu b("Beta", 1999, "SF", 90, Persoana("R2", 1960));
    b.adaugaActor(Persoana("Actor Comun", 1980));
    b.adaugaRecenzie(Recenzie("mihai", 10, ""));   // un singur vot de 10

    Titlu c("Gama", 2005, "Drama", 90, Persoana("R3", 1960));
    c.adaugaActor(Persoana("Actor Comun", 1980));
    c.adaugaRecenzie(Recenzie("ioana", 5, ""));
    c.adaugaRecenzie(Recenzie("radu", 5, ""));
    c.adaugaRecenzie(Recenzie("elena", 5, ""));

    catalog.adaugaTitlu(a);
    catalog.adaugaTitlu(b);
    catalog.adaugaTitlu(c);
    assert(catalog.numarTitluri() == 3);

    // un singur 10 nu bate trei note de 9
    const std::vector<Titlu> top = catalog.topDupaGen("sf", 5);
    assert(top.size() == 2);
    assert(top[0].getNume() == "Alfa");
    assert(catalog.scorPonderat(a) > catalog.scorPonderat(b));
    assert(catalog.topDupaGen("SF", 1).size() == 1);

    assert(catalog.cauta("ET").size() == 1);   // Beta
    assert(catalog.cauta("a").size() == 3);

    const std::vector<Titlu> filme = catalog.filmografie("Actor Comun");
    assert(filme.size() == 2);
    assert(filme[0].getNume() == "Beta");   // 1999 inainte de 2005

    // ana a dat 9 unui SF -> ii recomandam celelalte SF-uri nevotate
    const std::vector<Titlu> recomandate = catalog.recomandari("ana", 3);
    assert(recomandate.size() == 1);
    assert(recomandate[0].getNume() == "Beta");
    assert(catalog.recomandari("nimeni", 3).empty());

    const Recenzie noua("ana", 9, "");
    const bool adaugataLaGama = catalog.adaugaRecenzie("gama", noua);
    const bool adaugataLaInexistent = catalog.adaugaRecenzie("Nu exista", noua);
    assert(adaugataLaGama);
    assert(!adaugataLaInexistent);
}

void testScor() {
    assert(Scor(9.0).esteFoarteBun());
    assert(!Scor(7.5).esteFoarteBun());

    // scris altundeva decat pe ecran, scorul apare fara culori, doar ca numar
    std::ostringstream text;
    text << std::fixed << std::setprecision(2) << Scor(8.5);
    assert(text.str() == "8.50");
}

void testFilmSurpriza() {
    Catalog catalog(1);

    Titlu bun("Delta", 2010, "SF", 90, Persoana("R1", 1960));
    bun.adaugaRecenzie(Recenzie("ana", 10, ""));
    bun.adaugaRecenzie(Recenzie("mihai", 10, ""));
    bun.adaugaRecenzie(Recenzie("ioana", 10, ""));

    Titlu slab("Epsilon", 2011, "SF", 90, Persoana("R2", 1960));
    slab.adaugaRecenzie(Recenzie("ana", 2, ""));
    slab.adaugaRecenzie(Recenzie("mihai", 2, ""));
    slab.adaugaRecenzie(Recenzie("ioana", 2, ""));

    catalog.adaugaTitlu(bun);
    catalog.adaugaTitlu(slab);

    // singurul film foarte bun nevotat de radu este Delta, deci alegerea "la intamplare" e sigura
    const std::optional<Titlu> pentruRadu = catalog.filmSurpriza("radu");
    assert(pentruRadu.has_value());
    assert(pentruRadu->getNume() == "Delta");

    // ana a votat deja Delta, iar Epsilon nu e foarte bun: nu ramane nimic de propus
    assert(!catalog.filmSurpriza("ana").has_value());
}

} // namespace

int main() {
    testPersoana();
    testRecenzie();
    testTitlu();
    testCatalog();
    testScor();
    testFilmSurpriza();
    return 0;
}
