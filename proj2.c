/*
MARCO 2017
UNIVERSIDADE DE BRASILIA
DEPARTAMENTO DE CIENCIA DA COMPUTACAO

TEORIA E APLICACAO DE GRAFOS
DOCENTE RESPONSAVEL:
PROFESSOR DR. DIBIO LEANDRO BORGES

PROJETO 2 - ORGANIZACAO TOPOLOGICA DO FLUXO DE BACHARELADO EM CIENCIA DA COMPUTACAO
			COM IDENTIFICACAO DE CAMINHO CRITICO

DISCENTES:
GUILHERME GOMES PRANDI 15/0128274
MIGUEL A. MONTAGNER FILHO 13/0127302

COMPILADO UTILIZANDO gcc -ansi -Wall

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Adj
{
    int id;
    struct Adj *prox;
} Adj;

typedef struct Vertice
{
    int id;
    int qtdIncidentes;
    int creditos;
    float dificuldade;
    struct Vertice *prox;

    struct Adj *adj;

} Vertice;




/*define tamanho max de cada linha do arquivo disciplinas.txt*/
#define TAM_LINHA 100
/*Nome do arquivo a ser lido*/
#define NOME_ARQUIVO "disciplinas.txt"
/*define quantos caracteres tem a matricula*/
#define TAM_MATRICULA 6

/*CABECALHO DE FUNCOES*/
int povoaLista (Vertice **);

/*Funcoes de push e pop de adjacentes de vertice*/
void pushAdj(Adj **, int);
void popAdj(Adj **, int);
void liberaListaAdj(Adj **);

/*Funcoes de push e pop de vertices*/
void pushVertice(Vertice **, int);
void popVertice(Vertice **, int);
void liberaListaVertice(Vertice **);

/*funcoes de lista*/
void imprimeLista(Vertice *);

/*
ESTRUTURA DE DADOS UTILIZADA:
LISTA DE ADJACENCIA COM LISTA DE INCIDENCIA 
EXEMPLO:
LISTA DE VERTICES     LISTA ADJACENCIA
    113784				->112345->115432->119876->NULL
    ||
    \/               
    112364            	->116475->118253->NULL
    ||
    \/
    NULL
    
A UTILIZACAO DE UM VETOR FOI ESCOLHIDA PARA PROMOVER MELHOR DESEMPENHO 
NOS ACESSOS A LISTA, POIS O ID CONTIDO EM CADA NO PODE SER UTILIZADO PARA
UM ACESSO DIRETO, SEM NECESSIDADE DE PERCORRER TODA A LISTA PARA ENCONTRA-LO
*/

/*FUNCAO MAIN*/
int main () {


    /*declara vetor de ponteiros para Vertice*/
    Vertice *lista = NULL;

    povoaLista(&lista);


    imprimeLista(lista);
    printf("\nFIM DO PROGRAMA\n");
    /*desaloca a memoria utilizada*/
    liberaListaVertice(&lista);
    return 0;

}

void imprimeLista(Vertice *lista){

    Vertice *cursor = lista;
    Adj *cursorAdj = cursor->adj;

    while(cursor!=NULL) {
    	printf("%d(kr=%d)(dif:%.1f)", cursor->id, cursor->creditos, cursor->dificuldade);
    	cursorAdj = cursor->adj;
    	while(cursorAdj!= NULL) {
    		printf("->%d", cursorAdj->id);
    		cursorAdj = cursorAdj->prox;
    	}
    	printf("\n");
    	cursor = cursor->prox;
    }

}


/*Recebe uma lista de adj por referencia, cria um novo Adj, coloca no inicio da lista e atualiza o inicio fornecido*/
void pushAdj(Adj **inicio, int id) {

    /*aloca espaco para novo adj*/
    Adj *adjNovo = malloc(sizeof(Adj));

    /*coloca valor do id*/
    adjNovo->id = id;
    /*faz novo adj apontar para o inicio da lista de adj*/
    adjNovo->prox = *inicio;
    /*atualiza o valor do inicio da lista*/
    *inicio = adjNovo;
    
    

}

void popAdj(Adj **inicio, int id) {

	/*checa para ver se a lista eh nula*/
    if(*inicio == NULL) {
        return;
    }

    Adj *cursor = *inicio;
    Adj *cursorProx = cursor->prox;
    
    /*verifica se o inicio aponta para o elemento a ser removido*/
    if (cursor->id == id) {
    	*inicio = (*inicio)->prox;
    	free(cursor);
    	return;

    }
    
    while(cursorProx != NULL) {
    	if(cursorProx->id == id) {
    		cursor->prox = cursorProx->prox;
    		free(cursorProx);
    		return;
    	}
    	cursor = cursorProx;
    	cursorProx = cursorProx->prox;
    }
    /*como o while acima chama return qdo encontra, 
    esta linha so sera executada se sair do loop 
    por "== null", logo pode-se assumir que nao encontrou*/
    printf("\nATENCAO:POPADJ CHAMADO MAS ELEM NAO ENCONTRADO\n");
}

