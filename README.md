# CPU Scheduling Algorithm Simulator (SO-CLI)

## 📋 Descrição do Projeto

**SO-CLI** é um simulador interativo de algoritmos de escalonamento de CPU desenvolvido em **C** com interface de usuário via terminal (ncurses). O projeto implementa e compara diferentes estratégias de escalonamento de processos em tempo real, exibindo gráficos de Gantt, métricas detalhadas e análises quantitativas.

Este projeto foi desenvolvido como trabalho acadêmico para a disciplina de **Sistemas Operacionais**, sob orientação do **Prof. Maycon Leone M. Peixoto**, com foco em educação e visualização de conceitos fundamentais de escalonamento de processos.

---

## 🎯 Características Principais

### Algoritmos de Escalonamento Implementados

1. **FIFO (First In, First Out)**

   - Escalonamento não-preemptivo
   - Processa na ordem de chegada
   - Simples, mas pode resultar em espera longa

2. **SJF (Shortest Job First)**

   - Escalonamento não-preemptivo
   - Prioriza processos com menor tempo de execução
   - Minimiza tempo médio de espera

3. **EDF (Earliest Deadline First)**

   - Escalonamento preemptivo
   - Prioriza processos com deadline mais próximo
   - Ideal para sistemas com restrições de tempo real
   - Implementa **overhead (troca de contexto)** e **quantum**

4. **Round Robin (RR)**

   - Escalonamento preemptivo com quantum
   - Cada processo recebe tempo máximo (quantum)
   - Implementa overhead para trocas de contexto
   - Garante equidade no uso da CPU

5. **CFS (Completely Fair Scheduler)**
   - Escalonamento preemptivo baseado em tempo virtual
   - Usa **vruntime** (virtual runtime) para seleção de processos
   - Prioridades influenciam o cálculo de vruntime
   - Implementa overhead (troca de contexto)
   - Oferece escalonamento justo em sistemas multiprocessador

### Visualizações e Dados

- **Gráfico de Gantt Interativo**: Visualização da execução dos processos ao longo do tempo
- **Tabela de Métricas**: 10 métricas por processo em formato tabular com bordas
- **Resumo Quantitativo**: Estatísticas agregadas da simulação
- **Controles Interativos**: Navegação pelo gráfico, seleção de algoritmos, edição de parâmetros
- **Visualização de Memória**: Visualização de RAM e DISK com estados de paging
- **Detecção de Page Faults**: Indicador visual de faltas de página durante execução

---

## 💾 Sistema de Memória Virtual

O projeto implementa um **sistema de memória virtual com paginação**, permitindo simular faltas de página (page faults) e políticas de substituição de página:

### Características de Memória

- **Memória Configurável**: Ativar/desativar sistema de memória na tela de configuração
- **50 Frames de RAM**: Cada frame com 4 KB (total de 200 KB)
- **Paginação por Processo**: Cada processo tem páginas configuráveis (1-10 páginas)
- **Page Faults**: Simulação de faltas de página quando processo não está em RAM
- **Políticas de Substituição**:
  - **FIFO**: Remove a página carregada há mais tempo
  - **LRU (Least Recently Used)**: Remove a página menos utilizada recentemente

### Visualização de Memória

- **Grade RAM**: 50 frames dispostos em 5x10
- **Visualização de DISK**: Mostra processos não carregados em RAM
- **Animação de Paging**: Navegação temporal através dos estados de memória
- **Estatísticas**: Contagem de page faults por processo
- **Indicadores de Execução**: Cores indicam processos executando vs. aguardando

### Métricas por Processo (Tabela)

| Métrica         | Descrição                                              |
| --------------- | ------------------------------------------------------ |
| **Chegada**     | Tempo de chegada do processo na fila                   |
| **Execução**    | Tempo total de execução necessário                     |
| **Deadline**    | Prazo (relativo à chegada)                             |
| **Prioridade**  | Nível de prioridade (1-10)                             |
| **Início(s)**   | Instante em que a execução começou                     |
| **Término**     | Instante em que a execução terminou                    |
| **Espera**      | Tempo total que o processo ficou aguardando            |
| **Turnaround**  | Tempo total desde chegada até término                  |
| **Deadline OK** | Indicador se respeitou o deadline (EDF apenas)         |
| **PgFaults**    | Número de faltas de página (quando memória habilitada) |

