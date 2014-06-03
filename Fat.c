#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifndef CLUSTER
#define CLUSTER 4096
#endif
//Bloco com 256 linhas cada
//hexdump -C fat.part
/* tabela FAT, 1024 entradas de 4 bytess */
uint32_t fat[1024];
/* tabela BOOT_BLOCK, 4096 entradas de 1 byte */
uint8_t boot[CLUSTER];
/* entrada de diretorio, 32 bytes cada */
typedef struct  {
	uint8_t filename[16];
	uint8_t attributes;
	uint8_t reserved[7];
	uint32_t first_block; //olha na fat
	uint32_t size;
}dir_entry;

/* diretorios (incluindo ROOT), 128 entradas de diretorio com 32 bytes cada = 4096 bytes  */
dir_entry dir[128];

FILE  *memoria_fat;

void fillBoot(){
	int i;
	for (i = 0; i < CLUSTER; i++)
	{
		/* code */
		boot[i] = 0xa5;
	}
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, 0 , SEEK_SET);
	fwrite(&boot, CLUSTER, 1, memoria_fat);
	fclose(memoria_fat);
}

void fillFat(){
	int i;
	fat[0] = -1;
	fat[1] = -1;
	fat[2] = -1;
	for (i = 3; i < sizeof(fat)/sizeof(fat[0]); i++)
	{
		/* code */
		fat[i] = 0;
	}
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, CLUSTER, SEEK_SET);
	fwrite(&fat, sizeof(fat), 1, memoria_fat);
	fclose(memoria_fat);
}

void fillRootDir(){
	int i;
	for (i = 0; i < sizeof(dir) / sizeof(dir[0]); i++)
	{
		/* code */
		dir[i].size = 0x00;
		dir[i].first_block = 0x00;
		int j;
		for(j=0; j< sizeof(dir[i].filename)/ sizeof(dir[i].filename[0]);j++){
			dir[i].filename[j]=0x0;
		}
		dir[i].attributes = 0x0;
		for(j=0; j< sizeof(dir[i].reserved)/ sizeof(dir[i].reserved[0]);j++){
			dir[i].reserved[j]=0x0;
		}
	}
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, 2*CLUSTER, SEEK_SET);
	fwrite(&dir, sizeof(dir), 1, memoria_fat);
	fclose(memoria_fat);
}

void fillDataCluster(){
	int i;
	uint8_t data[CLUSTER];
	for (i = 0; i < CLUSTER; i++)
	{
		data[i] = 0x00;
	}
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, CLUSTER*3, SEEK_SET);
	for (i = 1; i <= 1021; i++)
	{
		fwrite(&data, CLUSTER, 1, memoria_fat);
	}
	fclose(memoria_fat);
}

void loadBoot(){
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, 0 , SEEK_SET);	
	fread(&boot, CLUSTER, 1, memoria_fat);
	fclose(memoria_fat);
}
void loadFat(){
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, CLUSTER, SEEK_SET);
	fread(&fat, CLUSTER, 1, memoria_fat);
	fclose(memoria_fat);
}

void loadRootDir(){
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, CLUSTER*2, SEEK_SET);
	fread(&dir, CLUSTER, 1, memoria_fat);
	fclose(memoria_fat);
}

void printArray(uint32_t array[]){
	int i;
	printf("%d\n", sizeof(array)*256 );
	for (i = 0; i < sizeof(array) *256; i++)
	{
		/* code */
		printf("%d\n",array[i]);	
	}
}

void init(){
	memoria_fat = fopen("fat.part","w+"); //cria arquivo
	fclose(memoria_fat);
	fillBoot();
	fillFat();
	fillRootDir();
	fillDataCluster();
	//printArray(fat);
}; 

int searchDirectory(char *directory){
	//setar dir
	int i;	
	for(i=0; sizeof(dir)/ sizeof(dir[0]);i++){
		dir[i];
	}
	//retorna 1 caso encontre, 0 caso não encontre
	return 0;
}


void shell(){
	char cmd[128];
	while(cmd[0] != 2){
		scanf("%s", &cmd[0]);	
		//printf("%s\n",cmd[0] );
	}
}

void load(){
	loadBoot();
	loadFat();
	loadRootDir();
}

void makeDir();

void makeFile();

void removeDir();

void removeFile();

void listDir(char *path){
	loadRootDir();
	int i=0;
	int initDir = 0;
	char *token;
	path = strdup(path);
	printf("%s\n",path);
	token = strtok(path, "/"); // ROOT
	printf("%s\n",token);
	token = strtok(NULL,"/");	//PRIMEIRA PASTA
	printf("%s\n",token);
	while(token != NULL){
		//searchDirectory(token);
		printf("%s\n",token);
		token = strtok(NULL,"/");
	}


}

void write();

void cat();


int main(int argc, char const *argv[])
{
	init();
	load();
	listDir("/1/2/3/4");


	//shell();
	return 0;
}



