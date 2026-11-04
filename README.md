# zipzip

A small C program that analyzes a file as a sequence of 4-byte blocks. It counts how often each unique block appears, sorts them by frequency, and splits the result into the most common blocks and the rest. It is groundwork for a block-based compression tool.

## How it works

1. **Read** – loads the whole file into memory (up to 33,556 bytes).
2. **Count** – walks the file 4 bytes at a time and builds a list of unique blocks with their occurrence count.
3. **Sort** – sorts the list by count, most frequent first.
4. **Split** – separates the top 4,196 blocks (`head`) from the remaining ones (`tail`).
5. **Print** – shows a summary of the full list, the head and the tail.

## Build

Requires a C compiler with C23 support (`-std=c2x`), such as recent Clang or GCC.

```sh
make          # clean build, produces ./zipzip
make debug    # AddressSanitizer + UBSan build, produces ./zipzip_debug
make clean    # remove build output
```

## Usage

```sh
./zipzip <file>
```

Example (running it on its own header):

```
$ ./zipzip zipzip.h
(amtlist *)list = {
  capacity = 65535
  length   = 431
}
    list->data[n]={0x7a202a2f,1}
    list->data[n]={0x697a7069,1}
    ...
(blocklist *)blocks = {
  length   = 431
}
 [0]=20202020
 [1]=696c746d
 ...
(blocklist *)listtail = {
  length   = 0
}
```

`list` is the unsorted block count, `blocks` holds the most frequent blocks (here `0x20202020`, four spaces, comes first), and `listtail` holds whatever didn't fit in the head.

## Limits

- Maximum input size is 33,556 bytes (`Maxfilesz` in `zipzip.h`).
- Trailing bytes that don't fill a full 4-byte block are ignored.
- Blocks are read in the machine's native byte order.

## Project layout

| File        | Purpose                                         |
|-------------|-------------------------------------------------|
| `zipzip.c`  | Program logic: reading, counting, sorting, filtering |
| `zipzip.h`  | Types, macros and function declarations         |
| `Makefile`  | Build targets                                   |

## Author

Kanishk Ranjan
