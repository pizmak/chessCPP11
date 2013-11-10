#pragma once

#include "zasady.h"
#include "ruchy.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>
//#include "Cmp.h"

//istringstream sstream

using namespace std;

struct debiut {
	string posuniecia;
	string nazwa;
};

class DebiutyCmp {
public:
	bool operator()(const struct debiut& l, const struct debiut& r) {
		if (l.posuniecia.length() != r.posuniecia.length()) 
			return (l.posuniecia.length() > r.posuniecia.length());
		else
			return (l.posuniecia > r.posuniecia); 
	}
};

class StringCmp {
public:
	bool operator()(const string& l, const string& r) const {
		return (l > r); 
	}
};

//set<struct debiut, DebiutyCmp> debiuty_set;
typedef set<struct debiut, DebiutyCmp> DEBSET;
extern DEBSET debiuty_set;

typedef multiset<string, StringCmp> DEBMULTISET;

int wczytaj_debiuty_z_pliku(char * nazwaPlikuZDebiutami);

struct ruch * wez_z_debiutow(char kto_rusza);
void uaktualnij_debiuty(struct ruch * po_ruchu);