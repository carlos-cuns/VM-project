// Carlos Cuns, Wilson Gil
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE 512
#define MAX_SIZE 100

typedef struct Instruction
{
    int op;
    int l;
    int m;
} Instruction;

// declare the starting array
int pas[ARRAY_SIZE] = {0};
int sp;
int bp;
int pc;
int start;
Instruction IR;

int base(int BP, int L)
{
    int arb = BP; // arb = activation record base
    while (L > 0) // find base L levels down
    {
        arb = pas[arb];
        L--;
    }
    return arb;
}

// read the input file and store each integer into the pas array
void readELF(char *filename)
{
    int buffer = 0;
    int i = 0;
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("failed to open file");
        exit(1);
    }
    while (fscanf(file, "%d", &buffer) != EOF)
    {
        pas[i] = buffer;
        i++;
    }
    // store the last index of the text
    start = i - 1;
    fclose(file);
}

// helper function to print inputs
void printText()
{
    printf("Printing file\n");
    for (int i = 0; i < sp; i += 3)
    {
        printf("%d %d %d\n", pas[i], pas[i + 1], pas[i + 2]);
    }
}

int main(int argc, char **argv)
{
    // make sure the cmd line prompt is valid
    if (argc < 2)
    {
        printf("Include a file name\n");
        exit(1);
    }
    char filename[MAX_SIZE];
    int halt = 1;
    strcpy(filename, argv[1]);
    readELF(filename);

    // initialize starting values
    pc = 0;
    sp = start;
    bp = sp + 1;
    IR.op = 0, IR.m = 0, IR.l = 0;
    printf("\t\t    PC  BP  SP  stack\n");
    printf("Initial values:     %d   %d  %d\n", pc, bp, sp);

    while (halt == 1)
    {
        // fetch
        // store each instruction into IR
        IR.op = pas[pc];
        IR.l = pas[pc + 1];
        IR.m = pas[pc + 2];
        // increment by three to get to next instruction
        pc += 3;

        // execute
        // each operation's logic is implemented parallel to the provided instructions
        switch (IR.op)
        {
        case 1: // LIT
            sp++;
            pas[sp] = IR.m;
            break;
        case 2: // OPR
            switch (IR.m)
            {
            case 0: // RTN
                sp = bp - 1;
                bp = pas[sp + 2];
                pc = pas[sp + 3];
                break;
            case 1: // ADD
                pas[sp - 1] = pas[sp - 1] + pas[sp];
                sp--;
                break;
            case 2: // SUB
                pas[sp - 1] = pas[sp - 1] - pas[sp];
                sp--;
                break;
            case 3: // MUL
                pas[sp - 1] = pas[sp - 1] * pas[sp];
                sp--;
                break;
            case 4: // DIV
                pas[sp - 1] = pas[sp - 1] / pas[sp];
                sp--;
                break;
            case 5: // EQL
                pas[sp - 1] = pas[sp - 1] == pas[sp];
                sp--;
                break;
            case 6: // NEQ
                pas[sp - 1] = pas[sp - 1] != pas[sp];
                sp--;
                break;
            case 7: // LSS
                pas[sp - 1] = pas[sp - 1] < pas[sp];
                sp--;
                break;
            case 8: // LEQ
                pas[sp - 1] = pas[sp - 1] <= pas[sp];
                sp--;
                break;
            case 9: // GTR
                pas[sp - 1] = pas[sp - 1] > pas[sp];
                sp--;
                break;
            case 10: // GEQ
                pas[sp - 1] = pas[sp - 1] >= pas[sp];
                sp--;
                break;
            }
            break;
        case 3: // LOD
            sp++;
            pas[sp] = pas[base(bp, IR.l) + IR.m];
            break;
        case 4: // STO
            pas[base(bp, IR.l) + IR.m] = pas[sp];
            sp--;
            break;
        case 5: // CAL
            pas[sp + 1] = base(bp, IR.l);
            pas[sp + 2] = bp;
            pas[sp + 3] = pc;
            bp = sp + 1;
            pc = IR.m;
            break;
        case 6: // INC
            sp += IR.m;
            break;
        case 7: // JMP
            pc = IR.m;
            break;
        case 8: // JPC
            if (pas[sp] == 0)
                pc = IR.m;
            sp--;
            break;
        case 9: // SYS
            switch (IR.m)
            {
            case 1: // WRITE
                printf("Output result is: ");
                printf("%d\n", pas[sp]);
                sp--;
                break;
            case 2: // READ
                printf("Please Enter an Integer: ");
                sp++;
                scanf("%d", &pas[sp]);
                break;
            case 3: // END
                halt = 0;
                break;
            }
            break;
        default: // "ERROR"
            printf("Not a valid ISA\n");
            return 1;
        }
        // figure out which mnemonic to print based on opcode
        printf("\t");
        switch (IR.op)
        {
        case 1:
            printf("LIT ");
            break;
        case 2:
            // figure out which operation is being done
            switch (IR.m)
            {
            case 0:
                printf("RTN ");
                break;
            case 1:
                printf("ADD ");
                break;
            case 2:
                printf("SUB ");
                break;
            case 3:
                printf("MUL ");
                break;
            case 4:
                printf("DIV ");
                break;
            case 5:
                printf("EQL ");
                break;
            case 6:
                printf("NEQ ");
                break;
            case 7:
                printf("LSS ");
                break;
            case 8:
                printf("LEQ ");
                break;
            case 9:
                printf("GTR ");
                break;
            case 10:
                printf("GEQ ");
                break;
            default:
                break;
            }
            break;
        case 3:
            printf("LOD ");
            break;
        case 4:
            printf("STO ");
            break;
        case 5:
            printf("CAL ");
            break;
        case 6:
            printf("INC ");
            break;
        case 7:
            printf("JMP ");
            break;
        case 8:
            printf("JPC ");
            break;
        case 9:
            printf("SYS ");
            break;
        default:
            break;
        }
        // printf("OP = %d ", IR.op);
        printf("%-2d %-2d   %-2d  %-2d  %-2d  ", IR.l, IR.m, pc, bp, sp);
        int i;
        for (i = (start + 1); i < (sp + 1); i++)
        {
            printf("%-2d ", pas[i]);
        }
        printf("\n");
    }
}