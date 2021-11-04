#ifndef STRING_H
#define STRING_H 1

#include <stddef.h>
#include <stdint.h>

int ffs(int i);

int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);


void    *memccpy(void *restrict, const void *restrict, int, size_t);
void    *memchr(const void *, int, size_t); // TODO
int      memcmp(const void *, const void *, size_t);
void    *memcpy(void *restrict, const void *restrict, size_t);
void    *memmove(void *, const void *, size_t);
void    *memset(void *, int, size_t);
char    *stpcpy(char *restrict, const char *restrict); // TODO
char    *stpncpy(char *restrict, const char *restrict, size_t); // TODO
char    *strcat(char *restrict, const char *restrict); // TODO
char    *strchr(const char *, int); // TODO
int      strcmp(const char *, const char *);
int      strcoll(const char *, const char *);
char    *strcpy(char *restrict, const char *restrict);
size_t   strcspn(const char *, const char *); // TODO
char    *strdup(const char *); // TODO
char    *strerror(int); // TODO
int      strerror_r(int, char *, size_t); // TODO
size_t   strlen(const char *);
char    *strncat(char *restrict, const char *restrict, size_t); // TODO
int      strncmp(const char *, const char *, size_t); // TODO
char    *strncpy(char *restrict, const char *restrict, size_t);
char    *strndup(const char *, size_t); // TODO
size_t   strnlen(const char *, size_t);
char    *strpbrk(const char *, const char *); // TODO
char    *strrchr(const char *, int); // TODO
char    *strsignal(int); // TODO
size_t   strspn(const char *, const char *); // TODO
char    *strstr(const char *, const char *); // TODO
char    *strtok(char *restrict, const char *restrict); // TODO
char    *strtok_r(char *restrict, const char *restrict, char **restrict); // TODO
size_t   strxfrm(char *restrict, const char *restrict, size_t); // TODO

#endif /* string_h */
