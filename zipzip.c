/* zipzip.c */
#include "zipzip.h"

int8 *cmdname;
int32 Filesz;


bool head(int16 idx, amtentry *val, void *arg1, void *arg2){
  int32 max;
  if( !val) 
    return false;
  max =$4 $8 arg2;
  return (idx > max) ? false : true;
}

bool tail(int16 idx, amtentry *val, void *arg1, void *arg2){
  int32 min;
  if ( !val) 
    return false;
  min =$4 $8 arg2;
  return (idx >= min) ? true : false;
}

bool amount(int16 idx, amtentry *val, void *arg1, void *arg2){
  int32 amt; 

  if(!val){
    return false;
  }
  amt = $4 $8 arg1;
  return (amt == val->amt) ;
}

blocklist *filterblocks(amtlist *list, filterfunc f, void *arg1, void *arg2){
  blocklist *p;
  int32 size;
  int16 n, items;

  if(!list) {
    return (blocklist *)0;
  }
  else if(!list->length) {
    p = (blocklist *)alloc(sizeof(struct s_blocklist));
    if(p)
      p->length = 0;
    return p;
  }
  size = (sizeof(struct s_blocklist)+(4196*sizeof(int32)));
  p = (blocklist  *)alloc(size);
  if(!p) 
    return (blocklist *)0;
  zero($1 p, size);
  for(n =0,items=0 ; n < list->length && (items < 4196); n++){
    if(f(n, &list->data[n], arg1, arg2))
      p->blocks[items++] = list->data[n].block;
  } 
  p->length = items;
  size = (sizeof(struct s_blocklist)+(items*sizeof(int32)));
  p = (blocklist *)ralloc(p, size);
  if(!p)
    return (blocklist *)0;

  return p;
}

amtlist *readblocks(int8 *contents, int32 sz){
  int32 *intptr;
  int32 interger;
  int8  *p;
  int32 n;
  amtlist *list;
  amtentry *e;

  if(!contents || !sz) {
    return (amtlist *)0;
  }

  list = mkamtlist();
  if (!list){
    return list;
  }

  for(p=contents, n=sz;n > 3 ; p+=4, n-=4) {
    intptr = (int32 *)p;
    interger = *intptr;
    e = amtsearch(list, interger);
    if (!e){
      amtadd(&list, (amtentry){.block=interger,.amt=$4 1});
    }
    else 
      e->amt++;
  }
  return list;
}

amtentry *amtsearch(amtlist *haystack, int32 needle) {
  amtentry *e;
  int32 n;

  if(!haystack)
    return (amtentry *)0;
  else if(!haystack->length)
    return (amtentry *)0;

  e = (amtentry *)0;
  for(n = 0; n < haystack->length;n++){
    if(haystack->data[n].block == needle){ 
      e = &haystack->data[n];
      break;
    }
  }

  return e;
}
int8 amtsort(entry e1_, entry e2_) {
    amtentry *e1, *e2;

    e1 = (amtentry *)&e1_;
    e2 = (amtentry *)&e2_;

    if (e1->amt > e2->amt)
        return MoreThan;
    else if (e1->amt == e2->amt)
        return Equal;
    else
        return LessThan;
}

int8 amtrevsort(entry e1_, entry e2_) {
    amtentry *e1, *e2;

    e1 = (amtentry *)&e1_;
    e2 = (amtentry *)&e2_;

    if (e1->amt > e2->amt)
        return LessThan;
    else if (e1->amt == e2->amt)
        return Equal;
    else
        return MoreThan;
}

amtlist *amtlistcopy(amtlist *list) {
    amtlist *new;
    int32 n;

    new = mkamtlist();
    if (!new)
        return (amtlist *)0;
    if (!list->length)
        return new;

    for (n=0; n<list->length; n++)
        amtadd(&new, list->data[n]);

    return new;
}

