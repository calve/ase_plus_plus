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

# Todo

Intégrer if_nfile, if_pfile et if_cfile dans un premier temps

Un shell qui permet de se déplacer dans le système de fichier, avec ``ls``, ``cat``, ``cd``


# BUGS

## kevin/tpfs/dir.c

voir ligne 272 --> valeur != 0 pour forcer à créer

