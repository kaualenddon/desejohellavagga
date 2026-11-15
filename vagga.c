/*
 * Jogo: O Desejo de Hella Vagga (RPG de Tabuleiro no Terminal)
 * Disciplina: DCT1106 - Programacao (UFRN)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* Constantes do jogo */
#define TOTAL_CASAS 24
#define LINHAS_TAB 4
#define COLUNAS_TAB 6
#define MAX_MOCHILA 10
#define MAX_INIMIGOS 3
#define TOTAL_FASES 3

/* Cores simples para o terminal */
#define COR_NORMAL   "\033[0m"
#define COR_VERMELHO "\033[1;31m"
#define COR_VERDE    "\033[1;32m"
#define COR_AMARELO  "\033[1;33m"
#define COR_AZUL     "\033[1;34m"
#define COR_ROXO     "\033[1;35m"
#define COR_CIANO    "\033[1;36m"
#define COR_LARANJA  "\033[38;5;208m"

/* =========================================================================
 * ENUMERACOES (ENUM)
 * ========================================================================= */
typedef enum {
    GUERREIRO = 1,
    MAGO = 2,
    ARQUEIRO = 3
} ClasseHeroi;

typedef enum {
    CASA_INICIO = 0,
    CASA_NADA = 1,
    CASA_LUTA = 2,
    CASA_ARMADILHA = 3,
    CASA_CURA = 4,
    CASA_BAU = 5,
    CASA_BOSS = 6
} TipoCasa;

typedef enum {
    ACAO_LUTAR = 1,
    ACAO_HABILIDADE = 2,
    ACAO_DEFENDER = 3,
    ACAO_FUGIR = 4
} AcaoBatalha;

/* =========================================================================
 * ESTRUTURAS (STRUCT)
 * ========================================================================= */
typedef struct {
    char nome[40];
    int ehArma;         /* 1 = Arma, 0 = Roupa/Armadura */
    int bonusDefesa;
    int bonusResMagica;
    int bonusDano;
} Item;

typedef struct {
    char nome[30];
    ClasseHeroi classe;
    int nivel;
    int xp;
    int vida;
    int vidaMax;
    int forca;
    int defesa;
    int agilidade;
    int inteligencia;
    int resMagica;
    int chanceCritico;

    /* Equipamentos atuais */
    Item roupa;
    Item arma;
    Item mochila[MAX_MOCHILA];
    int qtdItens;

    /* Habilidades */
    char nomeHabilidades[4][30];
    int danoHabilidades[4];
    int nivelHabilidades[4];
    int recargaHabilidade;
} Personagem;

typedef struct {
    char nome[35];
    char descricao[80];
    int nivel;
    int vida;
    int vidaMax;
    int forca;
    int defesa;
    int agilidade;
    int inteligencia;
    int resMagica;
    int bonusDefesa;
    int bonusDano;
    char nomeHabilidade[30];
    int danoHabilidade;
    int chanceCritico;
    int ataqueMagico;   /* 1 = Usa inteligencia, 0 = Usa forca */
} Monstro;

typedef struct {
    int numero;
    char titulo[40];
    char reliquia[40];
    char missao[100];
    int danoArmadilha;
    char textoArmadilha[80];
    TipoCasa casas[TOTAL_CASAS];
    Monstro monstros[3];
    Monstro boss;
} Fase;

/* =========================================================================
 * DECLARACAO DAS FUNCOES
 * ========================================================================= */
void limparTela(void);
void pausar(void);
int lerOpcao(int min, int max);
int rolarDado(int lados);

int telaMenuPrincipal(void);
void telaInformacoes(void);
int telaEscolherHeroi(Personagem *p);
void mostrarFichaHeroi(Personagem *p);
void telaHistoriaReiInicio(void);
void telaHistoriaReiFim(void);

void criarHeroi(Personagem *p, ClasseHeroi classe);
void levarDanoHeroi(Personagem *p, int dano);
void curarHeroi(Personagem *p, int cura);
void subirNivel(Personagem *p);
void telaStatusEMochila(Personagem *p);

void prepararFases(Fase fases[]);
void mostrarTabuleiro(Fase *fase, Personagem *p, int posicao);
void moverHeroi(int *posicao, int passos);
void abrirBau(Personagem *p, int numFase);
int executarCasa(Personagem *p, Fase *fase, int posicao);

void levarDanoMonstro(Monstro *m, int dano);
void ajustarMonstroParaGrupo(Monstro *m, int qtdInimigos);
int calcularDanoHeroi(Personagem *p, Monstro *m, int bonusHab, int *critico);
int calcularDanoMonstro(Monstro *m, Personagem *p, int usouHab, int *critico);
int iniciarBatalha(Personagem *p, Fase *fase, int ehBoss);

/* =========================================================================
 * FUNCAO PRINCIPAL (MAIN)
 * ========================================================================= */
