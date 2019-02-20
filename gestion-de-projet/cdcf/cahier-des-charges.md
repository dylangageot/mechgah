# Emulateur NES

## Presentation du projet

Ce projet porte sur l'émulation du système de la console de jeux NES. C'est à dire reproduire son comportement hardware et software de manière logicielle. La console de jeux NES est une console de jeux sortie en 1985 et développée par la société japonaise Nintendo.

## Objectifs

- Emuler le fonctionnement de la console NES de Nintendo
- Être en capacité d'émuler la plupart des jeux sous license
- Développer pour fonctionner sous Linux

## Outils de développement

Nous avons choisi développer notre émulateur à travers un Makefile, de tel manière à ce que chacun puisse utiliser son propre IDE (vim, Atom, CodeBlocks). A l'avenir, nous utiliserons CMake pour généraliser la compilation et la reprise du projet sur n'importe quel IDE.

## Comment fonctionne la NES

### CPU

#### Representation de la mémoire

![Representation-memoire](https://people.ece.cornell.edu/land/courses/ece4760/FinalProjects/s2009/bhp7_teg25/bhp7_teg25/index_files/image012.jpg)

#### Fonctionnement du processeur 6502

Le processeur est de type 8 bits. Ses registres de travail sont donc aussi de taille 8 bits. Cela implique que c'est aussi la taille maximale des données manipulables.

Cependant, le Programme Counter (PC) est lui de taille 16 bits. Le domaine d'adressage disponible est ainsi de 64Ko.

Il possède en plus un Multi-Memory Controller (MMC) qui permet d'adresser plus de mémoire. (Voir partie mémoire).

Le processeur possède un jeu d'instruction capable de manipuler les 64 Ko de mémoire et 6 registres.

**Registres 8 bits** :
  - **Stack register** : Garde l'adresse du haut de la pile, pile permettant de sauvegarder des données lors de l’exécution d'une fonction.

  - **Processor Status** : Registre de flags, il possède en tout 7 flags car le bit numéro 5 du registres n'est pas utilisé.  
      - Bit 0 : Carry out (C)  
      - Bit 1 : Zero flag (Z)
      - Bit 2 : Interrupt Disable Flag (I)
      - Bit 3 : Decimal mode (D)
      - Bit 5 : N/A
      - Bit 6 : Break Command (B)
      - Bit 7 : Negative Flag (N)


  - **Accumulator** : Registre de travail principal. Utilisé pour tous les instructions arithmétiques et logiques.

  - **Registre X** : Utilisé pour les adressages indexés et le contrôle des boucles.

  - **Registre Y** : Comparable au registre X mais possède moins de fonctionnalités.

**Registres 16 bits**
  - Program Counter : Adressage des 64 Ko de mémoire. Il contient l'adresse de la prochaine instruction à exécuter.

#### Les modes d'adressages

Le processeur 6502 possède 12 modes d'adressage utilisés par les instructions.

- Adressage immédiat : #$??
- Adressage absolu : $????
- Adressage page zéro : $??
- Adressage indirect absolu : ($????)
- Adressage absolu indexé : $????,X
- Adressage indexé page zéro : $??,X		
- Adressage indexé indirect	: ($??,X)
- Adressage indirect indexé : ($??),X
- Adressage relatif	: $??  ->signé
- Adressage implié : transparent dans l'instruction

#### Les instructions

Le processeur possède un jeu de 56 mnémoniques (instructions). Certaines peuvent faire l'objet de plusieurs modes d'adressages.

**Exemple de deux instructions**

**ADC** : Flags utilisés : N,Z,C,V

Additionne la valeur contenu dans l'Accumulator avec l'opérande désigné par le mode d'adressage et le bit de retenue. Le résultat est ensuite placé dans l'Accumulator. Il y a aussi une mise à jours des flags. Pour effectuer une addition vierge, il faut mettre à zéro le bit de retenue (C). Cette instruction peut utilisée 8 modes d'adressages différents.

**LDA** - Load Accumulator : Flags Utilisés : N,Z

On passe en paramètre une adresse. L'opérande situé à cette adresse en mémoire centrale est chargée dans l'Accumulator puis la valeur est évaluée puis déterminer les flags N et Z.  


### PPU

La PPU (Picture Processing Unit) a pour fonction de gérer l'affichage. La résolution des images produites sont de 256x240 pixels. Son fonctionnement est parallèle et indépendant de la CPU. Ainsi, la PPU possède son propre espace d'adressage.

#### Frame rendering

Le rendu des images/frames s'exécute à 60 Hz pour une NES NTSC et 50 Hz pour la version PAL. La PPU fonctionne avec une fréquence d'horloge 3 fois supérieur à celle de la CPU, ainsi **3 pixels sont rendu à l'écran en un cycle CPU**. On appelle scanline le rendu d'une ligne de pixels, comprenant également les pixels invisibles nécessaire au timing des signaux composites. Ainsi on décompte 262 scanlines, chacune d'entre elle étant composé de 341 pixels. Lorsque la PPU a fini de rendre l'image visible à l'écran, une succession de 20 scanlines prend place, on appelle cet période **vertical blank**. C'est durant cet période que l'on doit écrire dans la mémoire vidéo pour éviter de potentiels artefacts.

#### Pattern Tables

Pour pallier aux contraintes de l'époque, les données décrivant les informations à l'écran sont grossières : on ne stock pas en brut la couleur d'un pixel à une coordonnées précise, à la place, on créer des blocs contenant les informations nécessaires (dessin, couleur) puis on vient les appeler dans une table mémoire pour les afficher à l'écran. Un bloc élémentaire est constitué de **8x8 pixels** et est appelé **fun pattern**. Ces patterns permettent de décrire le décor (background) et les personnages/objets à l'écran (sprites).

La table des patterns est contenue dans une ROM (appelé CHR-ROM) sur le circuit imprimé de la cartouche de jeu. Cette ROM est généralement d'une taille de 8kB, permettant de **stocker 512 patterns**. Chaque pattern occupe 16 octets de mémoire, décrivant ainsi les couleurs avec deux bits par pixels, **soit 4 couleurs possibles pour sur un pattern** (voir l'illustration ci-dessous). Nous verrons dans la partie sur les palettes de couleur comment fonctionne le mécanisme de coloriage.

