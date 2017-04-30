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
    int qtdIncidentes; /*para facilitar o kahn*/
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
Vertice *copiaLista(Vertice *);
void atualizaIncidentes(Vertice *);

/*
ESTRUTURA DE DADOS UTILIZADA:
LISTA DE DISCIPLINAS COM LISTA DE ADJACENCIA
EXEMPLO:
LISTA DE VERTICES     LISTA ADJACENCIA
    113784				->112345->115432->119876->NULL
    ||
    \/               
    112364            	->116475->118253->NULL
    ||
    \/
    NULL
*/

/*FUNCAO MAIN*/
int main () {


    /*declara o ponteiro para Vertice cabeca da lista*/
    Vertice *lista = NULL;

    /*ETAPA DE COLETA DE DADOS*/
    povoaLista(&lista); /*Le o arquivo e prepara corretamente o TAD*/
	atualizaIncidentes(lista); /*com a lista pronta, eh possivel contar as incidencias em cada vertice*/

    /*ETAPA DE MANIPULACAO DE DADOS*/
    Vertice *listaCopia = copiaLista(lista);


    /*ETAPA DE APRESENTACAO DOS RESULTADOS*/
    imprimeLista(lista); /*imprime na saida padrao o TAD para o grafo*/
    

    /*ETAPA DE FINALIZACAO DO PROGRAMA*/
    liberaListaVertice(&lista);     /*desaloca a memoria utilizada para Vertice *lista*/

    printf("\nFIM DO PROGRAMA\n");
    return 0;

}

void organizacaoTopologica(Vertice *lista) {


}

void imprimeLista(Vertice *lista){

    Vertice *cursor = lista;
    Adj *cursorAdj = cursor->adj;

    while(cursor!=NULL) {
    	printf("%d(cr=%d)(dif:%.1f)(inc=%d)", 	cursor->id, 
    											cursor->creditos, 
    											cursor->dificuldade,
    											cursor->qtdIncidentes);
    	cursorAdj = cursor->adj;
    	while(cursorAdj!= NULL) {
    		printf("->%d", cursorAdj->id);
    		cursorAdj = cursorAdj->prox;
    	}
    	printf("\n");
    	cursor = cursor->prox;
    }

}


/*Recebe uma lista de adj por referencia, cria um novo Adj, coloca no fim da lista e atualiza o inicio fornecido se necessario*/
void pushAdj(Adj **inicio, int id) {

    /*aloca espaco para novo adj*/
    Adj *adjNovo = malloc(sizeof(Adj));
    Adj *cursor = *inicio;

    /*coloca valor do id*/
    adjNovo->id = id;

 	/*coloca NULL como proximo*/
 	adjNovo->prox = NULL;

 	/*caso a lista esteja vazia, apenas insere*/
 	if (*inicio == NULL) {
 		*inicio = adjNovo;
 		return;
 	}
 	/*percorre ate o final da lista*/
 	while(cursor->prox != NULL) cursor = cursor->prox;
 	/*insere no final*/
 	cursor->prox = adjNovo; 
    

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
/*Aloca espaco e coloca um vertice com o novo id no final da lista*/
void pushVerticeNoFinal(Vertice **inicio, int id) {
	Vertice *cursor = *inicio;

    /*aloca espaco para novo vertice*/
    Vertice *verticeNovo = malloc(sizeof(Vertice));

    /*coloca valor do id e inicia os outros campos*/
    verticeNovo->id = id;
    verticeNovo->adj = NULL;
    verticeNovo->qtdIncidentes = 0;

    if (*inicio == NULL) {
    	*inicio = verticeNovo;
    	verticeNovo->prox = NULL;
    	return;
    }
    /*faz novo vertice apontar para o inicio da lista de vertice*/
    while (cursor->prox != NULL) cursor = cursor->prox;
    verticeNovo->prox = NULL;
    cursor->prox = verticeNovo;    

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
    percorre o arquivo disciplinas.txt
    vai de linha em linha, colocando-as em uma string, supondo formato 
    			mat_vertice#creditos#dificuldade#mat_adj1#mat_adj2#mat_adj3#...#mat_adjN#\n
    percorre ate chegar a EOF
    a cada iteracao, aloca espaco para novo vertice e atribui a ele os valores lidos, e depois adiciona suas adjacencias
    */
    for(i = 0;i<200 && fgets(linhaAtual, TAM_LINHA, arquivo) != NULL; i++) {
    
		offsetLinha = 0;



        /*le a matricula da disciplina*/
		sscanf(linhaAtual, "%d#", &idMatricula);

		/*aloca espaco para o vertice inicializando a matricula*/
		pushVertice(lista, idMatricula);

		/*aumenta offset da string para andar ate o proximo valor. "+ 1" referente ao divisor #*/
		/*TAM_MATRICULA eh uma constante definida no cabecalho do programa*/
        offsetLinha = TAM_MATRICULA + 1;
        /*Le a qtd de creditos*/
        sscanf(linhaAtual + offsetLinha, "%d#", &(*lista)->creditos);
        /*aumenta offset (num_1_algarismo) + 1 (referente ao #)*/
        offsetLinha += 2;
        /*le a dificuldade*/
        sscanf(linhaAtual + offsetLinha, "%f#", &(*lista)->dificuldade);
        /*aumenta offset (1.0 == 3) + 1 (referente ao #)*/
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


/*Faz uma copia da lista, respeitando a ordem dos vertices*/
Vertice *copiaLista(Vertice *lista) {

	Vertice *novaLista = NULL;
	Vertice *cursor = lista;
	Adj *cursorAdj = NULL;

	while(lista != NULL) {
		pushVerticeNoFinal(&novaLista, lista->id);
		cursor = novaLista;
		while(cursor->prox != NULL) cursor = cursor->prox;
		cursor->qtdIncidentes = lista->qtdIncidentes;
		cursor->creditos = lista->creditos;
		cursor->dificuldade = lista->dificuldade;
		cursor->adj = NULL;
		cursorAdj = lista->adj;
		while (cursorAdj != NULL) {
			pushAdj(&cursor->adj, cursorAdj->id);
			cursorAdj = cursorAdj->prox;
		}
		lista = lista->prox;

	}
	return novaLista;
}

void atualizaIncidentes(Vertice *lista) {

	Vertice *cursor = lista;
	Vertice *cursorIncremento = lista;
	Adj *cursorAdj = NULL;
	/*percorre cada lista de adjacencia de cada vertice.
	para cada adjacente encontrado, volta na lista de
	vertices incrementa o valor de qtd de incidentes*/
	while(cursor != NULL) {
		cursorAdj = cursor->adj;
		/*percorre lista de adjacentes*/
		while(cursorAdj!= NULL) {
			/*procura o elemento incidente na lista*/
			cursorIncremento = lista;
			while(cursorIncremento != NULL) {
				if(cursorIncremento->id == cursorAdj->id) {
					cursorIncremento->qtdIncidentes += 1;
					break;
				}
				cursorIncremento = cursorIncremento->prox;
			}
			cursorAdj = cursorAdj->prox;

		}
		cursor = cursor->prox;

	}

}