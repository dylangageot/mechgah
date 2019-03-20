# Compte-rendu réunion 28/02/2019

## Sujet de la réunion

- Discussion autour des headers et des structures de données
- Répartition des instructions à émuler
- Nécessité de décrire un standard pour le passage de paramètre entre AddressingMode_Execute() et une instructions

## Headers

La structure NES contient les élements/composants nécessaires à son émulation (CPU, PPU, APU, Mapper...). Les mappers étant de nature différente en fonction de la cartouche, nous avons choisi d'adapter la philosophie orienté objet pour que la structure NES et ses méthodes puissent s'abstraire du fonctionnement interne du mapper (principe d'encapsulation et classe virtuel utilisé pour décrire les mappers).

La structure CPU contient les registres internes de fonctionnement (Accumulator, Index X et Y, Program Counter...) ainsi qu'une pointeur pointant sur le mapper instancié dans la structure NES.

La structure Mapper contient 2 pointeurs de fonctions et un pointeur void pour permettent l'abstraction du mapper une fois instancié au chargement de la ROM.

## Répartition des instructions :

 - 14 instructions par personne

- Baptiste : Instructions relatives à la pile, aux interruptions et aux transferts inter-registres.

```c
uint8_t _CLI(CPU* cpu, uint8_t *arg);
uint8_t _PHA(CPU* cpu, uint8_t *arg);
uint8_t _PHP(CPU* cpu, uint8_t *arg);
uint8_t _PLA(CPU* cpu, uint8_t *arg);
uint8_t _PLP(CPU* cpu, uint8_t *arg);
uint8_t _RTI(CPU* cpu, uint8_t *arg);
uint8_t _RTS(CPU* cpu, uint8_t *arg);
uint8_t _SEI(CPU* cpu, uint8_t *arg);
uint8_t _TAX(CPU* cpu, uint8_t *arg);
uint8_t _TAY(CPU* cpu, uint8_t *arg);
uint8_t _TSX(CPU* cpu, uint8_t *arg);
uint8_t _TXA(CPU* cpu, uint8_t *arg);
uint8_t _TXS(CPU* cpu, uint8_t *arg);
uint8_t _TYA(CPU* cpu, uint8_t *arg);
```
- Nicolas Hily : Instructions de comparaison, incrémentation/décrémentation et gestion de flag du registre P

```c
uint8_t _CLC(CPU* cpu, uint8_t *arg);
uint8_t _CLD(CPU* cpu, uint8_t *arg);
uint8_t _CLV(CPU* cpu, uint8_t *arg);
uint8_t _CMP(CPU* cpu, uint8_t *arg);
uint8_t _CPX(CPU* cpu, uint8_t *arg);
uint8_t _CPY(CPU* cpu, uint8_t *arg);
uint8_t _DEC(CPU* cpu, uint8_t *arg);
uint8_t _DEX(CPU* cpu, uint8_t *arg);
uint8_t _DEY(CPU* cpu, uint8_t *arg);
uint8_t _EOR(CPU* cpu, uint8_t *arg);
uint8_t _INC(CPU* cpu, uint8_t *arg);
uint8_t _INX(CPU* cpu, uint8_t *arg);
uint8_t _INY(CPU* cpu, uint8_t *arg);
uint8_t _AND(CPU* cpu, uint8_t *arg);
```

- Dylan : Instructions d'addition, de branchement et bit test

```c
uint8_t _ADC(CPU* cpu, uint8_t *arg);
uint8_t _ASL(CPU* cpu, uint8_t *arg);
uint8_t _BCC(CPU* cpu, uint8_t *arg);
uint8_t _BCS(CPU* cpu, uint8_t *arg);
uint8_t _BEQ(CPU* cpu, uint8_t *arg);
uint8_t _BIT(CPU* cpu, uint8_t *arg);
uint8_t _BMI(CPU* cpu, uint8_t *arg);
uint8_t _BNE(CPU* cpu, uint8_t *arg);
uint8_t _BPL(CPU* cpu, uint8_t *arg);
uint8_t _BRK(CPU* cpu, uint8_t *arg);
uint8_t _BVC(CPU* cpu, uint8_t *arg);
uint8_t _BVS(CPU* cpu, uint8_t *arg);
uint8_t _JMP(CPU* cpu, uint8_t *arg);
uint8_t _JSR(CPU* cpu, uint8_t *arg);
```
- Nicolas Chabanis : Instructions de chargement/stockage et arithmétique/logique

```c

uint8_t _LDA(CPU* cpu, uint8_t *arg);
uint8_t _LDX(CPU* cpu, uint8_t *arg);
uint8_t _LDY(CPU* cpu, uint8_t *arg);
uint8_t _LSR(CPU* cpu, uint8_t *arg);
uint8_t _NOP(CPU* cpu, uint8_t *arg);
uint8_t _ORA(CPU* cpu, uint8_t *arg);
uint8_t _ROL(CPU* cpu, uint8_t *arg);
uint8_t _ROR(CPU* cpu, uint8_t *arg);
uint8_t _SBC(CPU* cpu, uint8_t *arg);
uint8_t _SEC(CPU* cpu, uint8_t *arg);
uint8_t _SED(CPU* cpu, uint8_t *arg);
uint8_t _STA(CPU* cpu, uint8_t *arg);
uint8_t _STX(CPU* cpu, uint8_t *arg);
uint8_t _STY(CPU* cpu, uint8_t *arg);
```
