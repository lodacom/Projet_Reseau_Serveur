/*!
 * \file Serveur.cpp
 * \brief Programme permettant de lancer le serveur
 * \authors Duplouy Olivier, Burc Pierre
 * \version 1.0
 * \date 20 décembre 2012
 *
 * Enregistrement de rapports d'activité dans une entreprise
 *
 */

#include "sock.h"
#include "sockdist.h"
#include "sauvegarde.c"
#include "sauvegarde.h"
#include <sstream>
#include <iostream>
#include <fcntl.h>
#include <vector>
#include <list>
#include <stdio.h>
#include <errno.h>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
using namespace std;


vector<string> listString;

int destLocal=-1;
bool controleur_present=false;
int nombre_client=0;
sockaddr_in* adresseExp;
socklen_t taille;
char envoi[100];
char recu[100];
#define LISTE_ETABLIT_CONTROLEUR "liste_etablit_controleur"
#define LISTE_RAPPORT_FAIT "liste_rapport_fait"
#define LISTE_RAPPORT_ENCOURS "liste_rapport_encours"
FILE * liste_etablit_controleur_fichier;
pthread_mutex_t verrou_acces_liste_encours = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t verrou_acces_liste_fait = PTHREAD_MUTEX_INITIALIZER;
//-----------------------------------------

struct thread_data
{
    int mondesc;//le descripteur que l'on attribut à un client
    string pseudo;//pseudo de l'employé qui s'est connecté
}thread;
vector<thread_data> liste_thread;
vector<pthread_t> mesthreads;
/**
 * \brief Fonction permettant d'initialisé le serveur 
 */
void Serveur()
{
    SockDist Expd("0",31466);// pour autotest 31466/31467
    Sock BRlocal(SOCK_STREAM, 31467, 0);
    destLocal = BRlocal.getsDesc();
    taille=Expd.getsLen();
    adresseExp = Expd.getAdrDist();
    
    int test=listen(destLocal, 10);
    if (test==0)
    {
        perror("Démarrage du serveur, il est en écoute");
    }
    else
    {
        perror("Erreur de démarrage");
    }

}

/**
 * \param int desc,string pseudo
 * \brief Permet d'envoyer le rapport à l'employé qui a fini
 * son rapport mais également au controleur qui a fait une 
 * demande
 */
void TransfertRapport(int desc,string pseudo)
{
    char recup;
    string envoi;
    string mes;
    int cpt=0;
    char* chemin=(char*)"∕";
    strcat(chemin,(const char *)pseudo.c_str());
    strcat(chemin,"/temp.pdf");//construction du chemin pour trouver le rapport qu'il faut transferer
    FILE * rapport=fopen(chemin, "r");
    fseek(rapport, 236337, SEEK_SET);//on évite l'image
    while ((recup=fgetc(rapport))!=EOF)
    {
        mes += recup;
        cpt++;
        if (cpt == 100)
        {
            envoi="transfert_rapport>"+mes;
            send(desc,(const void *)envoi.c_str(),sizeof(envoi),0);
            mes="";
            cpt=0;
        }
    }
    envoi="fin_transfert>"+pseudo;
    send(desc,(const void *)envoi.c_str(),sizeof(envoi),0);
}

/**
 * \fn vector<string> LectureDansListeFait()
 * \brief On veut récupérer tous les employés qui ont fini leur rapport
 * \return renvoi la liste de tous les employés ayant fini leur rapport
 */
