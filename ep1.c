#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define SL_RESULT_INCOMPATIBLE 2
#define SL_RESULT_UNDEFINED 1
#define SL_RESULT_DEFINED 0

#define SPACE_CHAR 32
#define SEPARATOR_CHAR '.'
#define MAX_INT_DIGITS 20
#define MAX_FRAC_DIGITS 20

#define HEXA 16
#define OCTAL 8
#define BINARIO 2

#define CONVERT_OPT 'C'
#define LINEAR_SYSTEM_OPT 'S'
#define EQUATION_OPT 'E'
#define FINISH_OPT 'F'

#define MAX_BISSECTION_ERROR 0.00000001
#define MAX_BISSECTION_ITERATIONS 1000

/**
 * Converte número para caractere especifico da base
 */
char get_num(int num)
{
    if (num < 10)
        return 48 + num;
    else
        return 55 + num;
}

/**
 * Formatar Número para qualquer base
 * Entrada>>
 * - num    : número em decimal a ser convertido
 * - base   : a base de destino
 * - output : um ponteiro para uma string de saída
 */
void format_number(double num, int base, char *output)
{
    int in = (int)num;
    double fl = num - in;
    int k;
    int size = MAX_INT_DIGITS + MAX_FRAC_DIGITS + 2;

    output[size - 1] = 0;

    for (k = 0; k < size - 1; k++)
        output[k] = SPACE_CHAR;

    k = MAX_INT_DIGITS - 1;
    while (in >= base && k >= 0)
    {
        output[k] = get_num(in % base);
        in = in / base;
        k--;
    }
    output[k] = get_num(in);

    if (fl > 0)
    {
        output[MAX_INT_DIGITS] = SEPARATOR_CHAR;

        k = MAX_INT_DIGITS + 1;
        while (fl > 0 && k < size - 1)
        {
            fl = fl * base;
            output[k] = get_num((int)fl);

            fl = fl - ((int)fl);
            k++;
        }
    }
}

/**
 * Imprime uma matriz M de L linhas e C colunas
 */
void print_matrix(double** m, int l, int c)
{
    int i, j;
    for (i = 0; i < l; i++) {
        for (j = 0; j < c; j++) {
            printf("%10.3lf ", m[i][j]);
        }
        printf("\n");
    }
}

/**
 * Imprime um vetor de solução V
 */
void print_solution(double *v, int n)
{
    int i;

    for (i = 0; i < n; i++) {
        printf("\nx[%d] = %10.3lf ", i + 1, v[i]);
    }
}

/**
 * Libera uma matriz M de L linhas
 */
void delete_matrix(double** m, int l)
{
    int j;
    for (j = 0; j < l; j++)
        free(m[j]);

    free(m);
}

/**
 * Aloca uma matriz de L linhas por C colunas
 */
double **alloc_matrix(int l, int c)
{
    double **m;
    int i;
    m = malloc(sizeof(m) * l);

    if (m != NULL) {
        for (i = 0; i < l; i++) {
            m[i] = malloc(sizeof(double) * c);
            if (m[i] == NULL) {
                delete_matrix(m, l);
                return NULL;
            }
        }
    }

    return m;
}

/**
 * Aloca um vetor de N elementos
 */
double *alloc_vector(int n)
{
    double *x = malloc(sizeof(double) * n);
    return x;
}

/**
 * Aplica método da substituição simples para resolver um sistema linear diagonal (triangular superior e inferior) 
 * Entrada>>
 * - m : Matriz (N x N+1)
 * - n : Numero de variaveis
 * - x : O vetor solução
 */
int sl_simple(double **m, int n, double *x)
{
    int i, type = SL_RESULT_DEFINED;

    for (i = 0; i < n; i++) {
        if (m[i][i] == 0) {
            type = (m[i][n] == 0)? SL_RESULT_UNDEFINED : SL_RESULT_INCOMPATIBLE;
            x[i] = 0;
        } else {
            x[i] = m[i][n] / m[i][i];
        }
    }

    return type;
}

/**
 * Aplica o Método de Jordan para encontrar uma Matriz Diagonal (Triangular Superior e Inferior)
 * Entrada>>
 * - m : Matriz (N x N+1)
 * - n : Numero de variaveis
 */
void sl_jordan(double **m, int n)
{
    char i, j, k, t;
    double mult, aux;

    for(i = 0; i < n; i++) {

        if(m[i][i] == 0) {
            t = 1;
            while(m[i][i + t] == 0 && (i + t) < n) t++;

            if ((i + t) == n) {

                for(j = 0; j < n; j++) {
                    m[j][i] = 0;
                }

            } else {

                for(j = i, k = 0; k < n; k++) {
                    aux = m[i][k];
                    m[j][k] = m[j][k + t];
                    m[j][k + t] = aux;
                }

            }
        }

        for (j = 0; j < n; j++) {

            if(j != i) {

                if(m[i][i] == 0) {
                    mult = 0;
                } else {
                    mult = -(m[j][i] / m[i][i]);
                }

                for(k = 0; k <= n; k++) {
                    m[j][k] = m[j][k] + (mult * m[i][k]);
                }

            }

        }
    }
}

