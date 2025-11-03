# Conversor de AFN em AFD Utilizando Arquivos JFLAP

## Alunos: Rafaela Oliveira e João Pedro Conte

---

### Introdução

Este trabalho tem como objetivo apresentar um conversor em C de arquivos JFLAP de Autômatos Finitos Não Deterministicos (AFN) em Autômatos Finitos Deterministicos (AFD) ou, nas siglas em inglês, que irá encontrar ao longo do código, DFA e NFA, na ordem.
Basicamente a ideia principal do código foi entender como funciona um arquivo JFLAP, oude foi verificado que o mesmo tipo de arquivo é um arquivo do tipo .xml e após isso entender o mesmo, obter os dados do mesmo e fazer a covnersão dos dados, imprimindo novamente outro .xml. Abaixo está um exemplo do tipo de arquivo do JFLAP

```xml
<?xml version="1.0" encoding="UTF-8" standalone="no"?><!--Created with JFLAP 7.1.--><structure>
	<type>fa</type>
	<automaton>
		<!--The list of states.-->
		<state id="0" name="q0">
			<x>35.0</x>
			<y>85.0</y>
			<initial/>
		</state>
		<state id="1" name="q1">
			<x>190.0</x>
			<y>91.0</y>
		</state>
		<state id="2" name="q2">
			<x>386.0</x>
			<y>88.0</y>
			<final/>
		</state>
		<!--The list of transitions.-->
		<transition>
			<from>1</from>
			<to>0</to>
			<read>b</read>
		</transition>
		<transition>
			<from>0</from>
			<to>1</to>
			<read>a</read>
		</transition>
		<transition>
			<from>1</from>
			<to>2</to>
			<read>b</read>
		</transition>
		<transition>
			<from>2</from>
			<to>1</to>
			<read>a</read>
		</transition>
	</automaton>
</structure>
```

### Como Rodar o Projeto

Para rodar o projeto é bem simples, dentro da pasta ./test há 5 arquivos de exemplo de teste, entre eles há dois com erro para testar se as partes de erro estão corretos também, antes devemos compilar o código com o comando abaixo:

```bash
gcc -Wall -Wextra -std=c11 -I./src -o main main.c src/state_set.c src/jflap_parser.c src/converter.c src/reporter.c
```

Após isso um arquvio ./main irá ser gerado e o seguinte comando deve ser dado:

```bash
./main <arquivo_de_test>.jff
```

Se quiser utilizar algum arquivo de teste disponiblizado pode ser utilizado o comando abaixo

```bash
./main ./test/test.jff
```

### Estrutura do Projeto

As pastas do projeto foram estruturadas da seguinte forma:

```
nfa_dfa_converter_project
│   README.md
│   main.c
│   .gitignore
└───output
└───src
│   │   automata.h
│   │   converter.c
│   │   converter.h
│   │   jflap_parser.c
│   │   jflap_parser.h
│   │   reporter.c
│   │   reporter.h
│   │   state_set.c
│   │   state_set.h
└───test
    │   test.jff
    │   test2.jff
    │   test3.jff
    │   test4_error.jff
    │   test5_error.jff
```

Onde podemos encontrar todos arquivos necessários para rodar o mesmo.

### Funções e Estruturas

Neste ponto será apresentado o que está dentro de cada função do projeto, onde irá ser visto a ordem correta de apresentar o mesmo.

#### ARQUIVO: main.c

Ponto de entrada principal do programa. Ele orquestra o fluxo de execução:

1.  Valida os argumentos de entrada.
2.  Chama o 'parser' (analisador) para ler o arquivo JFLAP e carregar o AFN.
3.  Chama o 'reporter' (relator) para imprimir o AFN lido.
4.  Chama o 'converter' (conversor) para executar o algoritmo AFN -> AFD.
5.  Chama o 'reporter' novamente para imprimir e salvar o AFD resultante.

```c
#include "automata.h"
#include "jflap_parser.h"
#include "converter.h"
#include "reporter.h"

// --- Função Principal ---
int main(int argc, char *argv[])
{
    // Imprime um banner de boas-vindas no console.
    printf("\n------------- NFA → DFA Converter With JFLAP Files .jff -------------\n\n");

    // --- Validação de Entrada ---
    // 'argc' (argument count) deve ser 2: o nome do programa e o nome do arquivo.
    if (argc != 2)
    {
        fprintf(stderr, "Input: %s <file.jff>\n", argv[0]);
        return 1; // Retorna 1 para indicar um erro.
    }

    // Pega o nome do arquivo do segundo argumento ('argv[1]').
    const char *filename = argv[1];

    // Procura o último '.' no nome do arquivo para verificar a extensão.
    const char *extension = strrchr(filename, '.');
    if (!extension || strcmp(extension, ".jff") != 0)
    {
        fprintf(stderr, "Error: File must be JFLAP (.jff) format.\n");
        return 1;
    }

    // --- Execução Principal ---

    // Declara a estrutura do AFN que será preenchida.
    NFA nfa;
    // Tenta ler o arquivo. Se 'jflap_read_file' retornar 'false', houve um erro.
    if (!jflap_read_file(filename, &nfa))
    {
        fprintf(stderr, "Error: Can not read JFLAP file.\n");
        return 1;
    }

    // Se a leitura foi bem-sucedida, imprime o AFN lido no console.
    nfa_print(&nfa);

    // Declara a estrutura do AFD que será preenchida pela conversão.
    DFA dfa;
    // Executa o algoritmo de conversão.
    int convert_result = nfa_to_dfa_convert(&nfa, &dfa);
    if (convert_result == 0)
    {
        fprintf(stderr, "Error: Conversion from NFA to DFA failed.\n");
        return 0;
    }

    // Imprime o AFD resultante no console.
    dfa_print(&dfa, &nfa); // Passa o NFA junto para obter os nomes dos estados.

    // Salva o AFD em dois formatos de arquivo diferentes.
    dfa_save_txt(&dfa, &nfa, "./output/result.txt");
    dfa_save_jflap(&dfa, &nfa, "./output/result.jff");

    // Imprime a mensagem final de sucesso.
    printf("\n\nThe convertion was be finished!\n");
    printf("Files:\n");
    printf("./output/result.txt - Description\n");
    printf("./output/result.jff - JFLAP File\n");

    return 0; // Retorna 0 para indicar sucesso.
}
```

