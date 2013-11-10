#include "debug.h"
#include "zasady.h"
#include "ruchy.h"
#include "hash.h"
#include "Cmp.h"
#include "debiuty.h"
#include <algorithm>
#include <iostream>

bool czy_jest(short figura, short pole, short dokad) {
	for (int i = 0; i < 64; i++) {
		if (mozliwe_ruchy[figura][pole][i] == dokad)
			return true;
	}
	return false;
}

void wypisz_zmiany(short figura, short pole) {
	int i = -1;
	while (zmiany_kierunkow[figura][pole][++i] >= 0) 
		cout << " " << (int)zmiany_kierunkow[figura][pole][i];
}

void wypisz(short figura, short pole) {	
	cout << "zmiany: ";
	wypisz_zmiany(figura, pole);
	int i = 0;
	cout << "\nruchy :  ";
	while (i == i) {
		cout << (int)mozliwe_ruchy[figura][pole][i++] << " ";
		if (mozliwe_ruchy[figura][pole][i] == -1) 
			break;

	}
	cout << "\n\n##########";
	cout <<	"\n#";
	for (short p = 0; p < 64; p++) {
		
		if (czy_jest(figura, pole, p))
			cout << "*";
		else if (p == pole)
			cout << "X";
		else 
			cout << " ";
		if (p % 8 == 7)
			cout << "#\n#";
	}
	cout << "#########\n";
}

void drukuj_plansze() {
	cout << "\n\n##########";
	cout <<	"\n#";
	for (short pole = 0; pole < 64; pole++) {
		if (kolor[pole] == empty_square) 
			cout << (       ((pole % 2) + ((pole / 8) % 2) + 1) % 2 ? " " : "."         );
		else
			cout << figury[plansza[pole] + kolor[pole] * 7];
		if (pole % 8 == 7)
			cout<<"#\n#";
	}
	cout << "#########\n";
}

void drukuj_mozliwe_ruchy() {
	for (short x = 0; x < 64; x++) {
		cout << "\n\n";
		wypisz(pawn, x);
		cout << "\n\n";
		wypisz(bpawn, x);
		//continue;
		cout << "\n\n";
		wypisz(queen, x);
		cout << "\n\n";
		wypisz(bishop, x);
		cout << "\n\n";
		wypisz(knight, x);
		cout << "\n\n";
		wypisz(king, x);
		cout << "\n\n";
		wypisz(rook, x);
	}

	for (short x = 0; x < 64; x++) {
		cout << "\n\n";
		wypisz(pawna, x);
		cout << "\n\n";
		wypisz(bpawna, x);
	}
}

void sort_debug() { // kod testowy sortowania
	ruchy[0].t = ruchy[0].f = 'a';
	ruchy[1].t = ruchy[1].f = 'b';
	ruchy[2].t = ruchy[2].f = 'c';
	ruchy[3].t = ruchy[3].f = 'd';
	ruchy[0].score = 0;
	ruchy[1].score = -10;
	ruchy[2].score = 20;
	ruchy[3].score = -300;
	for (int x = 4; x < ILE_RUCHOW_W_TABLICY; x++) {
		ruchy[x].score = 40;
	}
	ruchy[9999].score = -2;
	ruchy[1023].score = -19;
	sort(ruchy, ruchy + ILE_RUCHOW_W_TABLICY, Cmp_malejaco());
	cout << ruchy[0].score << " " << ruchy[1].score << " " << ruchy[2].score << " "<< ruchy[3].score << endl;
	sort(ruchy, ruchy + ILE_RUCHOW_W_TABLICY, Cmp_rosnaco());
	ruchy[1023].score = -19;
	cout << ruchy[0].score << " " << ruchy[1].score << " " << ruchy[2].score << " " << ruchy[3].score << endl;
	cout << ruchy[0].f << " " << ruchy[1].f << " " << ruchy[2].f << " " << ruchy[3].f << endl;
	system("Pause");
}

void rusz_i_wypisz_ruch(struct ruch * r) {
	cout << "\n" <<(short)r->f << " " << (short)r->t << " " << (short)r->co_zbite;
	drukuj_plansze(); 
	rusz(r);
	drukuj_plansze();
}

void wstaw_figure(char figura, char kolor_, string pozycja) {
	plansza[notacja_do_liczby(pozycja)] = figura;
	kolor[notacja_do_liczby(pozycja)] = kolor_;
	if (figura == king) 
		king_position[kolor_] = notacja_do_liczby(pozycja);
}

void wyjmij_figure(string pozycja) {
	plansza[notacja_do_liczby(pozycja)] = empty_square;
	kolor[notacja_do_liczby(pozycja)] = empty_square;
}

void wyczysc_plansze() {
	for (short p = 0; p < 64; p++) { //czyszczenie planszy
		plansza[p] = empty_square;
		kolor[p] = empty_square;
	}
}

void wypisz_atakowane() {
	/*for (short p = 0; p < 64; p++) {
		plansza[p] = empty_square;
		kolor[p] = empty_square;
	}
*/
/*	for (short pos = 0; pos < 64; pos++) {
		plansza[42] = knight;
		kolor[42] = black;
		plansza[20] = rook;
		kolor[20] = black;
		plansza[pos] = knight;
		kolor[pos] = white;*/
		cout << "\n\n##########";
		cout <<	"\n#";
		rusz_notacyjnie("e2", "e4");

		rusz_notacyjnie("d2", "d4");
		for (short p = 0; p < 64; p++) {
			if (czy_pole_atakowane((char)p, white))
				cout << "*";
			else 
				if (kolor[p] != empty_square)
					cout << "@";
				else
					cout << " ";
			if (p % 8 == 7)
				cout<<"#\n#";
		}
		cout << "#########\n";
//		plansza[pos] = empty_square;
//		kolor[pos] = empty_square;
//	}
}