vector<string> LectureDansListeFait()
{
    vector<string> liste_rapport_fait;
    cout << "On cherche à établir la liste des rapports faits" << endl;
    FILE * lrf=fopen(LISTE_RAPPORT_FAIT, "r");
    cout << "On effectue la recherche..." << endl;
    fseek(lrf, 0, SEEK_END);
    if (ftell (lrf)==0)
    {
        cout << "On retourne " << endl;
        fclose(lrf);
        return liste_rapport_fait;
    }
    fseek(lrf, 0, SEEK_SET);
    string concat="";
    char recup;
    while ((recup=fgetc(lrf))!=EOF)
    {
        //cout << recup << endl;
        if(recup == '@')
        {
            liste_rapport_fait.push_back(concat);
            concat = "";
            cout << "On a trouvé le arobase dans la liste fait" << endl;
        }
        else
        {
            if (recup!=' ')
            {
                concat += recup;
            }
            //cout << "On concatène" << endl;
        }
    }
    fclose(lrf);
    return liste_rapport_fait;
}

/**
 * \fn int ChercheDansListeEnCours(string p_pseudo)
 * \brief Cherche dans le fichier des rapports en cours s'il y a p_pseudo
 * \param string p_pseudo
 * \return 0 quand on l'a trouvé -1 sinon
 */
int ChercheDansListeEnCours(string p_pseudo)
{
    int courant=0;
    int nb_espace=0;
    fpos_t pos;
    cout << "On cherche à enlever " << p_pseudo << " de la liste des rapports en cours" << endl;
    FILE * lrecf=fopen(LISTE_RAPPORT_ENCOURS, "r");
    if (lrecf==NULL)
    {
        cout << "Problème d'ouverture de fichier" << endl;
    }
    bool trouve=false;
    fseek(lrecf, 0, SEEK_END);
    if (ftell (lrecf)==0)
    {
        cout << "Y a un truc là!!!!" << endl;
        return -1;
    }
    else
    {
        courant=fseek(lrecf, 0, SEEK_SET);
        cout << "On effectue la recherche..." << endl;
        string concat="";
        char recup;
        while ((recup=fgetc(lrecf))!=EOF)
        {
            cout << recup << endl;
            if(recup == '@')
            {
                if(concat.compare(p_pseudo)==0)
                {
                    cout << "trouvé" << endl;
                    trouve = true;
                    break;
                }
                concat = "";
                courant=fgetpos(lrecf,&pos);
                nb_espace=0;
                cout << "On a trouvé le arobase dans la liste en cours" << endl;
            }
            else
            {
                if (recup!=' ')
                {
                        concat += recup;
                }
                else
                {
                    nb_espace++;
                }
                //cout << "On concatène" << endl;
            }
        }
    }
    fclose(lrecf);
    if (!trouve)
    {
        //écraser à partir de courant jusqu' à la taille de pseudo + 1
        cout << "On a pas trouvé l'employé" << endl;
        return -1;
    }
    else
    {
        cout << "Youpi on a trouvé l'employé: " << p_pseudo << endl;
        FILE * lrecf=fopen(LISTE_RAPPORT_ENCOURS, "r+");//on ouvre en écriture
        fsetpos(lrecf,&pos);//on se met là où on a repéré le pseudo
        int i=0;
        while (i<=p_pseudo.length()+nb_espace)
        {
            fputs (" ",lrecf);//on écrase le pseudo plus l'arobase
            i++;
        }
        fclose(lrecf);
        return 0;
    }
}

/**
 * \fn int ChercheDansListeEtablitControleur(string p_pseudo)
 * \brief Cherche dans le fichier établi par le controleur s'il y a p_pseudo
 * \param string p_pseudo
 * \return 0 quand on l'a trouvé -1 sinon
 */
