#include "ai.h"
#include "zasady.h"
#include "ruchy.h"
#include "hash.h"
#include "debug.h"
#include "Cmp.h"
#include <algorithm>
#include <iostream>

using namespace std;

#define KARA_ZA_DUBLOWANIE -20
#define KARA_ZA_IZOLOWANIE -20
//#define PASSED_PAWN_BONUS 20
#define SASIEDNI_PION_BONUS 3
#define LINIA7_BONUS 100
#define LINIA2_BONUS -100
//#define ROOK_SEMI_OPEN_FILE_BONUS = 10;
//#define ROOK_OPEN_FILE_BONUS = 15;
//#define ROOK_ON_SEVENTH_BONUS = 20;

    /* The "pcsq" arrays are piece/square tables. They're values
    added to the material value of the piece based on the
    location of the piece. */
    
	enum czesc_gry  {DEBIUT, SRODKOWA, KONCOWKA};
	czesc_gry aktualna_czesc_gry = DEBIUT;

	czesc_gry czy_koncowka() {
		int suma_pelna = 2 * 100 + 4 * 50 + 8 * 10 + 16 * 2;
		int suma = 0;
		for (int i = 0; i < 64; i ++) {
			if (plansza[i] == queen) 
				suma += 100;
			if (plansza[i] == rook) 
				suma += 50;
			if (plansza[i] == bishop || plansza[i] == knight) 
				suma += 10;
			if (plansza[i] == pawn || plansza[i] == bpawn) 
				suma += 2;
		}
		if (suma > suma_pelna - 2 * 10 - 4 * 2)
			return DEBIUT;
		if (suma <= 100)
			return KONCOWKA;
		return SRODKOWA;
	}

   short knight_bonus[] = {
            -10, -15, -10, -10, -10, -10, -15, -10,
            -10,   0,   0,   0,   0,   0,   0, -10,
            -10,   0,   5,   5,   5,   5,   0, -10,
            -10,   0,   5,  10,  10,   5,   0, -10,
            -10,   0,   5,  10,  10,   5,   0, -10,
            -10,   0,   5,   5,   5,   5,   0, -10,
            -10,   0,   0,   0,   0,   0,   0, -10,
            -10, -15, -10, -10, -10, -10, -15, -10
    };
    
    short bishop_bonus[] = {
            -10, -10, -10, -10, -10, -10, -10, -10,
            -10,   7,   0,   0,   0,   0,   7, -10,
            -10,   0,   5,   5,   5,   5,   0, -10,
            -10,   0,   5,  10,  10,   5,   0, -10,
            -10,   0,   5,  10,  10,   5,   0, -10,
            -10,   0,   5,   5,   5,   5,   0, -10,
            -10,   7,   0,   0,   0,   0,   7, -10,
            -10, -10, -20, -10, -10, -20, -10, -10
    };

   short king_end_bonus[] = {
             0,  10,  20,  30,  30,  20,  10,   0,
            10,  20,  30,  40,  40,  30,  20,  10,
            20,  30,  40,  50,  50,  40,  30,  20,
            30,  40,  50,  60,  60,  50,  40,  30,
            30,  40,  50,  60,  60,  50,  40,  30,
            20,  30,  40,  50,  50,  40,  30,  20,
            10,  20,  30,  40,  40,  30,  20,  10,
             0,  10,  20,  30,  30,  20,  10,   0
    };

    short king_start_bonus[] = {
            0,   0,   0,   0,   0,   0,   0,   0,
            5,  10,  15,  20,  20,  15,  10,   5,
            4,   8,  12,  16,  16,  12,   8,   4,
            3,   6,   9,  12,  12,   9,   6,   3,
            2,   4,   6,   8,   8,   6,   4,   2,
            1,   2,   3, -10, -10,   3,   2,   1,
            0,   0,   0, -40, -40,   0,   0,   0,
            0,   0,   0,   0,   0,   0,   0,   0
    };
    
    short white_king_start_bonus[] = {
            -40, -40, -40, -40, -40, -40, -40, -40,
            -40, -40, -40, -40, -40, -40, -40, -40,
            -40, -40, -40, -40, -40, -40, -40, -40,
            -40, -40, -40, -40, -40, -40, -40, -40,
            -40, -40, -40, -40, -40, -40, -40, -40,
            -40, -40, -40, -40, -40, -40, -40, -40,
            -20, -20, -20, -20, -20, -20, -20, -20,
              0,  20,  40, -20,   0, -20,  40,  20
    };

	short black_king_start_bonus[] = {
		     0,-20,-40, 20,  0, 20,-40,-20,
     		20, 20, 20, 20, 20, 20, 20, 20,
            40, 40, 40, 40, 40, 40, 40, 40,
            40, 40, 40, 40, 40, 40, 40, 40,
            40, 40, 40, 40, 40, 40, 40, 40,
            40, 40, 40, 40, 40, 40, 40, 40,
            40, 40, 40, 40, 40, 40, 40, 40,
            40, 40, 40, 40, 40, 40, 40, 40
    };
    
