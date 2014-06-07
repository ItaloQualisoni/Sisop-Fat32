#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <libgen.h>
#ifndef CLUSTER
#define CLUSTER 4096
#define mascDeleted 0x80
#define mascDir 0x08
#endif
//Bloco com 256 linhas cada
//hexdump -C fat.part

/* tabela FAT, 1024 entradas de 4 bytess */
uint32_t fat[1024];

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
char data[CLUSTER];


FILE  *memoria_fat;

void fillBoot(){
	uint8_t boot[CLUSTER];
	int i;
	for (i = 0; i < CLUSTER; i++)
	{
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

uint8_t* loadBoot(){
	uint8_t boot[CLUSTER];
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, 0 , SEEK_SET);	
	fread(&boot, CLUSTER, 1, memoria_fat);
	fclose(memoria_fat);
	return boot;
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

void loadFile(int fatPos){
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, CLUSTER*fatPos, SEEK_SET);
	fread(&dir, CLUSTER, 1, memoria_fat);
	fclose(memoria_fat);
}

void saveDirectory(){
	//atualDirectory utlizando o valor de atualDirectory
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, CLUSTER*atualDirectory, SEEK_SET);
	fwrite(&dir, CLUSTER, 1, memoria_fat);
	fclose(memoria_fat);
}

void saveFat(){
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, CLUSTER, SEEK_SET);
	fwrite(&fat, CLUSTER, 1, memoria_fat);
	fclose(memoria_fat);
}

void loadBlock(int fatPos){
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, CLUSTER*fatPos, SEEK_SET);
	fread(&data, CLUSTER, 1, memoria_fat);
	fclose(memoria_fat);
}

void saveBlock(int fatPos){
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, CLUSTER*fatPos, SEEK_SET);
	fwrite(&data, CLUSTER, 1, memoria_fat);
	fclose(memoria_fat);
}

void loadRootDir(){
	loadDirectory(2);
}

int isFile(dir_entry entry){
	return (entry.attributes & mascDir)==0;//criar mascara para verificar se é um diretorio ou um arquivo
}

int isDeleted(dir_entry entry){
	return (entry.attributes & mascDeleted)!= 0;
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
		if(dir[i].filename[0]==0 || isDeleted(dir[i])){
			return i;
		}
	}
	return -1;
}

int lookupFile(char *name){
	int i;	
	for(i=0; i < sizeof(dir)/ sizeof(dir[0]);i++){ //percorre todos os "arquivos" do diretorio corrente
		if(strcmp(dir[i].filename,name)==0 && !isDeleted(dir[i])){//verifica se é este a pasta solicitada.
			return i;
		}
	}
	return -1;
}

int searchDirectory(char *directory){ 
	// Search in current directory (one directory for time) if exist directory set in global directory and return 1; else return 0;
	int dirPos = lookupFile(directory);
	
	if(dirPos != -1){
		if(isFile(dir[dirPos]) == 0 && !isDeleted(dir[dirPos])){
			loadDirectory(dir[dirPos].first_block);
			return 1;
		}
	}
	return 0;
}

int setDirectory(char *path){
	loadRootDir();
	int i=0;
	int initDir = 0;
	char *token;
	path = strdup(path);
	token = strtok(path, "/"); // ROOT
	//token = strtok(NULL,"/");	//PRIMEIRA PASTA
	while(token != NULL){
		if(searchDirectory(token) == 0){
			printf("diretorio %s não encontrado\n",token);
			return 0;
		}
		token = strtok(NULL,"/");
	}
	return 1;
}

void init(){
	memoria_fat = fopen("fat.part","w+"); //cria arquivo
	fclose(memoria_fat);
	fillBoot();
	fillFat();
	fillRootDir();
	fillDataCluster();
}; 



void load(){
	loadBoot();
	loadFat();
	loadRootDir();
}

