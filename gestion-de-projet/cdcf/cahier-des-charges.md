# Emulateur NES

## Presentation du projet

Ce projet porte sur l'émulation du système de la console de jeux NES. C'est à dire reproduire son comportement hardware et software de manière logicielle. La console de jeux NES est une console de jeux sortie en 1985 et développée par la société japonaise Nintendo.

## Objectifs

- Emuler le fonctionnement de la console NES de Nintendo
- Être en capacité d'émuler la plupart des jeux sous license
- Développer pour fonctionner sous Linux

## Comment fonctionne la NES

### CPU

Nico. H

### PPU

La PPU (Picture Processing Unit) a pour fonction de gérer l'affichage. La résolution des images produites sont de 256x240 pixels. Son fonctionnement est parallèle et indépendant de la CPU. Ainsi, la PPU possède son propre espace d'adressage.

#### Frame rendering

Le rendu des images/frames s'exécute à 60 Hz pour une NES NTSC et 50 Hz pour la version PAL. La PPU fonctionne avec une fréquence d'horloge 3 fois supérieur à celle de la CPU, ainsi **3 pixels sont rendu à l'écran en un cycle CPU**. On appelle scanline le rendu d'une ligne de pixels, comprenant également les pixels invisibles nécessaire au timing des signaux composites. Ainsi on décompte 262 scanlines, chacune d'entre elle étant composé de 341 pixels. Lorsque la PPU a fini de rendre l'image visible à l'écran, une succession de 20 scanlines prend place, on appelle cet période **vertical blank**. C'est durant cet période que l'on doit écrire dans la mémoire vidéo pour éviter de potentiels artefacts.

#### Pattern Tables

Pour pallier aux contraintes de l'époque, les données décrivant les informations à l'écran sont grossières : on ne stock pas en brut la couleur d'un pixel à une coordonnées précise, à la place, on créer des blocs contenant les informations nécessaires (dessin, couleur) puis on vient les appeler dans une table mémoire pour les afficher à l'écran. Un bloc élémentaire est constitué de **8x8 pixels** et est appelé **un pattern**. Ces patterns permettent de décrire le décor (background) et les personnages/objets à l'écran (sprites).

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
- **UNROM** (mapper 2): Egalement développé par Nintendo et utilisé pour des jeux comme Mega man ou Castlevania, qui permet de choisir la banque de donnée chargée sur les premiers *16KB* et fixe les *16KB* de fin à la dernière banque de données.
- **MMC1** (mapper 1) : Mapper très utilisé, notemment pour The Legend of Zelda. Il offre une grande flexibilité sur la PRG-ROM et permet de charger la CHR-ROM.

Il en existe plus d'une centaine.

## Spécificités de l'émulateur

Dylan
Lecture de ROM
Fonctionnalités amélioratives

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