#### ARQUIVO: automata.h

Cabeçalho central do projeto. Define todas as constantes globais, bibliotecas padrão e as principais estruturas de dados (structs) que representam os autômatos (NFA, DFA) e seus componentes.

```c
#ifndef AUTOMATA_H // "Header Guard" para evitar inclusão dupla
#define AUTOMATA_H

// --- Inclusões de Bibliotecas Padrão ---
#include <stdio.h>   // Para entrada/saída (printf, FILE, fopen, etc.)
#include <stdlib.h>  // Para alocação de memória (malloc, exit) e conversão (atoi)
#include <string.h>  // Para manipulação de strings (strcmp, strstr, strcpy, etc.)
#include <stdbool.h> // Para usar os tipos 'bool', 'true' e 'false'
#include <ctype.h>   // Para funções de caracteres (isspace)

// --- Constantes Configuráveis ---
#define MAX_STATES 50            // Limite de estados do NFA
#define MAX_ALPHABET 26          // Limite de símbolos do alfabeto
#define MAX_STATE_NAME_LEN 32    // Comprimento máximo do nome de um estado (ex: "q0")
#define MAX_LINE_LEN 1024        // Tamanho máximo da linha lida do arquivo
#define MAX_TEMP_TRANSITIONS 500 // Limite de transições lidas do JFLAP
#define MAX_DFA_STATES 256       // Limite de estados gerados no DFA

// --- Estruturas de Dados ---

// Representa um conjunto de estados do NFA.
// Esta é a base para um estado do DFA.
typedef struct
{
    // Um array de booleanos. Se states[i] == true, o estado 'i' pertence ao conjunto.
    bool states[MAX_STATES];
    // Contador de quantos estados estão no conjunto (para otimização).
    int size;
} StateSet;

// Estrutura temporária usada apenas durante a leitura do JFLAP
// para armazenar transições antes de processá-las.
typedef struct
{
    int from_id; // ID original do estado de origem (do JFLAP)
    int to_id;   // ID original do estado de destino (do JFLAP)
    char symbol; // O símbolo (char) que dispara a transição
} TempTransition;

// Estrutura principal do Autômato Finito Não-Determinístico (NFA)
typedef struct
{
    int num_states;        // Número total de estados
    int num_symbols;       // Número total de símbolos no alfabeto
    int start_state_index; // Índice (interno) do estado inicial

    // Array de booleanos. Se final_states[i] == true, o estado 'i' é final.
    bool final_states[MAX_STATES];

    // Tabela de transição do NFA: [estado_origem][simbolo] -> Conjunto de estados de destino
    StateSet transitions[MAX_STATES][MAX_ALPHABET];

    // O alfabeto (array de caracteres)
    char alphabet[MAX_ALPHABET];

    // Nomes originais dos estados (ex: "q0", "q1") lidos do JFLAP
    char state_names[MAX_STATES][MAX_STATE_NAME_LEN];

    // Mapeia o ID do JFLAP (que pode ser não sequencial) para nosso índice interno (0, 1, 2...)
    int id_to_index_map[MAX_STATES * 2]; // Tamanho extra por segurança
} NFA;

// Estrutura principal do Autômato Finito Determinístico (DFA)
typedef struct
{
    int num_states;        // Número total de estados gerados
    int num_symbols;       // Número de símbolos (o mesmo do NFA)
    int start_state_index; // Índice (interno) do estado inicial do DFA

    // Array de booleanos para os estados finais do DFA.
    bool final_states[MAX_DFA_STATES];

    // Tabela de transição do DFA: [estado_origem][simbolo] -> UM estado de destino
    // Armazena o *índice* do estado de destino. -1 significa sem transição (estado de erro).
    int transitions[MAX_DFA_STATES][MAX_ALPHABET];

    // O alfabeto (copiado do NFA).
    char alphabet[MAX_ALPHABET];

    // Mapeia cada estado do DFA (índice 0, 1, 2...) ao seu
    // conjunto de estados do NFA correspondente.
    // Ex: dfa_states[0] pode ser o StateSet {q0}
    // Ex: dfa_states[1] pode ser o StateSet {q0, q1}
    StateSet dfa_states[MAX_DFA_STATES];
} DFA;

#endif // AUTOMATA_H
```

#### Módulo JFLAP Parser (jflap_parser.h / jflap_parser.c)

Este módulo define a interface pública (`jflap_parser.h`) e implementa a lógica (`jflap_parser.c`) para ler e analisar (fazer o _parsing_) de arquivos no formato `.jff` (JFLAP).

Sua única responsabilidade é abrir um arquivo `.jff` e carregar seus dados (estados, transições, alfabeto) para dentro da estrutura `NFA` (AFN) definida no projeto.

**COMO FUNCIONA (A Implementação em `jflap_parser.c`):**

Como os arquivos JFLAP (`.jff`) são baseados em XML, a implementação "disseca" o arquivo de forma manual, buscando por tags e atributos específicos (como `<state>`, `<transition>`, `<from>`, `<to>`, `<read>`, etc.) para extrair as informações e preencher corretamente a estrutura `NFA`.

