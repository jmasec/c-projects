#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

size_t strlen(const char* str);
char* strcpy(const char*src);
char* strncpy(const char*src, size_t n);
size_t strcmp(const char*str1, const char*str2);
size_t strncmp(const char*str1, const char*str2, size_t n);
char* strcat(const char*str1, const char*str2);
char* strncat(const char*str1, const char*str2, size_t n);
char* strchr(const char *str, char c);
char* strrchr(const char *str, char c);
char* strstr(const char *str, const char *sub_str);
size_t strspn(const char *str, const char *accept);
size_t strcspn(const char *str, const char *reject);
char** strtok(char *str, const char *delim);
char * strdup( const char *str1 );
void *memset(void *ptr, int x, size_t n);

size_t strlen(const char* str){
    size_t count = 0;
    if (str == NULL){
        return -1;
    }
    while (*str != '\0'){
        count++;
        str++;
    }
    return count;
}

char* strcpy(const char*src){
    if (src == NULL){
        return NULL;
    }
    char* buffer = (char*)malloc((strlen(src)+1));
    char* start = buffer;

    while(*src != '\0'){
        *buffer = *src;
        buffer++;
        src++;
    }
    *buffer = '\0';
    return start;
}

char* strncpy(const char*src, size_t n){
    if (src == NULL){
        return NULL;
    }
    char* buffer = (char*)malloc(n);
    char* start = buffer;
    size_t count = 0;

    while (count < n){
       
        *buffer = *src;
        buffer++;
        src++;
        count++;
    }
    *buffer = '\0';
    return start;
}

size_t strcmp(const char*str1, const char*str2){
    if (str1 == NULL || str2 == NULL){
        return -1;
    }
    if (strlen(str1) != strlen(str2)){
        return -1;
    }

    for(int i = 0; i < strlen(str1); i++){
        if (str1[i] != str2[i]){
            return -1;
        }
    }

    return 0;
}

size_t strncmp(const char*str1, const char*str2, size_t n){
    if (str1 == NULL || str2 == NULL){
        return -1;
    }
    for(int i = 0; i < n; i++){
        if (str1[i] != str2[i]){
            return -1;
        }
    }

    return 0;
}

char* strcat(const char*str1, const char*str2){
    if (str1 == NULL || str2 == NULL){
        return NULL;
    }
    size_t total_len = strlen(str1) + strlen(str2);
    char* buf = (char*)malloc(total_len+1);
    char* temp = buf;
    while(*str1 != '\0'){
        *temp = *str1;
        temp++;
        str1++;
    }
    while(*str2 != '\0'){
        *temp = *str2;
        temp++;
        str2++;
    }
    *temp = '\0';
    return buf;
}

char* strncat(const char*str1, const char*str2, size_t n){
    if (str1 == NULL || str2 == NULL){
        return NULL;
    }
    size_t total_len = n+n;
    char* buf = (char*)malloc(total_len+1);
    char* temp = buf;
    size_t count = 0;

    while (count < total_len){
        if (count < n){
            *temp = *str1;
            temp++;
            str1++;
            count++;
        }
        else{
            *temp = *str2;
            temp++;
            str2++;
            count++;
        }
    }

    *temp = '\0';
    return buf;
}

char* strchr(const char *str, char c){
    if (str == NULL){
        return NULL;
    }
    while (*str != '\0'){
        if (*str == c){
            return (char *)str;
        }
        str++;
    }
    return NULL;
}

char* strrchr(const char *str, char c){
    if (str == NULL){
        return NULL;
    }
    size_t len = strlen(str)-1;
    size_t count = 0;
    str += len;
    while (count < len){
        if (*str == c){
            return (char *)str;
        }
        str--;
        count++;
    }
    return NULL;

}

char* strstr(const char *str, const char *sub_str){
    if (*sub_str == '\0'){
        return NULL;
    }

    if (*sub_str == '\0') {
        return (char *)str;
    }

    while (*str != '\0'){
        if(strncmp(str, sub_str, strlen(sub_str)-1) == 0){
            return (char *)str;
        }
        else{
            str++;
        }
    }

    return NULL;
}