bool amtlisteq(amtlist *l1, amtlist *l2) {
    bool res;
    int32 n;

    if (!l1 || !l2)
        return false;

    if (l1->length != l2->length)
        return false;

    res = true;
    for (n=0; n<l1->length; n++)
        if ((l1->data[n].amt == l2->data[n].amt)
            && (l1->data[n].block == l2->data[n].block));
        else {
            res = false;
            break;
        }

    return res;
}

List *zipsort(List *old, sortfunc func) {
    amtlist *new;

    if (!old || !func)
        return (List *)0;

    new = (amtlist *)zipsort_(old, func);

    return (!new) ?
            old :
        zipsort((List *)new, func);
}

List *zipsort_(List *list_, sortfunc func) {
    int32 len, n;
    amtlist *new, *list;
    int8 res;
    bool changed;

    if (!func)
        return (List *)0;

    list = (amtlist *)list_;
    if (list->length < 2)
        return (List *)0;

    new = listcopy(list);
    if (!new)
        return (List *)0;
    len = list->length;

    for (n=0; n<(len-1); n++) {
        res = func((entry)new->data[n], (entry)new->data[(n+1)]);
        if (res == MoreThan)
            swap(&new->data[n], &new->data[(n+1)]);
    }

    changed = (listeq(list, new)) ?
            false :
        true;

    if (changed) {
        free(list);
        return (List *)new;
    }
    else {
        free(new);
        return (List *)0;
    }
}

void amtswap(amtentry *e1, amtentry *e2) {
    amtentry tmp;

    if (!e1 || !e2)
        return;

    tmp = *e1;
    *e1 = *e2;
    *e2 = tmp;

    return;
}

bool increase(amtlist **list) {
    amtlist *p;
    int64 size;
    int32 len;

    if (!*list || ((*list)->capacity >= Maxcap))
        return false;

    len = (*list)->capacity + Blocksize;
    size = (sizeof(struct s_amtlist)+(sizeof(amtentry)*len));
    p = (amtlist *)ralloc(*list, size);
    if (!p)
        return false;
    else {
        p->capacity = len;
        *list = p;
    }

    return true;
}

bool amtadd(amtlist **list, amtentry e) {
    int32 idx;

    if (!list)
        return false;

    if ((*list)->length == (*list)->capacity) {
        if ((*list)->capacity >= Maxcap)
            return false;
        else if (!increase(list))
            return false;
    }

    idx = (*list)->length;
    (*list)->data[idx] = e;
    (*list)->length++;

    return true;
}

void showamtlist(int8 *identifier, amtlist *list) {
    int32 n;

    if (!identifier || !list)
        return;

    printf("(amtlist *)%s = {\n  ", $c identifier);
    printf("capacity = %d\n  ", $i list->capacity);
    printf("length   = %d\n}\n", $i list->length);

    if (list->length) {
        for (n=0; (n<list->length) && (n<5); n++)
            show(list->data[n]);
        if (list->length > 4)
            printf("   [..]\n");
    }

    return;
}

void showblocklist(int8 *identifier, blocklist *list) {
    int32 n;

    if (!identifier || !list)
        return;

    printf("(blocklist *)%s = {\n  ", $c identifier);
    printf("length   = %d\n}\n", $i list->length);

    if (list->length) {
        for (n=0; (n<list->length) && (n<10); n++)
            printf(" [%d]=%.08x\n", $i n, list->blocks[n]);
        if (list->length > 9)
            printf("   [..]\n");
    }

    return;
}

void showamtentry(int8 *identifier, amtentry e) {
    if  (!identifier)
        return;

    printf("    %s={0x%.08x,%d}\n",
        $c identifier, $i e.block, $i e.amt);

    return;
}

amtlist *mkamtlist() {
    int32 size;
    amtlist *p;

    size = (sizeof(struct s_amtlist)+(sizeof(amtentry)*Blocksize));
    p = (amtlist *)alloc(size);
    if (!p)
        return (amtlist *)0;
    zero($1 p, size);

    p->length = 0;
    p->capacity = Blocksize;

    return p;
}

