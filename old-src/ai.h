#pragma once
#define _w64

#define GLEBOKOSC_ITERACYJNEGO_POGLEBIANIA 4
#define GLEBOKOSC 6

#include "ruchy.h"

inline short ocen_pozycje();
short alfabeta(char kto_gra, int glebokosc, short a, short b, ruch * kon);
ruch daj_ruch(Color kto_gra, int glebokosc, short a, short b, ruch * kon);

extern int _w64 max_wezel;
extern ruch najlepszy_ruch;
extern int ile_ocen_pozycji;
extern int ile_celnych_odczytow_hashu;