`jflap_parser.h`

```c
#ifndef JFLAP_PARSER_H
#define JFLAP_PARSER_H

#include "automata.h" // Precisa da definição de NFA

// --- Protótipo da Função Pública ---

// Lê o arquivo JFLAP (.jff) e preenche a estrutura NFA.
// Retorna 'true' em caso de sucesso e 'false' em caso de erro.
bool jflap_read_file(const char *filename, NFA *nfa);

#endif // JFLAP_PARSER_H
```

`jflap_parser.c`

```c
#include "jflap_parser.h" // Inclui seu próprio cabeçalho
#include "state_set.h"    // Precisa das funções de 'state_set'

// --- Funções Auxiliares Estáticas ---
// (static = visível apenas dentro deste arquivo)

// Inicializa a estrutura NFA com valores padrão (zeros e -1).
static void nfa_init(NFA *nfa)
{
    nfa->num_states = 0;
    nfa->num_symbols = 0;
    nfa->start_state_index = -1;
    // Preenche os arrays com valores padrão.
    memset(nfa->final_states, false, sizeof(nfa->final_states));
    memset(nfa->alphabet, 0, sizeof(nfa->alphabet));
    // -1 indica um ID não mapeado.
    for (int i = 0; i < MAX_STATES * 2; i++)
        nfa->id_to_index_map[i] = -1;

    // Inicializa a tabela de transição (matriz de conjuntos de estados).
    for (int i = 0; i < MAX_STATES; i++)
    {
        for (int j = 0; j < MAX_ALPHABET; j++)
        {
            state_set_init(&nfa->transitions[i][j]);
        }
    }
}

// Extrai o valor de um atributo XML (ex: de 'id="0"' extrai "0").
static char *extract_attribute(const char *line, const char *attr, char *buffer)
{
    char search_str[50];
    sprintf(search_str, "%s=\"", attr); // Cria a string de busca (ex: "id=\"")

    char *start = strstr(line, search_str); // Encontra a string de busca na linha
    if (!start)
        return NULL; // Atributo não encontrado

    start += strlen(search_str);    // Avança o ponteiro para depois de '="'
    char *end = strchr(start, '"'); // Encontra a aspa de fechamento
    if (!end)
        return NULL; // Linha mal formatada

    // Copia o conteúdo entre 'start' e 'end' para o 'buffer'.
    strncpy(buffer, start, end - start);
    buffer[end - start] = '\0'; // Adiciona o terminador nulo.
    return buffer;
}

// Extrai o valor entre tags XML (ex: de '<from>1</from>' extrai "1").
static char *extract_tag_value(const char *line, const char *tag, char *buffer)
{
    char start_tag[50], end_tag[50];
    sprintf(start_tag, "<%s>", tag); // ex: "<from>"
    sprintf(end_tag, "</%s>", tag);  // ex: "</from>"

    char *start = strstr(line, start_tag); // Encontra a tag de abertura
    if (!start)
        return NULL;
    start += strlen(start_tag); // Avança para depois da tag

    char *end = strstr(start, end_tag); // Encontra a tag de fechamento
    if (!end)
        return NULL;

    // Copia o conteúdo para o buffer.
    strncpy(buffer, start, end - start);
    buffer[end - start] = '\0';

    // --- Limpeza de Espaços em Branco (Trim) ---
    // Remove espaços no início
    char *p = buffer;
    while (*p && isspace((unsigned char)*p))
        p++;
    memmove(buffer, p, strlen(p) + 1); // Desloca a string para a esquerda

    // Remove espaços no fim
    char *end_p = buffer + strlen(buffer) - 1;
    while (end_p > buffer && isspace((unsigned char)*end_p))
        *end_p-- = '\0';

    return buffer;
}

// --- Função Pública ---

bool jflap_read_file(const char *filename, NFA *nfa)
{
    // Tenta abrir o arquivo para leitura ("r" = read).
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("ERROR when open file"); // 'perror' imprime o erro do sistema
        return false;
    }

    printf("Reading JFLAP file: %s\n", filename);
    nfa_init(nfa); // Prepara a estrutura NFA.

    // Array temporário para guardar as transições lidas.
    TempTransition temp_transitions[MAX_TEMP_TRANSITIONS];
    int num_temp_transitions = 0;

    char line[MAX_LINE_LEN]; // Buffer para ler cada linha
    int current_id = -1;     // ID do estado sendo processado no momento

    // --- 1ª PASSAGEM: Ler Estados ---
    // Lê o arquivo linha por linha para encontrar estados, seus nomes, ID,
    // e se são iniciais ou finais.
    while (fgets(line, sizeof(line), file))
    {
        char id_buffer[20], name_buffer[MAX_STATE_NAME_LEN];

        // Se a linha contém "<state id" ...
        if (strstr(line, "<state id"))
        {
            // Validação de segurança: verifica limite de estados.
            if (nfa->num_states >= MAX_STATES)
            {
                fprintf(stderr, "ERROR: Quantity of state extends than %d.\n", MAX_STATES);
                fclose(file);
                return false;
            }
            // Extrai o ID e o NOME do estado.
            extract_attribute(line, "id", id_buffer);
            extract_attribute(line, "name", name_buffer);
            current_id = atoi(id_buffer); // Converte ID de string para inteiro.

            // Validação de segurança: verifica se o ID é muito grande.
            if (current_id >= MAX_STATES * 2)
            {
                fprintf(stderr, "ERROR: State id '%d' is very lengthful.\n", current_id);
                fclose(file);
                return false;
            }

            // Mapeia o ID (ex: 5) para o índice interno (ex: 2).
            nfa->id_to_index_map[current_id] = nfa->num_states;
            // Armazena o nome do estado.
            strcpy(nfa->state_names[nfa->num_states], name_buffer);
            nfa->num_states++; // Incrementa o contador de estados.
        }
        // Se a linha contém "<initial/>" (e estamos dentro de um estado)
        if (strstr(line, "<initial/>") && current_id != -1)
        {
            nfa->start_state_index = nfa->id_to_index_map[current_id];
        }
        // Se a linha contém "<final/>" (e estamos dentro de um estado)
        if (strstr(line, "<final/>") && current_id != -1)
        {
            nfa->final_states[nfa->id_to_index_map[current_id]] = true;
        }
    }

    // Imprime NFA lidas (para depuração e demonstração).
    // for (int i = 0; i < nfa->num_states; i++)
    // {
    //     printf("State ID: %d, Name: %s, Index: %d, Is Final: %s\n",
    //            i,
    //            nfa->state_names[i],
    //            i,
    //            nfa->final_states[i] ? "Yes" : "No");
    // }
    // printf("Found states: %d\n", nfa->num_states);

    // --- 2ª PASSAGEM: Ler Transições ---
    rewind(file);               // Volta ao início do arquivo.
    bool in_transition = false; // Flag: estamos dentro de uma tag <transition>?
    int from = -1, to = -1;
    char read = '\0';

    while (fgets(line, sizeof(line), file))
    {
        // Detecta o início de um bloco de transição.
        if (strstr(line, "<transition>"))
        {
            in_transition = true;
            from = -1;
            to = -1;
            read = '\0'; // Reseta as variáveis
            continue;
        }
        // Detecta o fim de um bloco de transição.
        if (strstr(line, "</transition>"))
        {
            in_transition = false;
            // Se lemos todas as partes (from, to, read), armazena temporariamente.
            if (from != -1 && to != -1 && read != '\0')
            {
                // Validação de segurança: limite de transições.
                if (num_temp_transitions >= MAX_TEMP_TRANSITIONS)
                {
                    fprintf(stderr, "ERRO: Number of transitions exceeds the limit of %d.\n", MAX_TEMP_TRANSITIONS);
                    fclose(file);
                    return false;
                }
                temp_transitions[num_temp_transitions++] = (TempTransition){from, to, read};
            }
            continue;
        }
        // Se estivermos dentro de um bloco <transition>...
        if (in_transition)
        {
            char value_buffer[MAX_LINE_LEN];
            // Tenta extrair <from>, <to> e <read>.
            if (extract_tag_value(line, "from", value_buffer))
                from = atoi(value_buffer);
            if (extract_tag_value(line, "to", value_buffer))
                to = atoi(value_buffer);
            if (extract_tag_value(line, "read", value_buffer))
                read = value_buffer[0]; // Pega apenas o primeiro caractere.
        }
    }
    fclose(file); // Fecha o arquivo.

    // Imprime as transições lidas (para depuração e demonstração).
    // for (int i = 0; i < num_temp_transitions; i++)
    // {
    //     printf("Transition: from ID %d to ID %d on '%c'\n",
    //            temp_transitions[i].from_id,
    //            temp_transitions[i].to_id,
    //            temp_transitions[i].symbol);
    // }

    // --- 3ª ETAPA: Processar Transições Temporárias ---
    // Constrói o alfabeto e a tabela de transições final do NFA.
    for (int i = 0; i < num_temp_transitions; i++)
    {
        char symbol = temp_transitions[i].symbol;
        int symbol_index = -1;

        // Procura o símbolo no alfabeto.
        for (int j = 0; j < nfa->num_symbols; j++)
        {
            if (nfa->alphabet[j] == symbol)
            {
                symbol_index = j;
                break;
            }
        }
        // Se o símbolo é novo, adiciona ao alfabeto.
        if (symbol_index == -1)
        {
            // Validação de segurança: limite do alfabeto.
            if (nfa->num_symbols >= MAX_ALPHABET)
            {
                fprintf(stderr, "ERROR: Quantity of symbols exceeds the limit of %d.\n", MAX_ALPHABET);
                return false;
            }
            symbol_index = nfa->num_symbols++;
            nfa->alphabet[symbol_index] = symbol;
        }

        // Converte os IDs de 'from' e 'to' para nossos índices internos.
        int from_index = nfa->id_to_index_map[temp_transitions[i].from_id];
        int to_index = nfa->id_to_index_map[temp_transitions[i].to_id];

        // Se os índices são válidos, adiciona a transição na tabela.
        if (from_index != -1 && to_index != -1)
        {
            // Adiciona o estado 'to_index' ao conjunto de destino.
            state_set_add(&nfa->transitions[from_index][symbol_index], to_index);
        }
    }

    // Imprime o alfabeto extraído (para depuração e demonstração).
    // printf("Extract alphabet: ");
    // for (int i = 0; i < nfa->num_symbols; i++)
    //     printf("%c ", nfa->alphabet[i]);

    printf("JFLAP file read successfully!\n\n");
    return true;
}
```

