#include "ruchy.h"
#include "zasady.h"
#include <iostream>

struct ruch *ruchy_w_partii = new struct ruch[ILE_RUCHOW_W_PARTII];
struct ruch *ruchy = new struct ruch[ILE_RUCHOW_W_TABLICY];
char historia_gry[ILE_RUCHOW_W_PARTII][2*64];
//int pozycja_w_tablicy_ruchow = -1;
struct ruch *wezel = ruchy; // wezel wskazuje aktualny ruch
struct ruch *ostatni_ruch = ruchy_w_partii; 

//struct ruch * analizowany_ruch; // wezel wskazuje aktualny ruch

inline bool czy_pole_atakowane(register char pole, char gracz) {
	register char swoj_kolor = gracz ^ 1;
	//piony
	if (plansza[mozliwe_ruchy[swoj_kolor/*jest symbolem piona druzyny atakowanej*/][pole][0]] == gracz + 7 /*+7 daje symbol piona*/ || plansza[mozliwe_ruchy[swoj_kolor][pole][1]] == gracz + 7){ // moze byc -1 ale to nie szkodzi 
		return true;
	}
	// goniec hetman 
	register char pole_przesuwane = pole;
	register char index = 0;
	register char index_zmian = 0;
	while ((pole_przesuwane = mozliwe_ruchy[bishop][pole][index]) != -1) { 
		if (plansza[pole_przesuwane] != empty_square) {
			if (kolor[pole_przesuwane] == gracz && (plansza[pole_przesuwane] == bishop || plansza[pole_przesuwane] == queen)) 
				return true;
			if (zmiany_kierunkow[bishop][pole][index_zmian] != -1)  { // ewentualne przeskoczenie
				index = zmiany_kierunkow[bishop][pole][index_zmian];
				index_zmian++;
				continue;
			}
			// pole niepuste i nie jest to krolowka ani goniec przeciwnika i nie ma juz jak zmieniac kierunkow
			break;
		}
		// jak puste pole
		index++;
		if (zmiany_kierunkow[bishop][pole][index_zmian] == index)
			index_zmian++;
	}

	//return false;
	// wieza hetman 
	pole_przesuwane = pole;
	index = index_zmian = 0;
	while ((pole_przesuwane = mozliwe_ruchy[rook][pole][index]) != -1) { 
		if (plansza[pole_przesuwane] != empty_square) {
			if (kolor[pole_przesuwane] == gracz && (plansza[pole_przesuwane] == rook || plansza[pole_przesuwane] == queen)) 
				return true;
			if (zmiany_kierunkow[rook][pole][index_zmian] != -1) { // ewentualne przeskoczenie
				index = zmiany_kierunkow[rook][pole][index_zmian];
				index_zmian++;
				continue; // cos innego stoi na drodze, trzeba zmienic kierunek
			}
			// pole niepuste i nie jest to krolowka ani wieza
			break;
		}
		index++;
		if (zmiany_kierunkow[rook][pole][index_zmian] == index)
			index_zmian++;
	}

	//return false;
	// skok
	pole_przesuwane = pole; 
	index = 0;
	while ((pole_przesuwane = mozliwe_ruchy[knight][pole][index]) != -1) { 
		index++;
		if (plansza[pole_przesuwane] == knight && kolor[pole_przesuwane] == gracz) 
			return true;
	}

	// krol 
	//return false;
	if (odleglosc[king_position[gracz]][pole] == 1) 
		return true;

	return false;
}





void inicjalizcja_pierwszego_zaslepkowego_ruchu() {
	ostatni_ruch->co_zbite = ostatni_ruch->f = ostatni_ruch->t = 0;
	ostatni_ruch->score = 0;
	ostatni_ruch->flags = globalne_flagi; // wszystkie roszady dopuszczalne bez bicia w przelocie
	//analizowany_ruch = ostatni_ruch;
	ruchy[0] = *ostatni_ruch;
	wezel++; // wezel stoi na &ruchy[1]

}

