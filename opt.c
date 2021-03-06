#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opt.h"
#include "game.h"

struct user {
    char userName[11];
    char password[7];
    int bestScoreSum;
    int bestScoreSub;
    int bestScoreMul;
    int bestScoreDiv;
    int bestScoreMix;
};

struct treeNode {
    User* info;
    TreeNode* left;
    TreeNode* right;
};

struct tree {
    TreeNode* root;
};

//enum operation{ SUM, SUB, MUL, DIV, MIX };

Tree* tree_creates(void)
{
    Tree* t = (Tree*) malloc(sizeof(Tree));

    if(t == NULL) {
        printf("Não há memória disponível.\b");
        exit(1);
    }

    t->root = NULL;

    return t;
}

static User* creates_user(char *name, char* password, int sum, int sub, int mul, int div, int mix)
{
    User* u = (User*) malloc(sizeof(User));

    if(u == NULL) {
        printf("Não há memória disponível.\b");
        exit(1);
    }

    strcpy(u->userName, name);
    strcpy(u->password, password);

    u->bestScoreSum = sum;
    u->bestScoreSub = sub;
    u->bestScoreMul = mul;
    u->bestScoreDiv = div;
    u->bestScoreMix = mix;

    return u;
}

static TreeNode* insert(TreeNode* r, User* u)
{
    if(r == NULL) {
        r = (TreeNode*) malloc(sizeof(TreeNode));

        if(r == NULL) {
            printf("Não há memória disponível.\b");
            exit(1);
        }

        r->info = creates_user(u->userName, u->password, u->bestScoreSum, u->bestScoreSub,
                                    u->bestScoreMul, u->bestScoreDiv, u->bestScoreMix);
        r->left = r->right = NULL;
    }
    else if(strcmp(u->userName, r->info->userName) < 0)
        r->left = insert(r->left, u);
    else if(strcmp(u->userName, r->info->userName) > 0)
        r->right = insert(r->right, u);
    else
        printf("Esse nome de usuário já existe!\n");

    return r;
}

void bst_insert(Tree* t, User* u)
{
    t->root = insert(t->root, u);
}

static void Save(TreeNode* r, FILE* file)
{
  if(r != NULL) {
    Save(r->left, file);
    fwrite(r->info, sizeof(User), 1, file);
    Save(r->right, file);
  }
}

void SaveInFile(Tree* t)
{
  FILE* file = fopen("./user.b", "wb");

  if(file == NULL) {
    printf("O arquivo não pode ser aberto.\n");
    exit(1);
  }

  Save(t->root, file);
  fclose(file);
}

void readFromFile(Tree *t)
{
  FILE* file = fopen("./user.b", "rb");
  User u;

  if(file == NULL) {
    printf("O arquivo não pode ser aberto.\n");
    exit(1);
  }

  if(t->root == NULL) {
    while( fread(&u, sizeof(User), 1, file) ) {
        bst_insert(t, &u);
    }
  }
  fclose(file);
}

static void print_users(User* u)
{
  printf("Nome: %s\t", u->userName);
  printf("Sum: %d\t", u->bestScoreSum);
  printf("Sub: %d\t", u->bestScoreSub);
  printf("Mul: %d\t", u->bestScoreMul);
  printf("Div: %d\t", u->bestScoreDiv);
  printf("Mix: %d\n\n", u->bestScoreMix);
}

static void print(TreeNode* r)
{
  if(r != NULL) {
    print(r->left);
    print_users(r->info);
    print(r->right);
  }
}

void bst_imprime(Tree* t)
{
  print(t->root);
}

static int bestMix(TreeNode* r)
{
    if(r == NULL)
        return -1;

    int res = r->info->bestScoreMix;
    int lres = bestMix(r->left);
    int rres = bestMix(r->right);

    if(lres > res)
        res = lres;
    if(rres > res)
        res = rres;

    return res;
}

int bst_bestMix(Tree* t)
{
    return bestMix(t->root);
}

static TreeNode* Remove(TreeNode* r, User* u)
{
    if(r == NULL)
        return NULL;
    else if( strcmp(r->info->userName, u->userName) > 0 )
        r->left = Remove(r->left, u);
    else if( strcmp(r->info->userName, u->userName) < 0 )
        r->right = Remove(r->right, u);
    else { // achou o elemento
        // elemento sem filho
        if( r->left == NULL && r->right == NULL ) {
            free(r);
            r = NULL;
        }

        // só tem filho à direita
        else if(r->left == NULL) {
            TreeNode* t = r;
            r = r->right;
            free(t);
        }
        // só tem filho à esquerda
        else if(r->right == NULL) {
            TreeNode* t = r;
            r = r->left;
            free(t);
        }
        // tem os dois filhos
        else {
            TreeNode* t = r->left;
            while(t->right != NULL) {
                t = t->right;
            }

            r->info = t->info; // troca as informações
            t->info = u;
            r->left = Remove(r->left, u);
        }
    }

    return r;
}

void bst_remove(Tree* t, User* u)
{
    t->root = Remove(t->root, u);
}