size_t strspn(const char *str, const char *accept){

    if (!*str){
        return 0;
    }

    size_t count = 0;
    const char * start = accept;
    char inAccept = 0;

    while(*str){
        while(*accept){
            if(*accept == *str){
                count++;
                inAccept = 1;
                break;
            }
            accept++;
        }
        if(!inAccept){
            return count;
        }
        inAccept = 0;
        accept = start;
        str++;
    }
    return count;
}

size_t strcspn(const char *str, const char *reject){
    if (!*str){
        return 0;
    }

    size_t count = 0;
    const char * start = reject;
    char inReject = 0;

    while(*str){
        while(*reject){
            if(*reject == *str){
                inReject = 1;
                break;
            }
            reject++;
        }
        if(!inReject){
            count++;
        }
        else{
            return count;
        }
        inReject = 0;
        reject = start;
        str++;
    }
    return count;
}

char** strtok(char *str, const char *delim){
    char ** tokenize = malloc(sizeof(char *) * 50);
    char * start = str;
    size_t index = 0;

    while(*str){
        if (*str == *delim){
            size_t len = str - start;
            if (len > 0){ 
                tokenize[index] = strncpy((char *)start,len);
                str++;
                start = str;
                index++;
            }
            else{
                str++;
                start = str;
            }
        }
        else{
            str++;
        }
    }
    size_t len = str - start;
    tokenize[index] = strncpy((char *)start,len+1);
    index++;
    tokenize[index] = NULL;

    return tokenize;
}

char * strdup(const char *str1){
    // this one would need to malloc in here for the size, and then pass a buffer to strcpy who should not malloc itself
    char * str2 = strcpy(str1);
    return str2;
}

void *memset(void *ptr, int x, size_t n){
    // ptr -> start addr
    // x -> val to be filled
    // n -> number of bytes to be filled
    char * str = ptr;
    for(int i = 0; i < n; i ++){
        str[i] = x;
    }

    return (void*)str;
}

void main(){
    char str[50] = "GeeksForGeeks is for programming geeks.";
    memset(str + 13, '.', 8*sizeof(char));
    printf("%s\n", str);
    
    // char * s = strdup("duplicate meeeee!");
    // printf("%s\n",s);
    // free(s);
    
    // char* s = strchr("Hello World", 'o');
    // printf("%s\n", s);

    // char* a = strrchr("Hello World", 'o');
    // printf("%s\n", a);

    // char* b = strstr("Patient Age: 36", "Age");
    // if (b != NULL){
    //     printf("%s\n", b);
    // }

    // size_t i = strspn("123", "123");
    // printf("%d\n", i);

    // size_t j = strcspn("123", "abc");
    // printf("%d\n", j);

    // char ** tokens = strtok("hello,world ,I, am,, Jared",",");
    // // needs to handle array of deliminators too
    // size_t p = 0;
    // while (tokens[p] != NULL) {
    //     printf("%s\n", tokens[p]);
    //     p++;
    // }

    // char str[] = "Hello World";
    // size_t len = strlen(str);
    // printf("%d\n", len);

    // char* buf = strcpy(str);
    // printf("%s\n", buf);
    // free(buf);

    // char* buf2 = strncpy(str, 4);
    // printf("%s\n", buf2);
    // free(buf2);

    // char str1[] = "Hello123";
    // char str2[] = "Hello123";
    // size_t ret = strcmp(str1, str2);
    // printf("%d\n", ret);

    // char str3[] = "Herlo123";
    // char str4[] = "Hello123";
    // size_t ret2 = strncmp(str3, str4, 5);
    // printf("%d\n", ret2);

    // char str5[] = "Herlo";
    // char str6[] = "Hello";
    // char* buf3 = strcat(str5, str6);
    // printf("%s\n", buf3);
    // free(buf3);

    // char* buf4 = strncat("Herlo", "Hello", 3);
    // printf("%s\n", buf4);
    // free(buf4);
}


