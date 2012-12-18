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

*Classe de connexion pour les clients (chaque client a son dossier sur le serveur)
*Classe de rédaction d'un rapport
*Classe envoi de rapport (par bloc)-> dépôt dans chaque dossier correspondant à l'utilisateur
*Classe de déconnexion -> gérer la déconnexion du contrôleur
*Classe d'envoie de liste d'employé avec adresse ip de ceux-ci
*Classe de renvoie de rapport en pdf 
*Classe d'enregistrement de rapport (sous forme texte) -> appels systèmes (concaténation blocs par blocs) -> déjà fait par la prof
*Classe de consultation de rapport (envoie sous forme pdf) 
*Classe de vérification si l'employé doit rédiger un rapport

Schémas algorithmes:
--------------------
Employé:
Connexion
Si la liste d'employé existe
Alors l'employé s'authentifie
  Si l'authentification est bonne
		on vérifie s'il y'a une demande de rapport
		S'il y en a un
		rédaction du rapport-> accès en prenant le verrou de cette ressource
		envoi du rapport
		recevoir le rapport final en pdf
		Fin si
	Fin si
Fin si
Déconnexion

Serveur:

Attente d'une demande de connexion
Vérification de la connexion
Si vérification ok
Alors
	Si 
Sinon rejet
Fin si
	</body>
</html>
 
