#include "debiuty.h"
#include "debug.h"
#include <vector>
#include <ctype.h>

char plansza_poczatkowa[64] = 
	{rook,	knight,	bishop,	queen,	king,	bishop,	knight,	rook, 
	 bpawn,	bpawn,	bpawn,	bpawn,	bpawn,	bpawn,	bpawn,	bpawn,
	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
 	 pawn,	pawn,	pawn,	pawn,	pawn,	pawn,	pawn,	pawn,
	 rook,	knight,	bishop,	queen,	king,	bishop,	knight,	rook
};

char kolor_poczatkowa[64] = 
	{black, black, black, black, black, black, black, black, 
	 black, black, black, black, black, black, black, black, 
	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
  	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
	 empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	empty_square,	
 	 white, white,  white, white,  white, white,  white, white,  
	 white, white,  white, white,  white, white,  white, white
};

char king_position_poczatkowa[2] = {60, 4};

void odtworz_pozycje_poczatkowa() {
	for (int i = 0; i < 64; i++) {
		plansza[i] = plansza_poczatkowa[i];
		kolor[i] = kolor_poczatkowa[i];
	}
	king_position[0] = king_position_poczatkowa[0];
	king_position[1] = king_position_poczatkowa[1];
	inicjalizcja_pierwszego_zaslepkowego_ruchu();
}

DEBSET debiuty_set;

int obciete = 0;

struct debiut stworz_debiut(string posuniecia, string nazwa_) {
	//cout << posuniecia << "\n";
    string buf; 
    stringstream ss(posuniecia);
    vector<string> tokens;
	
	struct debiut ret_deb;
	ret_deb.nazwa = nazwa_;
	ret_deb.posuniecia = "";

	while (ss >> buf) {
        tokens.push_back(buf);	
		//cout << buf << "\n";
	}
	//cout << "\n";

	char kto_rusza = white;
	int mnoznik = kto_rusza ? -1 : 1;
	odtworz_pozycje_poczatkowa();
	
