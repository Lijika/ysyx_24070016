#include <elf.h>
#include <isa.h>

char *read_elf_file(char *elf_file) {
	FILE *fp = fopen(elf_file, "r");
	if(fp == NULL) {
		printf("fail to open the elf file.\n");
		assert(0);
	}
	fseek(fp, 0, SEEK_END);
	int elf_size = ftell(fp);
	char *elf_buf = (char *)malloc(elf_size);
	
	int a;
	rewind(fp);
	a = fread(elf_buf, elf_size, 1, fp);
	if(a == 0) {
		printf("fail to read elf file\n");
		assert(0);
	}

	return elf_buf;
}

void init_elf(char *elf_file) {
	char *elf_buf = read_elf_file(elf_file);

	Elf32_Ehdr *ehdr = (Elf32_Ehdr *)elf_buf;
	// printf("\n//%d//\n", ehdr.e_shstrndx);

	if((ehdr->e_ident[0] != 0x7F) ||
		ehdr->e_ident[1] != 'E' ||
		ehdr->e_ident[2] != 'L' ||
		ehdr->e_ident[3] != 'F') {
		printf("the file is not ELF.\n");
		assert(0);
	}

	Elf32_Shdr *shdr = (Elf32_Shdr *)(elf_buf + ehdr->e_shoff);


	printf("///%s ///\n", (char *)(shdr + ehdr->e_shstrndx + shdr->sh_name));
	assert(0);


}