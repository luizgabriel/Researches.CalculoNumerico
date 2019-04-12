#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <string.h>

#define SPACE_CHAR 32
#define MAX_INT_DIGITS 20
#define MAX_FLOAT_DIGITS 20

char get_num(int num)
{
    if (num < 10)
        return 48 + num;
    else
        return 55 + num;
}

void format_number(double num, int base, char *output, int int_digits, int float_digits)
{
    int in = (int)num;
    double fl = num - in;
    int k;
    int size = int_digits + float_digits + 2;

    output[size - 1] = 0;

    for (k = 0; k < size - 1; k++)
        output[k] = SPACE_CHAR;

    k = int_digits - 1;
    while (in >= base && k >= 0)
    {
        output[k] = get_num(in % base);
        in = in / base;
        k--;
    }
    output[k] = get_num(in);

    if (fl > 0)
    {
        output[int_digits] = '.';

        k = int_digits + 1;
        while (fl > 0 && k < size - 1)
        {
            fl = fl * base;
            output[k] = get_num((int)fl);
            fl = fl - ((int)fl);
            k++;
        }
    }
}

int main()
{
    char out[MAX_INT_DIGITS + MAX_FLOAT_DIGITS + 2];
    double num;

    while (printf("\nInforme um número: "), scanf("%lf", &num) && num != EOF)
    {
        format_number(num, 16, out, MAX_INT_DIGITS, MAX_FLOAT_DIGITS);
        printf("\nHEXA:\t%s", out);

        format_number(num, 8, out, MAX_INT_DIGITS, MAX_FLOAT_DIGITS);
        printf("\nOCTAL:\t%s", out);

        format_number(num, 2, out, MAX_INT_DIGITS, MAX_FLOAT_DIGITS);
        printf("\nBINARIO:%s", out);
    }

    return 0;
}