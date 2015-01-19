# MAJ

Compilation OK

# Structure

    |
    |- src       # Contient les sources ``.c`` et ``.h``
    |- bin       # Contient les binaires compilés
    |- include   # Contient surtou ``hardware.h``
    |- lib       # Contient les librairies externes

# Compilation

À la racine du projet, ``make`` compile le projet et déplace les binaires dans ``bin/``

# Éxecution

Il faut s'assurer de diposer de ``hardware.ini`` dans le dossier courant. Puis lancer le binaire

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

Intégrer if_nfile, if_pfile et if_cfile dans un premier temps.

On peut ajouter un ``pathname`` ainsi qu'un tableau d'enfants à la structure ``entry_s``.

Un shell qui permet de se déplacer dans le système de fichier, avec ``ls``, ``cat``, ``cd``


# BUGS

## kevin/tpfs/dir.c

voir ligne 272 --> valeur != 0 pour forcer à créer

