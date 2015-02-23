A de Busschere
K Franquenouille

# Structure

    |
    |- src           # Contient les sources ``.c`` et ``.h``
    |- bin           # Contient les binaires compilés
    |- include       # Contient surtout ``hardware.h``
    |- lib           # Contient les librairies externes
    |- vdiskA.bin    # Le disque dur virtuel
    |- hardware.ini  # Configuration du materiel virtuel

# Dépendance

La bibliothèque ``readline`` pour architecture 32 bits est requise pour compiler le shell

# Compilation

À la racine du projet, ``make`` compile le projet et déplace les binaires dans ``bin/``

# Éxecution

Il faut s'assurer de diposer de ``hardware.ini`` dans le dossier courant.

Il faut créer un disque virtuel une première fois, ensuite, seul le shell suffit.

## Création du disque virtuel

Lancer les commandes suivantes pour créer un disque virtuel, et initialiser un système de fichier dessus

    ./bin/mkhd
    ./bin/mkvol --cylinder 1 --sector 1 --size 16000 --type base
    ./bin/mknfs --volume 0 --serial 4242 --name thenameisbill

Vous pouvez aussi éxécuter ``make volume``

## Utilisation du shell

Exemple d'utilisation du shell ``./bin/shell``

    Welcome in shell. Build date Jan 20 2015 16:09:59
    Type ``help`` to find out all the available commands in this shell
    Volume 0 has been automatically mounted. Use ``mount`` to mount another
    Binded timer interruptions
    Add ``&`` in front of a command to run it in a new context (in background)

    />help
    List of built-in commands :
    cat path
    cd path
    ed path (the EDitor)
    help
    ls [path]
    mount volume
    mkdir path
    rm path
    />ls
    bepo
    />cd bepo
    /bepo/>ed fichier
    please wait ... file opened, you can now enter your text
    ceci est le contenu de mon fichier
    avec une
    nouvelle ligne. Je termine l'entrée avec un retour à la ligne puis EOF (^D)

    /bepo/>cat fichier
    ceci est le contenu de mon fichier
    avec une
    nouvelle ligne. Je termine l'entrée avec un retour à la ligne puis EOF (^D)

    /bepo/>rm fichier
    /bepo/>cat fichier
    Cannot open /bepo/fichier
    /bepo/>compute 5000
    finished computing, result 1188911504
    /bepo/>&compute 50000
    /bepo/>ls

    /bepo/>finished computing, result 118891150

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

  + **fix second_sleep**
  + builtin ``ps`` : liste les contextes sur chaques coeurs
  + klock() /* bloque tant que pas de verrou */
  + kunlock()
  + ktailblock()
  + intégrer CORE_LOCK

# BUGS