void wypisz_bitowoC(char x, int ile_bitow) {
	if (ile_bitow > 0) {
		wypisz_bitowoC(x >> 1, ile_bitow - 1);
		cout << (x & 0x0001);
	}
}

void wypisz_bitowo(unsigned short x, int ile_bitow) {
	if (ile_bitow > 0) {
		wypisz_bitowo(x >> 1, ile_bitow - 1);
		cout << (x & 0x0001);
	}
	else {
		cout << "\n";
	}
}

void informacje_o_ruchu(struct ruch * r) {
	cout << "m: " << liczba_do_notacji(r->f) << " " << liczba_do_notacji(r->t) << " " << r->score << " " << (int)r->co_zbite << "\n";
}

void informacje_o_ruchu_po_ruchu(struct ruch * r) {
	cout << "ruszylem: " << ((plansza[r->f] == empty_square) ? figury[plansza[r->t]] : figury[plansza[r->t]]) << liczba_do_notacji(r->t) << " z " << liczba_do_notacji(r->f) << " " << r->score << "\n";
}

void informacje_o_ruchu(struct ruch * r, char * s) {
	cout << s << ", move info: " << liczba_do_notacji(r->f) << " " << liczba_do_notacji(r->t) << " " << r->score << " " << (int)r->co_zbite << "\n";
}


//void poprawnosc_ruchu(

struct ruch * wczytaj_ruch(char kto_rusza) {
	string s;
	if (kto_rusza) 
		cout << "black";
	else
		cout << "white";

jeszcze_raz:
	cout << " to move, podaj ruch np: (e2e4)\n";
	cin >> s;
	if (s == "u" && ostatni_ruch > ruchy_w_partii + 2) { // cofniecie ruchu
		cofnij_ruch(ostatni_ruch);
		ostatni_ruch--;
		cofnij_ruch(ostatni_ruch);
		ostatni_ruch--;
		drukuj_plansze();
		cout << "cofnieto\n";
		goto jeszcze_raz;
	}
	string dokad = "", skad = "";
	skad += s[0];
	skad += s[1];
	if (s.length() >= 4) {
		dokad += s[2];
		dokad += s[3];
	}
	if (s.length() == 2) 
		dokad = skad;
	char f = notacja_do_liczby(skad);
	char t = notacja_do_liczby(dokad);

	wezel = &ruchy[1];
	wypelnij_tablice_ruchow(kto_rusza, ostatni_ruch);
	
	for (struct ruch * index = ruchy + 1; index < wezel; index++) { // poszukiwanie podanego wsrod mozliwych
		cout << liczba_do_notacji(index->f) << "->" << liczba_do_notacji(index->t) << "\n";
		if (t == index->t) {
			if (t != f) {
				if (f == index->f)
					return index;
			}
			else 
				return index;	
		}
		
	}
	return NULL;
}

char kto_rusza = white;
void wykonaj_ruch_gracza() {
	struct ruch * ruch_gracza = NULL;
	while (ruch_gracza == NULL) {
		ruch_gracza = wczytaj_ruch(kto_rusza);
		if (ruch_gracza == NULL) {
			cout << "zly ruch!!\n";
		}
	}

	rusz(ruch_gracza);
	uaktualnij_debiuty(ruch_gracza);
	ostatni_ruch++;
	*ostatni_ruch = *ruch_gracza;
	drukuj_plansze();
	//wypisz_bitowo(ostatni_ruch->flags, 16);
	kto_rusza = przeciwnik(kto_rusza);
}

void wykonaj_ruch_komputera() {
	struct ruch * ruch_z_debiutow; 
	ruch_z_debiutow = wez_z_debiutow(kto_rusza);
	if (!ruch_z_debiutow)
		daj_ruch(kto_rusza, GLEBOKOSC, MIN_SHORT, MAX_SHORT, &ruchy[0]); // UWAGA!!
	else
		najlepszy_ruch = *ruch_z_debiutow;
	cout << "\nUWAGA ilosc pozycji "  << ile_ocen_pozycji << "\n";
	cout << "\nUWAGA odczyty hasu " << ile_celnych_odczytow_hashu << "\n";
	rusz(&najlepszy_ruch);
	
	uaktualnij_debiuty(&najlepszy_ruch);
	ostatni_ruch++;
	*ostatni_ruch = najlepszy_ruch;
	//cout << "najlepszy ruch " << liczba_do_notacji(najlepszy_ruch.f) << "->" << liczba_do_notacji(najlepszy_ruch.t) <<  "\n";
	drukuj_plansze();
	informacje_o_ruchu_po_ruchu(&najlepszy_ruch);
	kto_rusza = przeciwnik(kto_rusza);
	//wypisz_bitowo(ostatni_ruch->flags, 16);
	
	//wypisz_bitowo(ostatni_ruch->flags, 16);
	//system("pause");
}


void test_alfabeta() {
	//wyczysc_plansze();
	//wstaw_figure(king, white, "e8");
	//wstaw_figure(rook, white, "f8");
	//wstaw_figure(rook, white, "g8");
	//wstaw_figure(king, black, "c7");
	//drukuj_plansze();	
	while (true) {
		//cout << ocen_pozycje() << " taka ocena\n";
		
		
		wykonaj_ruch_gracza();
		wykonaj_ruch_komputera();
		
		

	}
}
