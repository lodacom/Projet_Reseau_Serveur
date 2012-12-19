/*
 * Serveur.h
 *
 *  Created on: 7 déc. 2012
 *      Author: Lolo
 */

#ifndef SERVEUR_H_
#define SERVEUR_H_

#include "sock.h"
#include "sockdist.h"
#include "AnalyseurTrame.h"
#include <vector>
#include <string>
using namespace std;

class Serveur {
public:
	Serveur();
	~Serveur();
    string Analyse(string);
    void Ecoute();
    void AccueilEmploye();
    bool EstDansListeEtablitControleur(string);
    int ChercheDansListeEtablitControleur(string);
    int ChercheDansListeEnCours(string);
	string LancementServeur(string);
    string ActionServeur(string);
    void RedigeRapport();
public:
	int destLocal;
	sockaddr_in* adresseExp;
	socklen_t taille;
	char envoi[100];
    struct thread_data
    {
        int mondesc;//le descripteur que l'on attribut à un client
        vector<int> descs_cli;//vecteur de tous les descripteurs des clients
    };
    vector<thread_data> liste_thread;
    vector<int> descripteur_cli;
    pthread_t threads[100];
    vector<string> liste_etablit_controleur;
    vector<string> liste_rapport_fait;
    vector<string> liste_rapport_encours;
};

#endif /* SERVEUR_H_ */