#### Módulo StateSet (state_set.h / state_set.c)

**PROPÓSITO:**

Este módulo define a interface pública (`state_set.h`) e implementa (`state_set.c`) um conjunto de funções auxiliares essenciais para a manipulação da estrutura `StateSet` (Conjunto de Estados).

**COMO FUNCIONA (A Implementação em `state_set.c`):**
Sua finalidade é fornecer as ferramentas básicas para criar, comparar e gerenciar coleções de estados, que são fundamentais para o algoritmo de conversão AFN -> AFD.

`state_set.h`

```c
#ifndef STATE_SET_H
#define STATE_SET_H

#include "automata.h" // Precisa da definição de StateSet e NFA

// --- Protótipos das Funções ---

// Inicializa um conjunto de estados como vazio.
void state_set_init(StateSet *set);

// Adiciona um estado (por seu índice) a um conjunto.
void state_set_add(StateSet *set, int state_index);

// Compara dois conjuntos de estados. Retorna 'true' se forem idênticos.
bool state_set_equals(StateSet *set1, StateSet *set2);

// Adiciona todos os estados do conjunto 'source' (origem) ao 'dest' (destino).
void state_set_union(StateSet *dest, StateSet *source);

// Cria uma string formatada (ex: "{q0,q1}") para um conjunto de estados.
void state_set_sprint_names(char *buffer, StateSet *set, NFA *nfa);

#endif // STATE_SET_H
```

