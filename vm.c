// Carlos Cuns, Wilson Gil
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

void readELF(char *filename)
{
    printf("Reading file\n");
    int buffer = 0;
    int i = 0;
    FILE *file = fopen(filename, "r");
    while (fscanf(file, "%d", &buffer) != EOF)
    {
        pas[i] = buffer;
        i++;
    }
    start = i - 1;
}

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
    if (argc < 2)
    {
        printf("Include a file name\n");
        return 1;
    }
    char filename[MAX_SIZE];
    int halt = 1;
    strcpy(filename, argv[1]);
    readELF(filename);
    // printText();

    // printf("Start index: %d\n", start);
    pc = 0;
    sp = start;
    bp = sp + 1;
    IR.op = 0, IR.m = 0, IR.l = 0;

    while (halt == 1)
    {
        IR.op = pas[pc];
        IR.l = pas[pc + 1];
        IR.m = pas[pc + 2];

        switch (IR.op)
        {
        case 1:
            sp++;
            pas[sp] = IR.m;
            break;
        case 2:
            switch (IR.m)
            {
            case 0:
                sp = bp - 1;
                bp = pas[sp + 2];
                pc = pas[sp + 3];
                break;
            case 1:
                pas[sp - 1] = pas[sp - 1] + pas[sp];
                sp--;
                break;
            case 2:
                pas[sp - 1] = pas[sp - 1] - pas[sp];
                sp--;
                break;
            case 3:
                pas[sp - 1] = pas[sp - 1] * pas[sp];
                sp--;
                break;
            case 4:
                pas[sp - 1] = pas[sp - 1] / pas[sp];
                sp--;
                break;
            case 5:
                pas[sp - 1] = pas[sp - 1] == pas[sp];
                sp--;
                break;
            case 6:
                pas[sp - 1] = pas[sp - 1] != pas[sp];
                sp--;
                break;
            case 7:
                pas[sp - 1] = pas[sp - 1] < pas[sp];
                sp--;
                break;
            case 8:
                pas[sp - 1] = pas[sp - 1] <= pas[sp];
                sp--;
                break;
            case 9:
                pas[sp - 1] = pas[sp - 1] > pas[sp];
                sp--;
                break;
            case 10:
                pas[sp - 1] = pas[sp - 1] >= pas[sp];
                sp--;
                break;
            }
            break;
        case 3:
            sp++;
            pas[sp] = pas[base(bp, IR.l) + IR.m];
            break;
        case 4:
            pas[base(bp, IR.l) + IR.m] = pas[sp];
            sp--;
            break;
        case 5:
            pas[sp + 1] = base(bp, IR.l);
            pas[sp + 2] = bp;
            pas[sp + 3] = pc;
            bp = sp + 1;
            pc = IR.m;
            break;
        case 6:
            sp += IR.m;
            break;
        case 7:
            pc = IR.m - 3;
            break;
        case 8:
            if (pas[sp] == 0)
                pc = IR.m;
            sp--;
            break;
        case 9:
            switch (IR.m)
            {
            case 1:
                printf("Output result is: ");
                printf("%d\n", pas[sp]);
                sp--;
                break;
            case 2:
                printf("Please Enter an Integer: ");
                sp++;
                scanf("%d", &pas[sp]);
                break;
            case 3:
                halt = 0;
                break;
            }
            break;
        default:
            printf("Not a valid ISA\n");
            return 1;
        }
        printf("\t");
        switch (IR.op)
        {
        case 1:
            printf("LIT ");
            break;
        case 2:
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
        printf("%d ", IR.l);
        printf("%d\n", IR.m);
        pc += 3;
    }
}