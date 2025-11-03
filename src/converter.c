// =============================================================================
// ARQUIVO: converter.c
//
// PROPÓSITO:
//    Implementa o algoritmo de Construção de Subconjuntos (Subset Construction).
//    Este é o "cérebro" do conversor.
//
// COMO FUNCIONA:
//    1. Cria o estado inicial do DFA: o conjunto {estado inicial do NFA}.
//    2. Usa uma fila para processar novos estados do DFA ("conjuntos de estados").
//    3. Para cada estado (conjunto) na fila e cada símbolo do alfabeto:
//       a. Calcula o conjunto de destino (a união de todas as transições
//          possíveis do NFA a partir dos estados do conjunto atual).
//       b. Se esse conjunto de destino for novo, adiciona-o ao DFA e à fila.
//       c. Registra a transição na tabela do DFA.
//    4. Repete até a fila ficar vazia.
//    5. Marca os estados finais do DFA (aqueles que contêm um estado final do NFA).
// =============================================================================

#include "converter.h" // Inclui seu próprio cabeçalho
#include "state_set.h" // Precisa das funções de manipulação de conjuntos

// --- Função Pública ---

bool nfa_to_dfa_convert(NFA *nfa, DFA *dfa)
{
    printf("\n\n--- Starting conversion NFA → DFA ---\n");

    // --- VALIDAÇÃO BÁSICA DO NFA ---
    // Verifica se o NFA tem estado inicial e pelo menos um estado final.
    if (nfa->start_state_index == -1)
    {
        fprintf(stderr, "ERROR: NFA has no initial state defined.\n");
        return false;
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
        return false;
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
        return false;
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
                    return false; // Aborta a conversão
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

    return true;
}