### Resumo Quantitativo

- **Média de Execução**: Tempo médio de execução
- **Média de Espera**: Tempo médio em fila de espera
- **Média de Turnaround**: Tempo médio total desde chegada até fim
- **Throughput**: Quantidade de processos completados por unidade de tempo
- **Ociosidade (%)**: Percentual do tempo que a CPU ficou ociosa
- **Trocas de Contexto**: Total de trocas forçadas (contadas apenas quando há overhead)

---

## 🛠️ Requisitos de Sistema

### Dependências

- **GCC** (compilador C) ou Clang
- **Make** (sistema de build)
- **NCurses** (biblioteca de interface de terminal)
- **Linux/Unix** (ou subsistema WSL no Windows)

### Instalação de Dependências

#### Ubuntu/Debian

```bash
sudo apt-get install build-essential libncurses-dev
```

#### Fedora/RHEL

```bash
sudo dnf install gcc make ncurses-devel
```

#### macOS

```bash
brew install gcc ncurses
```

#### Windows (WSL2)

```bash
wsl --install
# Dentro do WSL:
sudo apt-get install build-essential libncurses-dev
```

---

## 🚀 Como Compilar e Executar

### Compilação

```bash
# Navegar até o diretório do projeto
cd /caminho/para/so-cli

# Compilar o projeto
make clean
make
```

### Execução

```bash
# Executar o simulador
./scheduler
```

---

## 📖 Como Usar

### Tela Inicial (Menu de Configuração)

1. **Start Simulation**: Inicia a simulação com as configurações atuais
2. **Configure Processes**: Edita o número e parâmetros dos processos
3. **Set Quantum & Overhead**: Ajusta quantum (para RR/EDF) e tempo de overhead
4. **Exit**: Sai do programa

### Configurar Processos

- Use `UP/DOWN` para navegar entre processos
- Pressione `ENTER` para editar o processo selecionado
- `A`: Adicionar novo processo
- `D`: Deletar processo
- `M`: Voltar ao menu principal

### Simulação (Tela Principal)

#### Atalhos de Teclado

| Tecla              | Ação                                       |
| ------------------ | ------------------------------------------ |
| `F1`               | Selecionar algoritmo FIFO                  |
| `F2`               | Selecionar algoritmo SJF                   |
| `F3`               | Selecionar algoritmo EDF                   |
| `F4`               | Selecionar algoritmo Round Robin           |
| `F5`               | Selecionar algoritmo CFS                   |
| `SPACE`            | Executar/Resetar simulação                 |
| `→` / `←`          | Avançar/Recuar no tempo                    |
| `< / >` ou `, / .` | Navegar animação de memória                |
| `/`                | Alternar modo follow (memória segue tempo) |
| `A` / `D`          | Scroll esquerda/direita no gráfico         |
| `H`                | Ir para o início (Home)                    |
| `E`                | Ir para o final (End)                      |
| `M`                | Menu de configuração                       |
| `Q`                | Sair do programa                           |

#### Seções da Interface

1. **Título**: Exibe o algoritmo selecionado
2. **Gráfico de Gantt**: Visualização das execuções com scroll horizontal
3. **Legenda**: Cores e símbolos utilizados
4. **Controles**: Lista de atalhos disponíveis
5. **Tabela de Métricas**: Resultados detalhados por processo
6. **Resumo Quantitativo**: Estatísticas da simulação
7. **Visualização de Memória**: RAM e DISK (quando habilitado)
8. **Indicador de Tempo Atual**: Tempo em que você está visualizando

### Cores no Gráfico de Gantt

| Cor          | Significado                         |
| ------------ | ----------------------------------- |
| 🟩 Verde     | Processo executando                 |
| 🟨 Amarelo   | Processo aguardando na fila         |
| 🟥 Vermelho  | Overhead (troca de contexto)        |
| 🔵 Cyan      | Page Fault (falta de página)        |
| ⬜ Branco    | Deadline não respeitado (EDF)       |
| Cinza        | Não chegou ou completado            |
| `\|` (traço) | Marcador de deadline absoluto (EDF) |
| `F`          | Indicador de falha de página        |

