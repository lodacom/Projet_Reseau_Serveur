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
#include "Authentification.h"

AnalyseurTrame::AnalyseurTrame(string p_message)
{
	// TODO Auto-generated constructor stub
	//this->tp=p_tp;
    this->message=p_message;
}

void AnalyseurTrame::Analyse()
{
	//read(tp,recu, 0);
    //char* message=recu;
    //string inter=this->message;
    printf("Analyse de la trame...\n");
    int i=0;
    while(i<this->message.length() and this->message.at(i)!='>')
    {
        i++;
    }
    this->action=this->message.substr(0,i);
    int j=i+1;
    cout << action << endl;
    while (j<this->message.length())
    {
        this->transmission+=this->message.at(j);
        j++;
    }
    /*this->transmission=this->message.substr(i+1,this->message.length());*/
    cout << "La trame a été analysée avec action: "<< this->action << " et message: " << this->transmission << endl;
}

string AnalyseurTrame::DecisionAction()
{
    printf("On décide de ce qu'on va faire\n");
    if (this->action.compare("connexion")==0)
    {
        Authentification* auth=new Authentification(this->transmission);
        string connexion=auth->Verification_Authentification(this->transmission);
        printf("On vérifie l'authentification\n");
        return connexion;
    }
    if (this->action.compare("connexion_controleur")==0)
    {
        return "controleur";
    }
    if (this->action.compare("connexion_employe")==0)
    {
        return "employe";
    }
    if (this->action.compare("connexion_refuse")==0)
    {
        return "refuse";
    }
    if (this->action.compare("partie_rapport")==0)
    {
        printf("On écrit le rapport...\n");
        return "partie_recue";
    }
    if (this->action.compare("partie_recue")==0 and this->transmission.compare("fin rapport")==0)
    {
        return "fin_partie_recue";
    }
    if (this->action.compare("partie_recue")==0)
    {
        return "partie_recue";
    }
    return "impossible";
}

AnalyseurTrame::~AnalyseurTrame()
{
	// TODO Auto-generated destructor stub
}

