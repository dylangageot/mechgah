/**
 * \file instructions.c
 * \author Hily Nicolas
 * \version 1.0
 * \date 2019-03-05
 */

 #include "instructions.h"

 uint8_t AddressingMode_Execute(CPU *cpu, Instruction *inst){
     uint16_t address = 0;
     uint8_t pageCrossed = 0;
     switch (inst->opcode.addressingMode) {
        case IMP : /* Implied : Nothing to do*/
                    break;

        case ACC : /*Accumulator : Nothing to do */
                    break;

        case ZEX :
                    address = (0x00FF) & (uint16_t)(inst->opcodeArg[0] + cpu->X);
                    inst->dataMem = cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address);
                    break;

        case ZEY :
                    address = (0x00FF) & (uint16_t)(inst->opcodeArg[0] + cpu->Y);
                    inst->dataMem = cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address);
                    break;

        case INX :
                    address = (0x00FF) & (uint16_t)(inst->opcodeArg[0] + cpu->X);
                    uint8_t lWeight = *(cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address));
                    uint8_t hWeight = *(cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address + 1));
                    address = (uint16_t)(hWeight<<8 + lWeight);
                    inst->dataMem = cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address);
                    break;

        case INY :
                    address = (0x00FF) & (uint16_t)(inst->opcodeArg[0]);
                    uint8_t lWeight = *(cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address));
                    uint8_t hWeight = *(cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address + 1));
                    address = (uint16_t)(hWeight<<8 + lWeight);
                    if ((address & 0xFF00) == ((address + cpu->Y) & 0xFF00))
                        inst->pageCrossed = 1;
                    address = (uint16_t)(hWeight<<8 + lWeight + cpu->Y);
                    inst->dataMem = cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address);
                    break;

        case IMM : /* Immediate : Nothing to do*/
                    break;

        case ZER :
                    address = (0x00FF) & (uint16_t)(inst->opcodeArg[0]);
                    inst->dataMem = cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address);
                    break;

        case REL : /* Immediate : Nothing to do*/
                    break;

        case ABS :
                    address = (uint16_t)(inst->opcodeArg[1]<<8 + inst->opcodeArg[0]);
                    inst->dataMem = cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address);
                    break;

        case ABX :
                    address = (uint16_t)(inst->opcodeArg[1]<<8 + inst->opcodeArg[0]);
                    if ((address & 0xFF00) == ((address + cpu->X) & 0xFF00))
                        inst->pageCrossed = 1;
                    address = (uint16_t)(inst->opcodeArg[1]<<8 + inst->opcodeArg[0] + cpu->X);
                    inst->dataMem = cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address);
                    break;

        case ABY :
                    address = (uint16_t)(inst->opcodeArg[1]<<8 + inst->opcodeArg[0]);
                    if ((address & 0xFF00) == ((address + cpu->X) & 0xFF00))
                        inst->pageCrossed = 1;
                    address = (uint16_t)(inst->opcodeArg[1]<<8 + inst->opcodeArg[0] + cpu->Y);
                    inst->dataMem = cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address);
                    break;

        case ABI :
                    address = (uint16_t)(inst->opcodeArg[1]<<8 + inst->opcodeArg[0]);
                    uint8_t lWeight = *(cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address));
                    uint8_t hWeight = *(cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address + 1));
                    address = (uint16_t)(hWeight<<8 + lWeight);
                    inst->dataMem = cpu->rmap->get(cpu->rmap->mapperData, AS_CPU, address);
                    break;

        default :
                    return 0;
                    break;
     }
     return 1;
 }