`state_set.c`

```c
#include "state_set.h"

// Inicializa um conjunto de estados, definindo todos os estados como 'false'
// e o tamanho como 0. 'memset' é uma função rápida para preencher memória.
void state_set_init(StateSet *set)
{
    memset(set->states, false, sizeof(set->states));
    set->size = 0;
}

// Adiciona um estado a um conjunto.
void state_set_add(StateSet *set, int state_index)
{
    // Verifica se o estado já não está no conjunto antes de adicionar.
    if (!set->states[state_index])
    {
        set->states[state_index] = true;
        set->size++; // Incrementa o contador de tamanho.
    }
}

// Compara dois conjuntos de estados.
bool state_set_equals(StateSet *set1, StateSet *set2)
{
    // 'memcmp' compara blocos de memória. É a forma mais rápida de
    // verificar se os dois arrays 'states' são idênticos.
    return memcmp(set1->states, set2->states, sizeof(set1->states)) == 0;
}

// Realiza a operação de união (destino = destino U origem).
void state_set_union(StateSet *dest, StateSet *source)
{
    // Itera por todos os estados possíveis.
    for (int i = 0; i < MAX_STATES; i++)
    {
        // Se o estado 'i' existe no conjunto de origem...
        if (source->states[i])
        {
            // ...adiciona-o ao conjunto de destino.
            // A função 'state_set_add' já lida com duplicatas.
            state_set_add(dest, i);
        }
    }
}

// Cria uma string legível para um conjunto de estados (ex: "{q0,q1}").
void state_set_sprint_names(char *buffer, StateSet *set, NFA *nfa)
{
    // Se o conjunto estiver vazio, usa o símbolo de conjunto vazio.
    if (set->size == 0)
    {
        strcpy(buffer, "∅");
        return;
    }

    // Inicia a string com '{'.
    strcpy(buffer, "{");
    bool is_first = true; // Flag para controlar a adição de vírgulas.

    // Itera por todos os estados do NFA.
    for (int i = 0; i < nfa->num_states; i++)
    {
        // Se o estado 'i' pertence a este conjunto...
        if (set->states[i])
        {
            // Adiciona uma vírgula antes de todos, exceto do primeiro.
            if (!is_first)
                strcat(buffer, ",");

            // Adiciona o nome do estado (ex: "q0") à string.
            strcat(buffer, nfa->state_names[i]);
            is_first = false; // Os próximos estados não são o primeiro.
        }
    }
    // Fecha a string com '}'.
    strcat(buffer, "}");
}
```

#### Módulo Converter (converter.h / converter.c)

Este módulo define a interface pública (`converter.h`) e implementa a lógica central (`converter.c`) para a conversão de um Autômato Finito Não Determinístico (AFN) em um Autômato Finito Determinístico (AFD). É o "cérebro" do conversor.

**COMO FUNCIONA (A Implementação em `converter.c`):**

1.  Cria o estado inicial do AFD, que é o conjunto contendo o estado inicial do AFN.
2.  Usa uma fila para processar novos estados do AFD (que são "conjuntos de estados" do AFN).
3.  Para cada estado (conjunto) na fila e para cada símbolo do alfabeto:
    a. Calcula o conjunto de destino: a união de todas as transições possíveis do AFN a partir dos estados do conjunto atual, para o símbolo atual.
    b. Se esse conjunto de destino for novo (ainda não está no AFD), ele é adicionado ao AFD e à fila de processamento.
    c. A transição (estado atual, símbolo) -> (conjunto de destino) é registrada na tabela do AFD.
4.  O processo é repetido até que a fila de processamento fique vazia.
5.  Ao final, os estados finais do AFD são definidos: qualquer estado (conjunto) do AFD que contenha pelo menos um estado final do AFN original é marcado como final.

`converter.h`

```c
#ifndef CONVERTER_H
#define CONVERTER_H

#include "automata.h" // Precisa das definições de NFA e DFA

// --- Protótipo da Função Pública ---

// Executa o algoritmo de Construção de Subconjuntos.
// Recebe o NFA (leitura) e preenche a estrutura DFA (escrita).
int nfa_to_dfa_convert(NFA *nfa, DFA *dfa);

#endif // CONVERTER_H
```

`converter.c`

