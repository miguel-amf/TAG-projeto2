/*
MARCO 2017
UNIVERSIDADE DE BRASILIA
DEPARTAMENTO DE CIENCIA DA COMPUTACAO

TEORIA E APLICACAO DE GRAFOS
DOCENTE RESPONSAVEL:
PROFESSOR DR. DIBIO LEANDRO BORGES

PROJETO 1 - LISTA DE ADJACENCIA DE AMIGOS DA TURMA, ORDENADO POR ORDEM DOS VERTICES

DISCENTES:
GUILHERME GOMES PRANDI 15/0128274
MIGUEL A. MONTAGNER FILHO 13/0127302

COMPILADO UTILIZANDO gcc -ansi -Wall

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    int id;
    struct Adj *prox;
} Adj;

typedef struct
{
    int id;
    char nome[30];
    char matricula[10];
    int qtdAdj;
    struct Adj *adj;

} Aluno;



/*define numero de alunos*/
#define NUM_ALUNOS 39
/*define tamanho max de cada linha do arquivo alunos.txt*/
#define TAM_LINHA 100
/*Nome do arquivo a ser lido*/
#define NOME_ARQUIVO "amigos_tag20171.txt"

/*CABECALHO DE FUNCOES*/
int povoaLista (Aluno *);
void pushAdj(Aluno *, int);
void freeAdj(Adj *);
void freeLista(Aluno *);
void Imprime(Aluno *, char);
void Ordena(Aluno *);
void popAdj(Aluno *, int);
Adj *maiorClique(Aluno *);
void buscaClique(Aluno *, Aluno *, Adj *);
int verificaAmizade(Aluno *, int, int);
void imprimeAdj(Aluno *, Aluno);
Adj *pushAdjNo(Adj *, int);
Adj *popAdjNo(Adj *, int);
Adj *copiaListaAdj(Adj *);

/*Declaracao de globais*/
int maiorGrau;
Aluno maiorCliqueEncontrado;


/*
ESTRUTURA DE DADOS UTILIZADA:
VETOR DE CABECAS (STRUCT ALUNO) DE LISTA (STRUCT ADJ) 
EXEMPLO:
VETOR ALUNO     LISTA ADJACENCIA
    0               ->3->5->8->NULL
    1               ->2->18->NULL
    ...             ...
    
A UTILIZACAO DE UM VETOR FOI ESCOLHIDA PARA PROMOVER MELHOR DESEMPENHO 
NOS ACESSOS A LISTA, POIS O ID CONTIDO EM CADA NO PODE SER UTILIZADO PARA
UM ACESSO DIRETO, SEM NECESSIDADE DE PERCORRER TODA A LISTA PARA ENCONTRA-LO
*/

/*FUNCAO MAIN*/
int main () {


    /*declara vetor de ponteiros para Aluno*/
    Aluno *lista;
    
    /*Inicializa globais*/
    maiorGrau = 0; 
    maiorCliqueEncontrado.nome[0] = 'M';
    maiorCliqueEncontrado.nome[1] = '\0';
    maiorCliqueEncontrado.qtdAdj = 0;
    maiorCliqueEncontrado.adj = NULL;
    maiorCliqueEncontrado.id = 0;
    maiorCliqueEncontrado.matricula[0] = '\0';

    /*Imprime mensagens guia*/
    printf("Ola, este programa pega o arquivo 'amigos_tag20171.txt' e:\n(1)Analisa maior clique e imprime na saida padrao\n(2)Contabiliza grau de cada vertice\n(3)Ordena vertices pelo grau deles\n(4)Imprime na saida padrao a lista ordenada\n#########\nIniciando povoamento da lista...");
    
    
    /*aloca memoria necessaria para o vetor de Aluno*/
    lista = calloc(NUM_ALUNOS, sizeof(Aluno));
    
    /*Procura no arquivo os vertices, colocando suas adjacencias*/
    if (!povoaLista(lista)) {
        /*trata o erro*/
        printf("ERRO: NAO FOI POSSIVEL TRANSFERIR DO ARQUIVO PARA O PROGRAMA\n");
        return -1;
    };
    
    printf("SUCESSO\n########\nCalculando maior clique...");
    /*chama a funcao maiorClique, recebendo o vetor de cabecas de lista*/
    maiorClique(lista);
    printf("SUCESSO\n");
    printf("Elementos do maior clique: ");
    /*Imprime na saida padrao o resultado, que eh colocado na global*/
    imprimeAdj(lista, maiorCliqueEncontrado);
    
    
    
    printf("\n########\n\nPressione ENTER para imprimir a lista ordenada");
    getchar();
    
    /*Ordena a lista com bubbleSort*/
    Ordena(lista);
    
    /*Imprime na saida padrao a lista ordenada, com flag 1 vertice por linha*/
    Imprime(lista, 'l');

    printf("\nFIM DO PROGRAMA\n");
    /*desaloca a memoria utilizada*/
    freeLista(lista);
    return 0;

}


