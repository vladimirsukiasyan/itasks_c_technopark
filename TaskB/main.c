#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <tgmath.h>
#include <assert.h>

struct buffer {
    char *string;
    size_t size;
    size_t capacity;
};

struct StackNode;

char *prepareDataForPostfixNotation(char *);

char *calculate(char *);

char *addCharacter(struct buffer *, const char);

char *scan();

int convertToArray(char *str, long long *);

int getDigitCount(long long sum);

char *doOperate(char *l_operate, char *r_operate, char operate);

int countSubstring(const char *str, const char *sub);

void clear(struct StackNode **head);

int main() {

    //TODO обработать все null от malloc
    char *expression = scan();
    if (!expression) {
        printf("%s", "[error]");
        return 0;
    }

    char *postfixExpression = prepareDataForPostfixNotation(expression);
    free(expression);

    if (!postfixExpression) {
        printf("%s", "[error]");
        return 0;
    }

    char *result = calculate(postfixExpression);
    free(postfixExpression);

    if (!result) {
        printf("%s", "[error]");
        return 0;
    }

    printf("%s", result);
    free(result);
    return 0;
}

//-------------------------------------<STACK>----------------------------------------//
struct StackNode {
    char *data;
    struct StackNode *next;
};

struct StackNode *newNode(const char *elem) {
    struct StackNode *node = (struct StackNode *) malloc(sizeof(struct StackNode));
    if (!node)
        return NULL;

    node->next = NULL;
    node->data = (char *) malloc(strlen(elem) + 1);
    if (!node->data) {
        free(node);
        return NULL;
    }
    strcpy(node->data, elem);
    return node;
}


char *push(struct StackNode **head, char *elem) {
    struct StackNode *stackNode = newNode(elem);
    if (!stackNode)
        return NULL;

    stackNode->next = *head;
    *head = stackNode;

    return (*head)->data;
}

char *pushOneSymbol(struct StackNode **head, char elem) {
    char *symbolWithNullTerm = (char *) malloc(2);
    if (!symbolWithNullTerm)
        return NULL;

    symbolWithNullTerm[0] = elem;
    symbolWithNullTerm[1] = '\0';

    struct StackNode *stackNode = newNode(symbolWithNullTerm);
    if (!stackNode) {
        free(symbolWithNullTerm);
        return NULL;
    }
    stackNode->next = *head;
    *head = stackNode;
    free(symbolWithNullTerm);

    return (*head)->data;
}

int isEmpty(struct StackNode **head) {
    return !(*head);
}

char *pop(struct StackNode **head) {
    if (isEmpty(head)) {
        return 0;
    }
    struct StackNode *tmp = *head;
    *head = (*head)->next;
    char *data = (char *) malloc(strlen(tmp->data)+1);

    if (!data)
        return NULL;

    strncpy(data, tmp->data, strlen(tmp->data)+1);
    free(tmp->data);
    free(tmp);

    return data;
}

const char *peek(struct StackNode **head) {
    if (isEmpty(head)) {
        return 0;
    }
    return (*head)->data;
}


//-------------------------------------</STACK>----------------------------------------//


//-------------------------------------<STRING>----------------------------------------//

char* concat(struct buffer *buf, const char *src) {
    for (int i = 0; i < strlen(src); i++) {
        char *result = addCharacter(buf, src[i]);
        if (!result)
            return NULL;
    }
    return buf->string;
}

//-------------------------------------</STRING>---------------------------------------//
char *addCharacter(struct buffer *buf, const char character) {
    if (buf->size + 1 > buf->capacity) {
        buf->capacity = !buf->capacity ? 1 : buf->capacity * 2;
        char *tmp = (char *) malloc((buf->capacity + 1));    //malloc more one byte then needed for '\0'
        if (!tmp) {     //if we got th••••••••e error from malloc, free buf and return NULL
            if (buf->string) {
                free(buf->string);
            }
            return NULL;
        }
        if (buf->string) {
            tmp = strcpy(tmp, buf->string);
            free(buf->string);
        }
        buf->string = tmp;
    }
    buf->string[buf->size] = character;
    buf->string[buf->size + 1] = '\0';
    buf->size++;
    return buf->string;
}

char *scan() {
    struct buffer buf = {NULL, 0, 0};
    char character = 0;
    while ((character = (char) fgetc(stdin)) && character != EOF) {
        if (character != ' ' && character != '\n') {
            //TODO if addCharacter return NULL;
            if (!addCharacter(&buf, character)) return NULL;
        }
    }
    return buf.string;
}

int operandPriority(char character) {
    switch (character) {
        case '+':
            return 1;
        case '-':
            return 1;
        case '*':
            return 2;
        case '/':
            return 2;
        default:
            return 0;
    }
}