```c
#include "converter.h" // Inclui seu próprio cabeçalho
#include "state_set.h" // Precisa das funções de manipulação de conjuntos

// --- Função Pública ---

int nfa_to_dfa_convert(NFA *nfa, DFA *dfa)
{
    printf("\n\n--- Starting conversion NFA → DFA ---\n");

    // --- VALIDAÇÃO BÁSICA DO NFA ---
    // Verifica se o NFA tem estado inicial e pelo menos um estado final.
    if (nfa->start_state_index == -1)
    {
        fprintf(stderr, "ERROR: NFA has no initial state defined.\n");
        return 0;
    }
    // Verifica se há pelo menos um estado final.
    bool has_final = false;
    // Percorre todos os estados do NFA.
    for (int i = 0; i < nfa->num_states; i++)
    {
        if (nfa->final_states[i])
        {
            has_final = true;
            break;
        }
    }
    // Se não houver estado final, aborta a conversão.
    if (!has_final)
    {
        fprintf(stderr, "ERROR: NFA has no final states defined.\n");
        return 0;
    }

    // Verifica se há somente um estado inicial
    int initial_state_count = 0;
    // Percorre todos os estados do NFA.
    for (int i = 0; i < nfa->num_states; i++)
    {
        // Se o estado 'i' for o estado inicial...
        if (i == nfa->start_state_index)
        {
            // ...incrementa o contador de estados iniciais.
            initial_state_count++;
        }
    }
    // Se houver mais de um estado inicial, aborta a conversão.
    if (initial_state_count != 1)
    {
        fprintf(stderr, "ERROR: NFA must have exactly one initial state.\n");
        return 0;
    }

    // --- PASSO 1: Inicializar o DFA ---
    dfa->num_states = 0;
    dfa->num_symbols = nfa->num_symbols;
    // Copia o alfabeto do NFA para o DFA.
    memcpy(dfa->alphabet, nfa->alphabet, sizeof(nfa->alphabet));

    // Fila de processamento. Armazena os índices dos estados do DFA a processar.
    int processing_queue[MAX_DFA_STATES];
    int queue_start = 0; // Cabeça da fila
    int queue_end = 0;   // Cauda da fila

    // --- PASSO 2: Criar o Estado Inicial do DFA ---
    // O estado inicial do DFA é o conjunto contendo apenas o estado inicial do NFA.
    state_set_init(&dfa->dfa_states[0]);
    state_set_add(&dfa->dfa_states[0], nfa->start_state_index);

    dfa->start_state_index = 0; // O primeiro estado (índice 0) é o inicial.
    dfa->num_states = 1;        // O DFA agora tem 1 estado.

    // Adiciona o estado inicial (índice 0) à fila de processamento.
    processing_queue[queue_end++] = 0;

    // --- PASSO 3: Loop Principal do Algoritmo ---
    // Processa enquanto a fila não estiver vazia.
    while (queue_start < queue_end)
    {
        // Printa a conversão atual
        printf("Current DFA States (%d total):\n", dfa->num_states);
        for (int i = 0; i < dfa->num_states; i++)
        {
            char state_names_buffer[256];
            state_set_sprint_names(state_names_buffer, &dfa->dfa_states[i], nfa);
            printf("DFA State %d: %s\n", i, state_names_buffer);
        }
        printf("\n");

        // Pega o próximo índice de estado do DFA da fila.
        int current_dfa_state_index = processing_queue[queue_start++];
        // Obtém o conjunto de estados do NFA correspondente a este índice.
        StateSet *current_state_set = &dfa->dfa_states[current_dfa_state_index];

        // Para cada símbolo do alfabeto...
        for (int s = 0; s < nfa->num_symbols; s++)
        {
            StateSet next_state_set; // O conjunto de destino
            state_set_init(&next_state_set);

            // --- PASSO 3a: Calcular o conjunto de destino ---
            // Para cada estado 'e' no conjunto atual...
            for (int e = 0; e < nfa->num_states; e++)
            {
                if (current_state_set->states[e])
                {
                    // ...faz a união das suas transições com o conjunto de destino.
                    state_set_union(&next_state_set, &nfa->transitions[e][s]);
                }
            }

            // Se o conjunto de destino for vazio (sem transição)...
            if (next_state_set.size == 0)
            {
                dfa->transitions[current_dfa_state_index][s] = -1; // -1 = sem transição
                continue;                                          // Próximo símbolo
            }

            // --- PASSO 3b: Verificar se este conjunto já existe no DFA ---
            int next_dfa_state_index = -1;
            for (int i = 0; i < dfa->num_states; i++)
            {
                if (state_set_equals(&next_state_set, &dfa->dfa_states[i]))
                {
                    next_dfa_state_index = i;
                    break;
                }
            }

            // --- PASSO 3c: Se for um novo estado... ---
            if (next_dfa_state_index == -1)
            {
                // Validação de segurança: limite de estados do DFA.
                if (dfa->num_states >= MAX_DFA_STATES)
                {
                    fprintf(stderr, "ERROR: Number of DFA states exceeds the limit of %d.\n", MAX_DFA_STATES);
                    return 0; // Aborta a conversão
                }

                // Adiciona o novo estado ao DFA.
                next_dfa_state_index = dfa->num_states++;
                dfa->dfa_states[next_dfa_state_index] = next_state_set;

                // Adiciona o índice do novo estado à fila para processamento.
                processing_queue[queue_end++] = next_dfa_state_index;
            }

            // --- PASSO 3d: Registrar a transição no DFA ---
            dfa->transitions[current_dfa_state_index][s] = next_dfa_state_index;
        }
    }

    // --- PASSO 4: Definir os Estados Finais do DFA ---
    // Um estado do DFA é final se seu conjunto de estados do NFA
    // contém pelo menos um estado final do NFA.
    for (int i = 0; i < dfa->num_states; i++)
    {
        dfa->final_states[i] = false; // Assume que não é final
        for (int j = 0; j < nfa->num_states; j++)
        {
            // Se o estado 'j' (do NFA) está no conjunto 'i' (do DFA)
            // E o estado 'j' (do NFA) é final...
            if (dfa->dfa_states[i].states[j] && nfa->final_states[j])
            {
                dfa->final_states[i] = true; // ...então o estado 'i' (do DFA) é final.
                break;                       // Otimização: já sabemos que é final.
            }
        }
    }
    printf("--- Conversion completed! DFA with %d states generated. ---\n", dfa->num_states);

    return 1;
}
```

#### Módulo Reporter (reporter.h / reporter.c)