int main(void) {
    int opcao;
    int jogando;
    int faseAtual;
    int posicao;
    int venceuFase;
    int dado;
    Personagem heroi;
    Fase fases[TOTAL_FASES];

    srand((unsigned int)time(NULL));

    while (1) {
        opcao = telaMenuPrincipal();

        if (opcao == 0) {
            limparTela();
            printf("Obrigado por jogar O Desejo de Hella Vagga!\n");
            break;
        }
        else if (opcao == 2) {
            telaInformacoes();
        }
        else if (opcao == 1) {
            /* Se o jogador apertar 0 na selecao, volta ao menu */
            if (telaEscolherHeroi(&heroi) == 0) {
                continue;
            }

            telaHistoriaReiInicio();
            prepararFases(fases);

            jogando = 1;
            faseAtual = 0;

            /* Loop das 3 fases */
            while (jogando == 1 && faseAtual < TOTAL_FASES) {
                posicao = 0;
                venceuFase = 0;

                /* Loop dentro do tabuleiro da fase */
                while (jogando == 1 && venceuFase == 0) {
                    mostrarTabuleiro(&fases[faseAtual], &heroi, posicao);

                    printf("\n[1] Rolar dado (d6)\n");
                    printf("[2] Verificar status e mochila\n");
                    printf("[0] Voltar ao menu principal\n");

                    opcao = lerOpcao(0, 2);

                    if (opcao == 0) {
                        jogando = 0;
                    }
                    else if (opcao == 2) {
                        telaStatusEMochila(&heroi);
                    }
                    else if (opcao == 1) {
                        dado = rolarDado(6);
                        moverHeroi(&posicao, dado);

                        mostrarTabuleiro(&fases[faseAtual], &heroi, posicao);
                        printf("\nVoce tirou %s%d%s no dado e foi para a casa %d!\n",
                               COR_AMARELO, dado, COR_NORMAL, posicao + 1);

                        venceuFase = executarCasa(&heroi, &fases[faseAtual], posicao);

                        /* Verifica se morreu */
                        if (heroi.vida <= 0) {
                            limparTela();
                            printf("%s=====================================%s\n", COR_VERMELHO, COR_NORMAL);
                            printf("%s             FIM DE JOGO             %s\n", COR_VERMELHO, COR_NORMAL);
                            printf("%s=====================================%s\n\n", COR_VERMELHO, COR_NORMAL);
                            printf("Sua vida chegou a zero!\n");
                            pausar();
                            jogando = 0;
                        }
                        /* Verifica se derrotou o Boss da fase */
                        else if (venceuFase == 1) {
                            limparTela();
                            printf("%s=====================================%s\n", COR_VERDE, COR_NORMAL);
                            printf("%s          VITORIA NA FASE %d!         %s\n", COR_VERDE, faseAtual + 1, COR_NORMAL);
                            printf("%s=====================================%s\n\n", COR_VERDE, COR_NORMAL);
                            printf("Voce Obteve o %s%s%s!\n\n", COR_AMARELO, fases[faseAtual].reliquia, COR_NORMAL);
                            printf("[1] Continuar\n");
                            printf("[0] Voltar ao menu principal\n");

                            if (lerOpcao(0, 1) == 0) {
                                jogando = 0;
                            } else {
                                faseAtual++;
                            }
                        }
                    }
                }
            }

            /* Se completou as 3 fases vivo, mostra o final */
            if (jogando == 1 && faseAtual == TOTAL_FASES) {
                telaHistoriaReiFim();
            }
        }
    }

    return 0;
}

/* =========================================================================
 * FUNCOES BASICAS DE TELA E ENTRADA
 * ========================================================================= */
void limparTela(void) {
    system("cls || clear");
}

