#define _CRT_SECURE_NO_WARNINGS
#include "maketar.h"
#include <deque>
using namespace std;

struct LIST *createDirList(struct OBJ *folder)
{
	struct LIST *dirList = (struct LIST*)malloc(sizeof(struct LIST));
	dirList->obj = folder;
	dirList->next = nullptr;
	return dirList;
}
struct LIST *createFileList(struct OBJ *file)
{
	struct LIST *fileList = (struct LIST*)malloc(sizeof(struct LIST));
	fileList->obj = file;
	fileList->next = nullptr;
	return fileList;
}
struct LIST *addDirToTail(struct OBJ *folder, struct OBJ *parent)
{
	struct LIST *cur = parent->headDir;
	for (int i = 0; i < parent->folders - 1; i++)
		cur = cur->next;
	struct LIST *newObj = (struct LIST*)malloc(sizeof(struct LIST));
	newObj->obj = folder;
	cur->next = newObj;
	newObj->next = nullptr;
	return newObj;
}
struct LIST *addFileToTail(struct OBJ *file, struct OBJ *parent)
{
	struct LIST *cur = parent->headFile;
	for (int i = 0; i < parent->files - 1; i++)
		cur = cur->next;
	struct LIST *newObj = (struct LIST*)malloc(sizeof(struct LIST));
	newObj->obj = file;
	cur->next = newObj;
	newObj->next = nullptr;
	return newObj;
}

struct OBJ *makeStruct(char *dirName, struct OBJ *parent)
{
	if (parent == NULL)
	{
		parent = (struct OBJ*)malloc(sizeof(struct OBJ));
		parent->type = 'D';
		parent->files = 0;
		parent->folders = 0;
		strcpy(parent->name, dirName);
		parent->size = 0;
		parent->headDir = nullptr;
		parent->headFile = nullptr;
	}
	WIN32_FIND_DATA fd;
	int dir = 0, file = 0;
	char path[500];
	strcpy(path, dirName);
	strcat(path, "*.*");
	HANDLE hFind = FindFirstFile(path, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			//if (fd.dwFileAttributes != FILE_ATTRIBUTE_REPARSE_POINT)
			if (strcmp(fd.cFileName, ".") != 0 && strcmp(fd.cFileName, "..") != 0)

			{
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					struct OBJ *folder = (struct OBJ*)malloc(sizeof(struct OBJ));
					folder->type = 'D';
					folder->files = 0;
					folder->folders = 0;
					strcpy(folder->name, parent->name);
					strcat(folder->name, fd.cFileName); 
					strcat(folder->name, "\\");
					folder->size = 0;
					if (parent->folders == 0)
					{
						parent->headDir = createDirList(folder);
					}
					else
					{
						struct LIST *obj = addDirToTail(folder, parent);
					}
					parent->folders++;
					makeStruct(folder->name, folder);
				}
				else
				{
					struct OBJ *file = (struct OBJ*)malloc(sizeof(struct OBJ));
					file->type = 'F';
					file->files = 0;
					file->folders = 0;
					strcpy(file->name, parent->name);
					strcat(file->name, fd.cFileName); 
					file->size = fd.nFileSizeLow;
					if (parent->files == 0)
					{
						parent->headFile = createFileList(file);
					}
					else
					{
						struct LIST *obj = addFileToTail(file, parent);
					}
					parent->files++;
				}
			}
		} while (FindNextFile(hFind, &fd));

		FindClose(hFind);
	}
	return parent;
}

void writeSignature(char *filename, struct OBJ *root,char *dirName)
{
	struct OBJ *temp, last;
	struct LIST *buf = nullptr;
	deque<struct OBJ*> data;
	int i;
	FILE *fp;
	fp = fopen(filename, "wb");
	fwrite("TAR", sizeof(char), 3, fp);
	int lengthOriginalPath = strlen(dirName);
	fwrite(&lengthOriginalPath, sizeof(int), 1, fp);
	fwrite(dirName, sizeof(char), lengthOriginalPath, fp);
	fwrite(root, sizeof(struct OBJ), 1, fp);

	data.push_back(root);
	while (data.size())
	{
		temp = data.front();
		data.pop_front();
		if (temp->files > 0)
		{
			buf = temp->headFile;
			fwrite(buf->obj, sizeof(struct OBJ), 1, fp);
		}
		i = 1;
		while (i < temp->files)
		{
			buf = buf->next;
			fwrite(buf->obj, sizeof(struct OBJ), 1, fp);
			i++;
		}
		if (temp->folders > 0)
		{
			buf = temp->headDir;
			fwrite(buf->obj, sizeof(struct OBJ), 1, fp);
			data.push_back(buf->obj);
		}
		i = 1;
		while (i < temp->folders)
		{
			buf = buf->next;
			fwrite(buf->obj, sizeof(struct OBJ), 1, fp);
			data.push_back(buf->obj);
			i++;
		}
	}
	//last.type = 'E';
	//fwrite(&last, sizeof(struct OBJ), 1, fp);
	fclose(fp);
}

void writeContent(char *filename, struct OBJ *root)
{
	FILE *fp_out, *fp_in;
	deque<struct OBJ*> data;
	struct OBJ *temp;
	struct LIST *buf;
	char ch;
	fp_out = fopen(filename, "ab");
	data.push_back(root);
	while (data.size())
	{
		temp = data.front();
		data.pop_front();
		if (temp->files > 0)
		{
			fp_in = fopen(temp->headFile->obj->name, "rb");
			for (int i = 0; i < temp->headFile->obj->size; i++)
			{
				fread(&ch, sizeof(char), 1, fp_in);
				fwrite(&ch, sizeof(char), 1, fp_out);
			}
			buf = (struct LIST*)malloc(sizeof(struct LIST));
			buf = temp->headFile;
			fclose(fp_in);
			for (int i = 1; i < temp->files; i++)
			{
				buf = buf->next;
				fp_in = fopen(buf->obj->name, "rb");
				for (int j = 0; j < buf->obj->size; j++)
				{
					fread(&ch, sizeof(char), 1, fp_in);
					fwrite(&ch, sizeof(char), 1, fp_out);
				}
				fclose(fp_in);
			}
		}
		if (temp->folders > 0)
		{
			buf = (struct LIST*)malloc(sizeof(struct LIST));
			buf = temp->headDir;
			data.push_back(buf->obj);
			for (int i = 1; i < temp->folders; i++)
			{
				buf = buf->next;
				data.push_back(buf->obj);
			}
		}
	}
}