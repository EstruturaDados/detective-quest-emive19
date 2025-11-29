#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_HASH 11   // tamanho da tabela hash, primo para melhorar distribuição

// ------------------------------------------------------------
// Estrutura da sala (nó da árvore binária da mansão)
// ------------------------------------------------------------
typedef struct Sala {
    char nome[50];
    char pista[100];
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;


// ------------------------------------------------------------
// Estrutura de nó da BST das pistas encontradas
// ------------------------------------------------------------
typedef struct PistaNode {
    char pista[100];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;


// ------------------------------------------------------------
// Estrutura da tabela hash: pista → suspeito
// ------------------------------------------------------------
typedef struct HashItem {
    char pista[100];
    char suspeito[50];
    struct HashItem* prox;
} HashItem;

HashItem* tabelaHash[TAM_HASH];


// ------------------------------------------------------------
// Função hash simples para strings
// ------------------------------------------------------------
int hashFunction(const char* str) {
    int soma = 0;
    for (int i = 0; str[i] != '\0'; i++)
        soma += str[i];
    return soma % TAM_HASH;
}


// ------------------------------------------------------------
// inserirNaHash() – insere pista/suspeito na tabela hash
// ------------------------------------------------------------
void inserirNaHash(const char* pista, const char* suspeito) {
    int index = hashFunction(pista);

    HashItem* novo = (HashItem*)malloc(sizeof(HashItem));
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    novo->prox = tabelaHash[index];
    tabelaHash[index] = novo;
}


// ------------------------------------------------------------
// encontrarSuspeito() – busca o suspeito de uma pista na hash
// ------------------------------------------------------------
char* encontrarSuspeito(const char* pista) {
    int index = hashFunction(pista);
    HashItem* atual = tabelaHash[index];

    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0)
            return atual->suspeito;
        atual = atual->prox;
    }

    return NULL; // não encontrado
}


// ------------------------------------------------------------
// criarSala() – cria dinamicamente um cômodo da mansão
// ------------------------------------------------------------
Sala* criarSala(const char* nome, const char* pista) {
    Sala* nova = (Sala*)malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}


// ------------------------------------------------------------
// inserirPista() – insere pista coletada na BST
// ------------------------------------------------------------
PistaNode* inserirPista(PistaNode* raiz, const char* pista) {
    if (raiz == NULL) {
        PistaNode* novo = (PistaNode*)malloc(sizeof(PistaNode));
        strcpy(novo->pista, pista);
        novo->esquerda = NULL;
        novo->direita = NULL;
        return novo;
    }

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);

    return raiz;
}


// ------------------------------------------------------------
// exibirPistas() – imprime as pistas da BST em ordem alfabética
// ------------------------------------------------------------
void exibirPistas(PistaNode* raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esquerda);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->direita);
}


// ------------------------------------------------------------
// contarPistasDoSuspeito() – conta quantas pistas apontam para alguém
// ------------------------------------------------------------
int contarPistasDoSuspeito(PistaNode* raiz, const char* suspeito) {
    if (raiz == NULL) return 0;

    int count = 0;

    char* s = encontrarSuspeito(raiz->pista);
    if (s != NULL && strcmp(s, suspeito) == 0)
        count++;

    count += contarPistasDoSuspeito(raiz->esquerda, suspeito);
    count += contarPistasDoSuspeito(raiz->direita, suspeito);

    return count;
}


// ------------------------------------------------------------
// verificarSuspeitoFinal() – fase de julgamento do jogador
// ------------------------------------------------------------
void verificarSuspeitoFinal(PistaNode* raiz) {
    char suspeito[50];

    printf("\n==============================================\n");
    printf("         FASE FINAL – ESCOLHA O CULPADO\n");
    printf("==============================================\n");

    printf("\nPistas coletadas:\n");
    exibirPistas(raiz);

    printf("\nDigite o nome do suspeito que deseja acusar: ");
    scanf(" %[^\n]", suspeito);

    int qtd = contarPistasDoSuspeito(raiz, suspeito);

    if (qtd >= 2) {
        printf("\n✔ Acusação aceita! Você encontrou %d pistas contra %s.\n", qtd, suspeito);
        printf("Parabéns, detetive! Você resolveu o caso.\n");
    } else {
        printf("\n✘ Acusação rejeitada! Apenas %d pista(s) apontam para %s.\n", qtd, suspeito);
        printf("O detetive falhou em provar a culpa do suspeito.\n");
    }
}


// ------------------------------------------------------------
// explorarSalas() – navega pela mansão e coleta pistas
// ------------------------------------------------------------
void explorarSalas(Sala* atual, PistaNode** arvPistas) {
    char opcao;

    while (1) {
        printf("\nVocê está em: %s\n", atual->nome);

        // mostrar pista da sala
        printf("Pista encontrada: %s\n", atual->pista);

        // inserir na BST
        *arvPistas = inserirPista(*arvPistas, atual->pista);

        // mostrar suspeito associado pela hash
        char* s = encontrarSuspeito(atual->pista);
        printf("Esta pista está associada ao suspeito: %s\n", s);

        // opções
        printf("\nEscolha o caminho:\n");
        printf("(e) Ir para a esquerda\n");
        printf("(d) Ir para a direita\n");
        printf("(s) Sair e ir para o julgamento\n");
        printf("Opção: ");
        scanf(" %c", &opcao);

        if (opcao == 'e' || opcao == 'E') {
            if (atual->esquerda != NULL)
                atual = atual->esquerda;
            else
                printf("Não há sala à esquerda!\n");
        }
        else if (opcao == 'd' || opcao == 'D') {
            if (atual->direita != NULL)
                atual = atual->direita;
            else
                printf("Não há sala à direita!\n");
        }
        else if (opcao == 's' || opcao == 'S') {
            printf("\nEncerrando exploração...\n");
            return;
        }
        else {
            printf("Opção inválida.\n");
        }
    }
}


// ------------------------------------------------------------
// main() – monta o mapa, tabela hash, inicia jogo
// ------------------------------------------------------------
int main() {

    // ---------- Limpar tabela hash ----------
    for (int i = 0; i < TAM_HASH; i++)
        tabelaHash[i] = NULL;

    // ---------- Criar salas da mansão ----------
    Sala* hall = criarSala("Hall de Entrada", "pegadas de lama");
    Sala* salaEstar = criarSala("Sala de Estar", "copo quebrado");
    Sala* biblioteca = criarSala("Biblioteca", "livro arrancado");
    Sala* cozinha = criarSala("Cozinha", "faca faltando");
    Sala* jardim = criarSala("Jardim", "terra remexida");

    // montar árvore binária
    hall->esquerda = salaEstar;
    hall->direita = biblioteca;

    salaEstar->esquerda = cozinha;
    salaEstar->direita = jardim;

    // ---------- Criar tabela hash pista → suspeito ----------
    inserirNaHash("pegadas de lama", "Carlos");
    inserirNaHash("copo quebrado", "Marina");
    inserirNaHash("livro arrancado", "Carlos");
    inserirNaHash("faca faltando", "Marina");
    inserirNaHash("terra remexida", "Carlos");

    // ---------- Árvore de pistas coletadas ----------
    PistaNode* arvPistas = NULL;

    printf("==== DETECTIVE QUEST – CAPÍTULO FINAL ====\n");
    explorarSalas(hall, &arvPistas);

    // ---------- Fase de julgamento ----------
    verificarSuspeitoFinal(arvPistas);

    return 0;
}