static TreeNode* search(TreeNode* r, char* name)
{
    if( r == NULL ) return NULL;
    else if( strcmp(r->info->userName, name) > 0 ) return search(r->left, name);
    else if( strcmp(r->info->userName, name) < 0 ) return search(r->right, name);
    else return r;
}

TreeNode* bst_search(Tree *t, char* name)
{
    return search(t->root, name);
}

static TreeNode* searchByValue(TreeNode* r, int v)
{
    if(r == NULL)
        return NULL; // árvore vazia: não encontrou
    else if(v == r->info->bestScoreMix)
        return r;
    else {
        TreeNode* p = searchByValue(r->left, v); // busca na sae
        if(p != NULL)
            return p;
        else
            return searchByValue(r->right, v); // busca na sad
    }
}

TreeNode* bst_searchByValue(Tree* t, int v)
{
    return searchByValue(t->root, v);
}

static int readUser(User* u)
{
    char s1[11];
    char s2[7];
    printf("Degite um nome de no máximo 10 caracteres: ");
    scanf("%s", s1);

    if(strlen(s1) > 10) {
      printf("\nO nome deve ter no máximo 10 caracteres.\n");
      printf("Tente novamente.\n\n");
      return -1;
    }

    strcpy(u->userName, s1);

    printf("Digite uma senha de no máximo 6 caracteres: ");
    scanf("%s", s2);

    if(strlen(s2) > 6) {
      printf("\nA senha deve ter no máximo 6 caracteres.\n");
      printf("Tente novamente.\n\n");
      return -1;
    }
    strcpy(u->password, s2);

    // inicializa os scores
    u->bestScoreSum = 0;
    u->bestScoreSub = 0;
    u->bestScoreMul = 0;
    u->bestScoreDiv = 0;
    u->bestScoreMix = 0;

    return 0;
}

static void release(TreeNode* r)
{
    if( r != NULL ) {
        release(r->left);
        release(r->right);
        free(r);
    }
}

void bst_release(Tree* t)
{
    release(t->root);
    free(t);
}

void update(char *name, Operation op, int score )
{
    Tree *t = tree_creates();
    readFromFile(t);

    TreeNode* iPtr = bst_search(t, name);

    switch(op) {
        case SUM:
            if(iPtr->info->bestScoreSum < score)
                iPtr->info->bestScoreSum = score;
            break;
        case SUB:
            if(iPtr->info->bestScoreSub < score)
                iPtr->info->bestScoreSub = score;
            break;
        case MUL:
            if(iPtr->info->bestScoreMul < score)
                iPtr->info->bestScoreMul = score;
            break;
        case DIV:
            if(iPtr->info->bestScoreDiv < score)
                iPtr->info->bestScoreDiv = score;
            break;
        case MIX:
            if(iPtr->info->bestScoreMix < score)
                iPtr->info->bestScoreMix = score;
            break;
    }

    SaveInFile(t);
}

void verify(char* name, char* password)
{
    Tree *t = tree_creates();
    readFromFile(t);

    // Realiza pesquisar para verificar se o usuário consta na lista
    TreeNode* iPtr = bst_search(t, name);

    // Caso o nome e senha conste na lista, é dado acesso ao jogo
    if( iPtr != NULL && strcmp( iPtr->info->userName, name ) == 0 && strcmp( iPtr->info->password, password ) == 0 )
        main_game(name);
    else
        printf("\nUsuário ou senha incorretos!\n\n");
}

void main_menu(void)
{
    printf("====== MENU ======\n"
           "1 - Jogar\n"
           "2 - Opções\n"
           "3 - Sair\n"
           "===================\n");
}

static void menu_opt(void)
{
    printf("======= Opções =======\n"
           "1 - Cadastrar usuário\n"
           "2 - Mostrar usuários\n"
           "3 - Apagar usuário\n"
           "4 - Voltar\n"
           "======================\n");
}

void main_opt(void)
{
    Tree *t = tree_creates();
    readFromFile(t);

    char op;
    menu_opt();
    printf("> ");
    scanf(" %c", &op);

    while(op != '4') {
        switch(op) {
            case '1': {
                User u;
                int r = readUser(&u);
                while(r == -1) {
                  r = readUser(&u);
                }

                bst_insert(t, &u);

                SaveInFile(t);
                break;
            }
            case '2': {
                int r = bst_bestMix(t);
                TreeNode *res = bst_searchByValue(t, r);
                printf("\n======== Melhor Pontuação Atual ========\n");
                printf("\tMelhor pontuação atual:\n\t%s\t\tMix: %d\n", res->info->userName, res->info->bestScoreMix);
                printf("========================================\n\n");

                bst_imprime(t);
                break;
            }
            case '3': {
                User u;
                char name[11];
                printf("Digite o nome do usuário: ");
                scanf(" %s", name);
                strcpy(u.userName, name);

                bst_remove(t, &u);
                SaveInFile(t);
                break;
            }
            default:
                printf("Opção inválida!\n");
                break;
        }

        menu_opt();
        printf("> ");
        scanf(" %c", &op);
    }
}
