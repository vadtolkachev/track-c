#include "MyProcessor.hpp"
#include <stdio.h>


MyProcessor::MyProcessor()
{
	m_code = nullptr;
	m_RAM = nullptr;
	m_ramSize = 0;
	m_index = 0;
	m_errno = SUCCESS;

	m_rax = 0;
	m_rbx = 0;
	m_rcx = 0;
	m_rdx = 0;

	char str[45] = "";
	getDumpFileName(str);
	m_dumpFile = fopen(str, "w");
	if(errno != 0)
	{
		assert(0);
		m_errno = OPEN_ERR;
	}

	dumpProc("Processor created");
}


MyProcessor::~MyProcessor()
{
	dumpProc("Processor destroyed");
	fclose(m_dumpFile);
}


int MyProcessor::readFile(FILE *file, char **str, long int *pSize)
{
	struct stat tmp;
	fstat(fileno(file), &tmp);
	long int size = tmp.st_size;

	char *bigString = (char *)calloc(size, sizeof(char));
	if((errno != 0) || (bigString == nullptr))
		return ALLOC_ERR;
	
	int checkErr = fread(bigString, size, 1, file);
	if(checkErr != 1)
	{
		printf("size = %ld\n", size);
		printf("checkErr = %d\n", checkErr);
		return READ_ERR;
	}

	*str = bigString;
	*pSize = size;
	return SUCCESS;
}


int MyProcessor::proc_exec(FILE *binFile)
{
	long int size;
	int checkErr = readFile(binFile, &m_code, &size);
	if(checkErr != SUCCESS)
		return checkErr;

	m_RAM = (char *)calloc(10*size, sizeof(char));
	if((errno != 0) || (m_RAM == nullptr))
		return ALLOC_ERR;
	
	m_ramSize = 10*size;
	m_index = 0;
	m_errno = SUCCESS;

	while(m_index < size)
	{
		switch(m_code[m_index])
		{
			#define CMD_DEF(name, num, asm_code, disasm_code, proc_code) \
			case N_##name : proc_code; break;
			#include "CmdDef.hpp"
			#undef CMD_DEF


			default : printf("m_index = %d\n", m_index); return PARSE_ERR;
		}

		if(m_errno != SUCCESS)
		{
			printf("m_errno = %d\n", m_errno);
			fclose(m_dumpFile);
			m_stack.~MyStack();
			assert(0);
			return PARSE_ERR;
		}
	}


	free(m_code);
	free(m_RAM);
	return SUCCESS;
}


void MyProcessor::getDumpFileName(char str[45]) const
{
	char str1[45] = "logs/procInfo[";
	char str3[6] = "].log";

	char str2[19] = "";
	sprintf(str2, "%p", this);

	strcat(str1, str2);
	strcat(str1, str3);
	strcpy(str, str1);
}


void MyProcessor::dumpProc(const char *str) const
{
	assert(m_dumpFile);
	fprintf(m_dumpFile, "%s\n", str);

	fprintf(m_dumpFile, "Processor[%p]\n{\n", this);
	fprintf(m_dumpFile, "\tm_index = %u\n", m_index);
	fprintf(m_dumpFile, "\tm_rax = %lg\n\tm_rbx = %lg\n\tm_rcx = %lg\n\tm_rdx = %lg\n}\n\n", m_rax, m_rbx, m_rcx, m_rdx);
	
}
