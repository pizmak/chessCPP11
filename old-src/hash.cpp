#include "hash.h"
#include "time.h"
#include <cstdlib>
#include <iostream>

using namespace std;

// cala tablica
hash_element tablica_hasujaca[TABLICA_HASUJACA_SIZE];

unsigned long long hash_maski[64][9][2]; // pola, figury, kolory

unsigned long long daj_losowy_hash() {
    unsigned long long ans = ((unsigned long long) (rand() & 0xF) << (15 * 4)) //
    + ((unsigned long long) rand() << (15 * 3)) + ((unsigned long long) rand() << (15 * 2)) + ((unsigned long long) rand() << (15 * 1))
            + ((unsigned long long) rand());

    return ans;
}

void inicjalizuj_hash_maski() {
    cout << "Inicjalizuje hashmaski\n";
    time_t rn;
    srand((unsigned) time(&rn));
    //cout << daj_losowy_hash() << endl;
    //cout << daj_losowy_hash() << endl;
    //cout << daj_losowy_hash() << endl;
    //cout << daj_losowy_hash() << endl;
    //cout << 0xFFFFFFFFFFFFFFFFULL;

    /*	for (int i = 0; i < 64; i++)
     hashe_pol[i] = daj_losowy_hash();
     for (int i = 0; i < 9; i++)
     hashe_figur[i] = daj_losowy_hash();
     for (int i = 0; i < 2; i++)
     hashe_kolorow[i] = daj_losowy_hash();*/

    for (auto &arr2 : hash_maski) {
        for (auto &arr : arr2) {
            for (auto &elem : arr) {
                //cout << i << " " << j << " " << k << "\n";
                elem = daj_losowy_hash();
            }
        }
    }
}

unsigned long long daj_hash_pozycji() {
    unsigned long long wynik = hash_maski[0][int(plansza[0])][int(kolor[0])];
    for (int i = 1; i < 64; i++) {
        wynik ^= hash_maski[i][int(plansza[i])][int(kolor[i])];
    }
    return wynik;
}

#define BITOW25OSTATNICH 0x1FFFFFFULL

void hashuj_aktualna_pozycje(short wartosc, unsigned char glebokosc) {
    hash_element element = {daj_hash_pozycji(), wartosc, glebokosc};

    int index = (int) (element.hash & BITOW25OSTATNICH);
    tablica_hasujaca[index] = element;
}

hash_element * daj_wartosc_z_tablicy_hasujacej() {
    unsigned long long hash_pozycji = daj_hash_pozycji();

    int index = (int) (hash_pozycji & BITOW25OSTATNICH); /* index w tablicy hasujacej to koncowka hash-a */
    if (tablica_hasujaca[index].hash == hash_pozycji) { // porownanie z pelnym hashem
        return &tablica_hasujaca[index];
    } else {
        return NULL;
    }
}
