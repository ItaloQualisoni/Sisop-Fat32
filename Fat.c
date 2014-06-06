#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <libgen.h>
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
int atualDirectory=2;//rootDir

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

void loadDirectory(int fatPos){
	atualDirectory = fatPos;
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, CLUSTER*fatPos, SEEK_SET);
	fread(&dir, CLUSTER, 1, memoria_fat);
	fclose(memoria_fat);
}

void saveDirectory(){
	//atualDirectory
}

void saveFat(){
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, CLUSTER, SEEK_SET);
	fwrite(&fat, CLUSTER, 1, memoria_fat);
	fclose(memoria_fat);
}

void loadRootDir(){
	loadDirectory(2);
}

int searchFreePositionInFat(){
	int i;
	for(i=0; i < sizeof(fat)/ sizeof(fat[0]);i++){ //percorre todos os "arquivos" do diretorio corrente
		if(fat[i]==0){
			return i;
		}
	}
	return -1;
}

int searchFreePosition(){
	//Procura a primeira posição livre dentro da pasta corrente.
	int i;
	for(i=0; i < sizeof(dir)/ sizeof(dir[0]);i++){ //percorre todos os "arquivos" do diretorio corrente
		if(dir[i].filename[0]==0){
			return i;
		}
	}
	return -1;
}

int lookupFile(char *name){
	int i;	
	for(i=0; i < sizeof(dir)/ sizeof(dir[0]);i++){ //percorre todos os "arquivos" do diretorio corrente
		if(strcmp(dir[i].filename,name)==0){//verifica se é este a pasta solicitada.
			return i;
		}
	}
	return -1;
}

int isFile(dir_entry entry){
	return 0;//criar mascara para verificar se é um diretorio ou um arquivo
}


int searchDirectory(char *directory){ 
	// Search in current directory (one directory for time) if exist directory set in global directory and return 1; else return 0;
	int dirPos = lookupFile(directory);
	
	if(dirPos != -1){
		if(isFile(dir[dirPos])){
			loadDirectory(dir[dirPos].first_block);
		}
	}
	//int i;	
	//for(i=0; i < sizeof(dir)/ sizeof(dir[0]);i++){ //percorre todos os "arquivos" do diretorio corrente
	//	if(strcmp(dir[i].filename,directory)==0){//verifica se é este a pasta solicitada.
	//		//carregar o arquivo
	//		loadDirectory(dir[i].first_block);
	//		break;
	//	}
	//}
	//return 0;
}

void setDirectory(char *path){
	loadRootDir();
	int i=0;
	int initDir = 0;
	char *token;
	path = strdup(path);
	token = strtok(path, "/"); // ROOT
	token = strtok(NULL,"/");	//PRIMEIRA PASTA
	while(token != NULL){
		searchDirectory(token);
		printf("%s\n",token);
		token = strtok(NULL,"/");
	}
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

void shell(){
	//char cmd[128];
	//while(cmd[0] != 2){
	//	scanf("%s", &cmd[0]);	
		//printf("%s\n",cmd[0] );
	//}
	


}

void load(){
	loadBoot();
	loadFat();
	loadRootDir();
}

void makeDir(char *path, char *directory){
	setDirectory(path);//carrega diretorio onde sera criado o diretorio
	if(lookupFile(directory) != -1){
		printf("Arquivo %s ja existente no diretorio %s",directory,path);
		return;
	}else{
		int dirPos = searchFreePosition(); //posição dentro do diretorio local aonde sera salvo o nosso querido diretorio
		int fatPos = searchFreePositionInFat(); //posição dentro da fat aonde sera salvo o nosso querido diretorio
		if(fatPos==-1){
			printf("Memoria cheia");
			return;
		}else if(dirPos==-1){
			printf("Diretorio cheio");
			return;
		}
		dir_entry entry;
		entry.size = 0x00;
		entry.first_block = fatPos;
		strcpy(entry.filename,directory);
		entry.attributes = 0x0;
		int j;		
		for(j=0; j< sizeof(entry.reserved)/ sizeof(entry.reserved[0]);j++){
			entry.reserved[j]=0x0;
		}
		//setar reservados
		dir[dirPos] = entry;


	}
}

void makeFile();

void removeDir();

void removeFile();

void listDir(char *path){
	


}

void write();

void cat();

int main(int argc, char const *argv[])
{
	init();
	load();
	/*Separando o diretorio do arquivo*/
	char *path = "/teste/italo/i";
	path = strdup(path);
	printf("Path: %s\n",path);
	printf("Basename: %s\n",basename(path));
	printf("Dirname : %s\n",dirname(path));
	makeDir(dirname(path),basename(path));
	

	//shell();
	return 0;
}



