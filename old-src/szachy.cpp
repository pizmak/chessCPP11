#include "zasady.h"
#include "ruchy.h"
#include "ai.h"
#include "hash.h"
#include "debug.h"
#include "debiuty.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void inicjalizacja() {
    inicjalizuj_hash_maski();
    debiuty_set.clear();
    uzupelnij_odleglosci();
    uzupelnij_tablice_ruchow();
    inicjalizcja_pierwszego_zaslepkowego_ruchu();
}

int main() {
    inicjalizacja();
    //wczytaj_debiuty_z_pliku("debiuty.txt");
    //wczytaj_debiuty_z_pliku("debiuty.txt");
    //wczytaj_debiuty_z_pliku("debiuty2.txt");
    //wczytaj_debiuty_z_pliku("debiuty2.txt");
    //sort_debug();
    test_alfabeta();
    system("PAUSE");
    return 0;
}