//int max_glebokosc = 1;
int _w64 max_wezel = 0;
struct ruch najlepszy_ruch;

inline short punkty_za_szach() {
	if (czy_pole_atakowane(king_position[white], black))
		return -5;
	if (czy_pole_atakowane(king_position[black], white))
		return 5;
	return 0;
}

inline short premia_za_centrum(int i) {
	if (i >= 18 && i <= 45 && i % 8 >= 2 && i % 8 <= 5) 
		return ((i >= 26 && i <= 37) ? 20 : 10);
	else
		return 0;
}

int ile_ocen_pozycji = 0;
int ile_celnych_odczytow_hashu = 0;

#define czy_biale_pole(a) (kolor[a] == white)

inline short ocen_pozycje() { // prosta funkcja oceniajaca
	ile_ocen_pozycji ++;
	struct hash_element * h_element = daj_wartosc_z_tablicy_hasujacej();
	if (h_element) {
		ile_celnych_odczytow_hashu++;
		return h_element->ocena_pozycji;
	}

	register short suma = 0;
	register char figura;
	register char kolor_figury;
	register int index = 0;
	register char dokad = 0;
	register short mnoznik;
	
	for (register int i = 0; i < 64; i ++) { // zsumowanie wartosci figur
		if (plansza[i] != empty_square) {
			figura = plansza[i];
			kolor_figury = kolor[i];
			mnoznik = kolor_figury ? -1 : 1; // dla czarnych ujemny dla bialych dodatni

			suma += (wartosci_figur[figura] + premia_za_centrum(i)) * mnoznik; // obliczenie materialu

			if (figura == pawn) { //bialy pionek
				if (czy_7linia(i)) {
					suma += LINIA7_BONUS;
				}
				else if (plansza[i + 9] == pawn || plansza[i + 7] == pawn || plansza[i - 1] == pawn) {
					suma += SASIEDNI_PION_BONUS;
				}
				else if (plansza[i + 8] == pawn) 
					suma += KARA_ZA_DUBLOWANIE;
			}
			else if (figura == bpawn) { // czarny pionek
				if (czy_2linia(i)) {
					suma += LINIA2_BONUS;
				}
				else if (plansza[i - 9] == bpawn || plansza[i - 7] == bpawn || plansza[i - 1] == bpawn) {
					suma -= SASIEDNI_PION_BONUS;
				}
				else if (plansza[i - 8] == bpawn) 
					suma -= KARA_ZA_DUBLOWANIE;
			}
			else if (figura == rook) {
				suma -= 2 * odleglosc[i][king_position[kolor_figury ^ 1]] * mnoznik;
				if (kolor_figury) 
					suma += (czy_7linia(i) * 20);
				else
					suma -= (czy_2linia(i) * 20);

			}
			else if (figura == knight) { //skok
				suma += knight_bonus[i] * mnoznik;
				//while ((dokad = mozliwe_ruchy[figura][i][index]) != -1) { // 
				//	if (kolor[dokad] != kto_rusza) { // jak tam cokolwiek stoi
				//		dodaj_ruch(pole, dokad, plansza[dokad] == empty_square ? 0 : plansza[dokad], flagi_z_poprzedniego_ruchu); // jest bicie
				//	}
				//	index++;
				//}
				suma -= 4 * odleglosc[i][king_position[kolor_figury ^ 1]] * mnoznik; // im wieksza odleglosc tym wieksza kara;
				if (aktualna_czesc_gry == DEBIUT) {
					if (i == 1 || i == 6 || i == 57 || i == 62) // kara za trzymanie skokow
						suma -= mnoznik * 30;
				}
				
			}
			else if (figura == bishop) {// goniec
				suma += bishop_bonus[i] * mnoznik;
				suma -= 2 * odleglosc[i][king_position[kolor_figury ^ 1]] * mnoznik; // im wieksza odleglosc tym wieksza kara;
			}
			else if (figura == queen) {
				if (aktualna_czesc_gry == DEBIUT) {
					suma += 3 * (odleglosc[i][27] + odleglosc[i][36]) * mnoznik; //premia za trzymanie hetmana na wodzy
				} 
				suma -= 3 * odleglosc[i][king_position[kolor_figury ^ 1]] * mnoznik; // im wieksza odleglosc tym wieksza kara;
			}
			else if (figura == king) {
				if (aktualna_czesc_gry == KONCOWKA) 
					suma += king_end_bonus[i] * mnoznik;
				else { // jesli nie koncowka to krol w bezpiecznym miejscu
					if (kolor_figury == white) {
						suma += white_king_start_bonus[i];
						if (i > 60) { // po roszadzie krotkiej // stopien ochrony krola pionami
							suma += (15 * (czy_biale_pole(53) + czy_biale_pole(54) + czy_biale_pole(55)) + 5 * (czy_biale_pole(46) + czy_biale_pole(47)));
						} 
						else if (i < 59) { // po dlugiej
							suma += (15 * (czy_biale_pole(48) + czy_biale_pole(49) + czy_biale_pole(50)) + 5 * (czy_biale_pole(40) + czy_biale_pole(41)));
						}
						
					}
					else { // stopien ochrony krola czarnego pionami
						suma += black_king_start_bonus[i]; 
						if (i > 4) { // po roszadzie krotkiej // stopien ochrony krola pionami
							suma -= (15 * (czy_biale_pole(13) + czy_biale_pole(14) + czy_biale_pole(15)) + 5 * (czy_biale_pole(22) + czy_biale_pole(23)));
						} 
						else if (i < 3) { // po dlugiej
							suma -= (15 * (czy_biale_pole(8) + czy_biale_pole(9) + czy_biale_pole(10)) + 5 * (czy_biale_pole(16) + czy_biale_pole(17)));
						}
					}
				}				
			}
		}
	}
	//suma += punkty_za_szach();
	hashuj_aktualna_pozycje(suma, 0);
	return suma;
}

