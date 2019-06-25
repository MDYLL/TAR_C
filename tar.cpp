#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include "maketar.h"

struct OBJ obj[1000];
struct OBJ *pobj[1000];
struct OBJ *root=nullptr,*root2;


int main()
{
	char dirName[1000] = "C:\\Users\\User\\source\\repos\\lab2\\";
	char filename[30] = "1.tar";
	char destination[50]="C:\\Users\\test\\";
	//root=makeStruct(dirName,root);
	//writeSignature(filename, root,dirName);
	//writeContent(filename, root);
	extractContent(destination, filename);

	return 0;
}
