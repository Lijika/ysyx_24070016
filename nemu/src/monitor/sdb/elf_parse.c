#include <elf.h>
#include <isa.h>

void init_elf(char *elf_file) {
	FILE *fd = fopen(elf_file, "r");
	if(fd == NULL) {
		printf("fail to open the elf file.\n");
		assert(0);
	}

	Elf32_Ehdr ehdr;
	int a;

	a = fread(&ehdr, sizeof(Elf32_Ehdr), 1, fd);
	if(a == 0) {
		printf("fail to read elf head.\n");
		assert(0);
	}
	// printf("\n//%d//\n", ehdr.e_shstrndx);

	if((ehdr.e_ident[0] != 0x7F) ||
		ehdr.e_ident[1] != 'E' ||
		ehdr.e_ident[2] != 'L' ||
		ehdr.e_ident[3] != 'F') {
		printf("the file is not ELF.\n");
		assert(0);
	}

	Elf32_Shdr *shdr = (Elf32_Shdr*)malloc(sizeof(Elf32_Shdr) * ehdr.e_shnum);
	fseek(fd, ehdr.e_shoff, SEEK_SET);
	a = fread(shdr, sizeof(Elf32_Shdr) * ehdr.e_shnum, 1, fd);

	printf("///%s ///\n", (char *)(shdr + ehdr.e_shstrndx + shdr->sh_name));
	assert(0);


}