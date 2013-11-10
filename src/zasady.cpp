#include "zasady.h"
#include <iostream>

using namespace std;

short wartosci_figur[9] = {0, 0, knight_val, bishop_val, rook_val, queen_val, king_val, pawn_val, pawn_val};

char figury[18] = "  NBRQKP nbrqkppp";

char mozliwe_ruchy[10][64][64];
char zmiany_kierunkow[10][64][11];
char odleglosc[64][64];

// ustawienie figur na planszy
char plansza[64] = 
	{rook,	knight,	bishop,	queen,	king,	bishop,	knight,	rook, 
	 bpawn,	bpawn,	bpawn,	bpawn,	bpawn,	bpawn,	bpawn,	bpawn,
	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
 	 pawn,	pawn,	pawn,	pawn,	pawn,	pawn,	pawn,	pawn,
	 rook,	knight,	bishop,	queen,	king,	bishop,	knight,	rook
};

unsigned short globalne_flagi = white_castles | black_castles; //aktualne oflagowanie planszy

// jaki kolor figury stoi na danym polu w dnej chwili
char kolor[64] = 
	{black, black, black, black, black, black, black, black, 
	 black, black, black, black, black, black, black, black, 
	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
  	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
 	 white, white,  white, white,  white, white,  white, white,  
	 white, white,  white, white,  white, white,  white, white
};

char skok_ruchy[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {2, -1}, {2, 1}, {1, -2}, {1, 2}};
// wektory ruchu krola
char krol_ruchy[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}};

char king_position[2] = {60, 4};

char przesun(char pole, char * wektor) {
	return pole + wektor[0] + 8 * wektor[1];
}

bool czy_w_szachownicy(char pole) {
	return (pole >= 0 && pole <= 63);
}

int znajdzWKolumnie(char figura_, char kolor_, char kolumna) {
	if (kolumna > 'h' || kolumna < 'a')
		for (int i = 0; i < 64; i++) {
			if (plansza[i] == figura_ && kolor[i] == kolor_)
				return i;
		}
	for (int i = kolumna - 'a'; i < 64; i += 8) {
		if (plansza[i] == figura_ && kolor[i] == kolor_)
			return i;
	}
	return -1;
}

int znajdzWLinii(char figura_, char kolor_, int linia) {
	if (linia > 8 || linia < 1) 
		for (int i = 0; i < 64; i++) {
			if (plansza[i] == figura_ && kolor[i] == kolor_)
				return i;
		}
	for (int i = 64 - (8 * linia); i % 8 != 7; i++) {
		if (plansza[i] == figura_ && kolor[i] == kolor_)
			return i;
	}
	return -1;
}

int znajdzOdKonca(char figura_, char kolor_) {
	for (int i = 63; i >= 0; i--) {
		if (plansza[i] == figura_ && kolor[i] == kolor_)
			return i;
	}
}

bool czy_w_szachownicy(char pole, char * wektor) {
	if (przesun(pole, wektor) < 0 || przesun(pole, wektor) > 63) 
		return false;

	int dxp = 7 - pole % 8;
	int dxl = pole % 8;

	if (wektor[0] < 0) {
		return ((-wektor[0]) <= dxl);
	}
	return (wektor[0] <= dxp);
}

void uzupelnij_odleglosci() {
	cout << "inicjalizuje odleglosci\n";
	for (char x = 0; x < 64; x++) 
		for (char y = 0; y < 64; y++) 
			odleglosc[x][y] = (char)max(abs(kolumna(x) - kolumna(y)), abs(wiersz(y) - wiersz(x)));
}

