# Structure

    |
    |- src       # Contient les sources ``.c`` et ``.h``
    |- bin       # Contient les binaires compilés
    |- include   # Contient surtou ``hardware.h``
    |- lib       # Contient les librairies externes

# Dépendance

La bibliothèque ``readline`` pour architecture 32 bits est requise pour compiler le shell

# Compilation

À la racine du projet, ``make`` compile le projet et déplace les binaires dans ``bin/``

# Éxecution

Il faut s'assurer de diposer de ``hardware.ini`` dans le dossier courant. Puis lancer le binaire

## Création du disque virtuel

```
./bin/mkhd
./bin/mkvol --cylinder 1 --sector 1 --size 16000 --type base
./bin/mknfs --volume 0 --serial 4242 --name thenameisbill
```

## Utilisation du shell

Exemple d'utilisation du shell ``./bin/shell``

```
/>help
List of built-in commands :
help
mount volume
mkdir path
/>mount 0
/>mkdir /plop
inumber_of_pathname(/)
/>^D
```

# Test

``make test`` lance une suite de test qui crée un disque virtuel, crée des partitions, des fichiers …

## Créer un fichier nommé

    $ CURRENT_VOLUME=0 ./bin/if_nfile
    Saisissez un nom de fichier :
    /monfichier
    inumber_of_pathname(/)
    9
    Le texte de mon fichier
    ^D
    $ CURRENT_VOLUME=0 ./bin/if_pfile "/monfichier"
    Le texte de mon fichier

# Todo

Un shell qui permet d'interagir avec le système de fichier :

  + ls
  + cd
  + mkdir
  + cat
  + creation de fichier

On peut regarder la bibliothèque ``cliparser`` pour gérer le coté shell interactif

# BUGS

