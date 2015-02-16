# Séance 6

## gestion des ctx avec les core 
 - A/ Liste de contexte unique et implémentation d' un verrou pour affecter 1 contexte avec 1 core
 - B/ Une liste de ctx par core

## Verrou
 - mise en place d'attente active afin de pouvoir laisser un coeur tourner et attendre que le coeur lache le verrou pour que le contexte sur l'autre coeur puisse le reprendre
