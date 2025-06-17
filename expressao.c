#include "expressao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define _USE_MATH_DEFINES
#include <math.h>
#ifndef M_PI
  #define M_PI 3.14159265358979323846f
#endif

#define TAM 512

// detecta sinal unário de menos (índice idx em string in)
static int isUnaryMinus(const char *in, int idx) {
    int j = idx - 1;
    while (j >= 0 && isspace((unsigned char)in[j])) j--;
    if (j < 0) return 1;
    if (in[j] == '(') return 1;
    if (strchr("+-*/%^", in[j])) return 1;
    return 0;
}

static int isOp(char *t) {
    return strlen(t) == 1 && strchr("+-*/%^", t[0]);
}
static int prec(char *t) {
    if (!strcmp(t, "+") || !strcmp(t, "-")) return 1;
    if (!strcmp(t, "*") || !strcmp(t, "/") || !strcmp(t, "%")) return 2;
    if (!strcmp(t, "^")) return 3;
    return 0;
}
static int isFun(char *t) {
    return !strcmp(t, "sen") || !strcmp(t, "cos") ||
           !strcmp(t, "tg")  || !strcmp(t, "raiz")||
           !strcmp(t, "log");
}
static int bal(const char *s) {
    int c = 0;
    for (int i = 0; s[i]; i++) {
        if (s[i] == '(') c++;
        if (s[i] == ')' && --c < 0) return 0;
    }
    return c == 0;
}

char *getFormaPosFixa(char *in) {
    // converte vírgulas em pontos
    for (char *q = in; *q; ++q)
        if (*q == ',') *q = '.';

    static char out[TAM];
    char *stk[TAM];
    int top = -1, i = 0, p = 0;
    out[0] = '\0';

    if (!bal(in)) {
        fprintf(stderr, "[Erro] parênteses desequilibrados\n");
        return out;
    }

    while (in[i]) {
        if (isspace((unsigned char)in[i])) { i++; continue; }

        // número com sinal unário permitido
        if ((in[i] == '-' && isUnaryMinus(in, i) && (isdigit(in[i+1]) || in[i+1] == '.')) ||
             isdigit((unsigned char)in[i]) ||
            (in[i] == '.' && isdigit((unsigned char)in[i+1]))) {

            if (in[i] == '-') out[p++] = in[i++];
            while (isdigit((unsigned char)in[i]) || in[i] == '.')
                out[p++] = in[i++];
            out[p++] = ' ';
            continue;
        }

        // função
        if (isalpha((unsigned char)in[i])) {
            char t[32] = {0}; int k = 0;
            while (isalpha((unsigned char)in[i])) t[k++] = in[i++];
            t[k] = '\0';
            if (isFun(t)) stk[++top] = strdup(t);
            else {
                fprintf(stderr, "[Erro] token inválido: %s\n", t);
                while (top >= 0) free(stk[top--]);
                return out;
            }
            continue;
        }

        // parênteses
        if (in[i] == '(') {
            stk[++top] = strdup("(");
            i++;
            continue;
        }
        if (in[i] == ')') {
            while (top >= 0 && strcmp(stk[top], "(")) {
                int L = strlen(stk[top]);
                memcpy(out + p, stk[top], L);
                p += L; out[p++] = ' ';
                free(stk[top--]);
            }
            if (top < 0) { fprintf(stderr, "[Erro] ) sem (\n"); return out; }
            free(stk[top--]);
            if (top >= 0 && isFun(stk[top])) {
                int L = strlen(stk[top]);
                memcpy(out + p, stk[top], L);
                p += L; out[p++] = ' ';
                free(stk[top--]);
            }
            i++;
            continue;
        }

        // operador
        {
            char t[2] = { in[i], 0 };
            if (isOp(t)) {
                while (top >= 0 && (isOp(stk[top]) || isFun(stk[top]))) {
                    if (isOp(stk[top])) {
                        int pt = prec(stk[top]), ct = prec(t);
                        if (pt > ct || (pt == ct && t[0] != '^')) {
                            int L = strlen(stk[top]);
                            memcpy(out + p, stk[top], L);
                            p += L; out[p++] = ' ';
                            free(stk[top--]);
                            continue;
                        }
                        break;
                    }
                    int L = strlen(stk[top]);
                    memcpy(out + p, stk[top], L);
                    p += L; out[p++] = ' ';
                    free(stk[top--]);
                }
                stk[++top] = strdup(t);
                i++;
                continue;
            }
        }

        // ignorar outros
        i++;
    }

    while (top >= 0) {
        int L = strlen(stk[top]);
        memcpy(out + p, stk[top], L);
        p += L; out[p++] = ' ';
        free(stk[top--]);
    }
    out[p] = '\0';
    return out;
}