//inline short przedocena_pozycji() { // prosta funkcja oceniajaca
//	short suma = 0;
//	for (int i = 0; i < 64; i ++) { // zsumowanie wartosci figur
//		if (plansza[i] != empty_square)
//			suma += wartosci_figur[plansza[i]] * (kolor[i] ? -1 : 1);
//	}
//	return suma + punkty_za_szach();
//}

bool interacyjne_poglebianie = false;

short alfabeta(struct ruch * po_ruchu, char kto_gra, int glebokosc, short a, short b, struct ruch * kon) {
	if (!glebokosc) {
		//struct hash_element * h_element = daj_wartosc_z_tablicy_hasujacej();
		//if (h_element) {
		//	ile_celnych_odczytow_hashu++;
		//	return h_element->ocena_pozycji;
		//}
		//else
		return ocen_pozycje();
	}
	wezel = kon + 1;

	int ile_ruchow_mozliwych = wypelnij_tablice_ruchow(kto_gra, po_ruchu); // zlicza ruchy mozliwe do wykonania
	if (!ile_ruchow_mozliwych) {

		//drukuj_plansze();
		if (czy_pole_atakowane(king_position[kto_gra], przeciwnik(kto_gra))) {
			//cout << MAX_SHORT * (2 * kto_gra - 1) << "\n";
			//system("PAUSE");
			return MAX_SHORT * (2 * kto_gra - 1);

		} else {
			//cout << 0 << "\n";
			//system("PAUSE");
			return 0;
		}
	}
	//cout << (int)kto_gra << " ma do wykonania " << ile_ruchow_mozliwych << " ruchow\n"; 
	struct ruch * koniec_tablicy = wezel;

	
	//*************************************************************
	if (glebokosc > 1) {
		// przedocena pozycji .. iteracyjne poglebianie
		for (struct ruch * index = kon + 1; index < koniec_tablicy; index++) {
			rusz(index);
			index->score = ocen_pozycje(); //alfabeta(index, przeciwnik(kto_gra), GLEBOKOSC_INTERACYJNEGO_POGLEBIANIA, a, b, wezel - 1);
			cofnij_ruch(index);
		}
		// koniec przedoceny

		if (kto_gra) // SORTOWANIE RUCHOW
			sort(kon + 1, koniec_tablicy, Cmp_rosnaco());
		else
			sort(kon + 1, koniec_tablicy, Cmp_malejaco());
		//*************************************************************
	}

	//struct ruch * new_koniec_tablicy = (koniec_tablicy - kon < 7 || interacyjne_poglebianie ? koniec_tablicy : kon + 5);
	if (kto_gra == black) {		
		for (struct ruch * index = kon + 1; index < koniec_tablicy; index++) {
			//drukuj_plansze();
			rusz(index);
			//drukuj_plansze();
			//system("pause");
			short lAlfabeta1 = alfabeta(index, przeciwnik(kto_gra), glebokosc - 1, a, b, wezel - 1);
			b = min(b, lAlfabeta1);
			cofnij_ruch(index);
			if (a >= b) {
//				cout << "ciecie\n";
				wezel = kon + 1;
				return a;
			}
		}
		wezel = kon + 1;
		return b;
	}
	else {
		for (struct ruch * index = kon + 1; index < koniec_tablicy; index++) {
			//drukuj_plansze();
			rusz(index);
			//drukuj_plansze();
			//system("pause");
			short lAlfabeta = alfabeta(index, przeciwnik(kto_gra), glebokosc - 1, a, b, wezel - 1);
			a = max(a, lAlfabeta);
			cofnij_ruch(index);
			if (a >= b) { //uwaga!!!!
//				cout << "ciecie\n";
				wezel = kon + 1;
				return b;
			}
		}
		wezel = kon + 1;
		return a;
	}
}