	for (int i = 0; i < (int)tokens.size(); i++) {
		char skad;
		char skad1 = 64;
		char dokad;
		char figura_;
		//cout << "przerabiam " << tokens[i] << "\n";
		
		if (tokens[i].length() == 2 && isdigit(tokens[i][1])) { // ruch pionem
			dokad = notacja_do_liczby(tokens[i]);
			if (plansza[dokad + 8 * mnoznik] == pawn + kto_rusza) 
				skad = dokad + 8 * mnoznik;
			else if (plansza[dokad + 16 * mnoznik] == pawn + kto_rusza)
				skad = dokad + 16 * mnoznik;
			else {
				//cerr << "nieudany ruch pionem DUPA1!!";
				obciete++;
				break;
				system("Pause");
			}
		} 
		else if (tokens[i].length() == 2 && isalpha(tokens[i][1])) {
			skad = dokad = 64;
			char kolumna_ =  tokens[i][0];
			char kolumna_2 =  tokens[i][1];
			for (int i = kolumna_ - 'a'; i < 64; i += 8) {
				for (int j = kolumna_2 - 'a'; j < 64; j += 8) {
					if (plansza[i] == pawn + kto_rusza && kolor[i] == kto_rusza /*&& plansza[j] == pawn + (kto_rusza ^ 1)*/ && kolor[j] == (kto_rusza ^ 1)) {
						//cout << "found";
						if (kolumna_ < kolumna_2 && kto_rusza == white && i == j + 7 ||
							kolumna_ > kolumna_2 && kto_rusza == white && i == j + 9 ||
							kolumna_ < kolumna_2 && kto_rusza == black && j == i + 9 ||
							kolumna_ > kolumna_2 && kto_rusza == black && j == i + 7) {
						
								skad = (char)i;
								dokad = (char)j;

						}
					}
				}
			}
			if (skad > 63 || skad < 0 || dokad > 63 || dokad < 0) {
				//drukuj_plansze();
				obciete++;
				//cerr << "nieudane bicie piona BLAD!!\n";
				break;
				system("PAUSE");
			}	
		}	
		
		else if (tokens[i].length() == 3 && islower(tokens[i][0]) && islower(tokens[i][1]) && isalpha(tokens[i][0]) && isalpha(tokens[i][1]) && isdigit(tokens[i][2])) {
			skad = dokad = 64;
			char kolumna_ =  tokens[i][0];
			char kolumna_2 =  tokens[i][1];
			int bicie_na_linii = tokens[i][2] - '1' + 1;
			for (int i = kolumna_ - 'a'; i < 64; i += 8) {
				for (int j = kolumna_2 - 'a'; j < 64; j += 8) {
					if (plansza[i] == pawn + kto_rusza && kolor[i] == kto_rusza && kolor[j] == (kto_rusza ^ 1) && j >= 64 - (8 * bicie_na_linii) && j <  64 - (8 * (bicie_na_linii - 1))) {
						//cout << "found";
						if (kolumna_ < kolumna_2 && kto_rusza == white && i == j + 7 ||
							kolumna_ > kolumna_2 && kto_rusza == white && i == j + 9 ||
							kolumna_ < kolumna_2 && kto_rusza == black && j == i + 9 ||
							kolumna_ > kolumna_2 && kto_rusza == black && j == i + 7) {
						
								skad = (char)i;
								dokad = (char)j;

						}
					}
				}
			}
			if (skad > 63 || skad < 0 || dokad > 63 || dokad < 0) {
				//cerr << "nieudane bicie piona BLAD2!!\n";
				obciete++;
				break;
				system("PAUSE");
			}	
		}	


		else if (tokens[i] == "o-o") {  // roszady
			skad = 60 - (kto_rusza * 56);
			dokad = 62 - (kto_rusza * 56);
		}
		else if (tokens[i] == "o-o-o") {
			skad = 60 - (kto_rusza * 56);
			dokad = 58 - (kto_rusza * 56);
		}
		else if (tokens[i].length() == 4 && islower(tokens[i][0])) {
			skad = notacja_do_liczby(tokens[i].substr(0, 2));
			dokad = notacja_do_liczby(tokens[i].substr(2));
		}
		else { //ruchy figur
			char kolumna_ = 'x';
			int linia_ = 0;

			switch (tokens[i][0]) {
				case 'N':
					figura_ = knight;
					break;
				case 'B':
					figura_ = bishop;
					break;
				case 'R':
					figura_ = rook;
					break;
				case 'Q':
					figura_ = queen;
					break;			
				case 'K':
					figura_ = king;
					break;
				default: 
					//cout << posuniecia;
					//drukuj_plansze();
					obciete++;
					//cerr << "nie ma figury! BLAD!!";
					goto obciecie_debiutu;
					break;
					system("Pause");		
			}
			
			if (tokens[i].length() == 3) {
				dokad = notacja_do_liczby(tokens[i].substr(1));
				skad = znajdzWKolumnie(figura_, kto_rusza, kolumna_);
			}
			else if (tokens[i].length() == 4) {
				dokad = notacja_do_liczby(tokens[i].substr(2));
				if (isdigit(tokens[i][1])) {
					linia_ = tokens[i][1] - '1' + 1;
					skad = znajdzWLinii(figura_, kto_rusza, linia_);
				}
				else {
					kolumna_ = tokens[i][1];
					skad = znajdzWLinii(figura_, kto_rusza, kolumna_);
				}
			}			
			
			skad1 = znajdzOdKonca(figura_, kto_rusza);
		}
		
		wezel = &ruchy[1];
		wypelnij_tablice_ruchow(kto_rusza, ostatni_ruch);

		struct ruch * ruch_z_debiutu = NULL;
		for (struct ruch * index = ruchy + 1; index < wezel; index++) { // poszukiwanie podanego wsrod mozliwych
			if (dokad == index->t && skad == index->f) {
				ruch_z_debiutu = index;
				break;
			}
		}

		if (!ruch_z_debiutu) {
			for (struct ruch * index = ruchy + 1; index < wezel; index++) { // poszukiwanie podanego wsrod mozliwych
				if (dokad == index->t && skad1 == index->f) {
					ruch_z_debiutu = index;
					break;
				}
			}
		}

		if (!ruch_z_debiutu) {
			//cerr << "nie znaleziono ruchu BLAD!!\n";
			obciete++;
			break;
			system("PAUSE");
		}
		
		rusz(ruch_z_debiutu);

		// tutaj jest plansza zmieniona, mozna ja zapamietac i zapisac do pliku.
		//drukuj_plansze();
		ret_deb.posuniecia += liczba_do_notacji(ruch_z_debiutu->f);
		ret_deb.posuniecia += liczba_do_notacji(ruch_z_debiutu->t);
		//cout << "aktualny ciag " << ret_deb.posuniecia << "\n";
		kto_rusza = kto_rusza ^ 1;
		mnoznik = -mnoznik;
	}