void zapisz_pozycje_do_historii() {
	int index = (int)(ostatni_ruch - ruchy_w_partii);
	for (int i = 0; i < 64; i++) {
		historia_gry[index][i] = plansza[i];
	}
	for (int i = 64; i <  2 * 64; i++) {
		historia_gry[index][i] = kolor[i - 64];
	}
}


bool czy_pozycja_taka_sama(int index_w_historii) {
	for (int i = 0; i < 64; i++) {
		if (historia_gry[index_w_historii][i] != plansza[i]) 
			return false;
	}
	for (int i = 64; i <  2 * 64; i++) {
		if (historia_gry[index_w_historii][i] != kolor[i - 64])
			return false;
	}
	return true;
}

bool czy_pozycja_byla_juz_2_razy() {
	int ile_razy_byla = 0;

	for (int i = 0; i <= (ostatni_ruch - ruchy_w_partii); i++) {
		if (czy_pozycja_taka_sama(i))
			ile_razy_byla++;
	}
	return (ile_razy_byla > 1);
}


inline void rusz(struct ruch * r) {
	plansza[r->t] = plansza[r->f]; // przestawienie figury
	kolor[r->t] = kolor[r->f];
	plansza[r->f] = (kolor[r->f] = empty_square);
	if (plansza[r->t] == king) {
		king_position[kolor[r->t]] = r->t;
		if (r->flags & castling) { //roszada
			if (r->t > r->f) { // krotka
				plansza[61 - kolor[r->t] * 56] = rook;
				kolor[61 - kolor[r->t] * 56] = kolor[r->t];
				plansza[63 - kolor[r->t] * 56] = (kolor[63 - kolor[r->t] * 56] = empty_square);
			} else { //dluga
				plansza[59 - kolor[r->t] * 56] = rook;
				kolor[59 - kolor[r->t] * 56] = kolor[r->t];
				plansza[56 - kolor[r->t] * 56] = (kolor[56 - kolor[r->t] * 56] = empty_square);
			}
		}
		return;
	}
	if (r->flags & promotions) {
		if (r->flags & queen_promotion)
			plansza[r->t] = queen;
		else
			plansza[r->t] = knight;
		return;
	} 

	if (r->flags & bicie_w_przelocie) { // znikniecie zbitego pionka
		plansza[(bicie_w_przelocie & r->flags) >> 8] = (kolor[(bicie_w_przelocie & r->flags) >> 8] = empty_square);
		//r->score = (kolor[r->t] ? -pawn_val : pawn_val);
	}
}

inline void cofnij_ruch(struct ruch * r) {
	plansza[r->f] = plansza[r->t]; // odstawienie figury na miejsce
	kolor[r->f] = kolor[r->t];
	plansza[r->t] = r->co_zbite == 0 ? empty_square : r->co_zbite; // zbita figura wraca
	kolor[r->t] = r->co_zbite ? kolor[r->f] ^ 1 : empty_square; // kolor przeciwny do ruszajacego lub pusty
	if (plansza[r->f] == king) {
		king_position[kolor[r->f]] = r->f;
		if (r->flags & castling) { //roszada
			if (r->t > r->f) { // krotka
				plansza[61 - kolor[r->f] * 56] = empty_square;
				kolor[61 - kolor[r->f] * 56] = empty_square;
				plansza[63 - kolor[r->f] * 56] = rook;
				kolor[63 - kolor[r->f] * 56] = kolor[r->f];
			} else { //dluga
				plansza[59 - kolor[r->f] * 56] = empty_square;
				kolor[59 - kolor[r->f] * 56] = empty_square;
				plansza[56 - kolor[r->f] * 56] = rook;
				kolor[56 - kolor[r->f] * 56] = kolor[r->f];
			}
		}
		return;
	} 
	if (r->flags & promotions) { // cofniecie promocji znow na piona
		plansza[r->f] = pawn + kolor[r->f];
		return;
	}

	if (r->flags & bicie_w_przelocie) { // pion zbity w przelocie pojawia sie
		char pozycja = (bicie_w_przelocie & r->flags) >> 8;
		kolor[pozycja] = (pozycja > 31 ? white : black);
		plansza[pozycja] = pawn + kolor[pozycja];
	}
}

