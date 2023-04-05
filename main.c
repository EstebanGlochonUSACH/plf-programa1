#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#ifdef WIN32
#include <io.h>
#define F_OK 0
#define access _access
#endif

/* Descripción: Comprueba si existe un archivo.
 * @param {char*} filename     Ruta del archivo.
 * @return {int}               Devuelve 0 o 1 dependeiendo de si es falso o verdadero.
 */
int file_exists(char *filename) {
    if (access(filename, F_OK) == 0) {
        return 0;
    }
    return 1;
};

/* Descripción: Comprueba si un carácter es una letra (mayúsculas o minúsculas).
 * @param {char} c     Ruta del archivo.
 * @return {int}       Devuelve 0 o 1 dependeiendo de si es falso o verdadero.
 */
int is_letter(char c);

/* Descripción: Comprueba si un carácter es un digito.
 * @param {char} c     Ruta del archivo.
 * @return {int}       Devuelve 0 o 1 dependeiendo de si es falso o verdadero.
 */
int is_digit(char c);

/* Descripción: Resetea el buffer y el contador de índice a los valores que representan
 *              un buffer vacío.
 * @param {char*} buffer         Arreglo dinamico de carácteres.
 * @param {size_t*} curr_idx     Puntero al contador que representa el largo del buffer utilizado.
 */
void clear_buffer(char *buffer, size_t *curr_idx);

/* Descripción: Lee recursivamente carácteres de tipo letra del archivo de entrada.
 *              Devuelve 0 en caso de encontrar EOF, en caso contrario 1.
 * @param {char*} buffer         Arreglo dinamico de carácteres.
 * @param {size_t*} curr_idx     Puntero al contador que representa el largo del buffer utilizado.
 * @param {FILE*} fp_entrada     Archivo de entrada.
 * @return {int}                 Devuelve 0 o 1.
 */
int process_letters(char *buffer, size_t *curr_idx, FILE *fp_entrada);

/* Descripción: Comprueba si una cadena de carácteres es una palabra reservada.
 * @param {char*} buffer         Arreglo dinamico de carácteres.
 * @param {size_t*} curr_idx     Puntero al contador que representa el largo del buffer utilizado.
 */
void check_reserved_word(char *buffer, size_t *curr_idx);

/* Descripción: Lee recursivamente carácteres de tipo digito del archivo de entrada.
 *              Devuelve 1 si el siguiente caracter luego de los digitos es una coma,
 *              en caso contrario 0.
 * @param {char*} buffer         Arreglo dinamico de carácteres.
 * @param {size_t*} curr_idx     Puntero al contador que representa el largo del buffer utilizado.
 * @param {FILE*} fp_entrada     Archivo de entrada.
 * @return {int}                 Devuelve 0 o 1.
 */
int process_digit(char *buffer, size_t *curr_idx, FILE *fp_entrada);

/* Descripción: Maneja el proceso de leer numberos desde el archivo de entrada, ya sean
 *              enteros o decimales. Luego se encarga de escribir en el archivo de salida
 *              "ENTERO" o "DECIMAL" acorde al caso.
 * @param {char*} buffer         Arreglo dinamico de carácteres.
 * @param {size_t*} curr_idx     Puntero al contador que representa el largo del buffer utilizado.
 * @param {FILE*} fp_entrada     Archivo de entrada.
 * @param {FILE*} fp_salida      Archivo de salida.
 */
void process_number(char *buffer, size_t *curr_idx, FILE *fp_entrada, FILE *fp_salida);

/* Descripción: Comprueba si esl carácter entregado como parámetro es un caracter reservado
 *              del lenguaje. Si corresponde a uno de los simbolos reservados, lo escribe en
 *              el archivo de salida, en caso contrario lo ignora.
 * @param {char} symbol          Arreglo dinamico de carácteres.
 * @param {FILE*} fp_salida      Archivo de salida.
 */
void process_symbol(char symbol, FILE *fp_salida);

int main(int argc, char *argv[])
{
    if(argc == 1){
        printf("Error: Faltan parámetros.\n");
        printf("Uso: %s archivo_entrada archivo_salida\n", argv[0]);
        return 1;
    }
    else if(argc == 2){
        printf("Error: Falta parámetro.\n");
        printf("Uso: %s archivo_entrada archivo_salida\n", argv[0]);
        return 1;
    }
    else if(argc > 3){
        printf("Error: Demasiados parámetros.\n");
        printf("Uso: %s archivo_entrada archivo_salida\n", argv[0]);
        return 1;
    }

    FILE *fp_entrada, *fp_salida;

    fp_entrada = fopen(argv[1], "r");
    if(fp_entrada == NULL){
        printf("Error: El archivo de entrada no existe.\n");
        return 1;
    }

    if (file_exists(argv[2]) == 0) {
        printf("Error: El archivo de salida ya existe.\n");
        return 1;
    }

    fp_salida = fopen(argv[2], "w");
    if(fp_salida == NULL){
        printf("Error: No se pudo abrir el archivo de salida.\n");
        return 1;
    }

    int c, _continue;
    char *symbol_buffer = (char*)malloc(1000);
    size_t curr_idx = 0;
    while (_continue && (c = fgetc(fp_entrada)) != EOF)
    {
        symbol_buffer[curr_idx++] = (char)c;
        if(is_letter(c)){
            _continue = process_letters(symbol_buffer, &curr_idx, fp_entrada);
            check_reserved_word(symbol_buffer, &curr_idx);
            fprintf(fp_salida, "%s\n", symbol_buffer);
            clear_buffer(symbol_buffer, &curr_idx);
        }
        else if(is_digit(c)){
            process_number(symbol_buffer, &curr_idx, fp_entrada, fp_salida);
            _continue = 1;
            clear_buffer(symbol_buffer, &curr_idx);
        }
        else{
            process_symbol(c, fp_salida);
            _continue = 1;
            clear_buffer(symbol_buffer, &curr_idx);
        }
    }

    fclose(fp_entrada);
    free(symbol_buffer);
    return 0;
};