	//cout << "\n";
	//struct debiut ret_deb = {posuniecia, nazwa};
obciecie_debiutu:
	return ret_deb;
	//return {posuniecia, nazwa};
}

int wczytaj_debiuty_z_pliku(char * nazwaPlikuZDebiutami) {
	cout << "wczytuje debiuty z " << nazwaPlikuZDebiutami << "\n";
	ifstream plikZDebiutami(nazwaPlikuZDebiutami);
	if (!plikZDebiutami.is_open()) 
		cerr << "blad!";

	string linia = "";
	string posuniecia = "";
	string nazwa = "nic";
	while (!plikZDebiutami.eof()) {
		getline(plikZDebiutami, linia);
		if (linia[0] == '[' || linia[0] == '#') { // nowy debiut
			if (posuniecia.length() > 5) {
				struct debiut deb = stworz_debiut(posuniecia, nazwa);
				debiuty_set.insert(deb);
				//system("pause");
			}
			
			posuniecia = "";
			nazwa = linia;
		}
		else {
			posuniecia += linia + " ";
		}
	}
	cout << debiuty_set.size();

	for (DEBSET::iterator it = debiuty_set.begin(); it!=debiuty_set.end(); it++) {
		struct debiut d = (struct debiut)(*it);
		//cout << d.nazwa << "\n";
		//cout << d.posuniecia << "\n";
	}
	
	cout << " obcialem " << obciete << " debiutow z " << debiuty_set.size() << "\n";
	plikZDebiutami.close();
	odtworz_pozycje_poczatkowa();
	return 2;
	
}

void uaktualnij_debiuty(struct ruch * po_ruchu) {
	int usunietych = 0;
	DEBSET aux;
	aux.clear();
	string prefix = liczba_do_notacji(po_ruchu->f) + liczba_do_notacji(po_ruchu->t);
	for (DEBSET::iterator it = debiuty_set.begin(); it!=debiuty_set.end(); it++) {
		struct debiut d = (struct debiut)(*it);
		if (d.posuniecia.length() < 4 || d.posuniecia.substr(0, 4) != prefix) {
			//debiuty_set.erase(it);
			usunietych++;
		} 
		else {
			d.posuniecia = d.posuniecia.substr(4);
			if (d.posuniecia.length() >= 4)
				aux.insert(d);
		}
	}
	if (usunietych > 0)
		cout << "usunalem " << usunietych << " debiutow\n";
	debiuty_set.clear();
	debiuty_set = aux;
}

struct ruch * wez_z_debiutow(char kto_rusza) {
	DEBMULTISET debmultiset;
	debmultiset.clear();
	for (DEBSET::iterator it = debiuty_set.begin(); it!=debiuty_set.end(); it++) { // wypelnienie nastepnymi ruchami
		struct debiut d = (struct debiut)(*it);
		if (d.posuniecia.length() >= 4)
			debmultiset.insert(d.posuniecia.substr(0, 4));
		//cout << "Mozna ruszyc: " << d.posuniecia.substr(0, 4) << "\n";
	} 

	int najczestszy_krotnosc = 0;
	string najczestszy = "";
	for (DEBMULTISET::iterator it = debmultiset.begin(); it!=debmultiset.end(); it++) {
		string ruch = (string)(*it);
		if ((int)debmultiset.count(ruch) > najczestszy_krotnosc) {
			najczestszy = ruch;
			najczestszy_krotnosc = (int)debmultiset.count(ruch);
			cout << najczestszy << " " << najczestszy_krotnosc << "\n";
		}
	}
	if (najczestszy.length() < 4)
		return NULL;
	char f = notacja_do_liczby(najczestszy.substr(0, 2));
	char t = notacja_do_liczby(najczestszy.substr(2, 2));

	wezel = &ruchy[1];
	wypelnij_tablice_ruchow(kto_rusza, ostatni_ruch);

	for (struct ruch * index = ruchy + 1; index < wezel; index++) { // poszukiwanie podanego wsrod mozliwych
		//cout << liczba_do_notacji(index->f) << "->" << liczba_do_notacji(index->t) << "\n";
		if (t == index->t && f == index->f) {
			return index;	
		}
	}
	return NULL;
}