#Seance 3


##Travail :
Finr shell et trouver solution pour implémenter l'ordonnancement.



##Idée : 
--> quand 1 ctx accède disque, mettre ne place mutex d'acces disque et donner la main aux autres processus
	Si l'acces au disque est bloqué et que le processus souhaite acceder au disque on change de processus

	Utilisation des taches en background ? Si on a "&" on chage de processus et une fois la tache terminée, on affiche uniquement le resultat

	Utilisation du "|" ? utilisation du resultat comme entrée pour la prochaine commande

	Comment tester ?

