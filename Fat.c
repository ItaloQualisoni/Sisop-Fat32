#include <stdio.h>
#include <stdint.h>
//Bloco com 256 linhas cada
//hexdump -C fat.part
/* tabela FAT, 1024 entradas de 32 bits */
uint32_t fat[1024];
/* tabela BOOT_BLOCK, 1024 entradas de 32 bits */
uint32_t boot[1024];
/* entrada de diretorio, 32 bytes cada */
typedef struct  {
	uint8_t filename[16];
	uint8_t attributes;
	uint8_t reserved[7];
	uint32_t first_block; //olha na fat
	uint32_t size;
}dir_entry;
/* diretorios (incluindo ROOT), 128 entradas de diretorio
com 32 bytes cada = 4096 bytes  */
dir_entry dir[128];

FILE  *memoria_fat;

void fillBoot(){
	int i;
	for (i = 0; i < sizeof(boot) / sizeof(int); i++)
	{
		/* code */
		boot[i] = 165;
	}
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, 0, SEEK_SET);
	fwrite(&boot, sizeof(boot), 1, memoria_fat);
	fclose(memoria_fat);
}

void fillFat(){
	int i;
	fat[0] = -1;
	fat[1] = -1;
	fat[2] = -1;
	for (i = 3; i < sizeof(fat) / sizeof(int); i++)
	{
		/* code */
		fat[i] = 0;
	}
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, sizeof(boot), SEEK_SET);
	fwrite(&fat, sizeof(fat), 1, memoria_fat);
	fclose(memoria_fat);
}

void fillRootDir(){
	int i;
	for (i = 0; i < sizeof(dir) / sizeof(dir[0]); i++)
	{
		/* code */
		dir_entry aux  = dir[i];
	}
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, 2*sizeof(boot), SEEK_SET);
	fwrite(&dir, sizeof(dir), 1, memoria_fat);
	fclose(memoria_fat);
}

void loadBoot(){
	memoria_fat = fopen("fat.part","r+");
	fread(&boot, sizeof(boot), 1, memoria_fat);
	fclose(memoria_fat);
}
void loadFat(){
	memoria_fat = fopen("fat.part","r+");
	fseek(memoria_fat, sizeof(boot), SEEK_SET);
	fread(&fat, sizeof(fat), 1, memoria_fat);
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
	memoria_fat = fopen("fat.part","w+");
	fclose(memoria_fat);
	fillBoot();
	fillFat();
	fillRootDir();
	//printArray(fat);
}; 

void load();

void makeDir();

void makeFile();

void removeDir();

void removeFile();

void listDir();

void write();

void cat();









int main(int argc, char const *argv[])
{
	init();
	return 0;
}