void ChercheDansListeEtablitControleur(string p_pseudo)
{
    cout << "On cherche à enlever " << p_pseudo << " de la liste établit par le controleur" << endl;
    FILE * lecf=fopen("liste_etablit_controleur", "r");
    /*if (lecf==NULL)
    {
        cout << "Problème d'ouverture de fichier" << endl;
    }
    fseek(lecf, 0, SEEK_END);
    if (ftell (lecf)==0)
    {
        cout << "Y a un truc là!!!!" << endl;
    }
    else
    {
        fseek(lecf, 0, SEEK_SET);*/
        cout << "On effectue la recherche..." << endl;
        char c;
        
        listString.clear();
        
        cout << "plop" << endl;
        
        string tmp = "";
        while((c = fgetc(lecf)) != EOF){
            if(c == '@'){
                listString.push_back(tmp);
                tmp = "";
                continue;
            }else if( c != '\n'){
                tmp += c;
                cout << "On passe" << endl;
            }
        }
        
        cout << "On passe ici" << endl;
        //listeNom.push_back(tmp);
        fclose(lecf);
        
        string reste = "";
        
        for(vector<string>::iterator i = listString.begin() ; i != listString.end() ; i++){
            if(p_pseudo.compare(*i) == 0){
                //retour = true;
            }else{
                if(reste.length() > 0){
                    reste += '@';
                }
                reste += *i;
            }
        }
        
        if(reste.length() > 0){
            reste += '@';
            lecf = fopen("liste_etablit_controleur", "w");
            fprintf(lecf,"%s",reste.c_str());
            fclose(lecf);
        }else{
            lecf = fopen("liste_etablit_controleur", "w");
            fclose(lecf);
        }

    //}
}

/**
 * \fn int ChercheDansListeFait(string p_pseudo)
 * \brief Cherche dans le fichier des rapports qui ont été fait s'il y a p_pseudo
 * \param string p_pseudo
 * \return 0 quand on l'a trouvé -1 sinon
 */
int ChercheDansListeFait(string p_pseudo)
{
    int courant=0;
    int nb_espace=0;
    fpos_t pos;
    cout << "On cherche à enlever " << p_pseudo << " de la liste des rapports faits" << endl;
    FILE * lrf=fopen(LISTE_RAPPORT_FAIT, "r");
    if (lrf==NULL)
    {
        cout << "Problème d'ouverture de fichier" << endl;
    }
    bool trouve=false;
    fseek(lrf, 0, SEEK_END);
    if (ftell (lrf)==0)
    {
        cout << "Y a un truc là!!!!" << endl;
        return -1;
    }
    else
    {
        courant=fseek(lrf, 0, SEEK_SET);
        cout << "On effectue la recherche..." << endl;
        string concat="";
        char recup;
        while ((recup=fgetc(lrf))!=EOF)
        {
            cout << recup << endl;
            if(recup == '@')
            {
                if(concat.compare(p_pseudo)==0)
                {
                    cout << "trouvé" << endl;
                    trouve = true;
                    break;
                }
                concat = "";
                courant=fgetpos(lrf,&pos);
                nb_espace=0;
                cout << "On a trouvé le arobase dans la recherche" << endl;
            }
            else
            {
                if (recup!=' ')
                {
                        concat += recup;
                }
                else
                {
                    nb_espace++;
                }
                //cout << "On concatène" << endl;
            }
        }
    }
    fclose(lrf);
    if (!trouve)
    {
        //écraser à partir de courant jusqu' à la taille de pseudo + 1
        cout << "On a pas trouvé l'employé" << endl;
        return -1;
    }
    else
    {
        cout << "Youpi on a trouvé l'employé: " << p_pseudo << endl;
        FILE * lrf=fopen(LISTE_RAPPORT_FAIT, "r+");//on ouvre en écriture
        fsetpos(lrf,&pos);//on se met là où on a repéré le pseudo
        int i=0;
        while (i<=p_pseudo.length()+nb_espace)
        {
            fputs (" ",lrf);//on écrase le pseudo plus l'arobase
            i++;
        }
        fclose(lrf);
        return 0;
    }
}

/**
 * \fn bool EstDansListeEtablitControleur(string p_pseudo)
 * \brief Cherche dans le fichier établit par le contrôleur s'il y a p_pseudo
 * \param string p_pseudo
 * \return vrai si p_pseudo est présent dans le fichier établit par le controleur
 */
