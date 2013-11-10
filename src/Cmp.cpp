#include "Cmp.h"
//#include "debiuty.h"

bool Cmp_rosnaco::operator()(const ruch &a, const ruch &b) {
	return a.score < b.score;
}

bool Cmp_malejaco::operator()(const ruch &a, const ruch &b) {
	return a.score > b.score;
}


//bool DebiutyCmp::operator()(const struct debiut& l, const struct debiut& r)  {
//	if (l.posuniecia.length() != r.posuniecia.length()) 
//		return (l.posuniecia.length() < r.posuniecia.length());
//	else
//		return (l.posuniecia < r.posuniecia); 
//}

//bool DebiutyCmp::operator()(const string& l, const string& r)  {
//	/*if (l.posuniecia.length() != r.posuniecia.length()) 
//		return (l.posuniecia.length() < r.posuniecia.length());
//	else
//		return (l.posuniecia < r.posuniecia); */
//	return true;
//}