char *getFormaInFixa(char *in) {
    for (char *q = in; *q; ++q)
        if (*q == ',') *q = '.';

    static char out[TAM];
    char *stk[TAM];
    int top = -1;
    out[0] = '\0';
    char buf[TAM]; strncpy(buf, in, TAM-1);

    for (char *tok = strtok(buf, " "); tok; tok = strtok(NULL, " ")) {
        if (isdigit((unsigned char)tok[0]) || (tok[0]=='.' && isdigit((unsigned char)tok[1])) ||
            (tok[0]=='-' && isdigit((unsigned char)tok[1]))) {
            stk[++top] = strdup(tok);
        }
        else if (isFun(tok)) {
            if (top < 0) { fprintf(stderr, "[Erro] pós-fixa inválida\n"); while(top>=0) free(stk[top--]); return out; }
            char *a = stk[top--], tmp[TAM];
            snprintf(tmp, TAM, "%s(%s)", tok, a);
            free(a);
            stk[++top] = strdup(tmp);
        }
        else if (isOp(tok)) {
            if (top < 1) { fprintf(stderr, "[Erro] pós-fixa inválida\n"); while(top>=0) free(stk[top--]); return out; }
            char *b = stk[top--], *a = stk[top--], tmp[TAM];
            snprintf(tmp, TAM, "(%s%s%s)", a, tok, b);
            free(a); free(b);
            stk[++top] = strdup(tmp);
        }
        else {
            fprintf(stderr, "[Erro] token inválido: %s\n", tok);
            while (top>=0) free(stk[top--]);
            return out;
        }
    }

    if (top >= 0) {
        strncpy(out, stk[top], TAM-1);
        free(stk[top--]);
    }
    while (top >= 0) free(stk[top--]);
    return out;
}

float getValorPosFixa(char *in) {
    float stk[TAM]; int top = -1;
    char buf[TAM]; strncpy(buf, in, TAM-1);
    for (char *tok = strtok(buf, " "); tok; tok = strtok(NULL, " ")) {
        if (isdigit((unsigned char)tok[0]) || (tok[0]=='.' && isdigit((unsigned char)tok[1])) ||
            (tok[0]=='-' && isdigit((unsigned char)tok[1]))) {
            stk[++top] = strtof(tok, NULL);
        }
        else if (isFun(tok)) {
            float x = stk[top--], r = 0;
            if (!strcmp(tok, "sen"))  r = sinf(x * M_PI/180.0f);
            if (!strcmp(tok, "cos"))  r = cosf(x * M_PI/180.0f);
            if (!strcmp(tok, "tg"))   r = tanf(x * M_PI/180.0f);
            if (!strcmp(tok, "raiz")) r = sqrtf(x);
            if (!strcmp(tok, "log"))  r = log10f(x);
            stk[++top] = r;
        }
        else if (isOp(tok)) {
            float b = stk[top--], a = stk[top--], r = 0;
            switch(tok[0]) {
                case '+': r = a + b; break;
                case '-': r = a - b; break;
                case '*': r = a * b; break;
                case '/':
                    if (b == 0) { fprintf(stderr, "[Erro] divisão por zero\n"); return 0; }
                    r = a / b; break;
                case '%':
                    if (b == 0) { fprintf(stderr, "[Erro] divisão por zero\n"); return 0; }
                    r = fmodf(a, b); break;
                case '^': r = powf(a, b); break;
            }
            stk[++top] = r;
        }
    }
    if (top == 0) return stk[top];
    fprintf(stderr, "[Erro] avaliação incompleta\n");
    return 0;
}

float getValorInFixa(char *s) {
    char *pf = getFormaPosFixa(s);
    return getValorPosFixa(pf);
}