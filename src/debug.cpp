#include "debug.h"
#include "zasady.h"
#include "ruchy.h"
#include "hash.h"
#include "debiuty.h"
#include "ai.h"
#include <algorithm>
#include <iostream>

bool czy_jest(Checker figura, short pole, short dokad) {
    for (int i = 0; i < 64; i++) {
        if (mozliwe_ruchy[int(figura)][pole][i] == dokad) {
            return true;
        }
    }
    return false;
}

void wypisz_zmiany(Checker figura, short pole) {
    int i = -1;
    while (zmiany_kierunkow[int(figura)][pole][++i] >= 0) {
        cout << " " << (int) zmiany_kierunkow[int(figura)][pole][i];
    }
}

void wypisz(Checker figura, short pole) {
    cout << "zmiany: ";
    wypisz_zmiany(figura, pole);
    int i = 0;
    cout << "\nruchy :  ";
    while (i == i) {
        cout << (int) mozliwe_ruchy[int(figura)][pole][i++] << " ";
        if (mozliwe_ruchy[int(figura)][pole][i] == -1) {
            break;
        }
    }
    cout << "\n\n##########";
    cout << "\n#";
    for (short p = 0; p < 64; p++) {
        if (czy_jest(figura, pole, p)) {
            cout << "*";
        } else if (p == pole) {
            cout << "X";
        } else {
            cout << " ";
        }
        if (kolumna(p) == 7) {
            cout << "#\n#";
        }
    }
    cout << "#########\n";
}

void drukuj_plansze() {
    cout << "\n\n##########";
    cout << "\n#";
    for (short pole = 0; pole < 64; pole++) {
        switch (kolor[pole]) {
        case Color::empty:
            cout << ((pole % 2 + pole / 8 % 2 + 1) % 2 ? " " : ".");
            break;
        default:
            cout << figury[int(plansza[pole]) + int(kolor[pole]) * 7];
        }
        if (czy_pole_na_prawym_brzegu(pole)) {
            cout << "#\n#";
        }
    }
    cout << "#########\n";
}

void drukuj_mozliwe_ruchy() {
    for (short x = 0; x < 64; x++) {
        cout << "\n\n";
        wypisz(Checker::pawn, x);
        cout << "\n\n";
        wypisz(Checker::bpawn, x);
        //continue;
        cout << "\n\n";
        wypisz(Checker::queen, x);
        cout << "\n\n";
        wypisz(Checker::bishop, x);
        cout << "\n\n";
        wypisz(Checker::knight, x);
        cout << "\n\n";
        wypisz(Checker::king, x);
        cout << "\n\n";
        wypisz(Checker::rook, x);
    }

    for (short x = 0; x < 64; x++) {
        cout << "\n\n";
        wypisz(Checker::pawna, x);
        cout << "\n\n";
        wypisz(Checker::bpawna, x);
    }
}

void sort_debug() { // kod testowy sortowania
    ruchy[0].t = ruchy[0].f = 'a';
    ruchy[1].t = ruchy[1].f = 'b';
    ruchy[2].t = ruchy[2].f = 'c';
    ruchy[3].t = ruchy[3].f = 'd';
    ruchy[0].score = 0;
    ruchy[1].score = -10;
    ruchy[2].score = 20;
    ruchy[3].score = -300;
    for (int x = 4; x < ILE_RUCHOW_W_TABLICY; x++) {
        ruchy[x].score = 40;
    }
    ruchy[9999].score = -2;
    ruchy[1023].score = -19;
    sort(ruchy, ruchy + ILE_RUCHOW_W_TABLICY, [](const ruch &a, const ruch &b) {return a.score > b.score;});
    cout << ruchy[0].score << " " << ruchy[1].score << " " << ruchy[2].score << " " << ruchy[3].score << endl;
    sort(ruchy, ruchy + ILE_RUCHOW_W_TABLICY, [](const ruch &a, const ruch &b) {return a.score < b.score;});
    ruchy[1023].score = -19;
    cout << ruchy[0].score << " " << ruchy[1].score << " " << ruchy[2].score << " " << ruchy[3].score << endl;
    cout << ruchy[0].f << " " << ruchy[1].f << " " << ruchy[2].f << " " << ruchy[3].f << endl;
    system("Pause");
}

void rusz_i_wypisz_ruch(ruch * r) {
    cout << "\n" << (short) r->f << " " << (short) r->t << " " << (short) r->co_zbite;
    drukuj_plansze();
    rusz(r);
    drukuj_plansze();
}

void wstaw_figure(Checker figura, Color kolor_, string pozycja) {
    plansza[notacja_do_liczby(pozycja)] = figura;
    kolor[notacja_do_liczby(pozycja)] = kolor_;
    if (figura == Checker::king) {
        king_position[int(kolor_)] = notacja_do_liczby(pozycja);
    }
}

void wyjmij_figure(string pozycja) {
    plansza[notacja_do_liczby(pozycja)] = Checker::empty;
    kolor[notacja_do_liczby(pozycja)] = Color::empty;
}

void wyczysc_plansze() {
    for (short p = 0; p < 64; p++) { //czyszczenie planszy
        plansza[p] = Checker::empty;
        kolor[p] = Color::empty;
    }
}

