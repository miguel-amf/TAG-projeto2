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

/*lista de caminhos, caminhos eh uma sequencia de Vertices*/
typedef struct Caminhos
{
	struct Vertice *vertices;
	float dificuldadeCaminho;
	struct Caminhos *prox;

} Caminhos; 




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
void pushVerticeNoFinal(Vertice **, int);
void popVertice(Vertice **, int);
void liberaListaVertice(Vertice **);

/*funcoes de lista*/
void imprimeLista(Vertice *);
Vertice *copiaLista(Vertice *);
void atualizaIncidentes(Vertice *);
Vertice *organizacaoTopologica(Vertice *);
Vertice *listaAdjacenciaParaIncidencia(Vertice *);
Vertice *caminhoCritico(Vertice *);

/*Funcoes auxiliares*/
char *idParaNome(int);

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

	printf("\nola, este programa ira analisar o fluxo do curso Ciencia da Computacao Bacharelado.\n");
	printf("para tal, o usuario ira receber na tela:\n");
	printf("(1) todas as disciplinas e quem a tem como pre-requisito (DAG)\n");
	printf("(2) todos os caminhos das materias e suas dificuldades, esta calculada sendo creditos*dificuldadePercebida\n");
	printf("(3) uma lista referente a uma ordenacao topologica (baseado no algoritmo de Kahn\n");
	printf("(4) o caminho critico, o caminho de maior dificuldade\n");
	printf("Pressione ENTER para mostrar (1)");
	getchar();
	printf("Mostrando(1):\n");

    /*declara o ponteiro para Vertice cabeca da lista*/
    Vertice *lista = NULL;

    /*ETAPA DE COLETA DE DADOS*/
    povoaLista(&lista); /*Le o arquivo e prepara corretamente o TAD*/
	atualizaIncidentes(lista); /*com a lista pronta, eh possivel contar as incidencias em cada vertice*/

    /*para mostrar (1)*/
    imprimeLista(lista); /*imprime na saida padrao o TAD para o grafo*/
    printf("\n\n\npressione ENTER para mostrar (2)");
    getchar();

    /*ETAPA DE MANIPULACAO DE DADOS*/
    Vertice *listaCopia = copiaLista(lista);

    Vertice *topologico = organizacaoTopologica(listaCopia);

    /*caminhoCritico imprime automaticamente (2)*/
    Vertice *cCritico = caminhoCritico(lista);
    printf("\n\n\npressione ENTER para mostrar (3)");
    getchar();


    /*ETAPA DE APRESENTACAO DOS RESULTADOS*/

    /*algoritmo para mostrar (3)*/
    printf("\nOrganizacaoTopologica: \n");
    Vertice *cursorTopologico = topologico;
    while(cursorTopologico != NULL) {
    	printf("-> %s(%d)", idParaNome(cursorTopologico->id), cursorTopologico->id);
    	cursorTopologico = cursorTopologico->prox;
    }
    printf("\n\n\npressione ENTER para mostrar (4)");
    getchar();
    /*algoritmo para mostrar (4)*/
    printf("\nCAMINHO CRITICO: (dificuldade: %.0f)\n", cCritico->dificuldade);
    Vertice *cursorCaminhoCritico = cCritico;
    while(cursorCaminhoCritico != NULL) {
    	printf("->%s(%d)", idParaNome(cursorCaminhoCritico->id), cursorCaminhoCritico->id);
    	cursorCaminhoCritico = cursorCaminhoCritico->prox;
    }
    

    /*ETAPA DE FINALIZACAO DO PROGRAMA*/
    liberaListaVertice(&lista);
    liberaListaVertice(&listaCopia);
    liberaListaVertice(&topologico);
    liberaListaVertice(&cCritico);



    printf("\nFIM DO PROGRAMA\n");
    return 0;

}



