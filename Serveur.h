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
#include "ListeEmploye.h"
#include "AnalyseurTrame.h"
#include <vector>
using namespace std;

class Serveur {
public:
	Serveur();
	~Serveur();
	bool Ecoute();
    bool AccueilClients();
	void LancementServeur(string);
    void ActionServeur(string);
public:
	int destLocal;
	sockaddr_in* adresseExp;
	socklen_t taille;
	char envoi[1000];
    struct thread_data
    {
        int mondesc;//le descripteur que l'on attribut à un client
        vector<int> descs_cli;//vecteur de tous les descripteurs des clients
    };
    vector<thread_data> liste_thread;
    vector<int> descripteur_cli;
    pthread_t threads[1000];
    
};

#endif /* SERVEUR_H_ */
