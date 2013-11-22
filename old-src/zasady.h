#pragma once

#include <string>

using namespace std;

static const unsigned short white_castle_kingside = 0x0001;
static const unsigned short white_castle_queenside = 0x0002;
static const unsigned short black_castle_kingside = 0x0004;
static const unsigned short black_castle_queenside = 0x0008;
static const unsigned short white_castles = white_castle_kingside | white_castle_queenside;
static const unsigned short black_castles = black_castle_kingside | black_castle_queenside;
static const unsigned short white_castle_kingside_R = ~white_castle_kingside;
static const unsigned short white_castle_queenside_R = ~white_castle_queenside;
static const unsigned short black_castle_kingside_R = ~black_castle_kingside;
static const unsigned short black_castle_queenside_R = ~white_castle_queenside;
static const unsigned short white_castles_R = ~white_castles;
static const unsigned short black_castles_R = ~black_castles;
static const unsigned short castling = 0x1000;

#define queen_promotion 0x0010
#define knight_promotion 0x0020
#define rook_promotion 0x0040
#define bishop_promotion 0x0080

#define no_promotion 0xFF0F
#define promotions 0x0030

static const unsigned short bicie_w_przelocie = 0x3F00;
static const unsigned short no_bicie_w_przelocie = ~bicie_w_przelocie;

#define START_BOARD \
    Checker::rook,  Checker::knight, Checker::bishop, Checker::queen, Checker::king,  Checker::bishop, Checker::knight, Checker::rook,\
    Checker::bpawn, Checker::bpawn,  Checker::bpawn,  Checker::bpawn, Checker::bpawn, Checker::bpawn,  Checker::bpawn,  Checker::bpawn,\
    Checker::empty, Checker::empty,  Checker::empty,  Checker::empty, Checker::empty, Checker::empty,  Checker::empty,  Checker::empty,\
    Checker::empty, Checker::empty,  Checker::empty,  Checker::empty, Checker::empty, Checker::empty,  Checker::empty,  Checker::empty,\
    Checker::empty, Checker::empty,  Checker::empty,  Checker::empty, Checker::empty, Checker::empty,  Checker::empty,  Checker::empty,\
    Checker::empty, Checker::empty,  Checker::empty,  Checker::empty, Checker::empty, Checker::empty,  Checker::empty,  Checker::empty,\
    Checker::pawn,  Checker::pawn,   Checker::pawn,   Checker::pawn,  Checker::pawn,  Checker::pawn,   Checker::pawn,   Checker::pawn,\
    Checker::rook,  Checker::knight, Checker::bishop, Checker::queen, Checker::king,  Checker::bishop, Checker::knight, Checker::rook

#define START_COLORS \
    Color::black, Color::black, Color::black, Color::black, Color::black, Color::black, Color::black, Color::black,\
    Color::black, Color::black, Color::black, Color::black, Color::black, Color::black, Color::black, Color::black,\
    Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty,\
    Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty,\
    Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty,\
    Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty, Color::empty,\
    Color::white, Color::white, Color::white, Color::white, Color::white, Color::white, Color::white, Color::white,\
    Color::white, Color::white, Color::white, Color::white, Color::white, Color::white, Color::white, Color::white

#define reset_flags 0x000F

enum class CheckerValues {
    pawn = 100,
    rook = 500,
    knight = 300,
    bishop = 300,
    queen = 900,
    king = 9000
};

enum class Checker {
    pawna = 0,
    bpawna = 1,
    knight = 2,
    bishop = 3,
    rook = 4,
    queen = 5,
    king = 6,
    pawn = 7,
    bpawn = 8,
    empty = 9,
};

enum class Color {
    white = 0,
    black = 1,
    empty = 17,
};

inline Checker colorizedPawn(Color kto_gra) {
    return Checker(int(Checker::pawn) + int(kto_gra));
}

#define koniec_ruchow -1

// kolumny od 0 do 7
#define kolumna(a) ((a) & 7)
// wiersze od 0 do 7 (0 -> 'h', 1 -> 'g' ...)
#define wiersz(a) ((a) >> 3)

#define przod(x) (x - 8)
#define tyl(x) (x + 8)
#define lewo(x) (x - 1)
#define prawo(x) (x + 1)
#define skosPL(x) (x - 9)
#define skosPP(x) (x - 7)
#define skosDL(x) (x + 7)
#define skosDP(x) (x + 9)

#define czy_pole_na_prawym_brzegu(pole) (kolumna(pole) == 7)
#define czy_pole_na_lewym_brzegu(pole) (kolumna(pole) == 0)

#define czy_skrajna_linia(pole) ((pole) <= 7 || (pole) >= 56)
template<int number> bool czy_linia(int pole) {
    return wiersz(pole) == 8 - number;
}

#define przeciwnik(kto_gra) Color(int(kto_gra) ^ 1)

char przesun(char pole, char * wektor);
bool czy_w_szachownicy(char pole); // musi byc funkcja, a nie makro bo istnieje druga o tej nazwie!!!
bool czy_w_szachownicy(char pole, char * wektor);

int znajdzWKolumnie(Checker figura_, Color kolor_, char kolumna);
int znajdzWLinii(Checker figura_, Color kolor_, int linia);
int znajdzOdKonca(Checker figura_, Color kolor_);

void uzupelnij_odleglosci();
void uzupelnij_tablice_ruchow();

char notacja_do_liczby(const string &s);
string liczba_do_notacji(char pos);

extern short wartosci_figur[9];
extern char figury[18];

// ustawienie figur na planszy
extern Checker plansza[64];
extern unsigned short globalne_flagi;
// jaki kolor figury stoi na danym polu w dnej chwili
extern Color kolor[64];

extern char king_position[2];

// wektory ruchu skoczka
extern char skok_ruchy[8][2];
// wektory ruchu krola
extern char krol_ruchy[8][2];

extern char mozliwe_ruchy[10][64][64];
extern char zmiany_kierunkow[10][64][11];
extern char odleglosc[64][64];
