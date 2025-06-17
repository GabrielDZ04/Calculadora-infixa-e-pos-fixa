#ifndef EXPRESSAO_H  
#define EXPRESSAO_H  

typedef struct {  
    char posFixa[512];   // Expressão pós-fixa, ex: "3 12 4 + *"  
    char inFixa[512];    // Expressão infixa,   ex: "3 * (12 + 4)"  
    float Valor;         // Valor numérico da expressão  
} Expressao;  

char *getFormaInFixa(char *Str);        // Str está em pós-fixa → retorna infixa  
char *getFormaPosFixa(char *Str);       // Str está em infixa  → retorna pós-fixa  
float  getValorPosFixa(char *StrPosFixa); // Avalia String em pós-fixa  
float  getValorInFixa(char *StrInFixa);   // Avalia String em infixa  

#endif  
