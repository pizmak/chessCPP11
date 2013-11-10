#pragma once

#include "zasady.h"
#include <string>

using namespace std;

#define ILE_RUCHOW_W_TABLICY 5000
#define ILE_RUCHOW_W_PARTII 500

struct ruch {
    char f; // skad
    char t; // dokad
    unsigned short flags; // flagi
    Checker co_zbite;
    short score; // punktacja
};

bool czy_pole_atakowane(register char pole, Color gracz);
void inicjalizcja_pierwszego_zaslepkowego_ruchu();

void rusz(ruch * r);
void cofnij_ruch(ruch * r);
/*
 #define dodaj_ruch(from, to, co_zbite_arg) \
	wezel->f = from; wezel->t = to;\
    wezel->co_zbite = co_zbite_arg;\
	wezel->score = wartosci_figur[co_zbite_arg];\
	rusz(wezel);\
	if (!czy_pole_atakowane(king_position[kolor[from]], kolor[from] ^ 1)) {\
		cofnij_ruch(wezel);\
		wezel++;\
	}\
	else\
		cofnij_ruch(wezel);\
 */

inline void dodaj_ruch(char from, char to, Checker co_zbite_arg);
inline void dodaj_ruch(char from, char to, Checker co_zbite_arg, unsigned short flags);
int wypelnij_tablice_ruchow(Color kto_rusza, ruch * po_ruchu);
inline void rusz(char skad, char dokad);
void rusz_notacyjnie(string skad, string dokad);
bool czy_koniec_gry();

//extern int pozycja_w_tablicy_ruchow;
extern ruch * wezel; // wezel wskazuje aktualny ruch
extern ruch * ostatni_ruch;
extern ruch *ruchy;
extern ruch *ruchy_w_partii;