char *prepareDataForPostfixNotation(char *expression) {
    struct StackNode *operands = NULL;
    char *postfixExpression = NULL;

    postfixExpression = (char *) malloc(strlen(expression) + 1);
    if (!postfixExpression) {
        return NULL;
    }

    int bracketIsOpen = 0;
    size_t index = 0;

    for (int i = 0; i < strlen(expression); i++) {
        char character = expression[i];

        if (character == '+' || character == '-' || character == '*') {
            if (bracketIsOpen) break;

            if (isEmpty(&operands)) {
                char *result = pushOneSymbol(&operands, character);
                if (!result) {
                    free(postfixExpression);
                    clear(&operands);
                    return NULL;
                }
            } else {
                //извлекаем последний операнд из стека и сравниваем его приоритетность с текущим
                while (!isEmpty(&operands) && operandPriority(character) <= operandPriority(peek(&operands)[0])) {
                    char *last_character = pop(&operands);
                    if (!last_character) {
                        free(postfixExpression);
                        clear(&operands);
                        return NULL;
                    }
                    postfixExpression[index++] = last_character[0];
                    free(last_character);
                }
                char *result = pushOneSymbol(&operands, character);
                if (!result) {
                    free(postfixExpression);
                    clear(&operands);
                    return NULL;
                }
            }
        } else if (character == '{' || character == '}') {
            postfixExpression[index] = character;
            if (character == '{') {
                bracketIsOpen = 1;
            }
            if (character == '}') {
                bracketIsOpen = 0;
                if (postfixExpression[index - 1] == '{') {
                    free(postfixExpression);
                    return NULL;
                }
            }
            index++;

        } else if (character >= '0' && character <= '9') {
            postfixExpression[index++] = character;

        } else if (character == ',' && bracketIsOpen) {
            postfixExpression[index++] = character;

        } else if (character == '(') {
            char *result = pushOneSymbol(&operands, character);
            if (!result) {
                free(postfixExpression);
                clear(&operands);
                return NULL;
            }

        } else if (character == ')') {
            char *operand;
            while (!isEmpty(&operands) && (operand = pop(&operands))[0] != ')' && operand && operand[0] != '(') {
                postfixExpression[index++] = operand[0];
                free(operand);
            }
            if (!operand) {
                free(postfixExpression);
                clear(&operands);
                return NULL;
            }
            free(operand);

        } else { //Если был введен некорректный символ
            free(postfixExpression);
            clear(&operands);
            return NULL;
        }
    }

    if (bracketIsOpen) { //незаконченный вектор
        free(postfixExpression);
        clear(&operands);
        return NULL;
    }

    while (!isEmpty(&operands)) {
        char *operand = pop(&operands);
        if (!operand) {
            free(postfixExpression);
            clear(&operands);
            return NULL;
        }
        postfixExpression[index++] = operand[0];
        free(operand);
    }
    postfixExpression[index] = '\0';
    return postfixExpression;
}

char *calculate(char *postfixExpression) {
    struct StackNode *operates = NULL;

    for (int i = 0; i < strlen(postfixExpression); i++) {
        char character = postfixExpression[i];
        if (character >= '0' && character <= '9') {
            char *number = NULL;

            size_t number_start = (size_t) i;
            while (postfixExpression[i + 1] >= '0' && postfixExpression[i + 1] <= '9') {
                i++;
            }
            size_t count = i - number_start + 1;

            number = (char *) malloc(count + 1);
            if (!number) {
                clear(&operates);
                return NULL;
            }
            strncpy(number, postfixExpression + number_start, count);
            number[count] = '\0';

            char *result = push(&operates, number);
            free(number);
            if (!result) {
                clear(&operates);
                return NULL;
            }

        } else if (character == '{') {
            struct buffer vector = {NULL, 0, 0};
            char vector_char = 0;

            while ((vector_char = postfixExpression[i]) != '}') {
                if (!addCharacter(&vector, vector_char)) {
                    clear(&operates);
                    free(vector.string);
                    return NULL;
                }
                i++;
            }

            if (!addCharacter(&vector, vector_char)) {
                clear(&operates);
                free(vector.string);
                return NULL;
            }

            if (!push(&operates, vector.string)) {
                clear(&operates);
                free(vector.string);
                return NULL;
            };
            free(vector.string);

        } else if (character == '+' || character == '-' || character == '*') {
            char *operate2 = pop(&operates);
            if(!operate2){
                clear(&operates);
                return NULL;
            }
            char *operate1 = pop(&operates);
            if(!operate1){
                free(operate2);
                clear(&operates);
                return NULL;
            }

            char *result = NULL;

            switch (character) {
                case '+': { //only {1,...,1}+{1,1,...,1}
                    if (strstr(operate1, "{") != NULL && strstr(operate2, "{") != NULL) {
                        result = doOperate(operate1, operate2, '+');
                    }
                    break;
                }
                case '-': { //only {1,...,1}-{1,1,...,1}
                    if (strstr(operate1, "{") != NULL && strstr(operate2, "{") != NULL) {
                        result = doOperate(operate1, operate2, '-');
                    }
                    break;
                }
                case '*': { //only {1,2,3}*3 or 3*{1,2,3}
                    if ((strstr(operate1, "{") == NULL) ^ (strstr(operate2, "{") == NULL)) {
                        result = doOperate(operate1, operate2, '*');
                    }
                    break;
                }
            }

            if (!result) {
                clear(&operates);
                free(operate1);
                free(operate2);
                return NULL;
            }

            push(&operates, result);
            free(result);
            free(operate1);
            free(operate2);
        }
    }
    return pop(&operates);
}

