/*
 * Serveur.cpp
 *
 *  Created on: 7 déc. 2012
 *      Author: Lolo
 */

#include "Serveur.h"
#include "sauvegarde.c"
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
                int index=this->ChercheDansListeEtablitControleur(transmission);
                this->liste_etablit_controleur.erase(this->liste_etablit_controleur.begin()+index);//on enlève l'employé de la liste des "rapport à faire"
                this->liste_rapport_encours.push_back(transmission);//on le met dans la liste des "rapport en cours"
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
        int i=0;
        while(i<transmission.length() and transmission.at(i)!='@')
        {
            i++;
        }
        string pseudo=transmission.substr(0,i);
        cout << "L'employé: " << pseudo << " écrit..." << endl;
        string mess=transmission.substr(i+1,transmission.length());
        if (Ecrit(mess.c_str(), pseudo.c_str())==-1)
        {
            cout << "Y a eu un truc qui s'est passé!!" << endl;
        }
        else
        {
            cout << "Sa à l'air bon......" << endl;
        }
        cout << "On écrit le rapport..." << endl;
        return "partie_recue";
    }
    if (action.compare("deconnexion")==0)
    {
        int index=this->ChercheDansListeEnCours(transmission);
        this->liste_rapport_encours.erase(this->liste_rapport_encours.begin()+index);
        this->liste_rapport_fait.push_back(transmission);
        if (OuvreRapport(transmission.c_str())==-1)
        {
            cout << "Erreur d'ouverture du rapport..." << endl;
        }
        else
        {
            cout << "On a réussi à ouvrir le rapport mais où?" << endl;
        }
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
    if (p_action=="impossible")
    {
        return p_action;
    }
    return "reimpossible";
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
        return i-1;//voir documentation vector dans C++ reference
    }
    else
    {
        return -1;
    }
}

int Serveur::ChercheDansListeEnCours(string p_pseudo)
{
    int i=0;
    while(i<this->liste_rapport_encours.size() and this->liste_rapport_encours[i].compare(p_pseudo)!=0)
    {
        i++;
    }
    if (i<this->liste_rapport_encours.size())
    {
        return i-1;//voir documentation vector dans C++ reference
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