void liberaListaAdj(Adj **inicio) {
	Adj *cursorProx = NULL;
	/*percorre a lista enquanto desaloca*/
	while(*inicio != NULL) {
		cursorProx = (*inicio)->prox;
		free(*inicio);
		*inicio = cursorProx;
	}
}

void pushVertice(Vertice **inicio, int id) {

    /*aloca espaco para novo vertice*/
    Vertice *verticeNovo = malloc(sizeof(Vertice));

    /*coloca valor do id e inicia os outros campos*/
    verticeNovo->id = id;
    verticeNovo->adj = NULL;
    verticeNovo->qtdIncidentes = 0;
    /*faz novo vertice apontar para o inicio da lista de vertice*/
    verticeNovo->prox = *inicio;
    /*atualiza o valor do inicio da lista*/
    *inicio = verticeNovo;
    
    

}

void popVertice(Vertice **inicio, int id) {

	/*checa para ver se a lista eh nula*/
    if(*inicio == NULL) {
        return;
    }

    Vertice *cursor = *inicio;
    Vertice *cursorProx = cursor->prox;
    
    /*verifica se o inicio aponta para o elemento a ser removido*/
    if (cursor->id == id) {
    	*inicio = (*inicio)->prox;
    	free(cursor);
    	return;

    }
    
    while(cursorProx != NULL) {
    	if(cursorProx->id == id) {
    		cursor->prox = cursorProx->prox;
    		free(cursorProx);
    		return;
    	}
    	cursor = cursorProx;
    	cursorProx = cursorProx->prox;
    }
    /*como o while acima chama return qdo encontra, 
    esta linha so sera executada se sair do loop 
    por "== null", logo pode-se assumir que nao encontrou*/
    printf("\nATENCAO:POPVERTICE CHAMADO MAS ELEM NAO ENCONTRADO\n");
   

}

void liberaListaVertice(Vertice **inicio) {

	if(*inicio == NULL) {
		return;
	}

    Vertice *cursorProx = (*inicio)->prox;

    while(*inicio != NULL) {
    	cursorProx = (*inicio)->prox;
    	/*libera sua lista de adjacencia antes*/
		liberaListaAdj(&(*inicio)->adj);
        free(*inicio);
        *inicio = cursorProx;
    }

}

/*Percorre o arquivo alunos.txt e povoa a lista de adjacencia*/
int povoaLista(Vertice **lista) {

    /*declara e inicializa variaveis a serem utilizadas*/
    FILE *arquivo = NULL;
    char linhaAtual[TAM_LINHA];
    int i = 0;
    int idAdj = 0;
    int offsetLinha = 0;
    int idMatricula = 0;



    /*Abre arquivo com nomes e matriculas, em modo leitura*/
    arquivo = fopen(NOME_ARQUIVO, "r");
    if (arquivo == NULL) {
        printf("\nERRO: Arquivo %s nao encontrado\n", NOME_ARQUIVO);
        return -1;
    }

    /*
    percorre o arquivo alunos.txt
    vai de linha em linha, colocando em uma string, supondo formato nome#matricula#idAdj1#idAdj2#...#idAdjn#\n
    percorre ate chegar a NUM_ALUNOS ou EOF (feof == 0)
    a cada iteracao, aloca espaco para novo vertice e atribui a ele os valores lidos, e depois adiciona suas adjacencias
    supoe id do aluno a partir da ordem de entrada

    */
    /*
    no for, primeira condicao de saida para caso ocorra algum problema, exista um limite
    segunda condicao se ja leu todos os alunos registrados
    terceira se chegou no final do arquivo
    */
    for(i = 0;i<200 && fgets(linhaAtual, TAM_LINHA, arquivo) != NULL; i++) {
    
		offsetLinha = 0;



        /*le o id da disciplina*/

		sscanf(linhaAtual, "%d#", &idMatricula);

		pushVertice(lista, idMatricula);

		/*aumenta offset da string para andar ate o proximo valor. 1 referente ao divisor #*/
        offsetLinha = TAM_MATRICULA + 1;
        /*Le a qtd de creditos*/
        sscanf(linhaAtual + offsetLinha, "%d#", &(*lista)->creditos);
        offsetLinha += 2;
        /*le a dificuldade*/
        sscanf(linhaAtual + offsetLinha, "%f#", &(*lista)->dificuldade);
        offsetLinha += 4;
        /*inicializa a variavel de qtd de incidentes*/
        (*lista)->qtdIncidentes = 0;


        /*Le os adjacentes, e sai povoando a lista de adjacencia*/
        while(sscanf(&linhaAtual[offsetLinha],"%d#", &idAdj) == 1) {
	           /*Coloca o novo vertice no inicio da lista de adj*/
	           pushAdj(&((*lista)->adj), idAdj);

	           /*anda na string para o prox valor*/
	           if(linhaAtual[offsetLinha + 1] == '\0') {
	           		break;
	           }
	           offsetLinha += TAM_MATRICULA + 1;
        }




    }
    return i;
}