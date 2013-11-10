#include "zasady.h"
#include "ruchy.h"
#include "ai.h"
#include "hash.h"
#include "debug.h"
#include "debiuty.h"
#include <iostream>

using namespace std;

void inicjalizacja() {
	inicjalizuj_hash_maski();
	debiuty_set.clear();
	uzupelnij_odleglosci();
	uzupelnij_tablice_ruchow();
	inicjalizcja_pierwszego_zaslepkowego_ruchu();
}


int main()
{
/*	inicjalizuj_hash_maski();
	cout << "\n" << RAND_MAX <<  endl;
	cout << daj_hash_pozycji() << endl; 
	plansza[0] = knight;
	cout << daj_hash_pozycji() << endl;
	system("PAUSE");
	return 0;*/
	inicjalizacja();	
	//wczytaj_debiuty_z_pliku("debiuty.txt");
	//wczytaj_debiuty_z_pliku("debiuty.txt");
	//wczytaj_debiuty_z_pliku("debiuty2.txt");
	//wczytaj_debiuty_z_pliku("debiuty2.txt");
	//sort_debug();
	test_alfabeta();
	system("PAUSE");
	return 0;
//	if (true)
//		return 0;
	//char x = 0;
	//x |= castle_kingside;
	//x ^= castle_queenside;
	//wypisz_bitowo(x, 8);
	//system("pause");
	//return 0;
	
	//for (int i = 0; i < 64; i ++) {
	//	cout << (int)plansza[i] << " "; 
	//	if (i % 8 == 7)
	//		cout << endl;
	//}


	//cout << endl;

	//for (int i = 0; i < 64; i ++) {
	//	cout << (int)kolor[i] << " "; 
	//	if (i % 8 == 7)
	//		cout << endl;
	//}

	////sort_debug();

	//system("pause");

	//rusz_notacyjnie("e2", "e4");
	//rusz_notacyjnie("d7", "d5");
	//rusz_notacyjnie("h7", "h5");
	////rusz_notacyjnie("e8", "e5");
	//rusz_notacyjnie("a1", "a5");
	//king_position[0] = notacja_do_liczby("a8");
	//wypelnij_tablice_ruchow(black);
	//int licznik_ruchow = 0;
	//struct ruch * ostatni_wezel = wezel;
	//wezel = &ruchy[1];

	//while (wezel < ostatni_wezel) {
	//	system("PAUSE");
	//	licznik_ruchow++;
	//	rusz(wezel);
	//	drukuj_plansze();
	//	cout << ocen_pozycje() << "\n";
	//	cofnij_ruch(wezel);
	//	wezel++;
	//	cout << " ============================ \n";
	//}
	//cout << licznik_ruchow;
	//system("PAUSE");
	///*
	//for (int i = 0; i < 64; i ++) {
	//	cout << (int)plansza[i] << " "; 
	//	if (i % 8 == 7)
	//		cout << endl;
	//}

//	cout << endl;
//
//	for (int i = 0; i < 64; i ++) {
//		cout << (int)kolor[i] << " "; 
//		if (i % 8 == 7)
//			cout << endl;
//	}
//
//	system("PAUSE");
//
//*/
//	return 0;
}

