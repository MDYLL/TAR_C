#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#define SHIFT 65

struct LIST {
	struct OBJ *obj;
	struct LIST *next;
};

struct OBJ {
	char type; //file or directory
	char name[1000];
	int files;
	int folders;
	long int size;
	struct LIST *headDir;
	struct LIST *headFile;
};




struct OBJ * makeStruct(char *dirName, struct OBJ *cur);
void writeSignature(char *filename, struct OBJ *root,char *dirName);
struct OBJ* readSignature(char *filename,FILE *fp,char dirName[]);
struct LIST *createDirList(struct OBJ *folder);
struct LIST *addDirToTail(struct OBJ *folder, struct OBJ *parent);
struct LIST *createFileList(struct OBJ *file);
struct LIST *addFileToTail(struct OBJ *file, struct OBJ *parent);
void writeContent(char *filename, struct OBJ *root);
void extractContent(char *destination,char *filename);
char *getName(char *dirName, char *path);