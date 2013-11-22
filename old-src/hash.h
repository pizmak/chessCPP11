#pragma once

#include "zasady.h"
#include "debug.h"

#define TABLICA_HASUJACA_SIZE (1 << 26)
struct hash_element {
    unsigned long long hash;
    short ocena_pozycji;
    unsigned char glebokosc;
};

/*extern unsigned long long hashe_pol[];
 extern unsigned long long hashe_figur[];
 extern unsigned long long hashe_kolorow[];*/

extern unsigned long long hash_maski[][9][2];

void inicjalizuj_hash_maski();

unsigned long long daj_hash_pozycji();

void hashuj_aktualna_pozycje(short wartosc, unsigned char glebokosc);

hash_element * daj_wartosc_z_tablicy_hasujacej();