// UWAGA ta funkcja wola sie tylko raz a juz przeprowadza rekurencyjna alafabete i zwraca najlepszy ruch w danej pozycji
struct ruch daj_ruch(char kto_gra, int glebokosc, short a, short b, struct ruch * kon)  {
	aktualna_czesc_gry = czy_koncowka();
	cout << "aktualna czesc gry: " << aktualna_czesc_gry << "\n";
	wezel = &ruchy[1];
	int ile_ruchow_mozliwych = wypelnij_tablice_ruchow(kto_gra, ostatni_ruch); // zlicza ruchy mozliwe do wykonania
	if (ile_ruchow_mozliwych <= 0) {
		cerr << "ERROR!!\n";
		return ruchy[0];
	}
	najlepszy_ruch = ruchy[1]; // najlepszy na poczatku musi byc piewszy

	short local_b = b, local_a = a;
	cout << "Wywolanie fukcji daj_ruch: " << (int)kto_gra << " ma do wykonania " << ile_ruchow_mozliwych << " ruchow\n"; 
	struct ruch * koniec_tablicy = wezel;


	//*************************************************************
	// przedocena pozycji .. iteracyjne poglebianie
	interacyjne_poglebianie = true;
	for (int glebokosc_iteracyjna = 2; glebokosc_iteracyjna < GLEBOKOSC_ITERACYJNEGO_POGLEBIANIA; glebokosc_iteracyjna++, glebokosc_iteracyjna++) {
		cout << "iteracyjne poglebianie " << glebokosc_iteracyjna << "\n";
		for (struct ruch * index = kon + 1; index < koniec_tablicy; index++) {
			rusz(index);
			index->score = alfabeta(index, przeciwnik(kto_gra), glebokosc_iteracyjna, a, b, wezel - 1);
			cofnij_ruch(index);
		}
		// koniec przedoceny
		interacyjne_poglebianie = false;

		if (kto_gra) // SORTOWANIE RUCHOW
			sort(&ruchy[1], koniec_tablicy, Cmp_rosnaco());
		else
			sort(&ruchy[1], koniec_tablicy, Cmp_malejaco());
		//*************************************************************
	}

	wezel = koniec_tablicy; // odtworzenie wezla
	if (kto_gra == black) {	
		for (struct ruch * index = kon + 1; index < koniec_tablicy; index++) {
			//drukuj_plansze();
			rusz(index);
			informacje_o_ruchu(index);
			//drukuj_plansze();
			//system("pause");
			short lAlfabeta = alfabeta(index, przeciwnik(kto_gra), glebokosc - 1, a, b, wezel - 1);
			cout << "ocena" << lAlfabeta << "\n";

			local_b = min(b, lAlfabeta);
			if (local_b < b) {
				najlepszy_ruch = *index;
				//informacje_o_ruchu(index);
			}
			b = local_b;
			cofnij_ruch(index);
		}
		//return b;
	}
	else {
		for (struct ruch * index = kon + 1; index < koniec_tablicy; index++) {
			//drukuj_plansze();
			rusz(index);
			//drukuj_plansze();
			//system("pause");
			informacje_o_ruchu(index, "ruchy bialego");
			short lAlfabeta1 = alfabeta(index, przeciwnik(kto_gra), glebokosc - 1, a, b, wezel - 1);
			cout << "ocena" << lAlfabeta1 << "\n";
			local_a = max(a, lAlfabeta1);
			if (local_a > a) {
				najlepszy_ruch = *index;
				//informacje_o_ruchu(index);
			}
			a = local_a;
			cofnij_ruch(index);
		}
		//return a;
	}
	return najlepszy_ruch;
}
