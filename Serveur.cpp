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
//FILE * liste_rapport_fait_fichier;
//FILE * liste_rapport_encours_fichier;
pthread_mutex_t verrou_acces_liste_encours = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t verrou_acces_liste_fait = PTHREAD_MUTEX_INITIALIZER;
//-----------------------------------------

struct thread_data
{
    int mondesc;//le descripteur que l'on attribut à un client
    string pseudo;//pseudo de l'employé qui s'est connecté
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
            //send(desc,envoi,sizeof(envoi),0);
            mes="";
            cpt=0;
        }
    }
    if (pseudo.compare("controleur")==0)
    {
        envoi="fin_transfert>controleur";
        //send(desc,envoi,sizeof(envoi),0);
    }
}

vector<string> LectureDansListeFait()
{
    vector<string> liste_rapport_fait;
    cout << "On cherche à établir la liste des rapports faits" << endl;
    FILE * lrf=fopen(LISTE_RAPPORT_FAIT, "r");
    cout << "On effectue la recherche..." << endl;
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
            concat += recup;
            //cout << "On concatène" << endl;
        }
    }
    fclose(lrf);
    return liste_rapport_fait;
}

/**
 * \fn int ChercheDansListeEnCours(string p_pseudo)
 * \brief Cherche dans la liste des rapports s'il y a p_pseudo
 * \param string p_pseudo
 * \return l'index i quand on l'a trouvé -1 sinon
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
 * \brief Cherche dans la liste établie par le controleur s'il y a p_pseudo
 * \param string p_pseudo
 * \return l'index i quand on l'a trouvé -1 sinon
 */
