# Mini Compilador para Linguagem de Expressões Funcionais

Um mini compilador completo desenvolvido em C++ que realiza análise léxica, sintática, semântica e geração de código intermediário para uma linguagem de expressões matemáticas com suporte a funções definidas pelo usuário.

## 📋 Índice

- [Descrição](#descrição)
- [Características](#características)
- [Requisitos](#requisitos)
- [Compilação](#compilação)
- [Uso](#uso)
- [Gramática da Linguagem](#gramática-da-linguagem)
- [Tabela Sintática](#tabela-sintática)
- [Exemplos](#exemplos)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Casos de Teste](#casos-de-teste)

## 📝 Descrição

Este projeto implementa um compilador completo que processa uma linguagem de expressões funcionais chamada `.neto`. O compilador realiza as seguintes fases:

1. **Análise Léxica**: Identifica tokens (palavras-chave, identificadores, números, operadores)
2. **Análise Sintática**: Verifica a estrutura gramatical e constrói uma AST (Árvore Sintática Abstrata)
3. **Análise Semântica**: Verifica escopo, declarações e chamadas de função
4. **Geração de Código**: Produz código intermediário em formato de três endereços

## ✨ Características

- ✅ Suporte a declaração de funções com parâmetros
- ✅ Variáveis locais e atribuições
- ✅ Operadores aritméticos: `+`, `-`, `*`, `/`, `^` (potenciação)
- ✅ Números inteiros e de ponto flutuante
- ✅ Chamadas de função aninhadas
- ✅ Escopo simples (variáveis locais por função)
- ✅ Verificação semântica completa
- ✅ Geração de código intermediário (três endereços)
- ✅ Tratamento de erros léxicos, sintáticos e semânticos

## 🔧 Requisitos

- **Compilador C++**: GCC ou Clang com suporte a C++17
- **Flex**: Versão 2.6 ou superior
- **Bison**: Versão 3.0 ou superior
- **Make**: Para build automation
- **Sistema Operacional**: Linux, macOS ou Windows (com WSL)

### Instalação das Dependências

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential flex bison
```

**macOS:**
```bash
brew install flex bison
```

**Fedora:**
```bash
sudo dnf install gcc-c++ flex bison
```

## 🏗️ Compilação

Para compilar o projeto, execute:

```bash
make
```

Isso irá:
1. Gerar os arquivos do Bison (`parser.tab.cc`, `parser.tab.hh`)
2. Gerar os arquivos do Flex (`lex.yy.cc`)
3. Compilar todos os arquivos fonte
4. Linkar o executável `compiler`

Para limpar os arquivos gerados:

```bash
make clean
```

## 🚀 Uso

### Compilação de um arquivo `.neto`

```bash
./compiler arquivo.neto
```

### Modo Verboso

Para ver informações detalhadas sobre cada fase da compilação:

```bash
./compiler -v arquivo.neto
```

### Exemplo

```bash
./compiler examples/valid1.neto
```

O compilador irá:
- Analisar o código fonte
- Verificar erros léxicos, sintáticos e semânticos
- Gerar código intermediário em `arquivo.ir` (se a compilação for bem-sucedida)

## 📚 Gramática da Linguagem

A gramática da linguagem é definida em BNF (Backus-Naur Form):

```
<program>          ::= <function_list>

<function_list>    ::= <function_decl> <function_list>
                     | ε

<function_decl>    ::= func <id> ( <params> ) { <statements> }

<params>           ::= <param_list>
                     | ε

<param_list>       ::= <id>
                     | <id> , <param_list>

<statements>       ::= <statement> <statements>
                     | ε

<statement>        ::= return <expr> ;
                     | <id> = <expr> ;

<expr>             ::= <term> <expr_p>

<expr_p>           ::= + <term> <expr_p>
                     | - <term> <expr_p>
                     | ε

<term>             ::= <factor> <term_p>

<term_p>           ::= * <factor> <term_p>
                     | / <factor> <term_p>
                     | ε

<factor>           ::= <base> <factor_p>

<factor_p>         ::= ^ <base> <factor_p>
                     | ε

<base>             ::= <num>
                     | <id>
                     | <id> ( <args> )
                     | ( <expr> )

<args>             ::= <arg_list>
                     | ε

<arg_list>         ::= <expr>
                     | <expr> , <arg_list>

<id>               ::= [a-zA-Z_][a-zA-Z0-9_]*

<num>              ::= [0-9]+
                     | [0-9]+\.[0-9]+
                     | [0-9]+\.
                     | \.[0-9]+
```

### Tokens

| Token | Descrição | Exemplo |
|-------|-----------|---------|
| `func` | Palavra-chave para declaração de função | `func soma(...)` |
| `return` | Palavra-chave para retorno | `return x;` |
| `id` | Identificador (variável ou função) | `soma`, `x`, `resultado` |
| `num` | Número (inteiro ou ponto flutuante) | `42`, `3.14`, `.5`, `10.` |
| `+` | Operador de adição | `a + b` |
| `-` | Operador de subtração | `a - b` |
| `*` | Operador de multiplicação | `a * b` |
| `/` | Operador de divisão | `a / b` |
| `^` | Operador de potenciação | `a ^ b` |
| `=` | Operador de atribuição | `x = 5;` |
| `(` `)` | Parênteses | `(a + b)` |
| `{` `}` | Chaves (delimitadores de função) | `{ ... }` |
| `,` | Separador de argumentos | `func(a, b)` |
| `;` | Delimitador de statement | `x = 5;` |
| `//` | Comentário de linha | `// comentário` |

## 📊 Tabela Sintática

### Precedência de Operadores

A precedência dos operadores (da menor para a maior) é:

| Precedência | Operadores | Associatividade | Exemplo |
|------------|------------|-----------------|---------|
| 1 (mais baixa) | `+`, `-` | Esquerda | `a + b - c` = `(a + b) - c` |
| 2 | `*`, `/` | Esquerda | `a * b / c` = `(a * b) / c` |
| 3 (mais alta) | `^` | Direita | `a ^ b ^ c` = `a ^ (b ^ c)` |

### Regras de Precedência

1. **Parênteses** têm a maior precedência
2. **Potenciação (`^`)** tem precedência sobre multiplicação e divisão
3. **Multiplicação e divisão (`*`, `/`)** têm precedência sobre adição e subtração
4. **Adição e subtração (`+`, `-`)** têm a menor precedência

### Exemplos de Precedência

```neto
// Expressão: a + b * c
// Interpretação: a + (b * c)

// Expressão: a * b ^ c
// Interpretação: a * (b ^ c)

// Expressão: a ^ b ^ c
// Interpretação: a ^ (b ^ c)  (associatividade à direita)

// Expressão: (a + b) * c
// Interpretação: (a + b) * c  (parênteses têm precedência)
```

### Árvore de Parsing (Exemplo)

Para a expressão `a + b * c ^ 2`:

```
        +
       / \
      a   *
         / \
        b   ^
           / \
          c   2
```

A ordem de avaliação é:
1. `c ^ 2` (potenciação - maior precedência)
2. `b * (c ^ 2)` (multiplicação)
3. `a + (b * (c ^ 2))` (adição - menor precedência)

## 💡 Exemplos

### Exemplo 1: Função Simples

```neto
func soma(a, b) {
    return a + b;
}
```

**Código intermediário gerado:**
```
=== Funcao: soma ===
  t0 = a + b
  RETURN t0
=== Fim: soma ===
```

### Exemplo 2: Função com Variáveis Locais

```neto
func media(x, y) {
    resultado = (x + y) / 2.0;
    return resultado;
}
```

**Código intermediário gerado:**
```
=== Funcao: media ===
  t0 = x + y
  t1 = t0 / 2.0
  resultado = t1
  RETURN resultado
=== Fim: media ===
```

### Exemplo 3: Função com Potenciação

```neto
func potencia(base, exp) {
    return base ^ exp;
}
```

**Código intermediário gerado:**
```
=== Funcao: potencia ===
  t0 = base ^ exp
  RETURN t0
=== Fim: potencia ===
```

### Exemplo 4: Chamadas de Função Aninhadas

```neto
func dobro(x) {
    return x * 2;
}

func quadrado(n) {
    return n * n;
}

func processar(a, b) {
    x = dobro(a);
    y = quadrado(b);
    resultado = x + y;
    return resultado;
}
```

**Código intermediário gerado:**
```
=== Funcao: dobro ===
  t0 = x * 2
  RETURN t0
=== Fim: dobro ===

=== Funcao: quadrado ===
  t1 = n * n
  RETURN t1
=== Fim: quadrado ===

=== Funcao: processar ===
  t2 = CALL dobro(a)
  x = t2
  t3 = CALL quadrado(b)
  y = t3
  t4 = x + y
  resultado = t4
  RETURN resultado
=== Fim: processar ===
```

### Exemplo 5: Números de Ponto Flutuante

```neto
func calcular() {
    pi = 3.14159;
    raio = 5.5;
    area = pi * raio * raio;
    return area;
}
```

**Código intermediário gerado:**
```
=== Funcao: calcular ===
  pi = 3.14159
  raio = 5.5
  t0 = pi * raio
  t1 = t0 * raio
  area = t1
  RETURN area
=== Fim: calcular ===
```

### Exemplo 6: Expressões Complexas

```neto
func quadratica(a, b, c, x) {
    termo1 = a * x ^ 2;
    termo2 = b * x;
    resultado = termo1 + termo2 + c;
    return resultado;
}
```

**Código intermediário gerado:**
```
=== Funcao: quadratica ===
  t0 = x ^ 2
  t1 = a * t0
  termo1 = t1
  t2 = b * x
  termo2 = t2
  t3 = termo1 + termo2
  t4 = t3 + c
  resultado = t4
  RETURN resultado
=== Fim: quadratica ===
```

## 📁 Estrutura do Projeto

```
compiler/
├── lexer.l                  # Especificação do analisador léxico (Flex)
├── parser.y                 # Especificação do analisador sintático (Bison)
├── token.h/cpp              # Definição de tokens
├── ast.h/cpp                # Árvore sintática abstrata
├── semantic.h/cpp            # Analisador semântico
├── codegen.h/cpp            # Gerador de código intermediário
├── compiler.h/cpp           # Orquestrador principal
├── parser_interface.h/cpp   # Interface entre Flex/Bison e o compilador
├── utils.h/cpp              # Utilitários (logs, etc.)
├── main.cpp                 # Ponto de entrada
├── Makefile                 # Build system
├── README.md                 # Este arquivo
└── examples/                # Casos de teste
    ├── valid*.neto          # Código válido
    ├── error_*.neto         # Código com erros
    └── *.ir                 # Código intermediário gerado
```

### Descrição dos Componentes

- **`lexer.l`**: Define os padrões regex para reconhecimento de tokens
- **`parser.y`**: Define a gramática BNF e as regras de construção da AST
- **`token.h/cpp`**: Estruturas de dados para tokens
- **`ast.h/cpp`**: Implementação da Árvore Sintática Abstrata
- **`semantic.h/cpp`**: Análise semântica (escopo, declarações, chamadas)
- **`codegen.h/cpp`**: Geração de código intermediário em três endereços
- **`compiler.h/cpp`**: Orquestra todas as fases da compilação
- **`main.cpp`**: Interface de linha de comando

## 🧪 Casos de Teste

O projeto inclui vários casos de teste na pasta `examples/`:

### Código Válido

- **`valid1.neto`**: Funções simples com operações básicas
- **`valid2.neto`**: Funções com variáveis locais
- **`valid3.neto`**: Chamadas de função aninhadas
- **`valid_float.neto`**: Demonstração de números de ponto flutuante
- **`valid_mixed_numbers.neto`**: Mistura de inteiros e ponto flutuante
- **`valid_nested_calls.neto`**: Chamadas aninhadas complexas
- **`valid_parentheses.neto`**: Uso de parênteses
- **`valid_complex.neto`**: Código complexo com múltiplas funções

### Código com Erros

- **`error_lexical.neto`**: Erro léxico (caractere inválido)
- **`error_syntax.neto`**: Erro sintático (sintaxe incorreta)
- **`error_semantic.neto`**: Erros semânticos (identificadores não declarados)
- **`error_function_args.neto`**: Erro de número incorreto de argumentos

### Executar Testes

Para executar todos os testes:

```bash
make test
```

Isso irá compilar e executar os casos de teste, mostrando os resultados da compilação.

## 🔍 Verificação Semântica

O compilador realiza as seguintes verificações semânticas:

1. **Declaração de Funções**: Verifica se há funções duplicadas
2. **Escopo de Variáveis**: Verifica se variáveis e parâmetros estão no escopo correto
3. **Chamadas de Função**: Verifica se a função existe e se o número de argumentos está correto
4. **Identificadores**: Verifica se todos os identificadores foram declarados

### Exemplos de Erros Semânticos

**Erro: Identificador não declarado**
```neto
func teste() {
    return x + 5;  // Erro: 'x' não foi declarado
}
```

**Erro: Função não declarada**
```neto
func main() {
    y = funcaoInexistente(10);  // Erro: função não existe
    return y;
}
```

**Erro: Número incorreto de argumentos**
```neto
func soma(a, b) {
    return a + b;
}

func teste() {
    x = soma(1, 2, 3);  // Erro: espera 2 argumentos, recebe 3
    return x;
}
```

## 📖 Formato do Código Intermediário

O código intermediário gerado segue o formato de três endereços:

```
=== Funcao: nomeFuncao ===
  t0 = arg1 op arg2          # Operação binária
  variavel = t0              # Atribuição
  t1 = CALL funcao(args)     # Chamada de função
  RETURN t1                  # Retorno
=== Fim: nomeFuncao ===
```

Onde:
- `t0`, `t1`, ... são variáveis temporárias geradas automaticamente
- `op` pode ser `+`, `-`, `*`, `/`, `^`
- `CALL` indica uma chamada de função
- `RETURN` indica o retorno de uma função

> **Nota**: A geração de código intermediário foi implementada como um recurso adicional (plus) para fins educacionais, demonstrando uma fase completa do processo de compilação. O código intermediário gerado pode ser útil para entender como expressões complexas são decompostas em operações simples de três endereços.