/**
 * Limpa a saída do console
 */
void clear_console()
{
    fflush(stdout);
    printf("\033c");
}

/**
 * Imprime o menu
 */
void print_menu()
{
    int i;

    clear_console();

    printf("\n"); for (i=0; i<40; i++) printf("-");
    printf("\nCALCULO NUMERICO");
    printf("\n"); for (i=0; i<40; i++) printf("-");
    printf("\nC - Conversão");
    printf("\nS - Sistema Linear");
    printf("\nE - Equação Algébrica");
    printf("\nF - Finalizar");
    printf("\n"); for (i=0; i<40; i++) printf("-");
}

/**
 * Ler opção do menu 
 */
char read_menu_option() 
{
    char option;

    print_menu();
    printf("\n Escolha uma opção: ");
    scanf("%c", &option);

    return toupper(option);
}

/**
 * Ação de CONVERTER números para as bases HEXA, OCTAL e BINARIO 
 */
void convert_option()
{
    double number;
    char output[MAX_INT_DIGITS + MAX_FRAC_DIGITS + 2];

    clear_console();
    printf("\nInforme um número decimal: ");
    scanf("%lf", &number);

    format_number(number, BINARIO, output);
    printf("\nBINARIO: %s", output);

    format_number(number, OCTAL, output);
    printf("\nOCTAL:   %s", output);

    format_number(number, HEXA, output);
    printf("\nHEXA:    %s", output);
}

/**
 * Lê um arquivo dado, popula a matriz M de N variaveis
 * Entrada>>
 * - Nome do Arquivo
 * Saída>>
 * - size : quantidade de variaveis
 * - m    : A matriz
 * Retorno>>
 *  -1 : Arquivo não encontrado
 *  -3 : Arquivo inválido
 *  1  : Sucesso!
 */
int read_matrix_file(char* file_name, int* size, double*** m)
{
    FILE *fp;
    int n, j, k;
    double fl;

    fp = fopen(file_name, "r");
    if (fp) {
        fscanf(fp, "%d", &n);

        *size = n;
        *m = alloc_matrix(n, n + 1);

        for (j = 0; j < n; j++) {
            for (k = 0; k < n + 1; k++) {
                //Se a matriz estiver incompleta
                if (feof(fp)) {
                    fclose(fp);
                    delete_matrix(*m, n);
                    *m = NULL;
                    *size = 0;

                    return -3;
                }

                fscanf(fp, "%lf", &fl);
                (*m)[j][k] = fl;
            }
        }

        fclose(fp);
        return 1;
    } else {
        return -1;
    }
}

/**
 * Ação de ler um arquivo dado o nome resolver a matriz usando metodo de jordan e exibir a matriz diagonal
 */
void linear_system_option()
{
    char file_name[255];
    double **m, *x;
    int n, file_status, result;

    printf("\nInforme o nome de um arquivo para leitura: ");
    scanf("%s", file_name);

    file_status = read_matrix_file(file_name, &n, &m);
    if (file_status == -1) {
        printf("\n Arquivo não encontrado (%s)", file_name);
    } else if (file_status == -3 || n == 0 || m == NULL) {
        printf("\n Arquivo formatado incorretamente. Forneça todos os dados corretamente. (%s)", file_name);
    } else {
        x = alloc_vector(n);

        if (x == NULL || m == NULL) {
            printf("\n Sem memória.");
            return;
        }

        printf("\nN = %d\n", n);
        print_matrix(m, n, n + 1);

        printf("\n\nMATRIZ DIAGONAL:\n");
        sl_jordan(m, n);
        print_matrix(m, n, n + 1);

        result = sl_simple(m, n, x);
        if (result == SL_RESULT_INCOMPATIBLE) {
            printf("Sistema Linear INCOMPATIVEL!\n");
        } else {
            if (result == SL_RESULT_UNDEFINED) {
                printf("Sistema Linear INDETERMINADO\n");
            } else {
                printf("Sistema Linear DETERMINADO\n");
            }

            print_solution(x, n);
        }

        delete_matrix(m, n);
        free(x);
    }
}

/**
 * Teorema de lagrange
 * Entrada>>
 * - e : Equação [a, b, c, ..., z] <=> a*x^n + b*x^(n-1) * ... * z*x^0
 * - n : Grau da equação
 * Saída>>
 * L = 1 + (B / An)^[1/(N-K)]
 */
double lagrange(double *e, int n)
{
    int i, k;
    double an, b;

    an = e[0];
    b = 0;
    k = 0;

    if (an == 0)
        return 0;

    for (i = 0; i <= n; i++) {
        if (e[i] < 0) {
            if ((n - i) > k) {
                k = (n - i);
            }

            if (e[i] < b) {
                b = e[i];
            }
        }
    }

    b = fabs(b);

    return 1 + pow(b / an, 1.0 / (n - k));
}

/**
 * Aplica o Teorema de Lagrange
 */