Vertice *caminhoCritico(Vertice *entrada) {


	Caminhos *caminhos = NULL;
	Caminhos *caminhosFinalizados = NULL; /*caminhos finalizados armazena os caminhos que chegaram ao final*/
	Vertice *caminhoCritico = NULL;
	Vertice *cursor = entrada;
	float maiorValor = 0;
	/*coloca todos os vertices sem incidencia como possiveis caminhos*/

	while(cursor != NULL) {
		if(cursor->qtdIncidentes ==  0) {
			if(caminhos == NULL ) {
				caminhos = malloc(sizeof(Caminhos));
				caminhos->prox = NULL;
				caminhos->dificuldadeCaminho = cursor->creditos*cursor->dificuldade;
				pushVertice(&caminhos->vertices, cursor->id);
				caminhos->vertices->dificuldade = cursor->dificuldade;
				caminhos->vertices->creditos = cursor->creditos;
			} else {
				Caminhos *aux = caminhos;
				caminhos = malloc(sizeof(Caminhos));
				pushVertice(&caminhos->vertices, cursor->id);
				caminhos->vertices->dificuldade = cursor->dificuldade;
				caminhos->vertices->creditos = cursor->creditos;
				caminhos->dificuldadeCaminho = cursor->creditos*cursor->dificuldade;
				caminhos->prox = aux;
			}
		}
		cursor = cursor->prox;
	}
	/*em caminhos agora existe uma lista de lista de vertice
	cada lista de vertice representa um caminho
	neste momento existem apenas caminhos incompletos, com 
	apenas um vertice, sendo estes vertices aqueles que nao tem incidencia,
	ou seja, sao disciplinas sem pre-requisitos

	a partir de cada um destes, vao sendo criados diversos outros caminhos,
	contendo todas as ramificacoes possiveis a partir deles
	cada caminho contem na cabeca a dificuldade
	essa dificuldade eh o somatorio das arestas entre os vertices

	agora, o algoritmo vai testar todas as possiveis ramificacoes de cada caminho
	para isso, vai criar copias de cada caminho ja existente
	cada copia sera o caminho original mais um adjacente do ultimo elemento dos vertices
	sera feito para todas as adjacencias
	quando nao houver mais adjacentes, o caminho pai eh removido da lista, ficando apenas os
	filhos que sao o pai + um adjancente do caminho
	quando um caminho nao tem mais filhos possiveis, ele eh removido da lista e colocado na lista
	de caminhosFinalizados

	apos descobrir cada um dos caminhos possiveis, vera qual o caminho mais longo, colocara em 
	caminhoCritico e retornara para a funcao chamadora*/

	while(caminhos != NULL) {

		Caminhos *caminhoVigente = caminhos;
		Vertice *listaVertices = caminhoVigente->vertices;
		/*procura o ultimo elemento do caminho*/
		while(listaVertices->prox != NULL) {
			listaVertices = listaVertices->prox;
		}
		int idUltimoElemento = listaVertices->id;
		/*procura na lista da entrada pelo id encontrado,
		a fim de encontrar suas adjacencias*/
		Vertice *ultimoVertice = entrada;
		Adj *cursorAdjDoUltimoElemento = NULL;
		while(ultimoVertice!= NULL) {
			if(ultimoVertice->id == idUltimoElemento) {
				break;
			}
			ultimoVertice = ultimoVertice->prox;
		}
		cursorAdjDoUltimoElemento = ultimoVertice->adj;

		/*percorre a lista de adjacencias
		se ela for nula, quer dizer que o caminho chegou ao
		fim. caso contrario, coloca os caminhos filhos na lista*/
		if(cursorAdjDoUltimoElemento == NULL) {
			
			/*caminho sem mais filhos, coloca em caminhosFinalizados*/
			Caminhos *aux = caminhosFinalizados;
			caminhosFinalizados = malloc(sizeof(Caminhos));
			caminhosFinalizados->vertices = copiaLista(caminhoVigente->vertices);
			caminhosFinalizados->dificuldadeCaminho = caminhoVigente->dificuldadeCaminho;
			caminhosFinalizados->prox = aux;


		} else {
			/*cria novos caminhos com adjacentes e coloca no
			inicio de caminhos*/
			while(cursorAdjDoUltimoElemento!= NULL) {

				/*cria novo caminhii*/
				Caminhos *caminhoNovo = malloc(sizeof(Caminhos));
				/*prepara a lista de vertices para o novo caminho*/
				Vertice *verticesNovo = copiaLista(caminhoVigente->vertices);
				pushVerticeNoFinal(&verticesNovo, cursorAdjDoUltimoElemento->id);
				/*atribui a nova lista de vertices no novo caminho*/
				caminhoNovo->vertices = verticesNovo;

				/*calcula a nova dificuldade*/
				caminhoNovo->dificuldadeCaminho = caminhos->dificuldadeCaminho + ultimoVertice->dificuldade*ultimoVertice->creditos;

				/*agora coloca o caminho no final da lista de caminhos*/
				Caminhos *cursorCaminhos = caminhos;
				while(cursorCaminhos->prox != NULL) {
					cursorCaminhos = cursorCaminhos->prox;
				}
				cursorCaminhos->prox = caminhoNovo;
				caminhoNovo->prox = NULL;


				cursorAdjDoUltimoElemento = cursorAdjDoUltimoElemento->prox;
			}
		}
		Caminhos *auxFree = caminhos;
		caminhos = caminhos->prox;
		liberaListaVertice(&auxFree->vertices);
		free(auxFree);	
	
	}


	/*imprime todos os caminhos encontrados e suas dificuldades*/
	Caminhos *auxDebug = caminhosFinalizados;
	printf("Dif.\tsequencia");
	while(auxDebug!= NULL){
		printf("\n(%.0f):", auxDebug->dificuldadeCaminho);
		Vertice *auxVerticeDebug = auxDebug->vertices;
		while(auxVerticeDebug!= NULL) {
			printf("->%s", idParaNome(auxVerticeDebug->id));
			auxVerticeDebug = auxVerticeDebug->prox;
		}
		auxDebug = auxDebug->prox;
	}


	/*verifica dentro dos caminhosFinalizados aquele que atingiu maior valor*/
	Caminhos *cursorCaminhosFinalizados = caminhosFinalizados;
	while(cursorCaminhosFinalizados != NULL) {
		if(cursorCaminhosFinalizados->dificuldadeCaminho > maiorValor) {
			caminhoCritico = cursorCaminhosFinalizados->vertices;
			maiorValor = cursorCaminhosFinalizados->dificuldadeCaminho;
		}
		cursorCaminhosFinalizados = cursorCaminhosFinalizados->prox;
	}
	/*cria uma copia propria, pois caminhoCritico aponta para um elemento de caminhosFinalizados
	caso contrario durante o free de caminhosFinalizados haveria ponteiro para memoria desalocada*/
	caminhoCritico = copiaLista(caminhoCritico);
	caminhoCritico->dificuldade = maiorValor;

	/*Libera memoria de caminhosFinalizados*/
	while(caminhosFinalizados!=NULL) {
		Caminhos *auxFree = caminhosFinalizados->prox;
		liberaListaVertice(&caminhosFinalizados->vertices);
		free(caminhosFinalizados);
		caminhosFinalizados = auxFree;
	}

	return caminhoCritico;
}