int is_letter(char c){
    return((c >= 65 && c <= 90) || (c >= 97 && c <= 122));
};

int is_digit(char c){
    return(c >= 48 && c <= 57);
};

void clear_buffer(char *buffer, size_t *curr_idx)
{
    buffer[0] = '\0';
    *curr_idx = 0;
};

#define SYMBOL_ADD 43
#define SYMBOL_SUB 45
#define SYMBOL_COLON 58
#define SYMBOL_SLASH 47
#define SYMBOL_EXPO 94
#define SYMBOL_PIPE 124
#define SYMBOL_EXCLAMATION 33
#define SYMBOL_L_PARENTHESIS 40
#define SYMBOL_R_PARENTHESIS 41
#define SYMBOL_EQUAL 61

void process_symbol(char symbol, FILE *fp_salida)
{
    switch(symbol){
        case SYMBOL_ADD:
            fprintf(fp_salida, "+\n");
            break;
        case SYMBOL_SUB:
            fprintf(fp_salida, "-\n");
            break;
        case SYMBOL_COLON:
            fprintf(fp_salida, ":\n");
            break;
        case SYMBOL_SLASH:
            fprintf(fp_salida, "/\n");
            break;
        case SYMBOL_EXPO:
            fprintf(fp_salida, "^\n");
            break;
        case SYMBOL_PIPE:
            fprintf(fp_salida, "|\n");
            break;
        case SYMBOL_EXCLAMATION:
            fprintf(fp_salida, "!\n");
            break;
        case SYMBOL_L_PARENTHESIS:
            fprintf(fp_salida, "(\n");
            break;
        case SYMBOL_R_PARENTHESIS:
            fprintf(fp_salida, ")\n");
            break;
        case SYMBOL_EQUAL:
            fprintf(fp_salida, "=\n");
            break;
        default:
            return;
    }
};

int process_letters(char *buffer, size_t *curr_idx, FILE *fp_entrada)
{
    int c = fgetc(fp_entrada);
    if(c == EOF) return 0;
    if(is_letter(c) || is_digit(c)){
        buffer[(*curr_idx)++] = (char)c;
        return process_letters(buffer, curr_idx, fp_entrada);
    }
    fseek(fp_entrada, -1, SEEK_CUR);
    return 1;
}

void check_reserved_word(char *buffer, size_t *curr_idx){
    buffer[(*curr_idx)++] = '\0';
    if(strcmp(buffer, "x") == 0) return;
    else if(strcmp(buffer, "PI") == 0) return;
    else if(strcmp(buffer, "MOD") == 0) return;
    else if(strcmp(buffer, "SQR") == 0) return;
    else if(strcmp(buffer, "CUR") == 0) return;
    else if(strcmp(buffer, "EXP") == 0) return;
    else if(strcmp(buffer, "LN") == 0) return;
    else if(strcmp(buffer, "LOG") == 0) return;
    else if(strcmp(buffer, "SGN") == 0) return;
    else if(strcmp(buffer, "INT") == 0) return;
    else if(strcmp(buffer, "FIX") == 0) return;
    else if(strcmp(buffer, "FRAC") == 0) return;
    else if(strcmp(buffer, "ROUND") == 0) return;
    else{
        sprintf(buffer, "IDENTIFICADOR");
        *curr_idx = 13;
    }
}

#define SYMBOL_COMMA 44

int process_digit(char *buffer, size_t *curr_idx, FILE *fp_entrada)
{
    int c = fgetc(fp_entrada);
    if(c == EOF) return 0;
    if(is_digit(c)){
        buffer[(*curr_idx)++] = (char)c;
        return process_digit(buffer, curr_idx, fp_entrada);
    }
    else if(c == SYMBOL_COMMA){
        buffer[(*curr_idx)++] = (char)c;
        return 1;
    }
    fseek(fp_entrada, -1, SEEK_CUR);
    return 0;
};

void process_number(char *buffer, size_t *curr_idx, FILE *fp_entrada, FILE *fp_salida)
{
    int has_comma = process_digit(buffer, curr_idx, fp_entrada);
    if(has_comma){
        process_digit(buffer, curr_idx, fp_entrada);
        if(buffer[(*curr_idx) - 1] == SYMBOL_COMMA){
            fprintf(fp_salida, "ENTERO\n");
        }
        else if(is_digit(buffer[(*curr_idx) - 1])){
            fprintf(fp_salida, "DECIMAL\n");
        }
    }
    else{
        fprintf(fp_salida, "ENTERO\n");
    }
};