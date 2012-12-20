/*!
 * \file Serveur.cpp
 * \brief Programme permettant de lancer le serveur
 * \author Duplouy Olivier Burc Pierre
 * \version 0.1
 * \date 20 décembre 2012
 *
 * Enregistrement de rapports d'activité dans une entreprise
 *
 */

#include "sock.h"
#include "sockdist.h"
#include "sauvegarde.c"
#include <vector>
#include <string>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
using namespace std;

int destLocal;
int nombre_client=0;
sockaddr_in* adresseExp;
socklen_t taille;
char envoi[100];
char recu[100];
vector<int> descripteur_cli;
vector<string> liste_etablit_controleur;
vector<string> liste_rapport_fait;
vector<string> liste_rapport_encours;

struct thread_data
{
    int mondesc;//le descripteur que l'on attribut à un client
    vector<int> descs_cli;//vecteur de tous les descripteurs des clients
}thread;
vector<thread_data> liste_thread;

/**
 * \brief Fonction permettant d'initialisé le serveur 
 */
void Serveur()
{
	/*SockDist Expd("0",31466);// pour autotest 31466/31467
	Sock BRlocal(SOCK_STREAM, 31467, 0);
	destLocal = BRlocal.getsDesc();
	taille=Expd.getsLen();

	adresseExp = Expd.getAdrDist();
    listen(destLocal, 100);*/
    printf("Démarrage du serveur....\n");

}

/**
 * \fn int ChercheDansListeEnCours(string p_pseudo)
 * \brief Cherche dans la liste des rapports s'il y a p_pseudo
 * \param string p_pseudo
 * \return l'index i quand on l'a trouvé -1 sinon
 */
int ChercheDansListeEnCours(string p_pseudo)
{
    int i=0;
    while(i<liste_rapport_encours.size() and liste_rapport_encours[i].compare(p_pseudo)!=0)
    {
        i++;
    }
    if (i<liste_rapport_encours.size())
    {
        return i;//voir documentation vector dans C++ reference
    }
    else
    {
        return -1;
    }
}

/**
 * \fn int ChercheDansListeEtablitControleur(string p_pseudo)
 * \brief Cherche dans la liste établie par le controleur s'il y a p_pseudo
 * \param string p_pseudo
 * \return l'index i quand on l'a trouvé -1 sinon
 */
int ChercheDansListeEtablitControleur(string p_pseudo)
{
    int i=0;
    while(i<liste_etablit_controleur.size() and liste_etablit_controleur[i].compare(p_pseudo)!=0)
    {
        i++;
    }
    if (i<liste_etablit_controleur.size())
    {
        return i;//voir documentation vector dans C++ reference
    }
    else
    {
        return -1;
    }
}

/**
 * \fn int ChercheDansListeFait(string p_pseudo)
 * \brief Cherche dans la liste des rapports qui ont été fait s'il y a p_pseudo
 * \param string p_pseudo
 * \return l'index i quand on l'a trouvé -1 sinon
 */
int ChercheDansListeFait(string p_pseudo)
{
    int i=0;
    while(i<liste_rapport_fait.size() and liste_rapport_fait[i].compare(p_pseudo)!=0)
    {
        i++;
    }
    if (i<liste_rapport_fait.size())
    {
        return i;//voir documentation vector dans C++ reference
    }
    else
    {
        return -1;
    }
}

/**
 * \fn bool EstDansListeEtablitControleur(string p_pseudo)
 * \brief Cherche dans la liste établit par le contrôleur s'il y a p_pseudo
 * \param string p_pseudo
 * \return vrai si p_pseudo est présent dans la liste établie par le controleur
 */