Este módulo define a interface pública (`reporter.h`) e implementa (`reporter.c`) todas as funções de geração de relatórios e saída de dados.

**COMO FUNCIONA (A Implementação em `reporter.c`):**
Sua responsabilidade é pegar as estruturas de autômatos (AFN ou AFD) e apresentá-las ao usuário de duas formas principais:

- Imprimir o autômato de forma legível no console.
- Salvar o autômato resultante em arquivos de saída (como `.txt` para visualização simples e `.jff` para reabrir no JFLAP).

`reporter.h`

```c
#ifndef REPORTER_H
#define REPORTER_H

#include "automata.h" // Precisa das definições de NFA e DFA

// --- Protótipos das Funções Públicas ---

// Imprime uma tabela formatada do NFA no console.
void nfa_print(NFA *nfa);

// Imprime uma tabela formatada do DFA no console.
// Requer o NFA para traduzir os conjuntos de estados em nomes.
void dfa_print(DFA *dfa, NFA *nfa);

// Salva um relatório de texto detalhado do DFA em um arquivo.
void dfa_save_txt(DFA *dfa, NFA *nfa, const char *filename);

// Salva o DFA resultante em um novo arquivo .jff, pronto para ser
// aberto no JFLAP.
void dfa_save_jflap(DFA *dfa, NFA *nfa, const char *filename);

#endif // REPORTER_H
```

`reporter.c`