void pausar(void) {
    int c;
    printf("\nPressione ENTER para continuar...");
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

int lerOpcao(int min, int max) {
    int numero;
    int leuCerto;
    int c;

    while (1) {
        printf("\nDigite sua opcao (%d a %d): ", min, max);
        leuCerto = scanf("%d", &numero);

        /* Limpa o teclado para evitar loop infinito com letras */
        while ((c = getchar()) != '\n' && c != EOF) {
        }

        if (leuCerto == 1 && numero >= min && numero <= max) {
            return numero;
        }

        printf("%sOpcao invalida! Tente novamente.%s\n", COR_VERMELHO, COR_NORMAL);
    }
}

int rolarDado(int lados) {
    return (rand() % lados) + 1;
}

/* =========================================================================
 * TELAS DE MENU E HISTORIA
 * ========================================================================= */
int telaMenuPrincipal(void) {
    limparTela();
    printf("%s================================================%s\n", COR_AMARELO, COR_NORMAL);
    printf("%s            O DESEJO DE HELLA VAGGA             %s\n", COR_AMARELO, COR_NORMAL);
    printf("%s================================================%s\n\n", COR_AMARELO, COR_NORMAL);
    printf("  [1] Iniciar Jogo (Comeca a Jornada)\n");
    printf("  [2] Informacoes Gerais (Rapido Tutorial)\n");
    printf("  [0] Sair\n");
    return lerOpcao(0, 2);
}

void telaInformacoes(void) {
    limparTela();
    printf("%s=== INFORMACOES GERAIS ===%s\n\n", COR_CIANO, COR_NORMAL);
    printf("O jogo conta com tres personagens principais, escolha o que\n");
    printf("preferir, venca batalhas para evoluir, encontre itens nos\n");
    printf("baus e colete os artefatos, derrota reinicia o jogo\n");
    printf("completamente. Boa sorte!\n\n");
    printf("Dicas:\n");
    printf("- Role o d6 para andar pelas 24 casas do tabuleiro.\n");
    printf("- Vencer batalhas sobe +1 nivel e recupera parte da vida.\n");
    printf("- Use [3] Defender nas lutas para reduzir o dano pela metade.\n\n");
    printf("[0] Voltar\n");
    lerOpcao(0, 0);
}

void mostrarFichaHeroi(Personagem *p) {
    limparTela();
    printf("%s============================================================%s\n", COR_AMARELO, COR_NORMAL);
    printf("                     FICHA DO PERSONAGEM                    \n");
    printf("%s============================================================%s\n\n", COR_AMARELO, COR_NORMAL);

    if (p->classe == GUERREIRO) {
        printf("%s      .---.      %s | Nome: Eilleen (Guerreira)\n", COR_LARANJA, COR_NORMAL);
        printf("%s     /     \\     %s | Vida: %d\n", COR_LARANJA, COR_NORMAL, p->vidaMax);
        printf("%s    | () () |    %s | Forca: %d   | Defesa: %d (+%d)\n", COR_LARANJA, COR_NORMAL, p->forca, p->defesa, p->roupa.bonusDefesa);
        printf("%s     \\  ^  /     %s | Agilidade: %d | Inteligencia: %d\n", COR_LARANJA, COR_NORMAL, p->agilidade, p->inteligencia);
        printf("%s    .='---'=.    %s | Res. Magica: %d (+%d)\n", COR_LARANJA, COR_NORMAL, p->resMagica, p->roupa.bonusResMagica);
        printf("%s   /  |   |  \\ / %s | Roupa: %s\n", COR_LARANJA, COR_NORMAL, p->roupa.nome);
        printf("%s  |   |===|   |/ %s | Arma: %s (+%d dano)\n", COR_LARANJA, COR_NORMAL, p->arma.nome, p->arma.bonusDano);
        printf("%s   \\__|___|__/   %s | Habilidade: %s (+%d dano)\n", COR_LARANJA, COR_NORMAL, p->nomeHabilidades[0], p->danoHabilidades[0]);
    }
    else if (p->classe == MAGO) {
        printf("%s       /^\\       %s | Nome: Bran (Mago)\n", COR_AZUL, COR_NORMAL);
        printf("%s      /___\\      %s | Vida: %d\n", COR_AZUL, COR_NORMAL, p->vidaMax);
        printf("%s     | o o |  *  %s | Forca: %d   | Defesa: %d (+%d)\n", COR_AZUL, COR_NORMAL, p->forca, p->defesa, p->roupa.bonusDefesa);
        printf("%s      \\ - /  /|  %s | Agilidade: %d | Inteligencia: %d\n", COR_AZUL, COR_NORMAL, p->agilidade, p->inteligencia);
        printf("%s    .-'---'-./|  %s | Res. Magica: %d (+%d)\n", COR_AZUL, COR_NORMAL, p->resMagica, p->roupa.bonusResMagica);
        printf("%s   /  /   \\  /   %s | Roupa: %s\n", COR_AZUL, COR_NORMAL, p->roupa.nome);
        printf("%s  |  |  *  |/    %s | Arma: %s (+%d dano)\n", COR_AZUL, COR_NORMAL, p->arma.nome, p->arma.bonusDano);
        printf("%s   \\__\\___/      %s | Habilidade: %s (+%d dano)\n", COR_AZUL, COR_NORMAL, p->nomeHabilidades[0], p->danoHabilidades[0]);
    }
    else {
        printf("%s      .---.      %s | Nome: Mael (Arqueiro)\n", COR_VERDE, COR_NORMAL);
        printf("%s     /     \\  )\\ %s | Vida: %d\n", COR_VERDE, COR_NORMAL, p->vidaMax);
        printf("%s    | >   < | | \\%s | Forca: %d   | Defesa: %d (+%d)\n", COR_VERDE, COR_NORMAL, p->forca, p->defesa, p->roupa.bonusDefesa);
        printf("%s     \\  -  /  | |%s | Agilidade: %d | Inteligencia: %d\n", COR_VERDE, COR_NORMAL, p->agilidade, p->inteligencia);
        printf("%s    .-'---'-. | |%s | Res. Magica: %d (+%d)\n", COR_VERDE, COR_NORMAL, p->resMagica, p->roupa.bonusResMagica);
        printf("%s   /  |   |  \\+->%s | Roupa: %s\n", COR_VERDE, COR_NORMAL, p->roupa.nome);
        printf("%s  |   |   |   | |%s | Arma: %s (+%d dano)\n", COR_VERDE, COR_NORMAL, p->arma.nome, p->arma.bonusDano);
        printf("%s   \\__|___|__/ )/ %s | Habilidade: %s (+%d dano)\n", COR_VERDE, COR_NORMAL, p->nomeHabilidades[0], p->danoHabilidades[0]);
    }

    printf("\n[1] Comecar\n");
    printf("[0] Voltar\n");
}

int telaEscolherHeroi(Personagem *p) {
    int escolha;
    int confirmar;

    while (1) {
        limparTela();
        printf("%s=== SELECAO DE PERSONAGENS ===%s\n\n", COR_AMARELO, COR_NORMAL);
        printf("  [1] %sEilleen (Guerreira)%s\n", COR_LARANJA, COR_NORMAL);
        printf("  [2] %sBran (Mago)%s\n", COR_AZUL, COR_NORMAL);
        printf("  [3] %sMael (Arqueiro)%s\n", COR_VERDE, COR_NORMAL);
        printf("  [0] Voltar\n");

        escolha = lerOpcao(0, 3);
        if (escolha == 0) {
            return 0;
        }

        criarHeroi(p, (ClasseHeroi)escolha);
        mostrarFichaHeroi(p);
        confirmar = lerOpcao(0, 1);

        if (confirmar == 1) {
            return 1;
        }
    }
}

void telaHistoriaReiInicio(void) {
    limparTela();
    printf("%s========================================================================%s\n", COR_AMARELO, COR_NORMAL);
    printf("   .::.  .::.  .::.   | Rei de Hella Vagga diz:\n");
    printf("   ':::\\/::::\\/:::'   | \"A tempo tenho desejado algo que estava alem de\n");
    printf("    |============|    | minhas capacidades, mas agora tenho a oportunidade\n");
    printf("    |  (o)  (o)  |    | de ter o que tanto almejo. Designo a voce a missao\n");
    printf("    |     __     |    | de coletar as tres reliquias que preciso:\n");
    printf("    |   /    \\   |    | 1. O Calice de Fridger, na Caverna de Pratoles.\n");
    printf("   / \\  \\____/  / \\   | 2. O Po de Sodium, no Deserto de Salinus.\n");
    printf("  |___|________|___|  | 3. O Ovo do Dragao Arcano, na Montanha de Eggland.\n");
    printf("  [REI DE HELLA VAGGA]| Va aventureiro e traga-os para mim!\"\n");
    printf("%s========================================================================%s\n", COR_AMARELO, COR_NORMAL);
    pausar();
}

void telaHistoriaReiFim(void) {
    limparTela();
    printf("%s========================================================================%s\n", COR_VERDE, COR_NORMAL);
    printf("   .::.  .::.  .::.   | Rei de Hella Vagga diz:\n");
    printf("   ':::\\/::::\\/:::'   | \"Meus parabens aventureiro, devo dizer que estou\n");
    printf("    |============|    | surpresso! Com as tres reliquias em maos...\n");
    printf("    |  (^)  (^)  |    |\n");
    printf("    |     __     |    | Agora poderei, enfim, saborear um OMELETE GIGANTE\n");
    printf("    |   \\____/   |    | que desejo ha tanto tempo!\"\n");
    printf("   / \\  '----'  / \\   |\n");
    printf("  |___|________|___|  | FIM DE JOGO!\n");
    printf("  [REI DE HELLA VAGGA]|\n");
    printf("%s========================================================================%s\n", COR_VERDE, COR_NORMAL);
    pausar();
}

/* =========================================================================
 * FUNCOES DO PERSONAGEM (USANDO PONTEIROS)
 * ========================================================================= */
void criarHeroi(Personagem *p, ClasseHeroi classe) {
    p->classe = classe;
    p->nivel = 1;
    p->xp = 0;
    p->qtdItens = 0;
    p->recargaHabilidade = 0;

    if (classe == GUERREIRO) {
        strcpy(p->nome, "Eilleen");
        p->vidaMax = 125;
        p->vida = 125;
        p->forca = 12;
        p->defesa = 10;
        p->agilidade = 6;
        p->inteligencia = 5;
        p->resMagica = 7;
        p->chanceCritico = 25;

        strcpy(p->roupa.nome, "Armadura de Placas");
        p->roupa.ehArma = 0;
        p->roupa.bonusDefesa = 6;
        p->roupa.bonusResMagica = 2;
        p->roupa.bonusDano = 0;

        strcpy(p->arma.nome, "Espada Longa");
        p->arma.ehArma = 1;
        p->arma.bonusDefesa = 0;
        p->arma.bonusResMagica = 0;
        p->arma.bonusDano = 5;

        strcpy(p->nomeHabilidades[0], "Golpe Pesado");
        p->danoHabilidades[0] = 20;
        p->nivelHabilidades[0] = 1;

        strcpy(p->nomeHabilidades[1], "Retalhar");
        p->danoHabilidades[1] = 35;
        p->nivelHabilidades[1] = 3;

        strcpy(p->nomeHabilidades[2], "Perfuracao");
        p->danoHabilidades[2] = 55;
        p->nivelHabilidades[2] = 6;

        strcpy(p->nomeHabilidades[3], "Punicao");
        p->danoHabilidades[3] = 80;
        p->nivelHabilidades[3] = 9;
    }
    else if (classe == MAGO) {
        strcpy(p->nome, "Bran");
        p->vidaMax = 105;
        p->vida = 105;
        p->forca = 5;
        p->defesa = 8;
        p->agilidade = 8;
        p->inteligencia = 14;
        p->resMagica = 10;
        p->chanceCritico = 25;

        strcpy(p->roupa.nome, "Tunica Magica");
        p->roupa.ehArma = 0;
        p->roupa.bonusDefesa = 5;
        p->roupa.bonusResMagica = 6;
        p->roupa.bonusDano = 0;

        strcpy(p->arma.nome, "Cajado de Rubi");
        p->arma.ehArma = 1;
        p->arma.bonusDefesa = 0;
        p->arma.bonusResMagica = 0;
        p->arma.bonusDano = 5;

        strcpy(p->nomeHabilidades[0], "Rajada de Mana");
        p->danoHabilidades[0] = 20;
        p->nivelHabilidades[0] = 1;

        strcpy(p->nomeHabilidades[1], "Bola de Fogo");
        p->danoHabilidades[1] = 35;
        p->nivelHabilidades[1] = 3;

        strcpy(p->nomeHabilidades[2], "Onda Eletrica");
        p->danoHabilidades[2] = 55;
        p->nivelHabilidades[2] = 6;

        strcpy(p->nomeHabilidades[3], "Explosao Arcana");
        p->danoHabilidades[3] = 80;
        p->nivelHabilidades[3] = 9;
    }
    else {
        strcpy(p->nome, "Mael");
        p->vidaMax = 105;
        p->vida = 105;
        p->forca = 9;
        p->defesa = 8;
        p->agilidade = 13;
        p->inteligencia = 6;
        p->resMagica = 8;
        p->chanceCritico = 30;

        strcpy(p->roupa.nome, "Armadura de Couro");
        p->roupa.ehArma = 0;
        p->roupa.bonusDefesa = 5;
        p->roupa.bonusResMagica = 2;
        p->roupa.bonusDano = 0;

        strcpy(p->arma.nome, "Arco e Flecha");
        p->arma.ehArma = 1;
        p->arma.bonusDefesa = 0;
        p->arma.bonusResMagica = 0;
        p->arma.bonusDano = 5;

        strcpy(p->nomeHabilidades[0], "Tiro Preciso");
        p->danoHabilidades[0] = 20;
        p->nivelHabilidades[0] = 1;

        strcpy(p->nomeHabilidades[1], "Saraivada");
        p->danoHabilidades[1] = 35;
        p->nivelHabilidades[1] = 3;

        strcpy(p->nomeHabilidades[2], "Flecha Relampago");
        p->danoHabilidades[2] = 55;
        p->nivelHabilidades[2] = 6;

        strcpy(p->nomeHabilidades[3], "Zumbido da Morte");
        p->danoHabilidades[3] = 80;
        p->nivelHabilidades[3] = 9;
    }

    /* Guarda os dois equipamentos iniciais na mochila */
    p->mochila[0] = p->roupa;
    p->mochila[1] = p->arma;
    p->qtdItens = 2;
}

void levarDanoHeroi(Personagem *p, int dano) {
    p->vida = p->vida - dano;
    if (p->vida < 0) {
        p->vida = 0;
    }
}

void curarHeroi(Personagem *p, int cura) {
    p->vida = p->vida + cura;
    if (p->vida > p->vidaMax) {
        p->vida = p->vidaMax;
    }
}

void subirNivel(Personagem *p) {
    int i;
    int sorteio;
    int curaPosLuta;

    p->nivel++;
    p->xp += 100;
    p->vidaMax += 20;

    curaPosLuta = 20 + (int)ceil(p->vidaMax * 0.12);
    curarHeroi(p, curaPosLuta);

    if (p->classe == GUERREIRO) {
        p->forca += 2;
        p->defesa += 2;
        p->agilidade += 1;
        p->inteligencia += 1;
        p->resMagica += 2;
    }
    else if (p->classe == MAGO) {
        p->forca += 1;
        p->defesa += 2;
        p->agilidade += 1;
        p->inteligencia += 2;
        p->resMagica += 2;
    }
    else {
        p->forca += 2;
        p->defesa += 2;
        p->agilidade += 2;
        p->inteligencia += 1;
        p->resMagica += 2;
    }

    printf("\n%s*** SUBIU PARA O NIVEL %d! ***%s\n", COR_AMARELO, p->nivel, COR_NORMAL);
    printf("Vida Maxima +20 e recuperou +%d de vida (%d/%d)!\n", curaPosLuta, p->vida, p->vidaMax);

    /* Verifica se desbloqueou habilidade nova */
    for (i = 0; i < 4; i++) {
        if (p->nivel == p->nivelHabilidades[i]) {
            printf("%sNova habilidade desbloqueada: %s (+%d dano)!%s\n",
                   COR_CIANO, p->nomeHabilidades[i], p->danoHabilidades[i], COR_NORMAL);
        }
    }

    /* Bonus aleatorio de atributo exigido na prova */
    sorteio = rand() % 5;
    if (sorteio == 0) {
        p->forca++;
        printf("Bonus aleatorio: +1 de Forca!\n");
    } else if (sorteio == 1) {
        p->defesa++;
        printf("Bonus aleatorio: +1 de Defesa!\n");
    } else if (sorteio == 2) {
        p->agilidade++;
        printf("Bonus aleatorio: +1 de Agilidade!\n");
    } else if (sorteio == 3) {
        p->inteligencia++;
        printf("Bonus aleatorio: +1 de Inteligencia!\n");
    } else {
        p->vidaMax += 5;
        p->vida += 5;
        printf("Bonus aleatorio: +5 de Vida Maxima!\n");
    }
}

void telaStatusEMochila(Personagem *p) {
    int i;
    int escolha;

    while (1) {
        limparTela();
        printf("%s=== STATUS DE %s (Nivel %d) ===%s\n\n", COR_CIANO, p->nome, p->nivel, COR_NORMAL);
        printf("Vida: %d / %d  |  XP: %d\n", p->vida, p->vidaMax, p->xp);
        printf("Forca: %d       |  Defesa: %d (+%d roupa)\n", p->forca, p->defesa, p->roupa.bonusDefesa);
        printf("Agilidade: %d   |  Inteligencia: %d\n", p->agilidade, p->inteligencia);
        printf("Res. Magica: %d (+%d roupa) | Critico: %d%%\n\n", p->resMagica, p->roupa.bonusResMagica, p->chanceCritico);

        printf("Equipado:\n");
        printf("- Roupa: %s (+%d Def, +%d Res.Mag)\n", p->roupa.nome, p->roupa.bonusDefesa, p->roupa.bonusResMagica);
        printf("- Arma:  %s (+%d Dano)\n\n", p->arma.nome, p->arma.bonusDano);

        printf("Habilidades:\n");
        for (i = 0; i < 4; i++) {
            if (p->nivel >= p->nivelHabilidades[i]) {
                printf("- %s (+%d dano) [Liberada]\n", p->nomeHabilidades[i], p->danoHabilidades[i]);
            } else {
                printf("- %s [Libera no Nivel %d]\n", p->nomeHabilidades[i], p->nivelHabilidades[i]);
            }
        }

        printf("\nMochila (digite o numero para equipar):\n");
        for (i = 0; i < p->qtdItens; i++) {
            printf("[%d] %s (+%d Def, +%d Res.Mag, +%d Dano)\n",
                   i + 1, p->mochila[i].nome,
                   p->mochila[i].bonusDefesa, p->mochila[i].bonusResMagica, p->mochila[i].bonusDano);
        }
        printf("[0] Voltar\n");

        escolha = lerOpcao(0, p->qtdItens);
        if (escolha == 0) {
            break;
        }

        if (p->mochila[escolha - 1].ehArma == 1) {
            p->arma = p->mochila[escolha - 1];
        } else {
            p->roupa = p->mochila[escolha - 1];
        }
        printf("\n%sItem equipado!%s\n", COR_VERDE, COR_NORMAL);
        pausar();
    }
}

/* =========================================================================
 * CONFIGURACAO DAS FASES, TABULEIRO E EVENTOS
 * ========================================================================= */
void prepararFases(Fase fases[]) {
    int f, i;
    /* Mapa exato do desenho: 24 casas */
    TipoCasa mapaBase[TOTAL_CASAS] = {
        CASA_INICIO,    CASA_NADA,      CASA_LUTA,      CASA_LUTA,      CASA_ARMADILHA, CASA_CURA,
        CASA_NADA,      CASA_LUTA,      CASA_NADA,      CASA_ARMADILHA, CASA_LUTA,      CASA_BAU,
        CASA_ARMADILHA, CASA_NADA,      CASA_ARMADILHA, CASA_LUTA,      CASA_LUTA,      CASA_CURA,
        CASA_NADA,      CASA_LUTA,      CASA_LUTA,      CASA_BAU,       CASA_ARMADILHA, CASA_BOSS
    };

    for (f = 0; f < TOTAL_FASES; f++) {
        for (i = 0; i < TOTAL_CASAS; i++) {
            fases[f].casas[i] = mapaBase[i];
        }
    }

    /* ---------------- FASE 1 ---------------- */
    fases[0].numero = 1;
    strcpy(fases[0].titulo, "Caverna de Pratoles");
    strcpy(fases[0].reliquia, "Calice de Fridger");
    strcpy(fases[0].missao, "Derrote Gweny, Lider dos Elfos Negros.");
    fases[0].danoArmadilha = 5;
    strcpy(fases[0].textoArmadilha, "Voce caiu em um buraco!");

    /* Monstros Fase 1 */
    fases[0].monstros[0] = (Monstro){"Goblin", "Criatura com porrete", 3, 38, 38, 8, 4, 5, 4, 4, 2, 2, "Investida", 7, 20, 0};
    fases[0].monstros[1] = (Monstro){"Lobo Gigante", "Lobo feroz", 4, 52, 52, 10, 5, 11, 4, 4, 0, 2, "Mordida", 8, 20, 0};
    fases[0].monstros[2] = (Monstro){"Esqueleto", "Guerreiro morto-vivo", 5, 45, 45, 9, 6, 8, 5, 6, 0, 3, "Corte", 8, 20, 0};
    fases[0].boss        = (Monstro){"Gweny (Elfo Negro)", "Guardiao do Calice", 9, 140, 140, 14, 10, 12, 12, 12, 4, 7, "Esmagar", 14, 25, 0};

    /* ---------------- FASE 2 ---------------- */
    fases[1].numero = 2;
    strcpy(fases[1].titulo, "Deserto de Salinus");
    strcpy(fases[1].reliquia, "Po de Sodium");
    strcpy(fases[1].missao, "Derrote a Bruxa Bruhild no deserto.");
    fases[1].danoArmadilha = 12;
    strcpy(fases[1].textoArmadilha, "Foi atingido por uma tempestade de areia!");

    /* Monstros Fase 2 */
    fases[1].monstros[0] = (Monstro){"Ladrao de Tesouro", "Saqueador armado", 12, 85, 85, 14, 10, 12, 8, 10, 3, 8, "Tiroteio", 14, 20, 0};
    fases[1].monstros[1] = (Monstro){"Mumia", "Guarda das ruinas", 15, 95, 95, 15, 9, 9, 8, 13, 2, 6, "Arranhar", 15, 20, 0};
    fases[1].monstros[2] = (Monstro){"Golem de Pedra", "Gigante de rocha", 18, 115, 115, 17, 15, 6, 8, 10, 0, 6, "Pedregulho", 16, 20, 0};
    fases[1].boss        = (Monstro){"Bruxa Bruhild", "Feiticeira suprema", 22, 240, 240, 16, 15, 18, 20, 18, 4, 12, "Cremar", 18, 25, 1};

    /* ---------------- FASE 3 ---------------- */
    fases[2].numero = 3;
    strcpy(fases[2].titulo, "Montanha de Eggland");
    strcpy(fases[2].reliquia, "Ovo do Dragao Arcano");
    strcpy(fases[2].missao, "Derrote o Dragao Arcano no topo da montanha.");
    fases[2].danoArmadilha = 18;
    strcpy(fases[2].textoArmadilha, "Voce foi atingido por um deslizamento de pedras!");

    /* Monstros Fase 3 */
    fases[2].monstros[0] = (Monstro){"Dragarto", "Reptil guerreiro", 20, 135, 135, 21, 16, 22, 18, 16, 4, 12, "Perfurar", 20, 25, 0};
    fases[2].monstros[1] = (Monstro){"Feiticeiro", "Mago sombrio", 25, 125, 125, 14, 14, 18, 23, 20, 4, 12, "Rajada Elemental", 20, 25, 1};
    fases[2].monstros[2] = (Monstro){"Urso Coruja", "Fera colossal", 28, 155, 155, 23, 18, 16, 12, 14, 0, 11, "Rugido", 20, 25, 0};
    fases[2].boss        = (Monstro){"Dragao Arcano", "Guardiao do Ovo", 32, 390, 390, 28, 24, 25, 28, 24, 0, 18, "Incinerar", 30, 25, 0};
}

void mostrarTabuleiro(Fase *fase, Personagem *p, int posicao) {
    int linha, coluna, casaReal;

    limparTela();
    printf("%s==================================================================%s\n", COR_AMARELO, COR_NORMAL);
    printf(" FASE %d: %s | Missao: %s\n", fase->numero, fase->titulo, fase->missao);
    printf(" Heroi: %s (Nv %d) | Vida: %d/%d\n", p->nome, p->nivel, p->vida, p->vidaMax);
    printf("%s==================================================================%s\n\n", COR_AMARELO, COR_NORMAL);

    for (linha = 0; linha < LINHAS_TAB; linha++) {
        printf(" +---------+---------+---------+---------+---------+---------+\n |");
        for (coluna = 0; coluna < COLUNAS_TAB; coluna++) {
            /* Linhas pares vao da esquerda para direita; impares da direita para esquerda */
            if (linha % 2 == 0) {
                casaReal = linha * COLUNAS_TAB + coluna;
            } else {
                casaReal = linha * COLUNAS_TAB + (5 - coluna);
            }

            if (casaReal == posicao) {
                if (p->classe == GUERREIRO) {
                    printf(" %s[PEAO E]%s|", COR_LARANJA, COR_NORMAL);
                } else if (p->classe == MAGO) {
                    printf(" %s[PEAO B]%s|", COR_AZUL, COR_NORMAL);
                } else {
                    printf(" %s[PEAO M]%s|", COR_VERDE, COR_NORMAL);
                }
            } else {
                printf(" Casa %02d |", casaReal + 1);
            }
        }
        printf("\n |");

        for (coluna = 0; coluna < COLUNAS_TAB; coluna++) {
            if (linha % 2 == 0) {
                casaReal = linha * COLUNAS_TAB + coluna;
            } else {
                casaReal = linha * COLUNAS_TAB + (5 - coluna);
            }

            if (fase->casas[casaReal] == CASA_INICIO)         printf(" INICIO  |");
            else if (fase->casas[casaReal] == CASA_NADA)      printf("  NADA   |");
            else if (fase->casas[casaReal] == CASA_LUTA)      printf("  LUTA   |");
            else if (fase->casas[casaReal] == CASA_ARMADILHA) printf("ARMADILHA|");
            else if (fase->casas[casaReal] == CASA_CURA)      printf("  CURA   |");
            else if (fase->casas[casaReal] == CASA_BAU)       printf("   BAU   |");
            else if (fase->casas[casaReal] == CASA_BOSS)      printf("%s  BOSS   %s|", COR_VERMELHO, COR_NORMAL);
        }
        printf("\n");
    }
    printf(" +---------+---------+---------+---------+---------+---------+\n");
}

void moverHeroi(int *posicao, int passos) {
    *posicao = *posicao + passos;
    if (*posicao >= TOTAL_CASAS - 1) {
        *posicao = TOTAL_CASAS - 1;
    }
}

void abrirBau(Personagem *p, int numFase) {
    int sorteio = rolarDado(2);
    Item premio;

    if (numFase == 1) {
        if (p->classe == GUERREIRO) {
            if (sorteio == 1) premio = (Item){"Armadura de Metal", 0, 10, 5, 0};
            else              premio = (Item){"Espadao", 1, 0, 0, 15};
        } else if (p->classe == MAGO) {
            if (sorteio == 1) premio = (Item){"Tunica Real", 0, 9, 10, 0};
            else              premio = (Item){"Grimorio", 1, 0, 0, 15};
        } else {
            if (sorteio == 1) premio = (Item){"Armadura Real", 0, 10, 5, 0};
            else              premio = (Item){"Arco Real", 1, 0, 0, 15};
        }
    }
    else if (numFase == 2) {
        if (p->classe == GUERREIRO) {
            if (sorteio == 1) premio = (Item){"Armadura de Platina", 0, 15, 8, 0};
            else              premio = (Item){"Espada de Platina", 1, 0, 0, 20};
        } else if (p->classe == MAGO) {
            if (sorteio == 1) premio = (Item){"Tunica de Loki", 0, 13, 15, 0};
            else              premio = (Item){"Orb Elemental", 1, 0, 0, 20};
        } else {
            if (sorteio == 1) premio = (Item){"Capuz Feerico", 0, 15, 8, 0};
            else              premio = (Item){"Arco Elfico", 1, 0, 0, 20};
        }
    }
    else {
        if (p->classe == GUERREIRO) {
            if (sorteio == 1) premio = (Item){"Armadura Arcana", 0, 20, 12, 0};
            else              premio = (Item){"Espada Heroica", 1, 0, 0, 25};
        } else if (p->classe == MAGO) {
            if (sorteio == 1) premio = (Item){"Manto Arcano", 0, 18, 20, 0};
            else              premio = (Item){"Enchiridion", 1, 0, 0, 25};
        } else {
            if (sorteio == 1) premio = (Item){"Armadura Arcana", 0, 20, 12, 0};
            else              premio = (Item){"Arco Arcano", 1, 0, 0, 25};
        }
    }

    if (p->qtdItens < MAX_MOCHILA) {
        p->mochila[p->qtdItens] = premio;
        p->qtdItens++;
    }

    printf("\n%s[BAU!]%s Voce encontrou: %s%s%s!\n", COR_AMARELO, COR_NORMAL, COR_VERDE, premio.nome, COR_NORMAL);

    /* Equipa automaticamente se for melhor que o item atual */
    if (premio.ehArma == 1 && premio.bonusDano > p->arma.bonusDano) {
        p->arma = premio;
        printf("Voce equipou %s automaticamente!\n", premio.nome);
    } else if (premio.ehArma == 0 && premio.bonusDefesa > p->roupa.bonusDefesa) {
        p->roupa = premio;
        printf("Voce equipou %s automaticamente!\n", premio.nome);
    }
}

int executarCasa(Personagem *p, Fase *fase, int posicao) {
    TipoCasa tipo = fase->casas[posicao];

    if (tipo == CASA_INICIO || tipo == CASA_NADA) {
        printf("\nNada aconteceu nesta casa.\n");
        pausar();
        return 0;
    }
    else if (tipo == CASA_ARMADILHA) {
        printf("\n%s[ARMADILHA!]%s %s Perdeu %d de vida.\n",
               COR_VERMELHO, COR_NORMAL, fase->textoArmadilha, fase->danoArmadilha);
        levarDanoHeroi(p, fase->danoArmadilha);
        pausar();
        return 0;
    }
    else if (tipo == CASA_CURA) {
        printf("\n%s[CURA!]%s Todos merecem um descanso. Sua vida foi restaurada!\n", COR_VERDE, COR_NORMAL);
        curarHeroi(p, p->vidaMax);
        pausar();
        return 0;
    }
    else if (tipo == CASA_BAU) {
        abrirBau(p, fase->numero);
        pausar();
        return 0;
    }
    else if (tipo == CASA_LUTA) {
        printf("\n%s[BATALHA!]%s Inimigos apareceram!\n", COR_VERMELHO, COR_NORMAL);
        pausar();
        iniciarBatalha(p, fase, 0);
        return 0;
    }
    else if (tipo == CASA_BOSS) {
        printf("\n%s[CHEFE DA FASE!]%s Prepare-se para enfrentar %s!\n",
               COR_ROXO, COR_NORMAL, fase->boss.nome);
        pausar();
        return iniciarBatalha(p, fase, 1);
    }

    return 0;
}

/* =========================================================================
 * SISTEMA DE BATALHA POR TURNOS
 * ========================================================================= */
void levarDanoMonstro(Monstro *m, int dano) {
    m->vida = m->vida - dano;
    if (m->vida < 0) {
        m->vida = 0;
    }
}

void ajustarMonstroParaGrupo(Monstro *m, int qtdInimigos) {
    /* Se vierem 2 ou 3 monstros juntos, reduz proporcionalmente para equilibrar */
    if (qtdInimigos == 2) {
        m->vida = (int)ceil(m->vida * 0.75);
        m->vidaMax = m->vida;
        m->forca = (int)ceil(m->forca * 0.75);
        m->inteligencia = (int)ceil(m->inteligencia * 0.75);
        m->bonusDano = (int)ceil(m->bonusDano * 0.75);
    }
    else if (qtdInimigos == 3) {
        m->vida = (int)ceil(m->vida * 0.58);
        m->vidaMax = m->vida;
        m->forca = (int)ceil(m->forca * 0.58);
        m->inteligencia = (int)ceil(m->inteligencia * 0.58);
        m->bonusDano = (int)ceil(m->bonusDano * 0.58);
    }
}

int calcularDanoHeroi(Personagem *p, Monstro *m, int bonusHab, int *critico) {
    int d20 = rolarDado(20);
    int ataque;
    int dano;

    *critico = 0;
    if (rolarDado(100) <= p->chanceCritico) {
        *critico = 1;
    }

    if (p->classe == MAGO) {
        ataque = d20 + p->inteligencia + p->arma.bonusDano + bonusHab;
        if (*critico == 1) {
            ataque = (int)ceil(ataque * 1.5);
        }
        dano = ataque - m->resMagica;
    } else {
        ataque = d20 + p->forca + p->arma.bonusDano + bonusHab;
        if (*critico == 1) {
            ataque = (int)ceil(ataque * 1.5);
        }
        dano = ataque - (m->defesa + m->bonusDefesa);
    }

    if (dano < 1) {
        dano = 1;
    }
    return dano;
}

int calcularDanoMonstro(Monstro *m, Personagem *p, int usouHab, int *critico) {
    int d20 = rolarDado(20);
    int bonus = 0;
    int ataque;
    int dano;

    if (usouHab == 1) {
        bonus = m->danoHabilidade;
    }

    *critico = 0;
    if (rolarDado(100) <= m->chanceCritico) {
        *critico = 1;
    }

    if (m->ataqueMagico == 1) {
        ataque = d20 + m->inteligencia + m->bonusDano + bonus;
        if (*critico == 1) {
            ataque = (int)ceil(ataque * 1.3);
        }
        dano = ataque - (p->resMagica + p->roupa.bonusResMagica);
    } else {
        ataque = d20 + m->forca + m->bonusDano + bonus;
        if (*critico == 1) {
            ataque = (int)ceil(ataque * 1.3);
        }
        dano = ataque - (p->defesa + p->roupa.bonusDefesa);
    }

    if (dano < 1) {
        dano = 1;
    }
    return dano;
}

int iniciarBatalha(Personagem *p, Fase *fase, int ehBoss) {
    Monstro inimigos[MAX_INIMIGOS];
    int qtdInimigos;
    int i;
    int maiorAgiInimigo = 0;
    int turnoJogador;
    int defendendo = 0;
    int acao, alvo, hab, dano, critico, vivos, curaDef;

    if (ehBoss == 1) {
        qtdInimigos = 1;
        inimigos[0] = fase->boss;
    } else {
        qtdInimigos = rolarDado(3);
        for (i = 0; i < qtdInimigos; i++) {
            inimigos[i] = fase->monstros[rand() % 3];
            ajustarMonstroParaGrupo(&inimigos[i], qtdInimigos);
        }
    }

    /* Verifica quem tem maior agilidade para comecar */
    for (i = 0; i < qtdInimigos; i++) {
        if (inimigos[i].agilidade > maiorAgiInimigo) {
            maiorAgiInimigo = inimigos[i].agilidade;
        }
    }

    if (p->agilidade >= maiorAgiInimigo) {
        turnoJogador = 1;
    } else {
        turnoJogador = 0;
    }

    p->recargaHabilidade = 0;

    while (1) {
        /* Conta quantos inimigos ainda estao vivos */
        vivos = 0;
        for (i = 0; i < qtdInimigos; i++) {
            if (inimigos[i].vida > 0) {
                vivos++;
            }
        }

        if (vivos == 0) {
            limparTela();
            printf("%s=== VITORIA NA BATALHA! ===%s\n", COR_VERDE, COR_NORMAL);
            subirNivel(p);
            pausar();
            return 1;
        }

        if (p->vida <= 0) {
            return 0;
        }

        if (turnoJogador == 1) {
            defendendo = 0;
            limparTela();
            printf("%s=== TELA DE BATALHA ===%s\n\n", COR_VERMELHO, COR_NORMAL);
            printf("%s (Nv %d) - Vida: %d / %d\n\n", p->nome, p->nivel, p->vida, p->vidaMax);

            printf("Inimigos:\n");
            for (i = 0; i < qtdInimigos; i++) {
                if (inimigos[i].vida > 0) {
                    printf("  [%d] %s (Nv %d) - Vida: %d / %d\n",
                           i + 1, inimigos[i].nome, inimigos[i].nivel, inimigos[i].vida, inimigos[i].vidaMax);
                } else {
                    printf("  [%d] %s [DERROTADO]\n", i + 1, inimigos[i].nome);
                }
            }

            printf("\n[1] Lutar\n");
            printf("[2] Habilidade\n");
            printf("[3] Defender\n");
            printf("[4] Fugir\n");

            acao = lerOpcao(1, 4);

            if (acao == ACAO_FUGIR) {
                if (ehBoss == 1) {
                    printf("\nVoce nao pode fugir do chefe!\n");
                    pausar();
                } else {
                    printf("\nVoce fugiu da batalha sem subir de nivel.\n");
                    pausar();
                    return 0;
                }
            }
            else if (acao == ACAO_DEFENDER) {
                defendendo = 1;
                curaDef = (int)ceil(p->vidaMax * 0.10);
                curarHeroi(p, curaDef);
                if (p->recargaHabilidade > 0) {
                    p->recargaHabilidade--;
                }
                printf("\nVoce se defendeu! Recuperou +%d de vida e reduzira o proximo dano pela metade.\n", curaDef);
                turnoJogador = 0;
                pausar();
            }
            else if (acao == ACAO_LUTAR || acao == ACAO_HABILIDADE) {
                hab = -1;
                if (acao == ACAO_HABILIDADE) {
                    if (p->recargaHabilidade > 0) {
                        printf("\nAguarde %d turno(s) para usar habilidade novamente!\n", p->recargaHabilidade);
                        pausar();
                        continue;
                    }
                    printf("\nEscolha a habilidade:\n");
                    for (i = 0; i < 4; i++) {
                        if (p->nivel >= p->nivelHabilidades[i]) {
                            printf("  [%d] %s (+%d dano)\n", i + 1, p->nomeHabilidades[i], p->danoHabilidades[i]);
                        }
                    }
                    printf("  [0] Cancelar\n");
                    hab = lerOpcao(0, 4) - 1;
                    if (hab < 0 || p->nivel < p->nivelHabilidades[hab]) {
                        continue;
                    }
                }

                /* Escolhe alvo se houver mais de 1 inimigo */
                if (qtdInimigos == 1) {
                    alvo = 0;
                } else {
                    printf("\nQual inimigo deseja atacar (1 a %d)? ", qtdInimigos);
                    alvo = lerOpcao(1, qtdInimigos) - 1;
                    if (inimigos[alvo].vida <= 0) {
                        printf("Esse inimigo ja foi derrotado!\n");
                        pausar();
                        continue;
                    }
                }

                if (hab >= 0) {
                    dano = calcularDanoHeroi(p, &inimigos[alvo], p->danoHabilidades[hab], &critico);
                    levarDanoMonstro(&inimigos[alvo], dano);
                    printf("\nVoce usou %s em %s!", p->nomeHabilidades[hab], inimigos[alvo].nome);
                    if (hab == 1 || hab == 2) p->recargaHabilidade = 1;
                    if (hab == 3) p->recargaHabilidade = 2;
                } else {
                    dano = calcularDanoHeroi(p, &inimigos[alvo], 0, &critico);
                    levarDanoMonstro(&inimigos[alvo], dano);
                    printf("\nVoce atacou %s com %s!", inimigos[alvo].nome, p->arma.nome);
                    if (p->recargaHabilidade > 0) {
                        p->recargaHabilidade--;
                    }
                }

                if (critico == 1) {
                    printf(" %sACERTO CRITICO!%s", COR_AMARELO, COR_NORMAL);
                }
                printf(" Causou %s%d de dano%s!\n", COR_VERDE, dano, COR_NORMAL);

                turnoJogador = 0;
                pausar();
            }
        }
        else {
            /* Turno dos inimigos */
            limparTela();
            printf("%s=== TURNO DOS INIMIGOS ===%s\n\n", COR_VERMELHO, COR_NORMAL);

            for (i = 0; i < qtdInimigos; i++) {
                if (inimigos[i].vida > 0 && p->vida > 0) {
                    int usouHab = (rolarDado(100) <= 25) ? 1 : 0;
                    dano = calcularDanoMonstro(&inimigos[i], p, usouHab, &critico);

                    if (defendendo == 1) {
                        dano = (int)ceil(dano * 0.5);
                        if (dano < 1) dano = 1;
                    }

                    levarDanoHeroi(p, dano);

                    if (usouHab == 1) {
                        printf("- %s usou %s! ", inimigos[i].nome, inimigos[i].nomeHabilidade);
                    } else {
                        printf("- %s atacou voce! ", inimigos[i].nome);
                    }

                    if (critico == 1) {
                        printf("%sCRITICO!%s ", COR_AMARELO, COR_NORMAL);
                    }
                    printf("Voce perdeu %s%d de vida%s (Vida: %d/%d).\n",
                           COR_VERMELHO, dano, COR_NORMAL, p->vida, p->vidaMax);
                }
            }

            turnoJogador = 1;
            pausar();
        }
    }
}
