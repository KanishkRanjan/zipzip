/* zipzip.h */
#pragma once
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

union u_entry;
 typedef union u_entry entry;
union u_list;
 typedef union u_list List;
struct s_amtentry;
 typedef struct s_amtentry amtentry;

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;
typedef unsigned long long int int64;
typedef int8 sortres;
typedef int8 (*sortfunc)(entry,entry);
typedef bool (*filterfunc)(int16,amtentry*,void*,void*);

#define Blocksize   0xffff
#define Maxcap      ((0xffffffff/8)-Blocksize-1)
#define Maxfilesz 33'556

#define $c (char *)
#define $i (int)
#define $v (void *)
#define $1 (int8 *)
#define $2 (int16)
#define $4 (int32)
#define $8 (int64)

#define alloc(x)        malloc($i (x))
#define destroy(x)      free((x))
#define ralloc(x,y)     realloc((x),$i (y))
#define show(x)         _Generic((x), \
    amtentry:           showamtentry, \
    blocklist*:         showblocklist, \
    amtlist*:           showamtlist) \
        ($1 (# x), (x))
#define swap(x,y)       _Generic((x), \
    amtentry*:          amtswap) \
        ((x),(y))
#define listcopy(x)     _Generic((x), \
    amtlist*:           amtlistcopy) \
        ((x))
#define listeq(x,y)     _Generic((x), \
    amtlist*:           amtlisteq) \
        ((x),(y))

#define fail(f, ...) do { \
  int8 _buf[128]; \
  snprintf($c _buf, 127, (f), __VA_ARGS__); \
  fail_(_buf); \
} while (false)

#define uniq(i,v) amount((i), (v), $v 1, $v 0);

struct s_amtentry {
    int32 block;
    int32 amt;
};

struct s_amtlist {
    int32 capacity;
    int32 length;
    amtentry data[];
};
typedef struct s_amtlist amtlist;

struct s_blocklist { 
  int16 length;
  int32 blocks[];
};

typedef struct s_blocklist blocklist; 

union u_entry {
    amtentry amt;
};

union u_list {
    amtlist *amt;
};

enum {
    MoreThan=0,
    Equal,
    LessThan
};

// constructors
amtlist *mkamtlist(void);

void showamtlist(int8*,amtlist*);
void showamtentry(int8*,amtentry);
void showblocklist(int8*,blocklist*);

bool amtadd(amtlist**,amtentry);
bool increase(amtlist**);
void amtswap(amtentry*,amtentry*);
void fail_(int8*);
int8 *readfile(int8*);
amtlist *readblocks(int8*,int32);

List *zipsort_(List*,sortfunc);
List *zipsort(List*,sortfunc);
amtlist *amtlistcopy(amtlist*);
bool amtlisteq(amtlist*,amtlist*);
int8 amtsort(entry,entry);
amtentry *amtsearch(amtlist*, int32);
bool head(int16,amtentry*,void*,void*);
blocklist *filterblocks(amtlist*, filterfunc, void*, void*);
bool tail(int16,amtentry*,void*,void*);
bool amount(int16,amtentry*,void*,void*);

void zero(int8*,int32);
int main(int, char**);
int main1(void);
int main2(void);
int main3(int8*);
