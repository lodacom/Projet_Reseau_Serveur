Serveur: Serveur.o sock.o sockdist.o sauvegarde.o
	g++ -o serveur Serveur.o sock.o sockdist.o sauvegarde.o
    		
sock.o: sock.cpp sock.h
	g++ -c sock.cpp

sockdist.o: sockdist.cpp sockdist.h
	g++ -c sockdist.cpp

serveur.o:Serveur.cpp Serveur.h
	g++ -c Serveur.cpp
	
sauvegarde.o:sauvergade.c sauvergade.h
	gcc -c `pkgconfig --cflags --libs gtk+-2.0` sauvegarde.c
