#include <stdio.h>
#include <string.h>
#include "expressao.h"

int main(){
    Expressao e;
    char ent[512];
    int op;
    do {
        printf("MENU INTERATIVO - CALCULADORA XP\n");
        printf("1-INF  2-POST  0-SAI\n");
        scanf("%d%*c", &op);

        if (op == 1) {
            printf("Infixa: ");
            fgets(ent, sizeof(ent), stdin);
            // ponto decimal
            for (char *p = ent; *p; ++p) if (*p == ',') *p = '.';
            ent[strcspn(ent, "\n")] = '\0';

            strncpy(e.inFixa, ent, sizeof(e.inFixa)-1);
            e.inFixa[sizeof(e.inFixa)-1] = '\0';
            strncpy(e.posFixa, getFormaPosFixa(e.inFixa), sizeof(e.posFixa)-1);
            e.posFixa[sizeof(e.posFixa)-1] = '\0';
            e.Valor = getValorInFixa(e.inFixa);
        }
        else if (op == 2) {
            printf("Posfixa: ");
            fgets(ent, sizeof(ent), stdin);
            for (char *p = ent; *p; ++p) if (*p == ',') *p = '.';
            ent[strcspn(ent, "\n")] = '\0';

            strncpy(e.posFixa, ent, sizeof(e.posFixa)-1);
            e.posFixa[sizeof(e.posFixa)-1] = '\0';
            strncpy(e.inFixa, getFormaInFixa(e.posFixa), sizeof(e.inFixa)-1);
            e.inFixa[sizeof(e.inFixa)-1] = '\0';
            e.Valor = getValorPosFixa(e.posFixa);
        }
        else if (op != 0) {
            printf("opc invalida\n");
        }

        if (op == 1 || op == 2) {
            printf("INF: %s\n", e.inFixa);
            printf("POS: %s\n", e.posFixa);
            printf("VAL: %g\n", e.Valor);
        }
    } while (op != 0);
    return 0;
}