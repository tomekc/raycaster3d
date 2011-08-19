
typedef unsigned long dword;

#define PACK 0x4B434150

typedef struct {
        dword id;
        dword dir_offset;
        dword dir_size;
} PAKheader;

typedef struct {
        char name[56];
        dword offset;
        dword length;
} PAKentry;

