# Scéance 8

mise en place de klock 
--> reprendre sémaphore pour les réimplémenter

sémaphore : appel depuis un ctx. Ca peut etre bloquant et si c'est le cas, on passe la main à un autre ctx

1/ reprendre l'impémentation d'ASE et la mettre sur le multicore

2/ implémentation "spin-lock" : une courte attente active et on espere que sur un autre core un process progresse et va me liberer. Sinon on bloque comme le 1/.
--> consommer un peu de temps CPU en espérant que ca va debloquer

Ce 2eme choix n'a de sens que sur du multicore
