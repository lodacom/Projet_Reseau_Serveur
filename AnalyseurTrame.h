/*
 * AnalyseurTrame.h
 *
 *  Created on: 13 déc. 2012
 *      Author: Lolo
 */

#ifndef ANALYSEURTRAME_H_
#define ANALYSEURTRAME_H_
#include <string>
using namespace std;

class AnalyseurTrame {
public:
	AnalyseurTrame();
	~AnalyseurTrame();
	string Analyse(string);
public:
	char recu[1000];
	int tp;
};

#endif /* ANALYSEURTRAME_H_ */