![pattern-pixel-color](https://s3.amazonaws.com/n3s/chr.png)

La figure ci-dessous illustre le contenu de la table des patterns pour le jeu Super Mario Bros. On retrouve des élements de background comme des sprites.

![pattern-table](https://s3.amazonaws.com/n3s/patterntable.png)

#### Colour Palette

La NES est capable d'afficher **52 couleurs**, cependant, dû aux limitations techniques de l'époque, seulement quelques couleurs pourront être affichés sur une frame. L'objectif de cette limitation est de limité l'espace mémoire qu'occupera les images. Ainsi, la solution fut de créer **des palettes de 4 couleurs** : **4 palettes pour le background et 4 autres pour les sprites**. Les éléments affichés à l'écran feront référence à une des palettes de couleurs (grâce à un index) afin d'être coloriés correctement.

Sur l'illustration qui suit, les quatre palettes du haut correspondent aux palettes pour les sprites. On remarque pour chacun d'eux que la dernière couleur semble être noire, or en réalité il s'agit de **la transparence** : un pixel possédant cet priorité laisse entrevoir le background. Juste en dessous, on retrouve les palettes pour le background.

![color-palette](https://s3.amazonaws.com/n3s/palettes.png)

#### Name Tables

Le background est constitué **d'une grille de 32x30 patterns**. En mémoire, on appelle cet grille/tableau une **name table**. On associé à cet espace une **attribute table**, une table permettant de décrire quel palette de couleur utilisé pour chaque pattern. L'espace d'adressage de la PPU permet **l'usage de 4 name tables**, ceci-dit, seulement deux sont physiquement présente sur la NES, les deux autres doivent provenir de la cartouche si nécessaires.

L'usage de multiple name tables permet d'effectuer du **scrolling**, principe utilisé dans les jeux de plateformes pour se déplacer dans un niveau (comme dans Super Mario Bros par exemple).

![nametable-scrolling](https://wiki.nesdev.com/w/images/a/ae/SMB1_scrolling_seam.gif)

En fonction de comment le joueur évolue sur la carte (verticalement ou horizontalement), il est possible d'organiser les name tables très précisément avec le principe de [**mirroring**](https://wiki.nesdev.com/w/index.php/Mirroring).

#### Object Attribute Memory

La NES est capable d'afficher 64 sprites sur une même frame. Cet caractéristique est toutefois contraintes par la limite de **8 sprites par scanlines**. Dans le cas d'un overflow de sprite sur une scanline, un bit est levé dans les registres d'états de la PPU. Les informations sur les sprites affichés à l'écran sont a écrire dans l'Object Attribute Memory (OAM) ou aussi appelé SPR-RAM (sprite RAM). Cet espace mémoire est **remis à zéro à chaque fois qu'une image a été rendu à l'écran**, ainsi, le jeu doit réécrire à chaque rendu pour que les sprites puissent être ré-affiché à l'écran.

Chaque sprite est représenté par 4 octets dans l'OAM :
- Position sur l'axe Y (bytes 0)
- Index du pattern à afficher (bytes 1)
- Attribut du sprite (bytes 2)
  - Palette de couleur utilisé
  - Priorité du sprite vis-à-vis du background
  - Mirroir horizontale
  - Mirroir verticalement
- Position sur l'axe X (bytes 3)

La priorité entre les différents sprites est gérée par l'ordre dans lequel les sprites sont écris dans l'OAM.

L'OAM peut être intégralement **écris en DMA** depuis le CPU, généralement après chaque vertical blank, dans le handler de l'interruption NMI.

### APU

Baptiste

### Mapper mémoire

La NES a besoin de **charger le contenu du jeux** dans la **mémoire de la CPU** (cf paragraphe sur la CPU). De ce fait, elle réserve 32KB pour la mémoire programme, ou PRG-ROM, entre *Ox8000* et *0xFFFF*. De plus, la PPU réserve *8KB* de ROM appelée CHR-ROM, pour stocker des élément graphiques du jeu.

Un cartouche de jeux contenant *16KB* de programme est chargée deux fois : à *0x8000* et à *0xC000*, et une cartouche contenant *32KB* de programme est chargée sur la totalité de la plage réservée. Cette taille suffisait pour les premier jeux, mais très vite les jeux étaient réalisé sur plusieures banques de *32KB*.

La NES utilise donc du hardware intégré à la cartouche et appelé MMC (Memory Management Chip), ou mapper mémoire, afin de savoir quelle partie de la cartouche doit être chargé dans la PRG-ROM. Lorsque le système a besoin d'accéder à des données situées **hors de la banque de donnée actuellement chargée**, le programme demande à la MCC de charger la banque de donnée d'intérêt dans la PRG-ROM, effaçant ainsi les données chargées.

Voici un courte description des MMC basiques :
- **NROM** (mapper 0) : Le premier mapper, développé par Nintendo. Les banques de données sont fixes et le chargement des données est celui décrit au paragraphe 2. Il n'existe pas de gestion du chargement de donnée dans la ROM de la PPU.
- **UNROM** (mapper 2): Également développé par Nintendo et utilisé pour des jeux comme Mega man ou Castlevania, qui permet de choisir la banque de donnée chargée sur les premiers *16KB* et fixe les *16KB* de fin à la dernière banque de données.
- **MMC1** (mapper 1) : Mapper très utilisé, notamment pour The Legend of Zelda. Il offre une grande flexibilité sur la PRG-ROM et permet de charger la CHR-ROM.

Il en existe plus d'une centaine.

## Spécificités de l'émulateur

### Format des fichiers iNES

Le format de fichier iNES (extension **.nes**) est très répandue pour le stockage des données des cartouches de jeux NES. Le fichier binaire est constitué d'un header (occupant 16 octets) décrivant les caractéristiques de la cartouche (mapper utilisé et taille des mémoires). Ce header est suivi par la mémoire programme (PRG-ROM, multiple de 16kB) puis la mémoire des patterns (CHR-ROM, multiple de 8kB).

### Types d'émulation

Il existe différentes méthodes pour émuler un support. Dans le cas de la NES, chaque composant (CPU, PPU, Mapper) fonctionne en parallèle, ce qui implique une quantité importante de données à traiter. La plupart des émulateurs développés au début de l'an 2000 devait faire en sorte d'être optimisé au mieux pour ne pas être limité par le processeur de la machine. Ainsi, l'une des techniques utilisés était la **prédiction**, une méthode permettant de prédire l'usage de tel ou telle composants et ne l’exécuter seulement lorsque c'est nécessaire. Aujourd'hui, nos processeurs n'ont rien à envier à l'ancienne génération, ces problématiques ne sont donc plus de l'ordre du jour et les émulateurs peuvent se permettre d'être **précis (accurate)**. On entend par précis le fait d’exécuter les composants à chaque instant de l'émulation, ce qui rapproche du fonctionnement machine.

Dans notre cas, nous avons choisi de concevoir un émulateur précis puisque aujourd'hui la quasi totalité des ordinateurs sont  capables de gérés un tel processus et parce que la prédiction nous aurais demandé un temps de développent bien plus élevé.

### Fonctionnalités des émulateurs

Voici une liste non-exhaustive de fonctionnalités que l'on retrouve sur les émulateurs NES :

- **Save** : permet de sauvegarder le contexte d'exécution de la machine pour reprendre la progression de son jeu plus tard
- **Movie/Tool-assisted speedrun** : permet de sauvegarder une série d'événement pouvant être re-exécuter plus tard
- **Pause/Resume** : stopper/reprendre l'exécution de son jeu
- **Speed x** : accélérer l'exécution de son jeu, pouvant être utile pendant des scènes de dialogues
- **Rescale** : agrandir l'affichage pour avoir un meilleur confort visuel
- **Configuration des touches claviers** : choisir ses touches claviers à associé aux contrôles de la NES

## Versions

### V0

1 seul mapper (pour DK par ex)

### V1

plusieurs mappers
interface de gestion

### V2

### V3

- Se rapprocher au plus du fonctionnement hardware de la NES
- Developper quelques mappers (deux ou trois) (mécanisme qui permet de gérer des ROM supplémentaire sur les cartouches)
- Avoir des performances graphiques fluides (NTSC - 60 FPS)
- Prise en charge du clavier AZERTY/QWERTY
- Interface de gestion (choix des ROMs, raccoucis clavier, paramètres graphiques et sonores)
- Pouvoir mettre à l'échelle l'affichage (proportionnel)
- Pouvoir enregistrer le contexte d'exécution d'un jeu pour sauvegarder sa partie
- Pouvoir accelèrer le gameplay (speed x fois) et mettre en pause
- (Optionnel) Developpement de l'APU (moteur sonore)
- (Optionnel) Prise en charge de manette