#pragma once

#include "ruchy.h"

class Cmp_rosnaco {
public:
	bool operator()(const ruch &a, const ruch &b);
};


class Cmp_malejaco {
public:
	bool operator()(const ruch &a, const ruch &b);
};

//class DebiutyCmp {
//public:
//	bool operator()(const struct debiut& l, const struct debiut& r);
//};
//class DebiutyCmp {
//public:
//	bool operator()(const string& l, const string& r);
//};