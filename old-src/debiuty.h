#pragma once

#include "zasady.h"
#include "ruchy.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>

using namespace std;

struct debiut {
    string posuniecia;
    string nazwa;
};

class DebiutyCmp {
public:
    bool operator()(const debiut& l, const debiut& r) {
        if (l.posuniecia.length() != r.posuniecia.length()) {
            return l.posuniecia.length() > r.posuniecia.length();
        } else {
            return l.posuniecia > r.posuniecia;
        }
    }
};

//set<struct debiut, DebiutyCmp> debiuty_set;
typedef set<debiut, DebiutyCmp> DEBSET;
extern DEBSET debiuty_set;

typedef multiset<string, std::greater<string> > DEBMULTISET;

int wczytaj_debiuty_z_pliku(char * nazwaPlikuZDebiutami);

ruch * wez_z_debiutow(Color kto_rusza);
void uaktualnij_debiuty(ruch * po_ruchu);