Vertice *listaAdjacenciaParaIncidencia(Vertice *entrada) {
	Vertice *adjacencia = entrada;
	Vertice *incidencia = copiaLista(adjacencia);
	Vertice *cursor = incidencia;

	/*limpa a lista de adjacencia da incidencia para
	poder povoar a lista com as incidencias*/
	while(cursor!=NULL) {
		liberaListaAdj(&cursor->adj);
		cursor = cursor->prox;
	}
	/*percorre lista de vertices. para cada adjacencia
	encontrada, coloca na lista de vertices incidencia
	o novo no*/
	while(adjacencia != NULL) {
		Adj *cursorAdj = adjacencia->adj;
		/*percorre lista de adjacencia de cada vertice*/
		while (cursorAdj!=NULL) {
			/*percorre a lista de incidencia a procura do adjacente*/
			Vertice *cursorInsereIncidente = incidencia;
			while(cursorInsereIncidente != NULL) {
				if (cursorInsereIncidente->id == cursorAdj->id) {
					pushAdj(&cursorInsereIncidente->adj, cursorAdj->id);
					break;
				}
				cursorInsereIncidente = cursorInsereIncidente->prox;
			}
			cursorAdj = cursorAdj->prox;
		}
		adjacencia = adjacencia->prox;
	}
	return incidencia;
}
/*
Algoritmo de Kahn para organizacao topologica
faz uma tabela de incidencias, quem possuir zero
incidentes eh removido da lista e seus adjacentes
possuem suas contagens de incidentes decrementadas em 1*/
Vertice *organizacaoTopologica(Vertice *entrada) {

	Vertice *lista = copiaLista(entrada);

	Vertice *cursor = lista;
	Vertice *cursorEncontraIncidentes = lista;
	Vertice *organizado = NULL;
	int matAdjacente = 0;
	Adj *cursorAdj = NULL;
	/*Percorre Procurando por incidentes == 0 */
	while(lista != NULL) {
		cursor = lista;
		while (cursor!= NULL) {
			/*verifica se o vertice nao tem setas apontando para ele*/
			if(cursor->qtdIncidentes == 0) {
				/*percorre a lista de adjacencias deste vertice*/
				cursorAdj = cursor->adj;
				while(cursorAdj != NULL) {
					/*percorre a lista de vertices procurando pelo vertice com a matricula da lista de adjacencia*/
					matAdjacente = cursorAdj->id;
					cursorEncontraIncidentes = lista; /*volta o cursor para o inicio da lista*/
					while (cursorEncontraIncidentes != NULL) {
						if(cursorEncontraIncidentes->id == matAdjacente) {
							cursorEncontraIncidentes->qtdIncidentes -= 1;
							break;
						}
						cursorEncontraIncidentes = cursorEncontraIncidentes->prox; /*anda para o prox da lista*/
					}
					if (cursorEncontraIncidentes == NULL) 
					printf("\nATENCAO: cursorEncontraIncidentes CHEGOU AO FINAL E NAO ENCONTROU ADJ");
					cursorAdj = cursorAdj->prox; /*anda para o prox da lista*/
				}
				pushVerticeNoFinal(&organizado, cursor->id);/*coloca no final da organizacao o vertice que pode ser retirado*/
				popVertice(&lista, cursor->id); /*tira da lista o vertice colocado*/
			}

			cursor = cursor->prox; /*anda para o prox da lista*/
		}
	}
	return organizado;
}

