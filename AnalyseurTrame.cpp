/*
 * AnalyseurTrame.cpp
 *
 *  Created on: 13 déc. 2012
 *      Author: Lolo
 */

#include "AnalyseurTrame.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <iostream>

AnalyseurTrame::AnalyseurTrame()
{
	// TODO Auto-generated constructor stub
	//this->tp=p_tp;
}

string AnalyseurTrame::Analyse(string p_message)
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
        /*Authentification* auth=new Authentification(this->transmission);
        string connexion=auth->Verification_Authentification(this->transmission);
        printf("On vérifie l'authentification\n");*/
        return "connexion...";
    }
    if (action.compare("connexion_controleur")==0)
    {
        return "controleur";
    }
    if (action.compare("connexion_employe")==0)
    {
        return "employe";
    }
    if (action.compare("connexion_refuse")==0)
    {
        return "refuse";
    }
    if (action.compare("partie_rapport")==0)
    {
        printf("On écrit le rapport...\n");
        return "partie_recue";
    }
    if (action.compare("partie_recue")==0 and transmission.compare("fin rapport")==0)
    {
        return "fin_partie_recue";
    }
    if (action.compare("partie_recue")==0)
    {
        return "partie_recue";
    }
    return "impossible";
}

AnalyseurTrame::~AnalyseurTrame()
{
	// TODO Auto-generated destructor stub
}
