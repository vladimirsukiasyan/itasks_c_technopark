/*
Сукиасян Владимир, группа АПО-13

Реализуйте функцию
    char** div_format(char **s) {
        ...
    }

и программу, использующую эту функцию для форматирования текстовой разметки состоящей из <div>, </div> и ASCII строк.
Правильное форматирование подразумевает расстановку 4х пробелов согласно вложенности.
То есть в исходном тексте необходимо игнорировать все "ведущие" и "закрывающие" пробелы, и форматировать вывод строк простановкой нового соответствующего количества пробелов.
В случае некорректной последовательности тэгов, выведите в поток вывода [error].

ВАЖНО! Программа в любом случае должна возвращать 0. Не пишите return -1, exit(1) и т.п.
Даже если обнаружилась какая-то ошибка, все равно необходимо вернуть 0! (и напечатать [error] в stdout).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAG "div>"
#define BEGIN_TAG "<div>"
#define END_TAG "</div>"

struct buffer {
    char *string;
    size_t size;
    size_t capacity;
};

char *scan();

bool checkHTML(char *);

bool is_space(char c);

void printFormatHTML(char *);

void printContent(char *, char *, int level);

void printTabulation(int);

void trim(char** begin, char** end);

char *trimWithNewMemory(char *);

int main() {
    char *content = scan();
    if (!content) {
        return 0;
    }

    content = trimWithNewMemory(content);
    if(!content){
        free(content);
        return 0;
    }
    bool success = checkHTML(content);
    if (!success) {
        printf("%s", "[error]");
        free(content);
        return 0;
    }
    printFormatHTML(content);
    free(content);
    return 0;
}

bool checkHTML(char *content) {
    int countBeginTag = 0;
    int countEndTag = 0;
    int level = 0;   //level of tag nesting
    char *pTag = NULL;

    while (*content != '\0') {
        if (pTag = strstr(content, TAG), pTag) { //We find a first occurrence of "div>" and detect what tag it is (<div> or </div>)
            if (*(pTag - 1) == '<') {   //Detect <div>
                pTag--;                 //Move pointer at start of tag
                content = pTag + strlen(BEGIN_TAG);
                countBeginTag++;
                level++;
            } else if (*(pTag - 1) == '/' && *(pTag - 2) == '<') {  //Detect </div>
                pTag -= 2;
                content = pTag + strlen(END_TAG);
                countEndTag++;
                level--;
                if(level<0){           //If we found out that the count of </div> tag is more than count of <div> tag, then wtf
                    return false;
                }
            }
        }
        else break;
    }
    if (countBeginTag == countEndTag) {
        return true;
    } else return false;
}

char *scan() {
    struct buffer buf = {NULL, 0, 0};
    char character = 0;
    while ((character = (char) fgetc(stdin)) && character != EOF) {
        if (buf.size + 1 > buf.capacity) {
            buf.capacity = !buf.capacity ? 1 : buf.capacity * 2;
            char *tmp = (char *) malloc((buf.capacity + 1) * sizeof(char));    //malloc more one byte then needed for '\0'
            if (!tmp) {     //if we got th••••••••e error from malloc, free buf and return NULL
                if (buf.string) {
                    free(buf.string);
                }
                return NULL;
            }
            if (buf.string) {
                tmp = strcpy(tmp, buf.string);
                free(buf.string);
            }
            buf.string = tmp;
        }
        buf.string[buf.size] = character;
        buf.string[buf.size + 1] = '\0';
        buf.size++;
    }
    return buf.string;
}

void printFormatHTML(char *content) {
    int level = 0;
    char *pTag = NULL;

    while (*content != '\0') {
        if (pTag = strstr(content, TAG), pTag) {
            if (*(pTag - 1) == '<') {
                pTag -= 1;

                if (level != 0 && content != pTag) {
                    printContent(content, pTag, level);
                }
                printTabulation(level);
                printf("%s\n", BEGIN_TAG);

                content = pTag + strlen(BEGIN_TAG);
                level++;
            } else if (*(pTag - 1) == '/' && *(pTag - 2) == '<') {
                pTag -= 2;
                if (level != 0 && content != pTag) {
                    printContent(content, pTag, level);
                }
                level--;
                printTabulation(level);
                printf("%s\n", END_TAG);

                content = pTag + strlen(END_TAG);
            }
        }
        else {
            printf("%s",content);
            break;
        }
    }
}

void printTabulation(int level) {
    if (level) {
        printf("%*c", 4 * level, ' ');
    }
}

void printContent(char *content, char *pTag, int level) {
    char *pointer = pTag-1;

    trim(&content, &pointer);
    while(strstr(content,"\n")){     //If we get string of content between two tags, which consists newlines, we need to split this string
        pointer=strstr(content,"\n");
        pointer--;
        trim(&content,&pointer);
        printTabulation(level);
        while(content!=pointer){
            printf("%c", *content++);
        }
        printf("\n");
        content=pointer+1;
    }

    pointer=pTag-1;   //Print the last part of complex string
    trim(&content, &pointer);
    if(content!=pointer){
        printTabulation(level);
        while (content!=pointer){
            printf("%c", *content++);
        }
        printf("\n");
    }
}

bool is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}


void trim(char** begin, char** end){
    while (*end > *begin && is_space(**end)) { //We go at the end of line and trim spaces(\t,\n,' ') before we reach "nospace" symbols
        (*end)--;
    }

    if((*begin==*end && **begin=='\0') || **end=='\0')   //if we get string, that consists of only null-terminated symbol \0, then do nothing
        return;
    (*end)++;
    **end = '\0';

    while (is_space(**begin)) {
        (*begin)++;
    }
}


char *trimWithNewMemory(char *str_p) {
    char *begin = str_p;
    char *end= begin + strlen(begin) - 1;
    trim(&begin,&end);

    char *trimmedString = (char *) malloc((strlen(begin) + 1) * sizeof(char));
    if(!trimmedString){
        free(str_p);
        return NULL;
    }
    strcpy(trimmedString, begin);

    free(str_p);
    return trimmedString;
}