int convertToArray(char *str, long long *vector) {
    int size = 0;
    int capacity = 16;

    char *token, *delim = ",";
    str++;         //пропускаем первую скобку '{'
    while ((token = __strtok_r(str, delim, &str))) {
        if (size % capacity == 0 && size != 0) {
            long long* tmp = (long long *) realloc(vector, capacity * 2 * sizeof(long long));
            if(!tmp){
                free(vector);
                return -1;
            }
        }
        vector[size] = (long long) strtol(token, NULL, 10);
        size++;
    }
    return size;
}

// returns count of non-overlapping occurrences of 'sub' in 'str'
int countSubstring(const char *str, const char *sub) {
    size_t length = strlen(sub);
    if (length == 0) return 0;
    int count = 0;
    for (str = strstr(str, sub); str; str = strstr(str + length, sub))
        ++count;
    return count;
}

int getDigitCount(long long sum) {
    int count = 0;
    if (sum == 0) {
        return 1;
    }
    while (sum != 0) {
        sum /= 10;
        count++;
    }
    return count;
}

char *doOperate(char *l_operate, char *r_operate, char operate) {
    long long *vector = NULL;
    long long *vector1 = NULL;
    long long *vector2 = NULL;

    int coeff = 0;
    char *vector_str = NULL;

    int size = 0;
    int size1 = 0;
    int size2 = 0;

    if (operate == '*') {
        if (strstr(l_operate, "{") != NULL) {
            coeff = atoi(r_operate);
            vector_str = l_operate;
        } else {
            coeff = atoi(l_operate);
            vector_str = r_operate;
        }
        size = countSubstring(vector_str, ",") + 1;
        vector = (long long *) calloc((size_t) size, sizeof(long long));
        if(!vector){
            return NULL;
        }
        if(convertToArray(vector_str, vector)==-1){
            free(vector);
            return NULL;
        };

    } else {

        size1 = countSubstring(l_operate, ",") + 1;
        size2 = countSubstring(r_operate, ",") + 1;
        size = size2 > size1 ? size2 : size1;

        vector1 = (long long *) calloc((size_t) size, sizeof(long long));
        if(!vector1)
            return NULL;

        vector2 = (long long *) calloc((size_t) size, sizeof(long long));
        if(!vector2){
            free(vector1);
            return NULL;
        }

        if(convertToArray(l_operate, vector1)==-1|| convertToArray(r_operate, vector2)==-1){
            free(vector1);
            free(vector2);
            return NULL;
        }
    }

    char* format = NULL;
    struct buffer result = {NULL, 0, 0};
    char *number = NULL;
    int digitCount = 0;
    long long res = 0;

    if(!addCharacter(&result, '{')){
        if(vector) free(vector);
        if(vector1) free(vector1);
        if(vector2) free(vector2);
        return NULL;
    }
    for (int i = 0; i < size; i++) {
        format = (i == 0 ? "%llu" : ",%llu");

        switch (operate) {
            case '+': {
                res = vector1[i] + vector2[i];
                break;
            }
            case '-': {
                res = vector1[i] - vector2[i];
                break;
            }
            case '*': {
                res = coeff * vector[i];
                break;
            }
            default:
                break;
        }
        digitCount = getDigitCount(res);
        if (res < 0) digitCount++;

        if (i != 0) digitCount++;
        number = (char *) malloc((digitCount + 1) * sizeof(char));
        if(!number){
            if(vector) free(vector);
            if(vector1) free(vector1);
            if(vector2) free(vector2);
            return NULL;
        }
        sprintf(number, format, res);
        if(!concat(&result, number)){
            if(vector) free(vector);
            if(vector1) free(vector1);
            if(vector2) free(vector2);
            free(number);
            return NULL;
        }
        free(number);
    }
    if(!addCharacter(&result, '}')){
        if(vector) free(vector);
        if(vector1) free(vector1);
        if(vector2) free(vector2);
        return NULL;
    }

    if (vector) free(vector);
    if (vector1) free(vector1);
    if (vector2) free(vector2);
    return result.string;

}

void clear(struct StackNode **head) {
    while (!isEmpty(head)) {
        char* result=pop(head);
        free(result);
    }
}
