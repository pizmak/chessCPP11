#pragma once


#include <string>

using namespace std;

#define BICIE_W_PRZELOCIE_MASKA 0x3F
#define BICIE_W_PRZELOCIE_MASKA_REV 0xFFC0
#define ROSZADA_DLUGA_W 0x40
#define ROSZADA_KROTKA_W 0x80
#define ROSZADA_DLUGA_B 0x100
#define ROSZADA_KROTKA_B 0x200

#define white_castle_kingside 0x0001
#define white_castle_queenside 0x0002
#define black_castle_kingside 0x0004
#define black_castle_queenside 0x0008
#define white_castles 0x0003
#define black_castles 0x000C 
#define white_castle_kingside_R  0xFFFE //(0xFFFF - 0x0001)
#define white_castle_queenside_R 0xFFFD //(0xFFFF - 0x0002)
#define black_castle_kingside_R  0xFFFB//(0xFFFF - 0x0004)
#define black_castle_queenside_R 0xFFF7 //(0xFFFF - 0x0008)
#define white_castles_R  0xFFFC//(0xFFFF - 0x0003)
#define black_castles_R  0xFFF3//(0xFFFF - 0x000C)
#define castling 0x1000
#define no_castling 0xEFFF

#define queen_promotion 0x0010
#define knight_promotion 0x0020
#define rook_promotion 0x0040
#define bishop_promotion 0x0080

#define no_promotion 0xFF0F
#define promotions 0x0030

#define bicie_w_przelocie 0x3F00
#define no_bicie_w_przelocie 0xC0FF


#define reset_flags 0x000F

#define pawn_val 100
#define rook_val 500
#define knight_val 300
#define bishop_val 300
#define queen_val 900
#define king_val 9000

#define pawna   0
#define bpawna  1
#define knight  2
#define bishop  3
#define rook    4
#define queen   5
#define king    6
#define pawn    7
#define bpawn   8

#define white 0
#define black 1
#define empty_square 17

#define koniec_ruchow -1

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) > (b) ? (b) : (a))
#define abs(a) ((a) < 0 ? (-(a)) : (a))
#define kolumna(a) ((a) & 7)
#define wiersz(a) ((a) >> 3)

#define przod(x) (x - 8)
#define tyl(x) (x + 8)
#define lewo(x) (x - 1)
#define prawo(x) (x + 1)
#define skosPL(x) (x - 9)
#define skosPP(x) (x - 7)
#define skosDL(x) (x + 7)
#define skosDP(x) (x + 9)

#define czy_pole_na_prawym_brzegu(pole) ((pole) % 8 == 7)
#define czy_pole_na_lewym_brzegu(pole) ((pole) % 8 == 0)

#define czy_skrajna_linia(pole) ((pole) <= 7 || (pole) >= 56)
#define czy_2linia(pole) ((pole) >= 48 && (pole) <= 55)
#define czy_7linia(pole) ((pole) >= 8 && (pole) <= 15)
#define czy_4linia(pole) ((pole) >= 32 && (pole) <= 39)
#define czy_5linia(pole) ((pole) >= 24 && (pole) <= 31) //!!!!!!!!!!!!!!!!!!!!!!11


#define przeciwnik(kto_gra) (kto_gra ^ 1)

char przesun(char pole, char * wektor);
bool czy_w_szachownicy(char pole);// musi byc funkcja, a nie makro bo istnieje druga o tej nazwie!!!
bool czy_w_szachownicy(char pole, char * wektor);

int znajdzWKolumnie(char figura_, char kolor_, char kolumna);
int znajdzWLinii(char figura_, char kolor_, int linia);
int znajdzOdKonca(char figura_, char kolor_);

void uzupelnij_odleglosci();
void uzupelnij_tablice_ruchow();

char notacja_do_liczby(string s);
string liczba_do_notacji(char pos);

#ifndef _ZASADY_

extern short wartosci_figur[9];
extern char figury[18];

// ustawienie figur na planszy
extern char plansza[64];
extern unsigned short globalne_flagi;
// jaki kolor figury stoi na danym polu w dnej chwili
extern char kolor[64];

extern char king_position[2];

// wektory ruchu skoczka
extern char skok_ruchy[8][2];
// wektory ruchu krola
extern char krol_ruchy[8][2];

extern char mozliwe_ruchy[10][64][64];
extern char zmiany_kierunkow[10][64][11];
extern char odleglosc[64][64];
#endif