/*Percorre o arquivo alunos.txt e povoa a lista de adjacencia*/
int povoaLista(Aluno *lista) {

    /*declara e inicializa variaveis a serem utilizadas*/
    FILE *arqAlunos = NULL;
    char linhaAtual[TAM_LINHA];
    int i = 0;
    int idAdj = 0;
    int offsetLinha = 0;



    /*Abre arquivo com nomes e matriculas, em modo leitura*/
    arqAlunos = fopen(NOME_ARQUIVO, "r");
    if (arqAlunos == NULL) {
        printf("\nERRO: Arquivo alunos.txt nao encontrado\n");
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
    no for, primeira condicao de saida verifica se houve erro
    segunda condicao se ja leu todos os alunos registrados
    terceira se chegou no final do arquivo
    */
    for(i = 0;(i < NUM_ALUNOS) && fgets(linhaAtual, TAM_LINHA, arqAlunos) != NULL; i++) {
    
        /*declara o offselinha, pois o arquivo sera lido linha a linha, e esta variavel ira funcionar como forma de apontar para o caracter a ser lido*/
        offsetLinha = 0;

        /*coloca o id do aluno*/
        lista[i].id = i+1;
        lista[i].qtdAdj = 0;
        sscanf(linhaAtual, "%[a-zA-Z ]#%9[0-9][^#]#", lista[i].nome, lista[i].matricula);
        /*calcula o novo offset, dado que foi lido nome e matricula*/



        offsetLinha = strlen(lista[i].nome) + strlen(lista[i].matricula) + 2;

        /*Le os adjacentes, e sai povoando a lista de adjacencia*/
        while(sscanf(&linhaAtual[offsetLinha],"%d#", &idAdj) == 1) {
           /*Coloca o novo vertice no inicio da lista de adj*/
           pushAdj(&lista[i], idAdj);

           (lista[i].qtdAdj)++;
           /*anda na string para o prox valor*/
           for(;linhaAtual[offsetLinha] != '#' || strlen(&linhaAtual[offsetLinha]) == 0; offsetLinha++);
           offsetLinha++;
        }




    }
    return i;
}


/*Recebe um aluno por referencia, cria um novo Adj e coloca no inicio da lista apontada pelo Aluno.adj*/
void pushAdj(Aluno *cabeca, int id) {

    /*aloca espaco para novo vertice*/
    Adj *adjTemp = malloc(sizeof(Adj));

    /*coloca valor do id*/
    adjTemp->id = id;
    /*aponta o prox do novo vertice para o primeiro elemento da lista de adjacencia do aluno*/
    adjTemp->prox = cabeca->adj;
    /*aponta primeiro elemento da lista de adj do aluno para novo vertice*/
    cabeca->adj = (Adj *)adjTemp;
    
    

}

/*procura na lista dada pela cabeca e tira da lista*/
void popAdj(Aluno *cabeca, int id) {

    Adj *adjTemp = (Adj *) cabeca->adj;
    Adj *adjAnterior = adjTemp;
    
    /*checa para ver se a lista eh nula*/
    if(adjTemp == NULL) {
        return;
    }
    
    
    if (adjTemp->id == id) {
        cabeca->adj = (Adj *) adjTemp->prox;
        free(adjTemp);
        cabeca->qtdAdj--;
        return;
    }
    adjTemp = (Adj *) adjTemp->prox;
    while(adjTemp != NULL) {
        if(adjTemp->id == id) {
            adjAnterior->prox = (Adj *) adjTemp->prox;
            cabeca->qtdAdj--;
            free(adjTemp);
            break;
        } else {
            adjAnterior = adjTemp;
            adjTemp = (Adj *)adjTemp->prox;
        }
        
    }

}

void freeAdj(Adj *adj) {

    Adj *temp = adj;
    Adj *next = NULL;
    while (temp != NULL) {
        next = (Adj *)temp->prox;
        free(temp);
        temp = next;
    }

}
void freeLista(Aluno *lista) {
    int i;
    /*Libera a lista de adjacencia primeiro*/
    for(i = 0; i < NUM_ALUNOS; i++) {
        freeAdj((Adj *)lista[i].adj);
    }
    /*Finalmente libera o vetor*/
    free(lista);

}
/*
Utiliza o algoritmo bubblesort, pela simplicidade de implementacao
*/
void Ordena(Aluno *lista)
{
    int i,j;
    int idTemp, qtdAdjTemp;
    char nomeTemp[30], matriculaTemp[10];
    for(j=0;j<NUM_ALUNOS;j++)
    {
        for(i=j+1;i<NUM_ALUNOS;i++)
        {
            if(lista[j].qtdAdj < lista[i].qtdAdj)
            {
                idTemp = lista[j].id;
                strcpy(nomeTemp, lista[j].nome);
                strcpy(matriculaTemp, lista[j].matricula);
                qtdAdjTemp = lista[j].qtdAdj;

                lista[j].id = lista[i].id;
                strcpy(lista[j].nome, lista[i].nome);
                strcpy(lista[j].matricula, lista[i].matricula);
                lista[j].qtdAdj = lista[i].qtdAdj;

                lista[i].id = idTemp;
                strcpy(lista[i].nome, nomeTemp);
                strcpy(lista[i].matricula, matriculaTemp);
                lista[i].qtdAdj = qtdAdjTemp;
            }
        }
    }
}


/*
imprime a lista toda, de formas diferentes dependendo do parametro
arg:
n = normal
l = um por linha

default: normal
*/
void Imprime(Aluno *lista, char arg)
{
    int i;

    if (arg == 'l')
    {
        printf("ID  NOME                  MATRICULA  GRAU\n");
        for(i=0;i<NUM_ALUNOS;i++)
        {

            printf("%02d||",lista[i].id);
            printf("%-20s||", lista[i].nome);
            printf("%s||", lista[i].matricula);
            printf(" %02d ||\n", lista[i].qtdAdj);
        }
        /*Volta para a funcao chamadora*/
        return;
    }
    for(i=0;i<NUM_ALUNOS;i++)
    {
        printf("id=%d\n",lista[i].id);
        printf("nome=%s\n", lista[i].nome);
        printf("matricula=%s\n", lista[i].matricula);
        printf("grau=%d\n\n", lista[i].qtdAdj);
    }
}

/*Funcao que prepara as chamadas iniciais da funcao recursiva buscaClique*/
Adj *maiorClique(Aluno *lista){

    int i=0;
    /*clique: variavel com apenas o aluno a ser testado*/
    Aluno *clique = malloc(sizeof(Aluno));
    /*possivelClique: cabeca com todos amigos do aluno a ser testado*/
    Aluno *possivelClique = malloc(sizeof(Aluno));
    possivelClique->adj = NULL;
    clique->adj = NULL;
    
    possivelClique->qtdAdj = 0;
    clique->qtdAdj = 0;
    
    maiorCliqueEncontrado.qtdAdj = 0;

    /*testa cada um dos 39 alunos, seguindo pelo id*/
    for(i = 0; i < NUM_ALUNOS; i++){
        /*zera a lista de adj, para poder pegar o do proximo aluno*/
        clique->adj = NULL;
        /*Copia lista de adj do aluno a ser testado para uma nova lista*/
        Adj *cursorAuxiliar = (Adj *) lista[i].adj;
        while(cursorAuxiliar != NULL) {
            pushAdj(possivelClique, cursorAuxiliar->id);
            cursorAuxiliar = cursorAuxiliar->prox;
        }
        /*coloca o aluno a ser testado dentro do clique*/
        pushAdj(clique, lista[i].id);
        
        /*chama a funcao*/
        buscaClique(lista, clique, (Adj *) possivelClique->adj);   
    
    }
    

    
    return NULL;

}
/*
lista: vetor de cabecas, para poder verificar amizades
clique: elementos que ja estao sendo avaliados como componentes de um clique
listaClique: elementos que ainda sao elegiveis para entrar no clique
*/
void buscaClique(Aluno *lista, Aluno *clique, Adj *listaClique) {

    /*condicao de parada: nao ha mais adjacentes a serem colocados*/
    if(listaClique == NULL) {
        
        /*verifica se o clique final eh o maior clique ate o momento*/
        if(clique->qtdAdj > maiorCliqueEncontrado.qtdAdj) {
            /*cria uma copia deste clique e coloca na global maiorCliqueEncontrado*/
            Adj *copia = copiaListaAdj(clique->adj);
            maiorCliqueEncontrado.adj = copia;
            maiorCliqueEncontrado.qtdAdj = clique->qtdAdj;
        }
        return;        
    }
    
    
    /*
    percorre listaClique
    1-coloca primeiro elemento da lista
    2-verifica qual dos outros tambem eh amigo deste primeiro elemento
    3-refaz a lista com apenas estes amigos
    4-chama buscaClique novamente, colocando (lista,clique(com este primeiro elemento incluso), lista do passo3)
    5-neste ponto, todas as possiveis permutacoes com este primeiro elemento foram exauridas, logo ele da um pop
        dele mesmo do clique.
    6-repete o processo, so que com o segundo
    7-ao chegar ao fim, simplesmente retorna
    */
    Adj *novaListaClique = NULL;
    Adj *cursorLista = listaClique->prox;
    while(listaClique != NULL){
        
        /*zera listaClique do loop anterior*/
        novaListaClique = NULL;
        
        /*coloca o elemento vigente dentro do clique*/
        pushAdj(clique, listaClique->id);
        /*aponta que houve aumento de um elemento no clique*/
        clique->qtdAdj++;

        
        /*anda na lista, para comparar apenas com os proximos, pois qualquer permutacao entre o aluno sendo testado com os anteriores da lista ja foi feito em iteracoes anteriores*/
        if (cursorLista != NULL) cursorLista = cursorLista->prox;
        /*verifica os amigos deste novo elemento dentro da listaClique*/        
        while(cursorLista!=NULL){
            
            if (verificaAmizade(lista, cursorLista->id, listaClique->id)) 
            {

                novaListaClique = (Adj *)pushAdjNo(novaListaClique, cursorLista->id);
                
            }
            cursorLista = cursorLista->prox;
        }

      
        
        buscaClique(lista, clique, (Adj *) novaListaClique);
        popAdj(clique, listaClique->id);
        
        listaClique = listaClique->prox;
        
        
    
    }
    

    
    
}
/*
1 = amigos
0 = nao amigos
requer lista nao ordenada, com id do aluno casando com indice do vetor
*/
int verificaAmizade(Aluno *lista, int amigo1, int amigo2) {
    
    Adj *cursor = NULL;
    cursor =  (Adj *)lista[amigo1 - 1].adj;
    while(cursor!=NULL) {
        if(cursor->id == amigo2) {
            return 1;
        }   
        cursor = cursor->prox;
    }
    return 0;
}

void imprimeAdj(Aluno *lista, Aluno aluno) {
    Adj *cursor = NULL;
    cursor = (Adj *)aluno.adj;
    while(cursor != NULL) {
        printf("\n(%d)%s", cursor->id, lista[cursor->id - 1].nome);
        cursor = (Adj *)cursor->prox;
    }
}



Adj *pushAdjNo(Adj *adj, int id) {
    Adj *aux1 = NULL;
    Adj *inicio = adj;
    if(adj == NULL) {
        aux1  = malloc(sizeof(Adj));
        aux1->id = id;
        aux1->prox = NULL;
        return aux1;
    }
    
    while(adj->prox != NULL){
        adj = (Adj *)adj->prox;
    }
    aux1  = malloc(sizeof(Adj));
    aux1->id = id;
    aux1->prox = NULL;
    adj->prox = (Adj *)aux1;
    
    return inicio;
    
}

Adj *popAdjNo(Adj *adj, int id) {
    Adj *inicio = (Adj *) adj;
    Adj *aux = NULL;
    if(adj->prox == NULL) {
        free(adj);
        return NULL;
    }
    aux = adj->prox;
    while(aux->id != id) {
        adj = (Adj *)adj->prox;
        aux = (Adj *)adj->prox;
    }
    aux = (Adj *)adj->prox;
    adj->prox = (Adj *)aux->prox;
    free (aux);
    
    return inicio;
    
}

Adj *copiaListaAdj(Adj *inicial) {
    Adj *nova = NULL;
    Adj *cursor = inicial;
    while(cursor!= NULL) {
        nova = pushAdjNo(nova, cursor->id);
        cursor = cursor->prox;
        
    }
    return nova;

}