void makeDir(char *path, char *directory){
	loadFat();
	//carrega diretorio onde sera criado o diretorio e verifica se este existe
	if(setDirectory(path) == 0 ){
		return;
	}
	if(lookupFile(directory) != -1){
		printf("Arquivo %s ja existente no diretorio %s\n",directory,path);
		return;
	}else{
		int dirPos = searchFreePosition(); //posição dentro do diretorio local aonde sera salvo o nosso querido diretorio
		int fatPos = searchFreePositionInFat(); //posição dentro da fat aonde sera salvo o nosso querido diretorio
		if(fatPos==-1){
			printf("Memoria cheia\n");
			return;
		}else if(dirPos==-1){
			printf("Diretorio cheio\n");
			return;
		}
		dir_entry entry;
		entry.size = 0x00;
		entry.first_block = fatPos;
		int j;		
		for(j=0; j< sizeof(entry.filename)/ sizeof(entry.filename[0]);j++){
			entry.filename[j]=0x0;
		}
		strcpy(entry.filename,directory);
		entry.attributes = 0x08;
		for(j=0; j< sizeof(entry.reserved)/ sizeof(entry.reserved[0]);j++){
			entry.reserved[j]=0x0;
		}
		//setar reservados
		dir[dirPos] = entry;
		fat[fatPos] = -1;
		saveFat();
		saveDirectory();

	}
}

void makeFile(char *path, char *file){
	loadFat();
	//carrega diretorio onde sera criado o diretorio e verifica se este existe
	if(setDirectory(path) == 0 ){
		return;
	}
	if(lookupFile(file) != -1){
		printf("Arquivo %s ja existente no diretorio %s\n",file,path);
		return;
	}else{
		int dirPos = searchFreePosition(); //posição dentro do diretorio local aonde sera salvo o nosso querido diretorio
		int fatPos = searchFreePositionInFat(); //posição dentro da fat aonde sera salvo o nosso querido diretorio
		if(fatPos==-1){
			printf("Memoria cheia\n");
			return;
		}else if(dirPos==-1){
			printf("Diretorio cheio\n");
			return;
		}
		dir_entry entry;
		entry.size = 0x00;
		entry.first_block = fatPos;
		int j;		
		for(j=0; j< sizeof(entry.filename)/ sizeof(entry.filename[0]);j++){
			entry.filename[j]=0x0;
		}
		strcpy(entry.filename,file);
		entry.attributes = 0x0;
		for(j=0; j< sizeof(entry.reserved)/ sizeof(entry.reserved[0]);j++){
			entry.reserved[j]=0x0;
		}
		//setar reservados
		dir[dirPos] = entry;
		fat[fatPos] = -1;
		saveFat();
		saveDirectory();

	}
}

void removeFile(char *path, char *file){
	loadFat();
	if(setDirectory(path) == 0 ){
		return;
	}
	int dirPos = lookupFile(file);
	if(dirPos == -1){
		printf("Arquivo %s não existente no diretorio %s\n",file,path);
		return;
	}

	int fatPos = dir[dirPos].first_block;

	while(fat[fatPos] != -1){
		int aux = fat[fatPos];
		fat[fatPos] = 0;
		fatPos= aux;
	}
	fat[fatPos] =0;
	
	dir[dirPos].attributes |= mascDeleted;

	saveFat();
	saveDirectory();
}


void rmDir(char *path, char *directory){
	loadFat();
	if(setDirectory(path) == 0 ){
		return;
	}
	int dirPos = lookupFile(directory);
	if(dirPos == -1){
		printf("Arquivo %s não existente no diretorio %s\n",directory,path);
		return;
	}
	int i;
	char *pathv1= strcat(strdup("/"),directory);
	char *pathv2= strdup(strcat(strdup(path),pathv1));
			
	for (i = 0; i <sizeof(dir)/ sizeof(dir[0]) ; i++)
	{
		if(!isDeleted(dir[i])){
			if (isFile(dir[i]))
			{
				removeFile(pathv2,dir[i].filename);
			}
			else{
				rmDir(pathv2,dir[i].filename);
			}
		}
	}
	setDirectory(path);
	dir[dirPos].attributes |= mascDeleted;
	fat[dir[dirPos].first_block] = 0;
	saveFat();
	saveDirectory();
}


void ls(char *path){
	setDirectory(path);
	int i;
	for (i = 0; i <sizeof(dir)/ sizeof(dir[0]) ; i++)
	{
		if(!isDeleted(dir[i]) && dir[i].filename[0]!=0){
			printf("%s\n",dir[i].filename);
		}
	}
}

void cleanData(){
	int i;
	for(i=0;i<CLUSTER;i++){
		data[i]=0;
	}
}