bool EstDansListeEtablitControleur(string p_pseudo)
{
    cout << "On cherche à savoir si " << p_pseudo << " est dans la liste établit par le controleur" << endl;
    FILE * lecf=fopen(LISTE_ETABLIT_CONTROLEUR, "r");
    if (lecf==NULL)
    {
        cout << "Problème d'ouverture de fichier" << endl;
    }
    bool trouve=false;
    fseek(lecf, 0, SEEK_END);
    if (ftell (lecf)==0)
    {
        cout << "Y a un truc là!!!!" << endl;
        fclose(lecf);
        return false;
    }
    else
    {
        fseek(lecf, 0, SEEK_SET);
        cout << "On effectue la recherche..." << endl;
        char concat[100];
        string concatNew = concat;
        concatNew="";
        char recup;
        while ((recup=fgetc(lecf))!=EOF)
        {
            //cout << recup << endl;
            if(recup == '@')
            {
                 if(concatNew.compare(p_pseudo)==0)
                 {
                        cout << "trouvé" << endl;
                        trouve = true;
                        break;
                 }
                 concatNew="";
                 cout << "Pseudo ne correspond pas" << endl;
            }
            else
            {
                if (recup!=' ')
                {
                        //cout << "On concatène" << endl;
                        concatNew+=recup;
                        //cout << concatNew << endl;  

                }
            }
        }
    }
    int test=fclose(lecf);
    if (test==0)
    {
        cout << "On ferme le fichier de la liste établit par le contrleur" << endl;
    }
    if (!trouve)
    {
        cout << "On a pas trouvé l'employé" << endl;
        return false;
    }
    else
    {
        cout << "Youpi on a trouvé l'employé: " << p_pseudo << endl; 
        return true;
    }
}

/**
 * \fn string Analyse(string p_message)
 * \brief Permet d'analyser ce qui arrive dans la trame que l'on a reçu 
 * \param string p_message
 * \return une string correspondant à ce qu'on veut renvoyer
 */
