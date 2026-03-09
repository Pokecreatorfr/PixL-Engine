#pragma once

#define malloc(...)  ENGINE_MALLOC_FORBIDDEN()
#define calloc(...)  ENGINE_CALLOC_FORBIDDEN()
#define realloc(...) ENGINE_REALLOC_FORBIDDEN()
#define free(...)    ENGINE_FREE_FORBIDDEN()

__attribute__((error("malloc() interdit dans le moteur")))
void ENGINE_MALLOC_FORBIDDEN(void);

__attribute__((error("calloc() interdit dans le moteur")))
void ENGINE_CALLOC_FORBIDDEN(void);

__attribute__((error("realloc() interdit dans le moteur")))
void ENGINE_REALLOC_FORBIDDEN(void);

__attribute__((error("free() interdit dans le moteur")))
void ENGINE_FREE_FORBIDDEN(void);