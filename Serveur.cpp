/*
 * Serveur.cpp
 *
 *  Created on: 7 déc. 2012
 *      Author: Lolo
 */

#include "Serveur.h"
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

string Serveur::Analyse(string p_message)
{
	//read(tp,recu, 0);
    //char* message=recu;
    //string inter=this->message;
    printf("Analyse de la trame...\n");
    int i=0;
    cout << p_message << endl;
    while(i<p_message.length() and p_message.at(i)!='>')
    {
        i++;
    }
    string action=p_message.substr(0,i);
    string transmission=p_message.substr(i+1,p_message.length());
    cout << "La trame a été analysée avec action: "<< action << " et message: " << transmission << endl;
    
    printf("On décide de ce qu'on va faire\n");
    if (action.compare("connexion")==0)
    {
        cout << "Un utilisateur se connecte..." << endl;
        this->liste_etablit_controleur.push_back("coucou");//on fait comme si coucou avait été ajouté par le controleur
        return "connexion...";
    }
    if (action.compare("connexion_employe")==0)
    {
        if (transmission.compare("controleur")==0)
        {
            cout << "Un controleur se connecte..." << endl;
            return "controleur";
        }
        else
        {
            cout << "Un employé se connecte..." << endl;
            if (this->EstDansListeEtablitControleur(transmission))
            {
                cout << "Vous êtes un employé au rapport!" << endl;
                return "employe";
            }
            else
            {
                cout << "Vous avez été refusé par le serveur" << endl;
                return "refuse";
            }
        }
    }
    if (action.compare("partie_rapport")==0)
    {
        
        while(i<transmission.length() and transmission.at(i)!='@')
        {
            i++;
        }
        string pseudo=transmission.substr(0,i);
        string mess=transmission.substr(i+1,transmission.length());
        if (this->EstDansListeEtablitControleur(pseudo))
        {
            int index=this->ChercheDansListeEtablitControleur(pseudo);
            this->liste_etablit_controleur.erase(this->liste_etablit_controleur.begin()+index);
        }
        cout << "On écrit le rapport..." << endl;
        return "partie_recue";
    }
    if (action.compare("suite_rapport")==0)
    {
        cout << "Suite de la chaine de la section" << endl;
        return "suite_recue";
    }
    if (action.compare("deconnexion")==0)
    {
        cout << "Un utilisateur se déconnecte" << endl;
        return "deconnexion";
    }
    return "impossible";
}

string Serveur::ActionServeur(string p_action)
{
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
    if (p_action=="deconnexion")
    {
        message="rapport>";
    }
    return message;
}

void Serveur::AccueilEmploye()
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
    printf("Bonjour mon employe...\n");
}

bool Serveur::EstDansListeEtablitControleur(string p_pseudo)
{
    int i=0;
    while(i<this->liste_etablit_controleur.size() and this->liste_etablit_controleur[i].compare(p_pseudo)!=0)
    {
        i++;
    }
    if (i<this->liste_etablit_controleur.size())
    {
        return true;
    }
    else
    {
        return false;   
    }
}

int Serveur::ChercheDansListeEtablitControleur(string p_pseudo)
{
    int i=0;
    while(i<this->liste_etablit_controleur.size() and this->liste_etablit_controleur[i].compare(p_pseudo)!=0)
    {
        i++;
    }
    if (i<this->liste_etablit_controleur.size())
    {
        return i-1;
    }
    else
    {
        return -1;
    }
}
string Serveur::LancementServeur(string p_message)
{
    // Reception du message
    printf("On lance le serveur...\n");
    string message=p_message;
    string action=this->Analyse(message);
    return this->ActionServeur(action);
}

Serveur::~Serveur() {
	// TODO Auto-generated destructor stub
}

int main(int argc, char** argv)
{
	Serveur* serv=new Serveur();
    string message;
    cout << "Saisissez une chaîne pour le test" << endl;
    getline(cin, message);
	while (serv->LancementServeur(message)!="rapport>")
    {
        cout << "Saisissez une chaîne pour le test" << endl;
        getline(cin, message);
    }
	return 0;
}