string Analyse(string p_message,int desc)
{
    int i=0;
    cout << p_message << endl;
    while(i<p_message.length() and p_message.at(i)!='>')
    {
        i++;
    }
    string action=p_message.substr(0,i);
    string transmission=p_message.substr(i+1,p_message.length());
    cout << "La trame a été analysée avec action: "<< action << " et message: " << transmission << endl;
    //printf("On décide de ce qu'on va faire\n");
    if (action.compare("connexion_employe")==0)
    {
        if (transmission.compare("controleur")==0)
        {
            if (!controleur_present)
            {
                cout << "Un controleur se connecte..." << endl;
                controleur_present=true;
                
                liste_etablit_controleur_fichier=fopen(LISTE_ETABLIT_CONTROLEUR, "r");
                fseek(liste_etablit_controleur_fichier, 0, SEEK_END);
                if (ftell(liste_etablit_controleur_fichier)==0)
                {
                    fclose (liste_etablit_controleur_fichier);
                    liste_etablit_controleur_fichier=fopen(LISTE_ETABLIT_CONTROLEUR, "w");
                    fclose(liste_etablit_controleur_fichier);
                }
                else
                {
                    cout<< "Liste déjà remplie. . ." << endl;
                    char tmp[100];
                    strcpy(tmp,"action_suivante_controleur>La liste est déjà remplie");
                    send(desc,tmp,sizeof(tmp),0);
                    cout << "Message envoyé" << endl;
                }
                return transmission;
            }
            else
            {
                //cout << "Un controleur existe déjà désolé" << endl;
                char rep[100] = "reponse>Désolé un controleur s'est déjà connecté";
                send(desc, rep, sizeof(rep),0);
                return "connexion_refuse>";
            }
        }
        
        else
        {
            cout << "Un employé se connecte..." << endl;
            //mutex 1 ici (mode lock)
            int verif=pthread_mutex_trylock(&verrou_acces_liste_encours);
            if (verif==0)
            {
                if (EstDansListeEtablitControleur(transmission))
                {
                    cout << "Bravo vous êtes connectés" << endl;
                    ChercheDansListeEtablitControleur(transmission);
                    cout << "On sort de la fonction" << endl;
                    transmission+="@";
                    FILE* lrecf=fopen(LISTE_RAPPORT_ENCOURS, "a");
                    fputs (transmission.c_str(),lrecf);//transfert dans la liste rapport en cours
                    fclose(lrecf);
                    cout << "Vous êtes un employé au rapport!" << endl;
                    char envoi[100] = "connexion_employe>";
                    send(desc, envoi,sizeof(envoi),0);
                    return transmission;//on retourne l'employé qui a été connecté
                }
                else
                {
                    cout << "Vous avez été refusé par le serveur" << endl;
                    char refus[100] = "connexion_refuse>";
                    send(desc, refus,sizeof(refus),0);
                    return "connexion_refuse>";
                }
            }
            //mutex 1 (mode unlock)
            pthread_mutex_unlock(&verrou_acces_liste_encours);
        }
    }
    if (action.compare("ajout_employe")==0)
    {
        if (!EstDansListeEtablitControleur(transmission))
        {
            transmission+="@";
            liste_etablit_controleur_fichier=fopen(LISTE_ETABLIT_CONTROLEUR, "a");
            fputs (transmission.c_str(),liste_etablit_controleur_fichier);//ajout d'un employé dans la liste établit par le controleur
            fclose(liste_etablit_controleur_fichier);
            cout << "Un employé a été ajouté dans le fichier" << endl;
        }
        else
        {
            //cout << "Désolé le pseudo existe déjà!!" << endl;
            string rep="reponse>Vous devez choisir un pseudo différent "+transmission+" existe déjà";
            send(desc,(const void *)rep.c_str(),sizeof(rep),0);
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
        
        char* chemin=(char*)"/Rapports_Clients/";
        strcat(chemin,(const char *)pseudo.c_str());//on crée une sauvegarde pour chaque employé
        FILE* verif=fopen(chemin,"r");
        fseek(verif, 0, SEEK_END);
        if (ftell(verif)!=0)
        {
            fclose(verif);
            FILE* rapport=fopen(chemin,"a");
            fputs (mess.c_str(),rapport);
            fclose(rapport);
        }
        else
        {
            fclose(verif);
            FILE* rapport=fopen(chemin,"w");
            fputs (mess.c_str(),rapport);
            fclose(rapport);
        }
        
        
        cout << "On sauvegarde la partie que l'on a reçu..." << endl;
        return "partie_recue";
    }
    if (action.compare("fin_section")==0)
    {
        char* chemin=(char*)"/Rapports_Clients/";
        strcat(chemin,(const char *)transmission.c_str());//on crée une sauvegarde pour chaque employé
        FILE* recopie=fopen(chemin,"r");
        fseek(recopie, 0, SEEK_END);
        int taille=ftell(recopie);
        fseek(recopie, 0, SEEK_SET);
        char* recup;
        fgets(recup,taille,recopie);//on recupère tout en un seul coup
        Ecrit(recup,transmission.c_str());//on copie
        fclose(recopie);
        FILE* efface=fopen(chemin,"w");//on efface tout après recopie
        fclose(efface);
        return "fin_section";
    }
    if (action.compare("demande_liste_rapport_fait")==0)
    {
        vector<string> temp=LectureDansListeFait();
        if (!temp.empty())
        {
            cout << "taille de temp : " << temp.size() << endl;
            for(int i=0;i<temp.size();i++)
            {
                //cout << temp[i] << endl;
                char tmp[100];
                strcpy(tmp,"liste_rapport_fait>");
                strcat(tmp,temp[i].c_str());
                send(desc,tmp,sizeof(tmp),0);
            }
            char tmp[100];
            strcpy(tmp,"action_suivante_controleur>La liste a été envoyée");
            send(desc,tmp,sizeof(tmp),0);
        }
        else
        {
            cout << "La liste est vide" << endl;
            char tmp[100];
            strcpy(tmp,"action_suivante_controleur>La liste est vide");
            send(desc,tmp,sizeof(tmp),0);
            cout << "Envoi effectué"<<endl;
        }
        return "liste_rapport_fait";
    }
    if (action.compare("demande_rapport")==0)
    {
        //fait par le controleur sur demande avec le pseudo de l'utilisateur demandé
        //mutex 2 ici en mode lock
        int verif=pthread_mutex_trylock(&verrou_acces_liste_fait);
        if (verif==0)
        {
            if (ChercheDansListeFait(transmission)==0)
            {
                if (OuvreRapport(transmission.c_str())==-1)
                {
                    cout << "Erreur d'ouverture du rapport..." << endl;
                }
                else
                {
                    cout << "On a réussi à ouvrir le rapport mais où?" << endl;
                }
                return "au_rapport";
            }
            else
            {
                //cout << "L'employé n'a pas encore fini d'écrire son rapport" << endl;
                char tmp[100];
                strcpy(tmp,"action_suivante_controleur>Désolé mais ");
                strcat(tmp,transmission.c_str());
                strcat(tmp," n'a pas encore fini d'écrire");
                send(desc,tmp,sizeof(tmp),0);
                return "pas_fini";
            }
        }
        cout << "Vous allez recevoir le rapport de: " << transmission << endl;
        //il faut transférer le rapport au controleur
        TransfertRapport(desc,transmission);/*on envoie au controleur 
                                             * le rapport de l'employé
                                             */
        //on enlève du fichier qui contient la liste des rapports qui sont fait
        //mutex 2 mode unlock
         pthread_mutex_unlock(&verrou_acces_liste_fait);
    }
    if (action.compare("deconnexion")==0)
    {
        //mutex 2 ici en mode lock
        if (transmission.compare("controleur")!=0)
        {
            int verif=pthread_mutex_trylock(&verrou_acces_liste_fait);
            if (verif==0)
            {
                ChercheDansListeEnCours(transmission);
                FILE* lrecf=fopen(LISTE_RAPPORT_FAIT, "a");
                transmission+="@";
                fputs (transmission.c_str(),lrecf);
                fclose(lrecf);
                //mutex 2 en mode unlock
                pthread_mutex_unlock(&verrou_acces_liste_fait);
                if (OuvreRapport(transmission.c_str())==-1)
                {
                    cout << "Erreur d'ouverture du rapport..." << endl;
                }
                else
                {
                    cout << "On a réussi à ouvrir le rapport mais où?" << endl;
                }
                //il faut transférer le rapport à l'employé
                TransfertRapport(desc,transmission);
            }
        }
        else
        {
            controleur_present=false;
        }
        //cout << "Un utilisateur ou un controleur se déconnecte" << endl;
        char tmp[100];
        strcpy(tmp,"deconnexion>");
        strcat(tmp,transmission.c_str());
        send(desc,tmp,sizeof(tmp),0);
        return "deconnexion>";
    }
    return "impossible";
}

/**
 * \fn int EnleveEmploye(string pseudo)
 * \brief Permet d'enlever l'employé qui s'est déconnecté du serveur
 * de la liste desthreads
 * \param string pseudo
 * \return l'index de la position où se trouve l'employe sinon -1
 */
int EnleveEmploye(string pseudo)
{
    int i=0;
    while (i<liste_thread.size() and liste_thread[i].pseudo.compare(pseudo)!=0)
    {
        i++;
    }
    if (i<liste_thread.size())
    {
        return i;
    }
    else
    {
        return -1;
    }
}

/**
 * \fn void* RunServeur(void* p)
 * \brief Permet de gérer plusieurs employés en même temps grâce aux threads
 * \param void* p
 * \return si ça s'est bien passé ou non
 */
void* RunServeur(void* p)
{
    //string message;
    string nom=((struct thread_data *)p)->pseudo;
    int desc=((struct thread_data *)p)->mondesc;
    
    cout << "Start run : " << nom << endl;
    cout << "Entrain d'exécuter le client num: " << desc << endl;
    //message="connexion>";
    recv(desc,recu,sizeof(recu), 0);
    char* message=recu;
    string inter=recu;
    while (Analyse(inter,desc).compare("deconnexion>")!=0)
    {
        cout << "On attend le prochain message" << endl;
        recv(desc,recu,sizeof(recu), 0);
        message=recu;
        inter=recu;
        /*message="connexion_employe>"+nom;
        LancementServeur(message,desc);
        
        if (nom.compare("controleur")==0)
        {
            message="ajout_employe>coucou";
            LancementServeur(message,desc);
            
            message="demande_rapport>coucou";
            while(LancementServeur(message,desc).compare("pas_fini")==0)
            {
                message="demande_rapport>coucou";
            }
        }
        else
        {
            message="connexion_employe>coucou";
            while(LancementServeur(message,desc).compare("refuse")==0)
            {
                message="connexion_employe>coucou";
            }
            
            message="partie_rapport>coucou@Coucou Olivier";
            LancementServeur(message,desc);
        }
        message="deconnexion>"+nom;
        cout << nom << " deconnecté" << endl;*/
    }
    //enlevé de la liste des threads
    int index=EnleveEmploye(nom);
    liste_thread.erase(liste_thread.begin()+index);
    pthread_exit(NULL);
}

/**
 * \brief Permet d'accueillir les employés en les acceptant à travers
 * notre socket local
 */
void AccueilEmploye()
{
    cout << "On attend des clients...." << endl;
    //cout << destLocal << endl;
    int accueil=accept(destLocal,(struct sockaddr *) adresseExp,&taille);
    cout << "On passe par là" << endl;
    if (accueil>=0)
    {
        cout << "On a accepté un client!!" << endl;
        recv(accueil,recu, sizeof(recu), 0);/*on attend tout
        *de suite une reception pour l'authentification 
                                           */
        string recup=Analyse(recu,accueil);
        if (recup.compare("connexion_refuse>")!=0)
        {  
            thread.mondesc=accueil;
            thread.pseudo=recup;
            liste_thread.push_back(thread);
            
            cout << "Création d'un thread" << endl;
            pthread_t mon_thread;
            pthread_create(&mon_thread,NULL,RunServeur,(void*)&liste_thread[nombre_client]);
            cout << "On lance le thread" << endl;
            nombre_client++;
            //cout << "Bonjour mon employe..." << endl;
        }
    }
    else
    {
        perror("Il y a eu un rejet d'un client");
    }
}

/**
 * \fn int main(int argc, char** argv)
 * \brief Point d'entré du programme et qui permet de le lancer
 * \param int argc, char** argv
 * \return 0 si ça s'est bien passé
 */
int main(int argc, char** argv)
{   
     Serveur();
     while(true)
     {
        AccueilEmploye();
     }
    
     for (int i=0;i<mesthreads.size();i++)
     {
         pthread_join(mesthreads[i],NULL);
     }
     /*
    //Premier utilisateur
    thread.mondesc=0;
    thread.pseudo="coucou";
    liste_thread.push_back(thread);
    
    //Deuxième utilisateur
    thread.mondesc=1;
    thread.pseudo="controleur";
    liste_thread.push_back(thread);
    cout << "On démarre le programme" << endl;
    pthread_create(&threads[1],NULL,RunServeur,(void*)&liste_thread[1]);
    pthread_create(&threads[0],NULL,RunServeur,(void*)&liste_thread[0]);
    
    pthread_join(threads[0],NULL);
    pthread_join(threads[1],NULL);*/
    return 0;
}