void write(char *path, char *file, char *text){
	setDirectory(path);
	int filePos = lookupFile(file);
	if (filePos==-1)
	{
		printf("Arquivo não encontrado no diretorio%s\n",path);
		return;
	}else{
		loadFat();

		int fatPos = dir[filePos].first_block;

		while(fat[fatPos]!=-1){fatPos = fat[fatPos];}

		int total = (int)strlen(text);
		int j=dir[filePos].size % CLUSTER;
		int i=0;
		printf("comecei em %d, size: %d\n", j,dir[filePos].size);
		loadBlock(fatPos);
		while(i<total){
			while(j < CLUSTER && i<total){
				data[j] = text[i];
				j++;
				i++;
			}
			printf("teste: %d\n",i);
			saveBlock(fatPos);
			int aux = searchFreePositionInFat();
			fat[fatPos] = aux;
			fatPos = aux;
			if(fatPos==-1){
				printf("Não há espaço suficiente na memoria");
				return;
			}
			fat[fatPos] = -1;
			j=0;
			cleanData();
		}
		dir[filePos].size += total;
		saveFat();
		saveDirectory();
	}
}

void cat(char *path, char *file){ //TODO revisar cat e write
	setDirectory(path);
	int filePos = lookupFile(file);
	if (filePos==-1)
	{
		printf("Arquivo não encontrado no diretorio%s\n",path);
		return;
	}else{
		int teste;
		int fatPos = dir[filePos].first_block;
		while(fat[fatPos]!=-1){
			printf("%d\n",fatPos);
			loadBlock(fatPos);
			for(teste=0;teste<CLUSTER;teste++){
				printf("%c",data[teste] );
			}
			fatPos = fat[fatPos];
		}
		loadBlock(fatPos);
		for(teste=0;teste<CLUSTER;teste++){
			printf("%c",data[teste] );
		}
		printf("\n");
	}

}

void shell(){
	while(1){
		char *cmd= strdup("");
		printf("Digite o comando:");;
		fgets(cmd,1024*CLUSTER,stdin);
		char *opcao = strtok(cmd, " ");
		printf("opcao : %s,%d\n",opcao,strcmp("init\n",opcao));
		if(strcmp("init\n",opcao)==0){
			init();
		}else if(("load",opcao)){
			load();
		}else if(strcmp("exit\n",opcao)){
			printf("volte sempre\n");
			return;
		}else{
			char *pathAndName = strtok(NULL, " ");
			char *path = strdup(basename(pathAndName));
			char *name = strdup(dirname(pathAndName));
			if(strcmp("mkdir",opcao)){
				mkdir(path,name);
			}else if(strcmp("rmdir",opcao)){
				rmDir(path,name);
			}else if(strcmp("create",opcao)){
				makeFile(path,name);
			}else if(strcmp("rm",opcao)){
				removeFile(path,name);
			}else if(strcmp("cat",opcao)){
				cat(path,name);
			}else if(strcmp("ls",opcao)){
				ls(path);
			}else if(strcmp("write",opcao)){
				char *text = strtok(NULL, " ");
				write(path,name,text);
			}
		}
	}
}


int main(int argc, char const *argv[])
{

	shell();


/*
	init();
	load();
	char *path = strdup("/ne");
	char *bname = strdup(basename(path));
	char *dname = strdup(dirname(path));
	
	makeDir(dname,bname);

	path = strdup("/ne/teste");
	bname = strdup(basename(path));
	dname = strdup(dirname(path));
	makeDir(dname,bname);

	path = strdup("/ne/teste2");
	bname = strdup(basename(path));
	dname = strdup(dirname(path));
	makeDir(dname,bname);

	path = strdup("/ne/teste2/newfile.txt");
	bname = strdup(basename(path));
	dname = strdup(dirname(path));
	makeFile(dname,bname);

	path = strdup("/ne/teste/newfile.txt");
	bname = strdup(basename(path));
	dname = strdup(dirname(path));
	makeFile(dname,bname);

	write(dname,bname,strdup("123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678"));
	write(dname,bname,strdup("123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789123456789"));

	cat(dname,bname);

	removeFile(dname,bname);


	printf("\n\n\n\n\n\n\n");
	path = strdup("/ne/teste2");
	ls(path);
*/
	//shell();
	return 0;
}