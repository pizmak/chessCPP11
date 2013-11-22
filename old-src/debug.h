#pragma once

#include "zasady.h"
#include "ruchy.h"

bool czy_jest(Checker figura, short pole, short dokad);
void wypisz_zmiany(Checker figura, short pole);
void wypisz(Checker figura, short pole);
void drukuj_plansze();
void drukuj_mozliwe_ruchy();
void sort_debug();
void rusz_i_wypisz_ruch(ruch * r);
void wypisz_atakowane();
void wypisz_bitowoC(char x, int ile_bitow);
void wypisz_bitowo(unsigned short x, int ile_bitow);
void test_alfabeta();
void informacje_o_ruchu(ruch * r);
void informacje_o_ruchu(ruch * r, const char * s);
