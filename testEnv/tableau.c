#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int inputs=10;
int propValues[8][3] = {{0, 0, 0},
                        {0, 0, 1},
                        {0, 1, 0},
                        {0, 1, 1},
                        {1, 0, 0},
                        {1, 0, 1},
                        {1, 1, 0},
                        {1, 1, 1}};

/*               Grammar
                 =======

        Formula := Proposition | Unary | Binary
          Unary := - Formula
         Binary := ( Formula BinaryOperation Formula )
    Proposition := p | q | r
BinaryOperation := v | ^ | >
*/

/* Non-Terminal Prototypes */
int Formula(char *s[]);
int Proposition(char *s[]);
int Unary(char *s[]);
int Binary(char *s[]);

/* Terminal Prototypes */
int leftParen(char *s[]);
int rightParen(char *s[]);
int negation(char *s[]);
int BinaryOperator(char *s[]);

/* string matching function */
int match(char *s[], const char *token) {
    if (strncmp(*s, token, strlen(token)) == 0) {
        *s = *s + strlen(token);
        return 1;
    }
    return 0;
}

/* Terminals */
int leftParen(char *s[]) {
    if (match(s, "("))
        return 1;
    return 0;
}
int rightParen(char *s[]) {
    if (match(s, ")"))
        return 1;
    return 0;
}
int negation(char *s[]) {
    if (match(s, "-"))
        return 1;
    return 0;
}
int BinaryOperator(char *s[]){
    if(match(s, "v") || match(s, "^") || match(s, ">")){
        return 1;
    }
    return 0;
}
int Proposition(char *s[]){
    if(match(s, "p") || match(s, "q") || match(s, "r")){
        return 1;
    }
    return 0;
}

/* Non Terminals */
int Formula(char *s[]){
    char *original = *s;
    if(Proposition(s) || Unary(s) || Binary(s)){
        return 1;
    }
    *s = original;
    return 0;
}
int Unary(char *s[]){
    char *original = *s;
    if(!negation(s)){
        *s = original;
        return 0;
    }
    if(!Formula(s)){
        *s = original;
        return 0;
    }
    return 1;
}
int Binary(char *s[]){
    char *original = *s;
    if(!leftParen(s)){
        *s = original;
        return 0;
    }
    if(!Formula(s)){
        *s = original;
        return 0;
    }
    if(!BinaryOperator(s)){
        *s = original;
        return 0;
    }
    if(!Formula(s)){
        *s = original;
        return 0;
    }
    if(!rightParen(s)){
        *s = original;
        return 0;
    }

    return 1;

}

/* Wrapper function */
int FormulaMain(char *s[]){
    char *original = *s;
    int res = Formula(s);
    if(res){
        if(strlen(original) == 1){
            return 1;
        } else if(original[0] == '-'){
            return 2;
        } else {
            return 3;
        }
    } else {
        return 0;
    }
}

/* Binary splitter functions */
int indexOfInnerBC(char *g){
    int len = strlen(g);
    int level = 0;
    for(int i = 0; i < len; i++){
        if(g[i] == '('){
            level++;
        } else if(g[i] == ')'){
            level--;
        }
        if((level == 1) && ((g[i] == 'v') || (g[i] == '^') || (g[i] == '>'))){
            return i;
        }
    }
    return -1;
}
char *partone(char *g){
    int index = indexOfInnerBC(g);
    int size = index - 1;
    char *substr = malloc(size);
    strncpy(substr, g+1, size);
    return substr;
}
char *parttwo(char *g){
    int index = indexOfInnerBC(g);
    int len = strlen(g);
    int size = len - index - 2;
    char *substr = malloc(size);
    strncpy(substr, g+index+1, size);
    return substr;
}

/* string manipulation functions */
char *replace_char(char *str, int p, int q, int r){
    char *string1;
    string1 = malloc(strlen(str)+1);
    strcpy(string1, str);

    for(int i = 0; i < (int)strlen(str); i++){
        if(str[i] == 'p'){
            string1[i] = '0' + p;
        } else if(str[i] == 'q'){
            string1[i] = '0' + q;
        } else if(str[i] == 'r'){
            string1[i] = '0' + r;
        }
    }
    free(string1);
    return string1;
}
char *replace_str(char *str, char *orig, char *rep){
    static char buffer[256];
    char *p;
    if(!(p = strstr(str, orig)))
        return str;

    strncpy(buffer, str, p-str);
    buffer[p-str] = '\0';

    sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

    return buffer;
}

