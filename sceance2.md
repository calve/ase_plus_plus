Gestion des interruption disque

  + import des contextes
  + _sleep() après seek() avec interruption quand lecture/écriture

On ne peut pas attnedre longtemps après l'interruption
Que se passe-t-il si plusieurs programme veulent lire ?

objectif 1 : un programme fait des I/O, un autre qui fait du calcul
objectif 2 : deux programmes qui font des I/O

utilisation de sémaphore ?


Objectifs sur trois scéances :

+ recouvrir les I/O
+ entrelacer les programmes
+ optimiser les déplacements du disque

