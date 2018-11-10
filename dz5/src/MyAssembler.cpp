#include "MyAssembler.hpp"


int MyAssembler::assemble(FILE *asmFile, FILE *txtFile, FILE *binFile)
{
	if((!asmFile) || (!txtFile) || (!binFile))
	{
		assert(0);
		return NULLPTR_ERR;
	}

	char *check;

	
	m_errno = SUCCESS;
	
	for(int i = 0; i < 2; i++)
	{
		fseek(asmFile, 0, SEEK_SET);
		fseek(txtFile, 0, SEEK_SET);
		fseek(binFile, 0, SEEK_SET);
		m_index = 0;
		while((check = fgets(m_buf, ASM_BUF_SIZE, asmFile), check != nullptr))
		{
			#define CMD_DEF(name, num, asm_code, disasm_code, proc_code) \
			asm_code;
			#include "CmdDef.hpp"
			#undef CMD_DEF


			if(m_errno != SUCCESS)
			{
				assert(0);
				return m_errno;
			}
		}
	}

	return SUCCESS;
}


void MyAssembler::findLabel(FILE *txtFile, FILE *binFile, char *label, char n_command)
{
	for(int i = 0; i < ASM_LABELS_SIZE; i++)
	{
		if((strncmp(label, m_labels[i].m_name, MAX_LABEL_LEN) == 0) && (m_labels[i].m_numb > -1))
		{
			char *tmp = (char *)&m_labels[i].m_numb;
			fprintf(txtFile, "%x %d\n", n_command, m_labels[i].m_numb);
			fprintf(binFile, "%c", n_command);
			for(int j = 0; j < (int)sizeof(int); j++)
				fprintf(binFile, "%c", tmp[j]);

			break;
		}
	}
}