int ChercheDansListeEtablitControleur(string p_pseudo)
{
    int courant=0;
    int nb_espace=0;
    fpos_t pos;
    cout << "On cherche à enlever " << p_pseudo << " de la liste établit par le controleur" << endl;
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
        return -1;
    }
    else
    {
        courant= fseek(lecf, 0, SEEK_SET);
        cout << "On effectue la recherche..." << endl;
        string concat="";
        char recup;
        while ((recup=fgetc(lecf))!=EOF)
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
                 fgetpos(lecf,&pos);
                 nb_espace=0;
                 //cout << "Position du curseur" << pos << endl;
                 cout << "On a trouvé le arobase dans la liste établit" << endl;
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
    fclose(lecf);
    if (!trouve)
    {
        //écraser à partir de courant jusqu' à la taille de pseudo + 1
        cout << "On a pas trouvé l'employé" << endl;
        return -1;
    }
    else
    {
        cout << "Youpi on a trouvé l'employé: " << p_pseudo << endl;
        FILE * lecf=fopen(LISTE_ETABLIT_CONTROLEUR, "r+");//on ouvre en écriture
        fsetpos(lecf,&pos);//on se met là où on a repéré le pseudo
        int i=0;
        while (i<=p_pseudo.length()+nb_espace)
        {
            cout << "On essaie d'écraser les valeurs" << endl;
            fputs (" ",lecf);//on écrase le pseudo plus l'arobase
            i++;
        }
        fclose(lecf);
        return 0;
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
 * \brief Cherche dans la liste établit par le contrôleur s'il y a p_pseudo
 * \param string p_pseudo
 * \return vrai si p_pseudo est présent dans la liste établie par le controleur
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
        return false;
    }
    else
    {
        fseek(lecf, 0, SEEK_SET);
        cout << "On effectue la recherche..." << endl;
        string concat="";
        char recup;
        while ((recup=fgetc(lecf))!=EOF)
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
                 cout << "On a trouvé le arobase . . . .l" << endl;
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
    }
    fclose(lecf);
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
    //recv(desc,recu,sizeof recu, 0);
    //char* message=recu;
    //string inter=message;
    //printf("Analyse de la trame...\n");
    int i=0;
    //cout << p_message << endl;
    while(i<p_message.length() and p_message.at(i)!='>')
    {
        i++;
    }
    string action=p_message.substr(0,i);
    string transmission=p_message.substr(i+1,p_message.length());
    //cout << "La trame a été analysée avec action: "<< action << " et message: " << transmission << endl;
    
    //printf("On décide de ce qu'on va faire\n");
    if (action.compare("connexion")==0)
    {
        //cout << "Un utilisateur se connecte..." << endl;
        return "connexion...";
    }
    if (action.compare("connexion_employe")==0)
    {
        if (transmission.compare("controleur")==0)
        {
            if (!controleur_present)
            {
                //cout << "Un controleur se connecte..." << endl;
                controleur_present=true;
                thread.mondesc=desc;
                thread.pseudo=transmission;
                liste_thread.push_back(thread);
                
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
                    //send(desc,envoi,sizeof(envoi),0);
                }
                return "connexion_controleur>";
            }
            else
            {
                cout << "Un controleur existe déjà désolé" << endl;
                //send(this->adresseExp,envoi,sizeof(envoi),0);
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
                    thread.mondesc=desc;
                    thread.pseudo=transmission;
                    liste_thread.push_back(thread);
                
                    cout << "Bravo vous êtes connectés" << endl;
                    ChercheDansListeEtablitControleur(transmission);
                    transmission+="@";
                    FILE* lrecf=fopen(LISTE_RAPPORT_ENCOURS, "a");
                    fputs (transmission.c_str(),lrecf);//transfert dans la liste rapport en cours
                    fclose(lrecf);
                    cout << "Vous êtes un employé au rapport!" << endl;
                    //send(desc,envoi,sizeof(envoi),0);
                    return "employe";
                }
                else
                {
                    cout << "Vous avez été refusé par le serveur" << endl;
                    //send(desc,envoi,sizeof(envoi),0);
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
            cout << "Désolé le pseudo existe déjà!!" << endl;
            //send(desc,envoi,sizeof(envoi),0);
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
    if (action.compare("demande_liste_rapport_fait")==0)
    {
        vector<string> temp=LectureDansListeFait();
        for(int i=0;i<temp.size();i++)
        {
            cout << temp[i] << endl;
            //send(desc,envoi,sizeof(envoi),0);
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
                cout << "L'employé n'a pas encore fini d'écrire son rapport" << endl;
                //send(desc,envoi,sizeof(envoi),0);
                return "pas_fini";
            }
        }
        cout << "Vous allez recevoir le rapport de: " << transmission << endl;
        //il faut transférer le rapport au controleur
        TransfertRapport(desc,"controleur");
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
        cout << "Un utilisateur ou un controleur se déconnecte" << endl;
        //send(desc,envoi,sizeof(envoi),0);
        return "deconnexion>";
    }
    return "impossible";
}

/**
 * \fn string LancementServeur(string p_message)
 * \brief Permet de recevoir tous les messages
 * \param string p_message
 * \return une string correspondant à ce qu'on veut renvoyer
 */
string LancementServeur(string p_message,int desc)
{
    // Reception du message
    //printf("On lance le serveur...\n");
    string message=p_message;
    return Analyse(message,desc);
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
    string nom=((struct thread_data *)p)->pseudo;
    int desc=((struct thread_data *)p)->mondesc;
    
    cout << "Start run : " << nom << endl;
    cout << "Entrain d'exécuter le client num: " << desc << endl;
    message="connexion>";
    while (LancementServeur(message,desc).compare("deconnexion>")!=0)
    {
        message="connexion_employe>"+nom;
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
        cout << nom << " deconnecté" << endl;
        //cout << "Saisissez une chaîne pour le test" << endl;
        //getline(cin, message);
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
     * recv(accueil,recu,sizeof recu, 0);
     * if (Analyse(message,desc).compare("refuse")==0)
     * {
     *  send(accueil,envoi,sizeof(envoi),0);
     * }
     * else
     * {
        pthread_create(&threads[nombre_client],NULL,RunServeur,(void*)&liste_thread[nombre_client]);
        nombre_client++;
        printf("Bonjour mon employe...\n");
     * }
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
     * 
     }
    */
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
    pthread_join(threads[1],NULL);
    return 0;
}