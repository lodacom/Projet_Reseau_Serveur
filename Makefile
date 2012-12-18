Employe: Employe.o Authentification.o
	g++ -o employe Employe.o Authentification.o
    
Serveur: Serveur.o sock.o sockdist.o ListeEmploye.o AnalyseurTrame.o
	g++ -o serveur Serveur.o sock.o sockdist.o ListeEmploye.o AnalyseurTrame.o
    		
sock.o: sock.cpp sock.h
	g++ -c sock.cpp

sockdist.o: sockdist.cpp sockdist.h
	g++ -c sockdist.cpp

serveur.o:Serveur.cpp Serveur.h
	g++ -c Serveur.cpp
	
AnalyseurTrame.o:AnalyseurTrame.cpp AnalyseurTrame.h
	g++ -c AnalyseurTrame.cpp

Authentification.o:Authentification.cpp Authentification.h
	g++ -c Authentification.cpp
	
#Consultation.o:ConsultationRapport.cpp  ConsultationRapport.h \
	g++ -c ConsultationRapport.cpp
	
Controleur.o:Controleur.cpp
	g++ -c Controleur.cpp
	
#demande_rapport.o:DemandeRapport.cpp DemandeRapport.h \
	g++ -c DemandeRapport.cpp
	
Employe.o:Employe.cpp Employe.h
	g++ -c Employe.cpp
	
EmployeRapport.o:EmployeRapport.cpp EmployeRapport.h
	g++ -c EmployeRapport.cpp

EmployeRole.o:EmployeRole.cpp EmployeRole.h
	g++ -c EmployeRole.cpp
		
ListeEmploye.o:ListeEmploye.cpp ListeEmploye.h  EmployeRole.o
	g++ -c ListeEmploye.cpp -o EmployeRole.o
		
ListeEmployeRapport.o:ListeEmployeRapport.cpp ListeEmployeRapport.h
	g++ -c ListeEmployeRapport.cpp
	
Personnel.o:Personnel.cpp Personnel.h
	g++ -c Personnel.cpp
	
#RedactionRapport.o:RedactionRapport.cpp RedactionRapport.h \
	g++ -c RedactionRapport.cpp
	
#sauvegarde.o:sauvergade.c sauvergade.h\
	gcc `pkgconfig --cflags --libs gtk+-2.0` sauvegarde.c
