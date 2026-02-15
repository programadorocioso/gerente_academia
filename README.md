# Gerente de Academia (Gerente de Alunos e Treinos)

Um sistema robusto desenvolvido em **Linguagem C** para gestão de alunos e prescrição de treinos, focado em performance e organização de dados. O projeto utiliza a biblioteca **IUP** para interface gráfica e oferece uma versão CLI para operações via terminal.

## 🚀 Diferenciais Técnicos

* **Arquitetura Modular:** Separação clara de responsabilidades entre arquivos de lógica (`aluno.c`, `treino.c`, `processamento.c`) e interfaces.
* **Persistência Eficiente:** Armazenamento em arquivos binários (`.bin`) indexados por ID único em um arquivo mestre (`alunos.idx`).
* **Gerenciamento de Memória:** Uso de **Listas Encadeadas** para carregar o índice de alunos na RAM, permitindo buscas instantâneas e listagens leves.
* **Geração de Relatórios:** Motor integrado para conversão de planos de treino em PDF para fácil compartilhamento.

## 🛠️ Tecnologias e Dependências

* **Linguagem:** C (Padrão C99)
* **Interface Gráfica:** [IUP Portable User Interface](https://www.tecgraf.puc-rio.br/iup/)
* **Compilação:** GNU Make e GCC (Suporte a Cross-Compilation)
* **Ferramentas Auxiliares (Redist):**
    * `pdf_build`: Engine para geração de PDFs (wkhtmltopdf).
    * `pdf_print`: Visualizador de documentos (SumatraPDF).

## 📂 Estrutura do Projeto

```text
├── data/               # Banco de dados (Arquivos .bin e .idx)
├── docs/               # Documentação técnica e licenças
├── libs/               # Bibliotecas estáticas (IUP Linux/Windows)
├── redist/             # Binários auxiliares de terceiros
├── *.c / *.h           # Código-fonte modularizado na raiz
└── Makefile            # Automação de compilação multiplataforma