//inline void dodaj_ruch(char from, char to, char co_zbite_arg) {
//	wezel->f = from; wezel->t = to;
//	wezel->co_zbite = co_zbite_arg;
//	wezel->score = wartosci_figur[co_zbite_arg];
//	wezel->flags = analizowany_ruch->flags & BICIE_W_PRZELOCIE_MASKA_REV; // flagi sa przepisane z poprzedniego ruchu i skasowane bicie w przelocie
//	rusz(wezel);
//	if (!czy_pole_atakowane(king_position[kolor[to]], kolor[to] ^ 1)) {  // sprawdzenie, czy krol nie jest pod szachem po tym ruchu
//		cofnij_ruch(wezel);
//		wezel++; // przejscie do nastepnego ruchu
//	}
//	else {
//		cofnij_ruch(wezel);
//	}
//}

inline void dodaj_ruch(char from, char to, char co_zbite_arg, unsigned short flags) {

	wezel->f = from; wezel->t = to;

	//if (wezel->f == 60 && wezel->t == 58) {
	//	cout << "dluga!\n";
	//}

	//if (wezel->f == 60 && wezel->t == 58) {
	//	cout << "dluga!\n";
	//}

	wezel->co_zbite = co_zbite_arg;
	//wezel->score = wartosci_figur[co_zbite_arg];
	wezel->flags = flags;
	rusz(wezel);
	if (!czy_pole_atakowane(king_position[kolor[to]], kolor[to] ^ 1)) {  // sprawdzenie, czy krol nie jest pod szachem po tym ruchu
		cofnij_ruch(wezel);
		wezel++; // przejscie do nastepnego ruchu
	}
	else {
		cofnij_ruch(wezel);
	}
}

//inline dodaj_promocje(from, to) {
//	wezel->f = from; wezel->t = to;
//	//wezel->co_zbite = 0;
//	rusz(wezel);
//	if (!czy_pole_atakowane(king_position[kolor[to]], kolor[to] ^ 1)) {  // sprawdzenie, czy krol nie jest pod szachem po tym ruchu
//		cofnij_ruch(wezel);
//		
//	}
//	else {
//		cofnij_ruch(wezel);
//	}
//}