void imprimeLista(Vertice *lista){

    Vertice *cursor = lista;
    Adj *cursorAdj = cursor->adj;

    printf("Nome    Cred/dif./qtdInc.\tLista de Adjacentes\n");
    while(cursor!=NULL) {
    	printf("%s(%d)\t%d/%.1f/%d\t\t", 	idParaNome(cursor->id),
    											cursor->id,
    											cursor->creditos, 
    											cursor->dificuldade,
    											cursor->qtdIncidentes);
    	cursorAdj = cursor->adj;
    	while(cursorAdj!= NULL) {
    		printf("->%s", idParaNome(cursorAdj->id));
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
    verticeNovo->creditos = 0;
    verticeNovo->dificuldade = 0;

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

char *idParaNome(int id) {
	switch(id){

	case 113107: return "Alg1"; break;
	case 113476: return "APC"; break;
	case 116882: return "Auto"; break;
	case 116378: return "BD"; break;
	case 113034: return "C1"; break;
	case 113042: return "C2"; break;
	case 113417: return "CN"; break;
	case 116351: return "CD"; break;
	case 117951: return "Comp"; break;
	case 117943: return "C.Exp."; break;
	case 116441: return "ES"; break;
	case 116319: return "ED"; break;
	case 117960: return "FSO"; break;
	case 113450: return "FTC"; break;
	case 116726: return "IS"; break;
	case 116653: return "IA"; break;
	case 113468: return "ISC"; break;
	case 113093: return "IAL"; break;
	case 116343: return "LP"; break;
	case 117366: return "LC1"; break;
	case 117919: return "M.Cien."; break;
	case 116394: return "OAC"; break;
	case 116327: return "OA"; break;
	case 115045: return "PE"; break;
	case 117935: return "PC"; break;
	case 117536: return "PAA"; break;
	case 116572: return "Redes"; break;
	case 117927: return "Seg"; break;
	case 116416: return "SI"; break;
	case 116432: return "SB"; break;
	case 117889: return "TP1"; break;
	case 117897: return "TP2"; break;
	case 117901: return "TAG"; break;
	case 116912: return "TG1"; break;
	case 116921: return "TG2"; break;
	default: return ""; break;
}
return "";
}