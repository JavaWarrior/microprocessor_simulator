#include <stdlib.h>
#include <stdio.h>

#define MOVE 0
#define ADD 1
#define SUB 2
#define BRA 3
#define CMP 4
#define BEQ 5
#define BNE 6
#define EXG 7   /*EXG exchanges the contents of two registers */
#define STOP 8
char *inst_set []={"MOV","ADD","SUB","BRA","CMP","BEQ","BNE","EXG","STP"};
char *fileName = "input";
void readMemory(unsigned char arr[]){
    FILE * fp = fopen(fileName,"r");
    if(fp == NULL){
        goto clear_arr;
    }
    int i =0 , j =0;
    char input_char;
    for(; i < 256;i++){
        arr[i] =0;
        for(j=0;j<2;j++){
            arr[i] = arr[i]<<4;
            input_char = fgetc(fp);
            if(input_char == EOF){
                return;
            }
            //to hexa
            input_char -= ((input_char >= '0' &&input_char <='9')?'0':((input_char >= 'a' &&input_char <='f')?('a'-10):('A'-10)));
            arr[i]|= input_char;
        }
    }
    clear_arr:
    for (j = i ; j < 256;++j){
        arr[j] =0;
    }
}
void printInstruction(unsigned char operand , unsigned char opcode,unsigned short int direction,
unsigned short int amode,unsigned short int pc,unsigned short int d0,unsigned short int a0,
unsigned short int CCR, unsigned short int MAR, unsigned short int MBR,unsigned char memory[] ){
    printf("PC:\t%04x \tD0:\t%04x \tA0:\t%04x \tCCR:\t%04x\n"
        "MAR:\t%04x \tMBR:\t%04x \tIR:\t%01x%01x%02x\n",
        pc,d0,a0,CCR,MAR,MBR,opcode,amode+(direction<<2),operand);
    if(direction == 0){
        switch (amode)
           {
               case 0: {printf("%s\t [%02x] D0\n",inst_set[opcode],operand);           break;}     /* absolute */
               case 1: {printf("%s\t %02x D0\n",inst_set[opcode],operand);           break;}     /* literal */
               case 2: {printf("%s\t [A0 + %02x] D0\n",inst_set[opcode],operand);      break;}     /* indexed */
               case 3: {printf("%s\t [PC + %02x] D0\n",inst_set[opcode],operand);      break;}     /* PC relative */
           }
    }else{
        switch (amode)
           {
               case 0: {printf("%s\t D0 [%02x]\n",inst_set[opcode],operand);           break;}     /* absolute */
               case 1: {printf("%s\t D0 %02x\n",inst_set[opcode],operand);           break;}     /* literal */
               case 2: {printf("%s\t D0 [A0 + %02x]\n",inst_set[opcode],operand);      break;}     /* indexed */
               case 3: {printf("%s\t D0 [PC + %02x]\n",inst_set[opcode],operand);      break;}     /* PC relative */
           }
    }

}
int main(int argv , char **argc)
{
    unsigned short int PC=0;        /* program counter */
    unsigned short int D0=0;        /* data register */
    unsigned short int A0=0;        /* address register */
    unsigned short int CCR=0;       /* condition code register */
    unsigned short int MAR=0;         /* memory address register */
    unsigned short int MBR=0;         /* memory buffer register */
    unsigned short int IR=0;          /* instruction register */
//    unsigned short int operand;     /* the 8-bit operand from the IR */ //should be char
    unsigned char operand;     /* the 8-bit operand from the IR */ //should be char
    unsigned short int source;      /* source operand */
    unsigned short int destination=0; /* the destination value*/
//    unsigned short int opcode;      /*the 4-bit op-code from the IR*/
    unsigned char opcode;      /*the 4-bit op-code from the IR*/
    unsigned short int amode;       /*the 2-bit addressing mode */
    unsigned short int direction;   /*the 1-bit data direction flag */
//    unsigned short int memory[256]; /*the memory */
    unsigned char memory[256]; /*the memory */
    unsigned short int run=1;       /*execute program while run is 1 */

    /* Instruction format:                              */
    /* 7  6  5  4  3  2  1  0                           */
    /*       00 address mode = absolute                 */
    /*       01 address mode = literal                  */
    /*       10 address mode = indexed                  */
    /*       11 address mode = relative                 */
    /* Bit 2        1-bit direction (source/operand)    */
    /* Bit 3        not used                            */
    /* Bit 7 to 4   4-bit instruction code              */
    if(argv == 1){
        //run normal
    }else{
        fileName = argc[1];
    }
    readMemory(memory);
    /* main loop */

   while(run)
   {
       MAR= PC;                 /*PC to MAR*/
       PC= PC + 1;              /*increment PC*/
       MBR= memory[MAR];        /*get next instruction*/
       IR= MBR;                 /*copy MBR to IR*/
       opcode= IR;              /*store the op-code bits*/
       MAR= PC;                 /*PC to MAR*/
       PC= PC + 1;              /*increment PC*/
       MBR= memory[MAR];        /*get the operand*/
       IR= MBR;                 /*copy MBR to IR*/
       operand= IR;             /*store the operand bits*/
       amode= opcode & 0x03;    /*extract the address mode bits*/
       direction= (opcode & 0x04) >> 2;     /*get data direction 0 =  register to memory
                                                                 1 =  memory to register */
       opcode = opcode >> 4;     /*get the 4-bit instruction code*/

       /* use the address mode to get the source operand */

       switch (amode)
       {

           case 0: {source = memory[operand];           break;}     /* absolute */
           case 1: {source = operand;                   break;}     /* literal */
           case 2: {source = memory[A0 + operand];      break;}     /* indexed */
           case 3: {source = memory[PC + operand];      break;}     /* PC relative */
       }

       /* now execute the instruction */
        printInstruction(operand,opcode,direction,amode,PC,D0,A0,CCR,MAR,MBR,memory); // print the instruction
       switch (opcode)
       {
           case MOVE: {if (direction == 0) destination = D0;
                       else D0 = source;
                       if (D0 == 0) CCR = 1; else CCR = 0;
                       break;}

           case ADD: {if(direction==0)
                        {
                            destination= D0 + source;
                            if (destination == 0) CCR = 1; else CCR = 0;
                        }
                      else
                        {
                            D0 = D0 + source;
                            if (D0 == 0 ) CCR = 1; else CCR = 0;
                        }
                      break;}

           case SUB: {if (direction == 0)
                        {
                            destination = D0 - source;
                            if (destination == 0) CCR = 1; else CCR = 0;
                        }
                      else
                        {
                            D0 = D0 + source;
                            if (D0 == 0 ) CCR = 1; else CCR = 0;
                        }
                      break;}

           case BRA: {if (amode ==0) PC = operand;
                      if (amode ==1) PC = PC + operand;
                      break;}

           case CMP: {MBR = D0 - source;
                      if (MBR == 0) CCR = 1;
                      else CCR = 0;
                      break;}

           case BEQ: {if (CCR == 1)
                       {
                           if (amode ==0) PC = operand;
                           if (amode ==1) PC = PC + operand;
                       }
                      break;}

           case BNE: {if (CCR != 1)
                        {
                            if (amode ==0) PC = operand;
                            if (amode == 1) PC = PC + operand;
                        }
                      break;}

           case EXG: {MBR = D0; D0 = A0; A0 = MBR; break;}

           case STOP: {run = 0; break;}
       }

       /* save result in memory if register to memory */

       if (direction == 0)
            switch(amode)
            {
                case 0: { memory[operand] = destination;    break;}     /* absolute */

                case 1: {                                   break;}     /* literal */

                case 2: {memory[A0 + operand] = destination; break;}    /* indexed */

                case 3: {memory[PC + operand] = destination; break;}    /* PC relative */
            }
        getchar();
   }
    return 0;
}
