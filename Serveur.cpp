/*
 * Serveur.cpp
 *
 *  Created on: 7 déc. 2012
 *      Author: Lolo
 */

#include "Serveur.h"
#include "Employe.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>

Serveur::Serveur()
{
	// TODO Auto-generated constructor stub
	/*SockDist Expd("0",31466);// pour autotest 31466/31467
	Sock BRlocal(SOCK_STREAM, 31467, 0);
	destLocal = BRlocal.getsDesc();
	taille=Expd.getsLen();

	adresseExp = Expd.getAdrDist();*/
    printf("Démarrage du serveur....\n");

}

bool Serveur::Ecoute()
{
	int ecoute=listen(destLocal, 1000);
	if (ecoute == 0)
	{
        cout << "Serveur à l'écoute" << endl;
		return true;
	}
	else
	{
        cout << "Démarrage du serveur échoué" << endl;
		return false;
	}
}

bool Serveur::AccueilClients()
{
    /*int accueil=accept(destLocal,(struct sockaddr *) adresseExp,&taille);
     if (accueil>=0)
     {
        this->descripteur_cli.push_back(accueil);
        return true;
     }
     {
        return false;
     }
     */
    return true;
}

void Serveur::LancementServeur(string p_message)
{
    // Reception du message
    printf("On lance le serveur...\n");
    string message=p_message;
    AnalyseurTrame* analyse=new AnalyseurTrame(message);
    analyse->Analyse();
    string action=analyse->DecisionAction();
    this->ActionServeur(action);
}

void Serveur::ActionServeur(string p_action)
{
    Employe* employ=new Employe();
    string message="";
    if (p_action=="controleur")
    {
        message="connexion_controleur>";
    }
    if (p_action=="employe")
    {
        message="connexion_employe>";
    }
    if (p_action=="refuse")
    {
        message="connexion_refuse>";
    }
    if (p_action=="partie_recue")
    {
        message="partie_recue>";
    }
    if (p_action=="fin_partie_recue")
    {
        message="fin_partie_recue>";
    }
    employ->PassageMessage(message);
}

Serveur::~Serveur() {
	// TODO Auto-generated destructor stub
}

/*int main(int argc, char** argv)
{
	Serveur* serv=new Serveur();
	serv->Ecoute();
	return 0;
}*/
