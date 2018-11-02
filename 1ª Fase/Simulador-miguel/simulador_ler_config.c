#define MAXBUF 100
#define DELIM "="

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void ler_config(char *aFilename, char *aParametro, int* retorno)
{

	FILE *ficheiro = fopen (aFilename, "rb");
    if (ficheiro != NULL)
    {
		char line[MAXBUF];
		char* aux;

		while(fgets(line, sizeof(line), ficheiro) != NULL)
		{
			char aux2[MAXBUF];
			char *cfline;
			int parameter_size;
			char parameter[MAXBUF];

			cfline = strstr((char *)line,DELIM);
			cfline = cfline + strlen(DELIM);

			parameter_size = strcspn(line, cfline);
			strncpy(parameter, line, parameter_size-1);
			parameter[parameter_size - 1] = '\0';


			if (! strcmp(parameter, aParametro))
			{
					strncpy(aux2, cfline, strlen(cfline));
					aux = strchr(aux2,'\r');
					*aux = '\0';
					*retorno = atoi(aux2);
			}
		} // Fim while
		fclose(ficheiro);
    } // Fim if aFile
	
}
 