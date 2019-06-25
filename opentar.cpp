#define _CRT_SECURE_NO_WARNINGS
#include "maketar.h"
#include <deque>
#include <direct.h>

using namespace std;

char *getName(char *dirName,char *path)
{
	char out[1000];
	int i = 0;
	while (dirName[i++] == path[i]);
	int j = 0;
	i--;
	while (path[i] != 0)
	{
		out[j++] = path[i++];
	}
	if (out[j - 1] == '\\')
		j--;
	out[j] = 0;
	return out;

}

struct OBJ* readSignature(char *filename,FILE *fp,char dirName[])
{
	//FILE *fp;
	struct OBJ *root, *temp;
	char buf1[30];
//	fp = fopen(filename, "rb");
	fread(buf1, sizeof(char), 3, fp);
	buf1[3] = 0;
	int lengthOriginalPath;
	fread(&lengthOriginalPath, sizeof(int), 1, fp);
	fread(dirName, sizeof(char), lengthOriginalPath, fp);
	dirName[lengthOriginalPath] = 0;
	struct LIST *buf = nullptr;
	deque<struct OBJ*> data;
	root = (struct OBJ*)malloc(sizeof(struct OBJ));
	fread(root, sizeof(struct OBJ), 1, fp);
	data.push_back(root);
	while (data.size())
	{
		temp = data.front();
		data.pop_front();
		if (temp->files > 0)
		{
			//cur = (struct OBJ*)malloc(sizeof(struct OBJ));
			temp->headFile = (struct LIST*)malloc(sizeof(sizeof LIST));
			temp->headFile->obj = (struct OBJ*)malloc(sizeof(struct OBJ));
			fread(temp->headFile->obj, sizeof(struct OBJ), 1, fp);
			buf = temp->headFile;
		}
		for (int i = 1; i < temp->files; i++)
		{
			buf->next = (struct LIST*)malloc(sizeof(sizeof LIST));
			buf->next->obj = (struct OBJ*)malloc(sizeof(struct OBJ));
			fread(buf->next->obj, sizeof(struct OBJ), 1, fp);
			buf = buf->next;
		}

		if (temp->folders > 0)
		{
			//cur = (struct OBJ*)malloc(sizeof(struct OBJ));
			temp->headDir = (struct LIST*)malloc(sizeof(sizeof LIST));
			temp->headDir->obj = (struct OBJ*)malloc(sizeof(struct OBJ));
			fread(temp->headDir->obj, sizeof(struct OBJ), 1, fp);
			buf = temp->headDir;
			data.push_back(buf->obj);
		}
		for (int i = 1; i < temp->folders; i++)
		{
			buf->next = (struct LIST*)malloc(sizeof(sizeof LIST));
			buf->next->obj = (struct OBJ*)malloc(sizeof(struct OBJ));
			fread(buf->next->obj, sizeof(struct OBJ), 1, fp);
			buf = buf->next;
			data.push_back(buf->obj);
		}

		if (temp->type == 'E')
			break;
	}

	//fclose(fp);
	return root;
}

void extractContent(char *destination, char *filename)
{
	deque<struct OBJ*> data;
	struct OBJ *temp;
	struct LIST *buf;
	char name[400],ch,dirName[1000];
	FILE *fp_out, *fp_in;
	fp_in = fopen(filename, "rb");
	struct OBJ *root = readSignature(filename,fp_in,dirName);
	data.push_back(root);
	while (data.size())
	{
		temp = data.front();
		data.pop_front();
		if (temp->folders > 0)
		{
			buf = (struct LIST*)malloc(sizeof(struct LIST));
			buf = temp->headDir;
			strcpy(name, destination);
			strcat(name, getName(dirName,buf->obj->name));
			_mkdir(name);
			data.push_back(buf->obj);
			for (int i = 1; i < temp->folders; i++)
			{
				buf = buf->next;
				strcpy(name, destination);
				strcat(name, getName(dirName,buf->obj->name));
				_mkdir(name);
				data.push_back(buf->obj);
			}
		}
		if (temp->files > 0)
		{
			buf = (struct LIST*)malloc(sizeof(struct LIST));
			buf = temp->headFile;
			strcpy(name, destination);
			strcat(name, getName(dirName,buf->obj->name));
			fp_out = fopen(name, "wb");
			for (int i = 0; i < buf->obj->size; i++)
			{
				fread(&ch, sizeof(char), 1, fp_in);
				fwrite(&ch, sizeof(char), 1, fp_out);
			}
			fclose(fp_out);
			for (int i = 1; i < temp->files; i++)
			{
				buf = buf->next;
				strcpy(name, destination);
				strcat(name, getName(dirName,buf->obj->name));
				fp_out = fopen(name, "wb");
				for (int i = 0; i < buf->obj->size; i++)
				{
					fread(&ch, sizeof(char), 1, fp_in);
					fwrite(&ch, sizeof(char), 1, fp_out);
				}
				fclose(fp_out);
			}
		}
	}

}