#include <stdio.h>
#include <unistd.h>
#include <time.h>

int cfileexists(const char *filename);
void add_row_csv(char* nome_dispositivo, int value);

int cfileexists(const char *filename)
{
    /* try to open file to read */
    FILE *file;
    if (file = fopen(filename, "r"))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

void add_row_csv(char* nome_dispositivo, int value)
{
    char mapResponse[2][10] = {"Desligou", "Ligou"};
    char filename[10] = {"data.csv"};
    FILE *fp;
    int file_exists;
    file_exists = cfileexists(filename);
    fp = fopen(filename, "a");
    if (file_exists != 1)
    {
      fprintf(fp, "Objetivo,valor,dia,hora");
    }
    time_t t = time(NULL);
    struct tm date = *localtime(&t);
    fprintf(fp, "\n%s, %d.%d,", nome_dispositivo, value);
    fprintf(fp, " %d-%02d-%02d, %02d:%02d:%02d", date.tm_mday, date.tm_mon + 1, date.tm_year + 1900, date.tm_hour, date.tm_min, date.tm_sec);

    fclose(fp);
}