---

## 📁 Estrutura de Arquivos

```
so-cli/
├── Makefile                 # Script de compilação
├── README.md               # Documentação básica
├── main.c                  # Ponto de entrada do programa
│
├── globals.h / globals.c   # Variáveis e estruturas globais
│   ├── Process struct
│   ├── ProcessState enum (estados de processo)
│   ├── ProcessStatus enum (status final)
│   ├── MetricsIndex enum (índices de métricas)
│   ├── SummaryStats struct (estatísticas)
│
├── scheduler.h / scheduler.c  # Lógica de escalonamento
│   ├── execute_fifo()       # Algoritmo FIFO
│   ├── execute_sjf()        # Algoritmo SJF
│   ├── execute_edf()        # Algoritmo EDF
│   ├── execute_rr()         # Algoritmo Round Robin
│   ├── execute_cfs()        # Algoritmo CFS
│   ├── run_current_algorithm()
│   ├── reset_simulation()
│   └── initialize_default_processes()
│
├── ui.h / ui.c            # Interface de usuário (ncurses)
│   ├── draw_gantt_chart()  # Renderiza o gráfico de Gantt
│   ├── draw_legend()       # Desenha legenda de cores
│   ├── draw_interface()    # Renderiza tela principal
│   ├── draw_memory_visualization()  # Visualiza RAM e DISK
│   └── show_screen_size_error()
│
├── config_ui.h / config_ui.c  # Interface de configuração
│   ├── show_main_menu()       # Menu principal
│   ├── edit_process_screen()  # Edição de processos
│   ├── get_int_input()        # Entrada de números
│   └── show_configuration_screen()
│
├── memory.h / memory.c    # Sistema de memória virtual
│   ├── init_memory_system()       # Inicializa memória
│   ├── check_page_fault()         # Detecta faltas de página
│   ├── load_page_to_ram()         # Carrega página em RAM
│   ├── find_victim_frame_fifo()   # Substitui página FIFO
│   ├── find_victim_frame_lru()    # Substitui página LRU
│   └── save_memory_state()        # Registra estado para animação
│
├── metrics_utils.h / metrics_utils.c  # Utilitários de métricas
│   ├── compute_metrics_for_all()  # Calcula métricas por processo
│   └── compute_summary_stats()    # Calcula resumo quantitativo
│
├── screen_utils.h / screen_utils.c  # Utilitários de tela
│   ├── update_screen_size()  # Atualiza dimensões da tela
│   ├── is_screen_too_small()  # Valida tamanho mínimo
│   ├── show_screen_size_error()  # Erro de tamanho insuficiente
│   └── msleep()  # Função para delay
│
└── .gitignore              # Arquivos ignorados pelo Git
```

---

## 🔧 Parâmetros Configuráveis

### Ao Adicionar Processos

- **Arrival Time**: Quando o processo chega na fila (0 a TOTAL_TIME-1)
- **Execution Time**: Tempo total de CPU necessário (1 a TOTAL_TIME)
- **Deadline**: Prazo relativo (tempo máximo permitido desde chegada)
- **Priority**: Nível de prioridade (1-10, afeta escalonamento CFS)
- **Pages**: Número de páginas do processo (1-10, para sistema de memória)

### Configuração Global

- **Total Time**: Duração total da simulação (padrão: 100)
- **Quantum**: Fatia de tempo por processo em RR/EDF (padrão: 2)
- **Overhead**: Tempo de troca de contexto (padrão: 1)
- **Max Processes**: Máximo de 6 processos
- **Memory**: Habilita/desabilita sistema de memória virtual (padrão: OFF)
- **Memory Policy**: Política de substituição de página - FIFO ou LRU (padrão: FIFO)

---

## 💡 Exemplos de Uso

### Exemplo 1: Comparar FIFO vs SJF