```c
#include "reporter.h"  // Inclui seu próprio cabeçalho
#include "state_set.h" // Precisa de 'state_set_sprint_names'

// --- Funções Públicas ---

// Imprime o NFA lido.
void nfa_print(NFA *nfa)
{
    printf("\nNFA Readed:\n");
    printf("Initial State: %s\n", nfa->state_names[nfa->start_state_index]);
    printf("Final States: ");
    bool is_first = true;
    for (int i = 0; i < nfa->num_states; i++)
    {
        if (nfa->final_states[i])
        {
            if (!is_first)
                printf(", ");
            printf("%s", nfa->state_names[i]);
            is_first = false;
        }
    }
    printf("\n\t\t Transition Table\t\n");
    printf("==================================================\n");
    char buffer[256]; // Buffer para nomes de conjuntos

    // Imprime cabeçalho da tabela (símbolos)
    printf("||\t%s\t||", "State");
    for (int i = 0; i < nfa->num_symbols; i++)
        printf("\t%c\t||", nfa->alphabet[i]);
    printf("\n");

    // Imprime linhas da tabela (uma para cada estado)
    for (int i = 0; i < nfa->num_states; i++)
    {
        // Adiciona prefixos "->" (inicial) e "*" (final)
        char prefix[5] = "  ";
        if (i == nfa->start_state_index)
            strcpy(prefix, "->");
        if (nfa->final_states[i])
            strcat(prefix, "*");
        else
            strcat(prefix, " ");

        // Imprime o nome do estado
        printf("||\t%s %s\t||\t", prefix, nfa->state_names[i]);

        // Imprime as transições para cada símbolo
        for (int j = 0; j < nfa->num_symbols; j++)
        {
            // Gera a string do conjunto de destino (ex: "{q1,q2}")
            state_set_sprint_names(buffer, &nfa->transitions[i][j], nfa);
            printf("%s\t||\t", buffer); // Imprime com alinhamento
        }
        printf("\n");
    }
}

// Imprime o DFA resultante.
void dfa_print(DFA *dfa, NFA *nfa)
{
    char buffer[512]; // Buffer para nomes de conjuntos (estados do DFA)
    printf("\nDFA Resulted:\n");

    // Imprime estado inicial
    state_set_sprint_names(buffer, &dfa->dfa_states[dfa->start_state_index], nfa);
    printf("Initial State: %s\n", buffer);

    // Imprime estados finais
    printf("Final States: ");
    bool is_first = true;
    for (int i = 0; i < dfa->num_states; i++)
    {
        if (dfa->final_states[i])
        {
            if (!is_first)
                printf(", ");
            state_set_sprint_names(buffer, &dfa->dfa_states[i], nfa);
            printf("%s", buffer);
            is_first = false;
        }
    }

    printf("\n\t\t Transition Table\t\n");
    printf("==================================================\n");

    // Imprime cabeçalho da tabela (símbolos)

    printf("||   %s\t||", "State");
    for (int i = 0; i < dfa->num_symbols; i++)
        printf("\t%c\t||", dfa->alphabet[i]);
    printf("\n");

    // Imprime linhas da tabela (uma para cada estado do DFA)
    for (int i = 0; i < dfa->num_states; i++)
    {
        // Adiciona prefixos "->" (inicial) e "*" (final)
        char prefix[5] = "  ";
        if (i == nfa->start_state_index)
            strcpy(prefix, "->");
        if (nfa->final_states[i])
            strcat(prefix, "*");
        else
            strcat(prefix, " ");

        // Imprime o nome do estado (ex: "{q0,q1}")
        state_set_sprint_names(buffer, &dfa->dfa_states[i], nfa);
        printf("||  %s%s\t||", prefix, buffer);

        // Imprime as transições para cada símbolo
        for (int j = 0; j < dfa->num_symbols; j++)
        {
            int dest_index = dfa->transitions[i][j];
            if (dest_index != -1)
            {
                // Gera a string do conjunto de destino
                state_set_sprint_names(buffer, &dfa->dfa_states[dest_index], nfa);
                printf("\t%s\t||", buffer);
            }
            else
            {
                printf("\t%s\t||", "∅"); // Sem transição
            }
        }
        printf("\n");
    }
}

// Salva o relatório em .txt.
void dfa_save_txt(DFA *dfa, NFA *nfa, const char *filename)
{
    FILE *f = fopen(filename, "w"); // Abre o arquivo para escrita ("w" = write)
    if (!f)
    {
        perror("ERROR when saving .txt file");
        return;
    }

    char buffer[512]; // Buffer para nomes de estados

    fprintf(f, "DFA Report\n");
    fprintf(f, "=============================\n\n");

    // Salva estado inicial
    state_set_sprint_names(buffer, &dfa->dfa_states[dfa->start_state_index], nfa);
    fprintf(f, "Initial State: %s\n\n", buffer);

    // Salva lista de estados finais
    fprintf(f, "Final States:\n");
    for (int i = 0; i < dfa->num_states; i++)
    {
        if (dfa->final_states[i])
        {
            state_set_sprint_names(buffer, &dfa->dfa_states[i], nfa);
            fprintf(f, "  - %s\n", buffer);
        }
    }

    // Imprime a tabela de transições detalhada
    fprintf(f, "\nDetailed Transition Table:\n");
    fprintf(f, "==================================================\n");
    // Imprime cabeçalho da tabela (símbolos)
    fprintf(f, "||     %s      ||", "State");
    for (int i = 0; i < dfa->num_symbols; i++)
        fprintf(f, "      %c      ||", dfa->alphabet[i]);
    fprintf(f, "\n");

    // Imprime linhas da tabela (uma para cada estado do DFA)
    for (int i = 0; i < dfa->num_states; i++)
    {
        // Adiciona prefixos "->" (inicial) e "*" (final)
        char prefix[5] = "  ";
        if (i == nfa->start_state_index)
            strcpy(prefix, "->");
        if (nfa->final_states[i])
            strcat(prefix, "*");
        else
            strcat(prefix, " ");

        // Imprime o nome do estado (ex: "{q0,q1}")
        state_set_sprint_names(buffer, &dfa->dfa_states[i], nfa);
        fprintf(f, "||  %s%s\t\t", prefix, buffer);
        if(i == 0){
            fprintf(f, "\t\t||");
        }else{
            fprintf(f, "||");
        }

        // Imprime as transições para cada símbolo
        for (int j = 0; j < dfa->num_symbols; j++)
        {
            int dest_index = dfa->transitions[i][j];
            if (dest_index != -1)
            {
                // Gera a string do conjunto de destino
                state_set_sprint_names(buffer, &dfa->dfa_states[dest_index], nfa);
                fprintf(f, "\t\t%s\t ", buffer);
                if(buffer[3] == ',' && i == 1){
                    fprintf(f, "||");
                }else{
                    fprintf(f, "\t||");
                }
            }
            else
            {
                fprintf(f, "\t%s\t||", "∅"); // Sem transição
            }
        }
        fprintf(f, "\n");
    }

    fclose(f); // Fecha o arquivo
}

// Salva o relatório em .jff.
void dfa_save_jflap(DFA *dfa, NFA *nfa, const char *filename)
{
    FILE *f = fopen(filename, "w");
    if (!f)
    {
        perror("ERROR when saving .jff file");
        return;
    }

    char buffer[512]; // Buffer para nomes de estados

    // --- Escreve o Cabeçalho XML do JFLAP ---
    fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
    fprintf(f, "\n");
    fprintf(f, "<structure>\n\t<type>fa</type>\n\t<automaton>\n");

    // --- Escreve os Estados ---
    fprintf(f, "\t\t\n");
    int x = 50, y = 100; // Posições X/Y para layout visual no JFLAP
    for (int i = 0; i < dfa->num_states; i++)
    {
        // O nome do estado no JFLAP será o nosso conjunto (ex: "{q0,q1}")
        state_set_sprint_names(buffer, &dfa->dfa_states[i], nfa);
        fprintf(f, "\t\t<state id=\"%d\" name=\"%s\">\n", i, buffer);
        fprintf(f, "\t\t\t<x>%.1f</x>\n\t\t\t<y>%.1f</y>\n", (float)x, (float)y);

        // Adiciona tags <initial/> e <final/> se necessário
        if (i == dfa->start_state_index)
            fprintf(f, "\t\t\t<initial/>\n");
        if (dfa->final_states[i])
            fprintf(f, "\t\t\t<final/>\n");

        fprintf(f, "\t\t</state>\n");

        // Atualiza as coordenadas para o próximo estado
        x += 180;    // Move para a direita
        if (x > 800) // Se passar da borda, quebra a linha
        {
            x = 50;
            y += 100;
        }
    }

    // --- Escreve as Transições ---
    fprintf(f, "\t\t\n");
    for (int i = 0; i < dfa->num_states; i++) // Para cada estado de origem
    {
        for (int j = 0; j < dfa->num_symbols; j++) // Para cada símbolo
        {
            int dest_index = dfa->transitions[i][j];
            // Se houver uma transição (não for -1)...
            if (dest_index != -1)
            {
                fprintf(f, "\t\t<transition>\n");
                fprintf(f, "\t\t\t<from>%d</from>\n", i);                // ID de origem
                fprintf(f, "\t\t\t<to>%d</to>\n", dest_index);           // ID de destino
                fprintf(f, "\t\t\t<read>%c</read>\n", dfa->alphabet[j]); // Símbolo
                fprintf(f, "\t\t</transition>\n");
            }
        }
    }

    // --- Fecha o XML ---
    fprintf(f, "\t</automaton>\n</structure>\n");
    fclose(f); // Fecha o arquivo
}
```

### Conclusão
A principal dificuldade encontrada no trabalho foi entender a forma que é feita a conversão de um autômato não deterministico para um deterministico e implementar a lógica no código, podemos concluir que o trabalho demonstrou com clareza como funciona a conversão de um tipo de autômato para outro e pode ser entendido com profundidade a ideia.