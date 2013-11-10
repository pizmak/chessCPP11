#pragma once

#include <string.h>
#include <iostream>

bool czy_jest(short figura, short pole, short dokad);
void wypisz_zmiany(short figura, short pole);
void wypisz(short figura, short pole);
void drukuj_plansze();
void drukuj_mozliwe_ruchy();
void sort_debug();
void rusz_i_wypisz_ruch(struct ruch * r);
void wypisz_atakowane();
void wypisz_bitowoC(char x, int ile_bitow);
void wypisz_bitowo(unsigned short x, int ile_bitow);
void test_alfabeta();
void informacje_o_ruchu(struct ruch * r);
void informacje_o_ruchu(struct ruch * r, char * s);