int wypelnij_tablice_ruchow(char kto_rusza, struct ruch * po_ruchu) {
	char pole, przeciwnik = kto_rusza ^ 1, figura;
	struct ruch * wezel_poczatkowy = wezel;
	register char dokad;
	register short index;
	short index_zmian;
	register unsigned short flagi_z_poprzedniego_ruchu = po_ruchu->flags & reset_flags; // promocje sa tylko w jednym ruchu
	for (pole = 0; pole < 64; pole++) {
		if (kolor[pole] == kto_rusza) { // znaleziono figure
			if (plansza[pole] == pawn + kto_rusza)  { // pion jednego z kolorow
				index = -1;
				while ((dokad = mozliwe_ruchy[pawn + kto_rusza][pole][++index]) != -1) { // pion do przodu
					if (kolor[dokad] == empty_square) { // pion wchodzi na puste pole
						if (dokad - pole == 8 || dokad - pole == -8)  { // jesli bialy lub czarny nie musi przeskakiwac
							if (czy_skrajna_linia(dokad)) {
								dodaj_ruch(pole, dokad, 0, flagi_z_poprzedniego_ruchu | queen_promotion);
								dodaj_ruch(pole, dokad, 0, flagi_z_poprzedniego_ruchu | knight_promotion);
							}
							else
								dodaj_ruch(pole, dokad, 0, flagi_z_poprzedniego_ruchu); 
						}
						else // trzeba sprawdzic, czy przypadkiem nie przeskakuje, tutaj rozwaza sie ruch o 2 pola
						{
							if (kolor[(dokad + pole) / 2] == empty_square) {
								dodaj_ruch(pole, dokad, 0, flagi_z_poprzedniego_ruchu); // zwykly ruch o 2
							}
						}
					}
				} 
				index = -1;
				while ((dokad = mozliwe_ruchy[pawna + kto_rusza][pole][++index]) != -1) { // pion bije
					if (kolor[dokad] == przeciwnik) { // na tym polu musi stac cos przeciwnika
						dodaj_ruch(pole, dokad, plansza[dokad], flagi_z_poprzedniego_ruchu);
					}
				}				
				if (czy_4linia(pole) && kto_rusza == black) { // bicia w przelocie czarnych
					//if (plansza[po_ruchu->t] == pawn)
					//	if	(po_ruchu->f - po_ruchu->t == 16) 
					//		if	(pole < 39 && po_ruchu->t == pole + 1 || pole > 32 && po_ruchu->t == pole - 1) {
					if (plansza[po_ruchu->t] == pawn && 
						(po_ruchu->f - po_ruchu->t == 16) && 
						(pole < 39 && po_ruchu->t == pole + 1 
						|| pole > 32 && po_ruchu->t == pole - 1)) {
						dodaj_ruch(pole, po_ruchu->f - 8, 0, flagi_z_poprzedniego_ruchu | ((unsigned short)((unsigned short)po_ruchu->t << 8)));
					}
				}
				else if (czy_5linia(pole) && kto_rusza == white) { // bicia w przelocie bialych
					if (plansza[po_ruchu->t] == bpawn && po_ruchu->t - po_ruchu->f == 16 && (pole < 31 && po_ruchu->t == pole + 1 || pole > 24 && po_ruchu->t == pole - 1)) {
						dodaj_ruch(pole, po_ruchu->f + 8, 0, flagi_z_poprzedniego_ruchu | ((unsigned short)((unsigned short)po_ruchu->t << 8)));
					}
				} // koniec oslugi piona
			} 
			else // inna figura niz pion
			{
				figura = plansza[pole];
				index = index_zmian = 0;

				if (figura == knight) { // skok, figury krotkiego zasiegu
					while ((dokad = mozliwe_ruchy[figura][pole][index]) != -1) { // 
						if (kolor[dokad] != kto_rusza) { // jak tam cokolwiek stoi
							dodaj_ruch(pole, dokad, plansza[dokad] == empty_square ? 0 : plansza[dokad], flagi_z_poprzedniego_ruchu); // jest bicie
						}
						index++;
					}
				}  // krol i skoczek maja taki sam kod PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP
				else if (figura == king) { // krol wymaga odzielnego potraktowania, figury krotkiego zasiegu
					while ((dokad = mozliwe_ruchy[figura][pole][index]) != -1) { // 
						if (kolor[dokad] != kto_rusza) { // jak tam cokolwiek stoi
							dodaj_ruch(pole, dokad, plansza[dokad] == empty_square ? 0 : plansza[dokad], flagi_z_poprzedniego_ruchu & (kto_rusza == white ? white_castles_R : black_castles_R)); // poprzedni ruch, z niego bierzemy flagi, zdjecie roszad
						}
						index++;
					}
					if (pole == 60 && kto_rusza == white) {
						if ((flagi_z_poprzedniego_ruchu & white_castle_kingside) &&
							plansza[61] == empty_square && 
							plansza[62] == empty_square && 
							plansza[63] == rook && 
							kolor[63] == white && 
							!czy_pole_atakowane(60, black) && 
							!czy_pole_atakowane(61, black) && 
							!czy_pole_atakowane(62, black)) {

								dodaj_ruch(60, 62, 0, flagi_z_poprzedniego_ruchu & white_castles_R | castling);
						}
						if ((flagi_z_poprzedniego_ruchu & white_castle_queenside) && //UUUUUUUUUUUUUUUUUUUUUUUUU
							plansza[59] == empty_square && 
							plansza[58] == empty_square && 
							plansza[57] == empty_square &&
							plansza[56] == rook && 
							kolor[56] == white && 
							!czy_pole_atakowane(60, black) && 
							!czy_pole_atakowane(59, black) && 
							!czy_pole_atakowane(58, black)) {

								dodaj_ruch(60, 58, 0, flagi_z_poprzedniego_ruchu & white_castles_R | castling);
						}
					} else if (pole == 4 && kto_rusza == black) {
						if ((flagi_z_poprzedniego_ruchu & black_castle_kingside) &&
							plansza[5] == empty_square && 
							plansza[6] == empty_square && 
							plansza[7] == rook && 
							kolor[7] == black && 
							!czy_pole_atakowane(4, white) && 
							!czy_pole_atakowane(5, white) && 
							!czy_pole_atakowane(6, white)) {

								dodaj_ruch(4, 6, 0, flagi_z_poprzedniego_ruchu & black_castles_R | castling);
						}
						if ((flagi_z_poprzedniego_ruchu & black_castle_queenside) && //UUUUUUUUUUUUUUUUUUUUUUUUU
							plansza[3] == empty_square && 
							plansza[2] == empty_square && 
							plansza[1] == empty_square && 
							plansza[0] == rook && 
							kolor[0] == black && 
							!czy_pole_atakowane(4, white) && 
							!czy_pole_atakowane(3, white) && 
							!czy_pole_atakowane(2, white)) {

								dodaj_ruch(4, 2, 0, flagi_z_poprzedniego_ruchu & black_castles_R | castling);
						}
					}

				}
				else { // figury dlugiego zasiegu
					unsigned short flagi_figury_dlugiego_zasiegu = flagi_z_poprzedniego_ruchu;
					while ((dokad = mozliwe_ruchy[figura][pole][index]) != -1) { // 
						if (figura == rook) { // strata roszady przez ruch wieza
							switch (pole) {
								case 0: 
									flagi_figury_dlugiego_zasiegu &= black_castle_queenside_R;
									break;
								case 7: 
									flagi_figury_dlugiego_zasiegu &= black_castle_kingside_R;
									break;
								case 56: 
									flagi_figury_dlugiego_zasiegu &= white_castle_queenside_R;
									break;
								case 63: 
									flagi_figury_dlugiego_zasiegu &= white_castle_kingside_R;
									break;
							}
						}
						//wieza zmienia strone
						if (kolor[dokad] != empty_square) { // jak tam cokolwiek stoi
							if (kolor[dokad] == przeciwnik) {// jak to przeciwnik to bicie
								dodaj_ruch(pole, dokad, plansza[dokad], flagi_figury_dlugiego_zasiegu); // jest bicie
							}
							// jak nie to nic nie robimy, tylko zmiana kierunku
							if (zmiany_kierunkow[figura][pole][index_zmian] != -1) { // ewentualne przeskoczenie
								index = zmiany_kierunkow[figura][pole][index_zmian];
								index_zmian++;
								continue;
							}							
							break;
						} 
						else { // jak nie bylo, to dodajemy ruch w tym kierunku
							dodaj_ruch(pole, dokad, 0, flagi_figury_dlugiego_zasiegu); // nie ma bicia
						}
						if (zmiany_kierunkow[figura][pole][index_zmian] == index + 1) { // czy trzeba zaktualizowac index zmian
							index_zmian++; // poprawienie zmiany kierunku
						}
						index++;
					} // while
				}
			}
		} // znaleziona figura
	} // iteracja po szachownicy
	return (int)(wezel - wezel_poczatkowy);
}

inline void rusz(char skad, char dokad) {
	plansza[dokad] = plansza[skad];
	kolor[dokad] = kolor[skad];
	plansza[skad] = (kolor[skad] = empty_square);
}

void rusz_notacyjnie(string skad, string dokad) {
	rusz(notacja_do_liczby(skad), notacja_do_liczby(dokad));
}

bool czy_koniec_gry() {
	return false;
}
