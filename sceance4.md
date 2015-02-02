1. ctx + fs
2. recouvrement d'io

Plusieurs implémentations possibles :

+ contexte séparer : lecture disk = tache productrice
  - seek devient yield
  - que devient l'interruption ? réactivation de la tache fs. Modification de l'ordonnanceur (tache critique)

+ ordonnancement : on bloque les taches en attente d'io
+ logique d'état avec les interruptions