void wypisz_atakowane() {
    cout << "\n\n##########";
    cout << "\n#";
    rusz_notacyjnie("e2", "e4");

    rusz_notacyjnie("d2", "d4");
    for (short p = 0; p < 64; p++) {
        if (czy_pole_atakowane((char) p, Color::white)) {
            cout << "*";
        } else {
            cout << (kolor[p] != Color::empty ? "@" : " ");
        }
        if (czy_pole_na_prawym_brzegu(p)) {
            cout << "#\n#";
        }
    }
    cout << "#########\n";
//		plansza[pos] = empty_square;
//		kolor[pos] = empty_square;
//	}
}

void wypisz_bitowoC(char x, int ile_bitow) {
    if (ile_bitow > 0) {
        wypisz_bitowoC(x >> 1, ile_bitow - 1);
        cout << (x & 0x0001);
    }
}

void wypisz_bitowo(unsigned short x, int ile_bitow) {
    if (ile_bitow > 0) {
        wypisz_bitowo(x >> 1, ile_bitow - 1);
        cout << (x & 0x0001);
    } else {
        cout << "\n";
    }
}

void informacje_o_ruchu(ruch * r) {
    cout << "m: " << liczba_do_notacji(r->f) << " " << liczba_do_notacji(r->t) << " " << r->score << " " << (int) r->co_zbite << "\n";
}

void informacje_o_ruchu_po_ruchu(ruch * r) {
    cout << "ruszylem: " << ((plansza[r->f] == Checker::empty) ? figury[int(plansza[r->t])] : figury[int(plansza[r->t])])
            << liczba_do_notacji(r->t) << " z " << liczba_do_notacji(r->f) << " " << r->score << "\n";
}

void informacje_o_ruchu(ruch * r, const char * s) {
    cout << s << ", move info: " << liczba_do_notacji(r->f) << " " << liczba_do_notacji(r->t) << " " << r->score << " " << (int) r->co_zbite
            << "\n";
}

//void poprawnosc_ruchu(

ruch * wczytaj_ruch(Color kto_rusza) {
    string s;
    cout << (kto_rusza == Color::black ? "black" : "white");

    jeszcze_raz: cout << " to move, podaj ruch np: (e2e4)\n";
    cin >> s;
    if (s == "u" && ostatni_ruch > ruchy_w_partii + 2) { // cofniecie ruchu
        cofnij_ruch(ostatni_ruch);
        ostatni_ruch--;
        cofnij_ruch(ostatni_ruch);
        ostatni_ruch--;
        drukuj_plansze();
        cout << "cofnieto\n";
        goto jeszcze_raz;
    }
    string dokad = "", skad = "";
    skad += s[0];
    skad += s[1];
    if (s.length() >= 4) {
        dokad += s[2];
        dokad += s[3];
    }
    if (s.length() == 2) {
        dokad = skad;
    }
    char f = notacja_do_liczby(skad);
    char t = notacja_do_liczby(dokad);

    wezel = &ruchy[1];
    wypelnij_tablice_ruchow(kto_rusza, ostatni_ruch);

    for (ruch * index = ruchy + 1; index < wezel; index++) { // poszukiwanie podanego wsrod mozliwych
        cout << liczba_do_notacji(index->f) << "->" << liczba_do_notacji(index->t) << "\n";
        if (t == index->t) {
            if (t != f) {
                if (f == index->f) {
                    return index;
                }
            } else {
                return index;
            }
        }

    }
    return NULL;
}

Color kto_rusza = Color::white;
void wykonaj_ruch_gracza() {
    ruch * ruch_gracza = NULL;
    while (ruch_gracza == NULL) {
        ruch_gracza = wczytaj_ruch(kto_rusza);
        if (ruch_gracza == NULL) {
            cout << "zly ruch!!\n";
        }
    }

    rusz(ruch_gracza);
    uaktualnij_debiuty(ruch_gracza);
    ostatni_ruch++;
    *ostatni_ruch = *ruch_gracza;
    drukuj_plansze();
    //wypisz_bitowo(ostatni_ruch->flags, 16);
    kto_rusza = przeciwnik(kto_rusza);
}

void wykonaj_ruch_komputera() {
    ruch * ruch_z_debiutow;
    ruch_z_debiutow = wez_z_debiutow(kto_rusza);
    if (!ruch_z_debiutow) {
        daj_ruch(kto_rusza, GLEBOKOSC, numeric_limits<short>::min(), numeric_limits<short>::max(), &ruchy[0]); // UWAGA!!
    } else {
        najlepszy_ruch = *ruch_z_debiutow;
    }
    cout << "\nUWAGA ilosc pozycji " << ile_ocen_pozycji << "\n";
    cout << "\nUWAGA odczyty hasu " << ile_celnych_odczytow_hashu << "\n";
    rusz(&najlepszy_ruch);

    uaktualnij_debiuty(&najlepszy_ruch);
    ostatni_ruch++;
    *ostatni_ruch = najlepszy_ruch;
    //cout << "najlepszy ruch " << liczba_do_notacji(najlepszy_ruch.f) << "->" << liczba_do_notacji(najlepszy_ruch.t) <<  "\n";
    drukuj_plansze();
    informacje_o_ruchu_po_ruchu(&najlepszy_ruch);
    kto_rusza = przeciwnik(kto_rusza);
    //wypisz_bitowo(ostatni_ruch->flags, 16);

    //wypisz_bitowo(ostatni_ruch->flags, 16);
    //system("pause");
}

void test_alfabeta() {
    while (true) {
        wykonaj_ruch_gracza();
        wykonaj_ruch_komputera();
    }
}