bool EstDansListeEtablitControleur(string p_pseudo)
{
    int i=0;
    while(i<liste_etablit_controleur.size() and liste_etablit_controleur[i].compare(p_pseudo)!=0)
    {
        i++;
    }
    if (i<liste_etablit_controleur.size())
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * \fn string Analyse(string p_message)
 * \brief Permet d'analyser ce qui arrive dans la trame que l'on a reçu 
 * \param string p_message
 * \return une string correspondant à ce qu'on veut renvoyer
 */
string Analyse(string p_message)
{
	//read(mydata->desc1,recu,sizeof recu);
    //char* message=recu;
    //string inter=message;
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
        //liste_etablit_controleur.push_back("coucou");//on fait comme si coucou avait été ajouté par le controleur
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
            if (EstDansListeEtablitControleur(transmission))
            {
                int index=ChercheDansListeEtablitControleur(transmission);
                liste_etablit_controleur.erase(liste_etablit_controleur.begin()+index);//on enlève l'employé de la liste des "rapport à faire"
                liste_rapport_encours.push_back(transmission);//on le met dans la liste des "rapport en cours"
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
    if (action.compare("ajout_employe")==0)
    {
        liste_etablit_controleur.push_back(transmission);
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
    if (action.compare("demande_liste_rapport_fait")==0)
    {
        for (int i=0;i<liste_rapport_fait.size();i++)
        {
            cout << "employé: " << liste_rapport_fait[i] << endl;
        }
        return "liste_rapport_fait";
    }
    if (action.compare("demande_rapport")==0)
    {
        if (OuvreRapport(transmission.c_str())==-1)
        {
            cout << "Erreur d'ouverture du rapport..." << endl;
        }
        else
        {
            cout << "On a réussi à ouvrir le rapport mais où?" << endl;
        }
        cout << "Vous allez recevoir le rapport de: "<< transmission     << endl;
        int index=ChercheDansListeFait(transmission);
        liste_rapport_fait.erase(liste_rapport_fait.begin()+index);
        return "au_rapport";
    }
    if (action.compare("deconnexion")==0)
    {
        int index=ChercheDansListeEnCours(transmission);
        liste_rapport_encours.erase(liste_rapport_encours.begin()+index);
        liste_rapport_fait.push_back(transmission);
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

/**
 * \fn string ActionServeur(string p_action)
 * \brief Permet de répondre au client en fonction du message que l'on a eu
 * \param string p_action
 * \return une string correspondant à ce qu'on veut renvoyer
 */
string ActionServeur(string p_action)
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

/**
 * \fn string LancementServeur(string p_message)
 * \brief Permet de recevoir tous les messages
 * \param string p_message
 * \return une string correspondant à ce qu'on veut renvoyer
 */
string LancementServeur(string p_message)
{
    // Reception du message
    printf("On lance le serveur...\n");
    string message=p_message;
    string action=Analyse(message);
    return ActionServeur(action);
}

/**
 * \fn void* RunServeur(void* p)
 * \brief Permet de gérer plusieurs employés en même temps grâce aux threads
 * \param void* p
 * \return si ça s'est bien passé ou non
 */
void* RunServeur(void* p)
{
    string message;
    struct thread_data *mydata;
    mydata=(struct thread_data *)p;
    if(mydata->mondesc == mydata->descs_cli[0])
    {
        cout << "Entrain d'exécuter le client num: " << mydata->mondesc << endl;
        
        cout << "Saisissez une chaîne pour le test" << endl;
        getline(cin, message);
        cout << "Le message reçue est: " << message << endl;
        while (LancementServeur(message).compare("rapport>")!=0)
        {
            cout << "Saisissez une chaîne pour le test" << endl;
            getline(cin, message);
        }
    }
    if(mydata->mondesc == mydata->descs_cli[1])
    {
        cout << "Entrain d'exécuter le client num: " << mydata->mondesc << endl;
        
        cout << "Saisissez une chaîne pour le test" << endl;
        getline(cin, message);
        while (LancementServeur(message).compare("rapport>")!=0)
        {
            cout << "Saisissez une chaîne pour le test" << endl;
            getline(cin, message);
        }
    }
    
    pthread_exit(NULL);
}

/**
 * \brief Permet d'accueillir les employés en les acceptant
 */
void AccueilEmploye()
{
    /*int accueil=accept(destLocal,(struct sockaddr *) adresseExp,&taille);
     if (accueil>=0)
     {
        thread.mondesc=accueil;
        thread.descs_cli.push_back(accueil);
        liste_thread.push_back(thread);
        pthread_create(&threads[nombre_client],NULL,RunServeur,(void*)&liste_thread[nombre_client]);
        nombre_client++;
        printf("Bonjour mon employe...\n");
        return true;
     }
     {
     return false;
     }
     */
    printf("Bonjour mon employe...\n");
}

/**
 * \fn int main(int argc, char** argv)
 * \brief Point d'entré du programme et qui permet de le lancer
 * \param int argc, char** argv
 * \return 0 si ça s'est bien passé
 */
int main(int argc, char** argv)
{
    pthread_t threads[100];
    /*
     Serveur();
     while(true)
     {
        AccueilEmploye();
     }
    */
    //Premier utilisateur
    thread.mondesc=0;
    thread.descs_cli.push_back(0);
    liste_thread.push_back(thread);
    
    //Deuxième utilisateur
    thread.mondesc=1;
    thread.descs_cli.push_back(1);
    liste_thread.push_back(thread);
    
    pthread_create(&threads[0],NULL,RunServeur,(void*)&liste_thread[0]);
    pthread_create(&threads[1],NULL,RunServeur,(void*)&liste_thread[1]);
    
    pthread_join(threads[0],NULL);
    pthread_join(threads[1],NULL);
	return 0;
}