1. Abra o programa: `./scheduler`
2. No menu, clique em **Configure Processes**
3. Adicione 4 processos com tempos variados:
   - P1: Arrival=0, Execution=8
   - P2: Arrival=1, Execution=4
   - P3: Arrival=2, Execution=2
   - P4: Arrival=3, Execution=6
4. Clique **Start Simulation**
5. Pressione `F1` para FIFO e `SPACE` para executar
6. Observe o Gráfico de Gantt e as métricas
7. Pressione `F2` para SJF e `SPACE` novamente
8. Compare os resultados na tabela de métricas

### Exemplo 2: Testar EDF com Deadlines Críticos

1. Configure processos com deadlines próximos:
   - P1: Arrival=0, Execution=3, Deadline=5
   - P2: Arrival=1, Execution=2, Deadline=4
2. Pressione `F3` para EDF
3. Configure Quantum=2 e Overhead=1
4. Execute e observe a métrica **deadline_ok** na tabela

---

## 📐 Algoritmos em Detalhes

### FIFO

```
1. Manter fila de processos prontos
2. Selecionar processo que chegou primeiro
3. Executar até completar
4. Passar para próximo processo
```

**Vantagem**: Simples de implementar
**Desvantagem**: Pode deixar processos curtos esperando por longos tempos

### SJF

```
1. Entre todos os processos prontos
2. Selecionar o com menor tempo de execução restante
3. Executar até completar
4. Repetir
```

**Vantagem**: Minimiza tempo médio de espera
**Desvantagem**: Favorece processos curtos (inanição possível)

### EDF

```
1. Entre todos os processos prontos
2. Selecionar o com deadline mais próximo
3. Executar por no máximo quantum unidades de tempo
4. Se não completar:
   - Adicionar overhead (troca de contexto)
   - Recolocar na fila
5. Repetir
```

**Vantagem**: Ótimo para sistemas de tempo real
**Desvantagem**: Complexidade maior, pode falhar se sistema sobrecarregado

### Round Robin (RR)

```
1. Manter fila circular de processos
2. Executar primeiro processo por quantum unidades
3. Se não completar:
   - Adicionar overhead
   - Enviar para final da fila
4. Se completar: remover da fila
5. Repetir
```

**Vantagem**: Justo, boa responsividade
**Desvantagem**: Overhead aumenta com muitas trocas

### CFS (Completely Fair Scheduler)

```
1. Calcular vruntime (virtual runtime) de cada processo
2. vruntime = vruntime + tempo_executado * peso_prioridade
3. Entre todos os prontos, selecionar com menor vruntime
4. Se preempção necessária:
   - Adicionar overhead
   - Recolocar na fila
5. Repetir
```

**Vantagem**: Escalonamento justo baseado em tempo virtual, adapta-se a prioridades
**Desvantagem**: Complexidade maior, overhead afeta latência

## 📈 Cenários de Teste Recomendados

### Teste 1: Variação de Tempos de Execução

Processos com tempos muito diferentes para observar comportamento dos algoritmos.

### Teste 2: Deadlines Críticos (EDF)

Processos com deadlines muito próximos para visualizar falhas e trocas de contexto.

### Teste 3: Quantum Reduzido (RR)

Quantum pequeno para ver mais trocas de contexto e overhead.

### Teste 4: Chegadas Escalonadas

Processos chegando em momentos diferentes para análise de fila de espera.

### Teste 5: Teste de Memória com Page Faults

Habilitar memória e criar processos com múltiplas páginas para observar faltas de página e substituição.

### Teste 6: Comparação de Políticas de Substituição

Testar FIFO vs LRU para entender diferenças em comportamento de substituição de página.

---

## 🧪 Requisitos de Tela

- **Largura mínima**: 125 colunas
- **Altura mínima**: 45 linhas
- **TOTAL_TIME mínimo**: 15 unidades de tempo

_Nota: Se a tela for muito pequena, o simulador exibirá mensagem de erro._

Este projeto foi desenvolvido pelos alunos **Erick Peneluca**, **Gabriel Cury**, **Natan Mendes** e **Mario Roque** como trabalho acadêmico da disciplina de **Sistemas Operacionais**.

**Orientador**: Prof. Maycon Leone M. Peixoto

---

**Última atualização**: Novembro de 2025