/* string reduce functions */
char *reduceBrackets(char *string){
    char *string1;
    string1 = malloc(strlen(string));
    strcpy(string1, string);
    while(strstr(string1, "(0)")){
        strcpy(string1, replace_str(string1, "(0)", "0"));
    }
    while(strstr(string1, "(1)")){
        strcpy(string1, replace_str(string1, "(1)", "1"));
    }
    return string1;
}
char *negate(char *string){
    char *string1;
    string1 = malloc(strlen(string)+1);
    strcpy(string1, string);

    while(strstr(string1, "-1")){
        strcpy(string1, replace_str(string1, "-1", "0"));
    }
    while(strstr(string1, "-0")){
        strcpy(string1, replace_str(string1, "-0", "1"));
    }
    return string1;
}
char *or(char *string){
    char *string1;
    string1 = malloc(strlen(string)+1);
    strcpy(string1, string);
    while(strstr(string1, "1v1")){
        strcpy(string1, replace_str(string1, "1v1", "1"));
    }
    while(strstr(string1, "1v0")){
        strcpy(string1, replace_str(string1, "1v0", "1"));
    }
    while(strstr(string1, "0v1")){
        strcpy(string1, replace_str(string1, "0v1", "1"));
    }
    while(strstr(string1, "0v0")){
        strcpy(string1, replace_str(string1, "0v0", "0"));
    }
    return string1;
}
char *and(char *string){
    char *string1;
    string1 = malloc(strlen(string)+1);
    strcpy(string1, string);

    while(strstr(string1, "1^1")){
        strcpy(string1, replace_str(string1, "1^1", "1"));
    }
    while(strstr(string1, "1^0")){
        strcpy(string1, replace_str(string1, "1^0", "0"));
    }
    while(strstr(string1, "0^1")){
        strcpy(string1, replace_str(string1, "0^1", "0"));
    }
    while(strstr(string1, "0^0")){
        strcpy(string1, replace_str(string1, "0^0", "0"));
    }
    return string1;
}
char *implies(char *string){
    char *string1;
    string1 = malloc(strlen(string)+1);
    strcpy(string1, string);

    while(strstr(string1, "0>0")){
        strcpy(string1, replace_str(string1, "0>0", "1"));
    }
    while(strstr(string1, "0>1")){
        strcpy(string1, replace_str(string1, "0>1", "1"));
    }
    while(strstr(string1, "1>0")){
        strcpy(string1, replace_str(string1, "1>0", "0"));
    }
    while(strstr(string1, "1>1")){
        strcpy(string1, replace_str(string1, "1>1", "1"));
    }

    return string1;

}

/* Formula evaluation functions (High-Level) */
int simplify(char *string){
    char *string1;
    string1 = malloc(strlen(string)+1);
    strcpy(string1, string);


    while(strlen(string1) > 1){
        strcpy(string1, negate(string1));
        strcpy(string1, or(string1));
        strcpy(string1, and(string1));
        strcpy(string1, implies(string1));
        strcpy(string1, reduceBrackets(string1));
    }
    return atoi(string1);
}
int evaluate(char *string, int p, int q, int r){
    char *string1;
    string1 = malloc(strlen(string)+1);
    strcpy(string1, string);

    strcpy(string1, replace_char(string1, p, q, r));

    return simplify(string1);
}

/* Truth table generator and satisfiable checker (High-Level) */
int *truthTable(char *string){
    char *string1;
    string1 = malloc(strlen(string)+1);
    strcpy(string1, string);

    static int d[8];
    for(int i = 0; i < 8; i++){
        d[i] = evaluate(string1, propValues[i][0], propValues[i][1], propValues[i][2]);
    }
    return d;
}
int satisfiable(char *string){
    char *string1;
    string1 = malloc(strlen(string)+1);
    strcpy(string1, string);

    int *tt = truthTable(string1);
    for(int i = 0; i < 8; i++){
        if(tt[i] == 1) return 1;
    }
    return 0;
}

int main(){

    FILE *fp, *fpout;

    if ((  fp=fopen("input.txt","r"))==NULL){printf("Error opening input file");exit(1);}
    fpout = fopen("output.txt","w+");
    int j;
    for(j=0;j<inputs;j++)
    {
        char *name = malloc(50);
        fscanf(fp, "%s",name);
        char *orig = name;
        char *orig2 = name;
        switch (FormulaMain(&name))
        {
            case(0): fprintf(fpout, "%s is not a formula.  ", orig);break;
            case(1): fprintf(fpout, "%s is a proposition.  ", orig);break;
            case(2): fprintf(fpout, "%s is a negation.  ", orig);break;
            case(3):fprintf(fpout, "%s is a binary. The first part is %s and the second part is %s.  ", orig, partone(orig), parttwo(orig));break;
            default:fprintf(fpout, "What the f***!  ");
        }
        if (FormulaMain(&orig)!=0)
        {
            if (!satisfiable(orig2))  fprintf(fpout, "%s is not satisfiable.\n", orig2);
            else fprintf(fpout, "%s is satisfiable.\n", orig2);
        }
        else  fprintf(fpout, "I told you, %s is not a formula.\n", orig2);
    }

    fclose(fp);
    fclose(fpout);

    return 0;
}