void uzupelnij_tablice_ruchow() {
	cout << "inicjalizuje tablice ruchow\n";
	// czyszczenie tablicy
	for (int f = 0; f < 10; f++)
		for (int b = 0; b < 64; b++)
			for (int a = 0; a < 64; a++) 
				mozliwe_ruchy[f][b][a] = -1;
	
	for (int f = 0; f < 10; f++)
		for (int b = 0; b < 64; b++)
			for (int a = 0; a < 11; a++) 
				zmiany_kierunkow[f][b][a] = -1;


	// bicia pionow
	for (char pole = 0; pole < 64; pole++) {
		// ominienicie 2 rzedow skrajnych
		char num_w = 0;
		char num_b = 0;
		if (czy_skrajna_linia(pole))
			continue;
	
		if (!czy_pole_na_lewym_brzegu(pole)) {
			mozliwe_ruchy[pawna][pole][num_w++] = skosPL(pole);
			mozliwe_ruchy[bpawna][pole][num_b++] = skosDL(pole);
		}
		if (!czy_pole_na_prawym_brzegu(pole)) {
			mozliwe_ruchy[pawna][pole][num_w++] = skosPP(pole);
			mozliwe_ruchy[bpawna][pole][num_b++] = skosDP(pole);
		}
	}

	// pion bialy i czarny	
	for (char pole = 0; pole < 64; pole++) {
		// ominienicie 2 rzedow skrajnych
		if (czy_skrajna_linia(pole))
			continue;
		if (czy_2linia(pole)) 
			mozliwe_ruchy[pawn][pole][1] = pole - 16;
		if (czy_7linia(pole)) 
			mozliwe_ruchy[bpawn][pole][1] = pole + 16;
		mozliwe_ruchy[pawn][pole][0] = przod(pole);
		mozliwe_ruchy[bpawn][pole][0] = tyl(pole);
	}
	
	// skoczek
	for (char pole = 0; pole < 64; pole++) {
		char num = 0;
		for (int i = 0; i < 8; i++) 
			if (czy_w_szachownicy(pole, skok_ruchy[i]))  {
				mozliwe_ruchy[knight][pole][num++] = przesun(pole, skok_ruchy[i]);
			}
	}

	// krol
	for (char pole = 0; pole < 64; pole++) {
		char num = 0;
		for (int i = 0; i < 8; i++) 
			if (czy_w_szachownicy(pole, krol_ruchy[i])) {
				mozliwe_ruchy[king][pole][num++] = przesun(pole, krol_ruchy[i]);
			}
	}

	//goniec 
	for (char pole = 0; pole < 64; pole++) {
		char num = 0;
		char zmiany = 0;
		char akt_pole = pole; 
		
		bool nie_na_brzegu = !czy_pole_na_prawym_brzegu(akt_pole);

		if (nie_na_brzegu) { // sprawdzamy w prawo, jesli nie jest na brzegu
			akt_pole = skosPP(akt_pole);
			while (czy_w_szachownicy(akt_pole) && nie_na_brzegu) {
				mozliwe_ruchy[bishop][pole][num++] = akt_pole;
				nie_na_brzegu = !czy_pole_na_prawym_brzegu(akt_pole);
				akt_pole = skosPP(akt_pole);
			}

			akt_pole = pole;
			nie_na_brzegu = !czy_pole_na_prawym_brzegu(akt_pole);
			akt_pole = skosDP(akt_pole); 

			if (czy_w_szachownicy(akt_pole) && num != 0) 
				zmiany_kierunkow[bishop][pole][zmiany++] = num; // zmiana kierunku

			while (czy_w_szachownicy(akt_pole) && nie_na_brzegu) {
				mozliwe_ruchy[bishop][pole][num++] = akt_pole;
				nie_na_brzegu = !czy_pole_na_prawym_brzegu(akt_pole);
				akt_pole = skosDP(akt_pole);
			}
		}

		akt_pole = pole;
		nie_na_brzegu = !czy_pole_na_lewym_brzegu(akt_pole);
		if (nie_na_brzegu) {// sprawdzamy w lewo, jesli nie jest na brzegu
			akt_pole = pole;
			akt_pole = skosDL(akt_pole); 
			
			if (czy_w_szachownicy(akt_pole) && num != 0) 
				zmiany_kierunkow[bishop][pole][zmiany++] = num; // zmiana kierunku
			
			while (czy_w_szachownicy(akt_pole) && nie_na_brzegu) {
				mozliwe_ruchy[bishop][pole][num++] = akt_pole;
				nie_na_brzegu = !czy_pole_na_lewym_brzegu(akt_pole);
				akt_pole = skosDL(akt_pole);
			}

			akt_pole = pole;
			nie_na_brzegu = !czy_pole_na_lewym_brzegu(akt_pole);
			akt_pole = skosPL(akt_pole); 
			
			if (czy_w_szachownicy(akt_pole) && num != 0) 
				zmiany_kierunkow[bishop][pole][zmiany++] = num; // zmiana kierunku
			
			while (czy_w_szachownicy(akt_pole) && nie_na_brzegu) {
				mozliwe_ruchy[bishop][pole][num++] = akt_pole;
				nie_na_brzegu = !czy_pole_na_lewym_brzegu(akt_pole);
				akt_pole = skosPL(akt_pole);
			}
		}
	}

	// wieza
	for (char pole = 0; pole < 64; pole++) {
		char num = 0;
		char zmiany = 0;
		char akt_pole = pole; 

		akt_pole = przod(akt_pole);
		while (czy_w_szachownicy(akt_pole)) {
			mozliwe_ruchy[rook][pole][num++] = akt_pole;
			akt_pole = przod(akt_pole);
		}

		akt_pole = pole;
		akt_pole = tyl(akt_pole); 
		if (czy_w_szachownicy(akt_pole) && num != 0) 
			zmiany_kierunkow[rook][pole][zmiany++] = num; // zmiana kierunku
		while (czy_w_szachownicy(akt_pole)) {
			mozliwe_ruchy[rook][pole][num++] = akt_pole;
			akt_pole = tyl(akt_pole);
		}

		akt_pole = pole;
		bool nie_na_brzegu = !czy_pole_na_lewym_brzegu(akt_pole);
		if (nie_na_brzegu) {
			akt_pole = lewo(akt_pole); 
			if (czy_w_szachownicy(akt_pole) && num != 0) 
				zmiany_kierunkow[rook][pole][zmiany++] = num; // zmiana kierunku
			while (czy_w_szachownicy(akt_pole) && nie_na_brzegu) {
				mozliwe_ruchy[rook][pole][num++] = akt_pole;
				nie_na_brzegu = !czy_pole_na_lewym_brzegu(akt_pole);
				akt_pole = lewo(akt_pole);
			}
		}
		
		akt_pole = pole;
		nie_na_brzegu = !czy_pole_na_prawym_brzegu(akt_pole);
		if (nie_na_brzegu) {
			akt_pole = prawo(akt_pole); 
			if (czy_w_szachownicy(akt_pole) && num != 0) 
				zmiany_kierunkow[rook][pole][zmiany++] = num; // zmiana kierunku
			while (czy_w_szachownicy(akt_pole) && nie_na_brzegu) {
				mozliwe_ruchy[rook][pole][num++] = akt_pole;
				nie_na_brzegu = !czy_pole_na_prawym_brzegu(akt_pole);
				akt_pole = prawo(akt_pole);
			}
		}
	}

	// hetman 
	for (char pole = 0; pole < 64; pole++) {
		char num = 0;
		char num_temp_po_goncu = -1;
		char zmiany = 0;

		// kopiowanie ruchow gonca
		while (mozliwe_ruchy[bishop][pole][num] >= 0) {
			mozliwe_ruchy[queen][pole][num] = mozliwe_ruchy[bishop][pole][num];
			num++;
		}
		
		// kopiowanie ruchow wiezy
		int num_wiezy = 0;
		if (mozliwe_ruchy[rook][pole][0] >= 0) 
			num_temp_po_goncu = num;	
		while (mozliwe_ruchy[rook][pole][num_wiezy] >= 0) {
			mozliwe_ruchy[queen][pole][num] = mozliwe_ruchy[rook][pole][num_wiezy];
			num++;
			num_wiezy++;
		}
		
		while (zmiany_kierunkow[bishop][pole][zmiany] >= 0) {
			zmiany_kierunkow[queen][pole][zmiany] = zmiany_kierunkow[bishop][pole][zmiany];
			zmiany++;
		}

		int zmiany_wiezy = 0;
		if (num_temp_po_goncu >= 0) {
			zmiany_kierunkow[queen][pole][zmiany] = num_temp_po_goncu;
			zmiany++;
			while (zmiany_kierunkow[rook][pole][zmiany_wiezy] >= 0) {
				zmiany_kierunkow[queen][pole][zmiany] = zmiany_kierunkow[rook][pole][zmiany_wiezy] + num_temp_po_goncu;
				zmiany++;
				zmiany_wiezy++;
			}
		}
	}
}

char notacja_do_liczby(string s) {
	return (7 - (s[1] - '1')) * 8 + s[0] - 'a';
}

string liczba_do_notacji(char pos) {
	char ans[3] = "\0\0";
	ans[0] = 'a' + pos % 8;
	ans[1] = '8' - (pos / 8);
	//string s = ans;
	return ans;
}

