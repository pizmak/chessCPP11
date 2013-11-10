#pragma once
#define _w64
#define MIN_SHORT -32768
#define MAX_SHORT  32767
#define GLEBOKOSC_ITERACYJNEGO_POGLEBIANIA 4
#define GLEBOKOSC 6

inline short ocen_pozycje();
short alfabeta(char kto_gra, int glebokosc, short a, short b, struct ruch * kon);
struct ruch  daj_ruch(char kto_gra, int glebokosc, short a, short b, struct ruch * kon);

#ifndef _AI_
//extern int max_glebokosc;
extern int _w64 max_wezel;
extern struct ruch najlepszy_ruch;
extern int ile_ocen_pozycji;
extern int ile_celnych_odczytow_hashu;

#endif
