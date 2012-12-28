<html>
<body>
<pre>
Projet_Reseau
=============
Projet d'enregistrement de rapports d'activités dans une entreprise:

Employé:
--------
*Se connecter
*Vérifier s'il doit rédiger rapport
*Rédiger et envoyer le contenu du rapport (par bloc de message)

*Juste avant la déconnexion de l'employé le serveur envoie la version final pdf
à l'employé et prévient le contrôleur.
Contrôleur:
-----------
*Se connecter
*Envoyer la liste des employés (ceux qui doivent rédiger un rapport)
*Vérifier la présence d'un rapport (doit attendre la déconnexion d'un employé pour consulter)
*Consulter un rapport 

Serveur:
--------
*On suppose que le premier utilisateur connecté devient le contrôleur
*Recevoir une liste d'employés
*Recevoir les rapports sous forme texte
*Renvoyer au format pdf à l'expéditeur
*Enregistrer les rapports
*Envoyer au contrôleur aussi s'il y a eu demande de sa part

Outils:
-------
*Eclipse
*NetBeans
*GitHub

Langages de programmations:
---------------------------
*C++ -> coder en objet 

Protocole de connexion/déconnexion:
-----------------------------------
*TCP/IP 

Architecture  de l'application "Enregistrement de rapports d'activités dans une entreprise":

*Il n'y a pas de phase d'inscription c'est nous qui établissons la liste des employés et désignons le contrôleur dans le fichier

*Fonction de connexion pour les clients (chaque client a son dossier sur le serveur)
*Fonction de rédaction d'un rapport
*Fonction envoi de rapport (par bloc)-> dépôt dans chaque dossier correspondant à l'utilisateur
*Fonction de déconnexion -> gérer la déconnexion du contrôleur
*Fonction d'envoie de liste d'employé avec adresse ip de ceux-ci
*Fonction de renvoie de rapport en pdf 
*Fonction d'enregistrement de rapport (sous forme texte) -> appels systèmes (concaténation blocs par blocs) -> déjà fait par la prof
*Fonction de consultation de rapport (envoie sous forme pdf) 
*Fonction de vérification si l'employé doit rédiger un rapport

Schémas algorithmes:
--------------------
Employé:
Connexion
L'employé s'authentifie
  Si l'authentification est bonne
		en fonction du role attribué (controleur ou employe)
		*Employe
		rédaction du rapport section par section
		envoi du rapport section par section
		recevoir le rapport final en pdf
		*Contoleur
		créer une liste d'employe
		TQ le controleur n'est pas déconnecté
			consulter la liste des employés qui ont fait leur rapport
			consulter le rapport d'un employé
			se déconnecter
		Fin TQ
	Fin si
Fin si
Déconnexion

Serveur:
TQ vrai
	Attente d'une demande de connexion
	Analyse de la trame
	Si demande authentification
		Si c'est un controleur 
			on vérifie qu'il est le seul contoleur à l'être
			Si c'est le cas 
				on regarde si la liste établit par le controleur est vide
				Si c'est le cas
					on envoi une demande de création de liste
				Sinon 
					on envoi directement l'action suivante (voir côté employé)
				Fin si
			Sinon 
				on envoi un refus
			Fin si
		Sinon
			on vérifie que l'employé est dans le fichier de la liste établit par le controleur
			Si c'est le cas
				on enlève l'employé du fichier
				on le transfert dans le fichier des rapports en cours
				et on envoi le role attribué
			Sinon on envoi un refus
		Fin si
	Fin si
	Si c'est une demande d'ajout d'employe
		on ajout l'employé dans la liste établit par le controleur
	Fin si
	Si c'est une partie de rapport
		on l'ajout dans un fichier temporaire propre à l'employé
	Fin si
	Si c'est une fin de section (de rapport)
		on écrit la section dans le fichier .tex
	Fin si
	Si c'est une demande de liste rapport fait (de la part du controleur)
		on envoie la liste au controleur (employé par employé)
	Fin si
	Si c'est une demande de rapport (de la part du controleur)
		on envoi le rapport au controleur (en respectant la taille d'envoi)
		et on enlève l'employé (qui a été consulté par le controleur) de la liste des rapports faits
	Fin si
	Si c'est une deconnexion
		Si c'est le controleur qui l'a demandé
			on envoie juste un message de déconnexion
		Sinon
			on enlève de la liste des rapports en cours
			on le met dans la liste des rapports fait
			on envoi le rapport à l'employé (en respectant la taille d'envoi)
			on envoi un message de déconnexion
		Fin si
	Fin si
Fin TQ
	</body>
</html>
 