void equation_lagrange_option(double *e, int n)
{
    int i;
    double *e1, *e2, *e3, l, l1, l2, l3;

    e1 = alloc_vector(n + 1);
    e2 = alloc_vector(n + 1);
    e3 = alloc_vector(n + 1);

    if (e1 == NULL || e2 == NULL || e3 == NULL) {
        printf("\n Sem memória.");
        return;
    }

    for (i = 0; i <= n; i++) {
        e1[n - i] = e[i];
        e2[i] = i % 2 == 0 ? e[i]: -e[i];
        e3[n - i] = e2[i];
    }

    l  = lagrange(e, n);
    l1 = 1.0 / lagrange(e1, n);
    l2 = -lagrange(e2, n);
    l3 = - 1.0 / lagrange(e3, n);

    free(e1);
    free(e2);
    free(e3);

    printf("\nIntervalo das raízes:");
    printf("\n %lf < x+ < %lf", l1, l);
    printf("\n %lf < x- < %lf", l2, l3);
}

/**
 * Aplica o método de Briot Ruffini para calcular aproximação de p(x)
 */
double solve_equation(double *p, int n, double x)
{
    int i;
    double aux[n];

    aux[0] = p[0];
    for (i = 0; i < n - 1; i++) {
        aux[i + 1] = aux[i]*x + p[i + 1];
    }

    return aux[n - 1];
}

/**
 * Método da Bisseção
 * Entrada>>
 * - a, b : um intervaldo (a, b)
 * - p : uma equação polinomial
 * - n : o grau da equação polinomal
 * Saida>>
 * Uma raiz aproximada de p(x)
 */
double bissection(double a, double b, double *p, int n)
{
    double m, err, fa, fb, fm;
    int i = 0;

    while (i < MAX_BISSECTION_ITERATIONS && err <= MAX_BISSECTION_ERROR) {
        m = (a + b) / 2.0;
        err = (b - a) / 2.0;
        fa = solve_equation(p, n, a);
        fb = solve_equation(p, n, b);
        fm = solve_equation(p, n, m);

        if (fa * fm < 0) {
            b = m;
        } else if (fb * fm < 0) {
            a = m;
        } else {
            return m;
        }

        i++;
    }

    return m;
}

/**
 * Aplica o Teorema de Bisseção 
 */
void equation_bissection_option(double *e, int n)
{
    double a, b, x; 

    printf("\nInforme um intervaldo (a, b): ");
    printf("\n a: ");
    scanf("%lf", &a);

    printf("\n b: ");
    scanf("%lf", &b);

    if ((solve_equation(e, n, a) * solve_equation(e, n, b)) > 0) {
        printf("\n O intervaldo informado não possui uma quantidade ímpar de raízes.");
    } else {
        x = bissection(a, b, e, n);
        printf("\n Uma raiz aproximada do polinômio no intervaldo: %lf", x);
    }
}

/**
 * Imprime um polinômio
 */
void print_poli(double *v, int n)
{
    int i;
    char sign;

    for (i = 0; i <= n; i++) {
        if (v[i] < 0 || i > 0) {
            sign = v[i] > 0 ? '+' : '-';
            printf(" %c ", sign);
        }

        if (i == 0) {
            printf("\n%10.3lfx^%d", fabs(v[0]), n - i);
        } else if (i == n - 1) {
            printf("%10.3lfx", fabs(v[i]));
        } else if (i == n) {
            printf("%10.3lf", fabs(v[i]));
        } else {
            printf("%10.3lfx^%d", fabs(v[i]), n - i);
        }
    }
}

/**
 * Ler equaçao algebrica e aplicar teoremas de lagrange, bolzano e bissessão
 */
void equation_option()
{
    int n, i, f;
    double *e;

    printf("\nInforme o grau da equação: ");
    scanf("%d", &n);

    e = alloc_vector(n + 1);

    if (e == NULL) {
        printf("\n Sem memória.");
        return;
    }

    for (i = 0; i <= n; i++) {
        do {
            f = 1;
            printf("\nCoeficiente de x^%d: ", n - i);
            scanf("%lf", &e[i]);    
            if (i == 0 && e[i] <= 0) {
                printf("\nx^%d NÃO pode ser MENOR OU IGUAL ZERO.", n);
                f = 0;
            }

            if (i == n && e[n] == 0) {
                printf("\nTermo independente NÃO pode ser ZERO.");
                f = 0;
            }
        } while (f == 0);
    }

    print_poli(e, n);

    equation_lagrange_option(e, n);
    equation_bissection_option(e, n);

    free(e);
}

int main()
{
    setlocale(LC_ALL, "Portuguese");

    char opt;
    while (opt = read_menu_option(), opt != FINISH_OPT) {
        if (opt == CONVERT_OPT)
            convert_option();
        else if (opt == EQUATION_OPT)
            equation_option();
        else if (opt == LINEAR_SYSTEM_OPT)
            linear_system_option();
        else {
            printf("\n OPCAO INVÁLIDA");
        }
        
        printf("\n\nPrecione ENTER para voltar para o menu...\n");
        while ((opt = getchar()) != '\n' && opt != EOF);
        getchar();
    }

    return 0;
}