void zero(int8 *dst, int32 size) {
    int8 *p;
    int32 n;

    for (p=dst, n=size; n; p++, n--)
        *p = 0;

    return;
}

void fail_(int8 *str) {
  fprintf(stderr, "%s\n", str);
  exit(1);

  return;
}

int main1() {
    amtlist *list;
    List *new;
    amtlist *new_;
    int32 n;

    list = mkamtlist();
    if (!list)
        return 1;
    show(list);

    for (n=0; n<0x11000; n++)
        amtadd(&list, (amtentry){0x10000000+n, n});

    swap(&list->data[1], &list->data[3]);
    show(list);

    new = zipsort((List *)list, &amtsort);
    new_ = (amtlist *)new;
    show(new_);

   destroy(new_);

    return 0;
}


int main2() {
    amtlist *list;
    int32 n;
    amtentry *e;

    list = mkamtlist();
    if (!list)
        return 1;

    for (n=0; n<0x11000; n++)
        amtadd(&list, (amtentry){0x10000000+n, n});

    e = amtsearch(list, 0x10000000);
    if (e)
      show(*e);
    else
      printf("Not found \n");
  
    destroy(list);
  return 0;
  
}

int8 *readfile(int8* filename) {
  int32 size, fd;
  int64 filesz;
  struct stat sbuf;
  signed int ret;
  int8 *contents, *p;

  ret = open($c filename, O_RDONLY);
  if (ret < 0){
     fail("%s: No such file\n", filename);
    return $1 0;
  }
  else 
    fd =$4 ret;
  ret = fstat($i fd,  &sbuf);
  if (ret) {
    close($i fd);
    fail("%s: stat() error", $c filename);
    return $1 0;
  }
  else 
    filesz = sbuf.st_size;
  if(filesz > Maxfilesz ){
    close($i fd);
    fail("%s: File is too big", $c filename);
    return $1 0;
  }

  size = filesz + 4;
  contents = $1 alloc(size);
  if(!contents) { 
    close($i fd);
    fail("%s", "alloc(): Memory Error");
    return $1 0;

  }

  zero(contents, size);
  p = contents;
  do {
    ret = read($i fd, $c p,512 );
    if( ret > 0)
      p += ret;
    else if (ret < 0){
      close($i fd);
      fail("%s: Unable to read file", $c filename);
      return $1 0;
    }
  } while (ret) ;
  close($i fd);

  Filesz = filesz;
  
  return contents;
}

int main3(int8 *file){
  int8 *contents;
  amtlist *list, *sorted;
  blocklist *blocks,*listtail ;

  if(!file) {
    fail("%s","No file specified");
    return 1;
  }

  contents = readfile(file);
  list = readblocks(contents, Filesz);
  if (!list){
    fail("%s: Unable to read blocks", $c file);
    return 1;
  }

  show(list);
  sorted = (amtlist *)zipsort((List *)list, amtrevsort);
  if (!sorted) { 
    fail("%s: Unable to sort block list", $c file);
    destroy(contents);
    return 1;
  }

  blocks = filterblocks(sorted, &head,$v 0,$v $8 4195);
  listtail = filterblocks(sorted, &tail,$v 0, $v $8 blocks->length);

  

  if(!blocks || !listtail) {
    fail("%s", "filterblocks() filter");
    destroy(sorted);
    destroy(contents);
    return 1;
  }

  show(blocks);
  show(listtail);
  show(sorted);
  destroy(sorted);
  destroy(contents);
  destroy(blocks);
  destroy(listtail);

  return 0;
}

int main(int argc, char *argv[]) { 
  int8* filename;
  cmdname = $1 0;
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <file>\n", *argv);
    return 1;
  }
  else {
    cmdname = $1 *argv;
    filename = $1 argv[1];
  }
  return $i main3(filename);
}

