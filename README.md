# Estruturas de Dados II

Repositório contendo as implementações e experimentos desenvolvidos na disciplina **TN730 - Estruturas de Dados II** da UFRRJ. Os projetos estão organizados em branches separadas, abordando manipulação de arquivos em memória secundária, indexação e estruturas de dispersão.

## Projetos

- **[Particionamento (Seleção Natural)](https://github.com/ravsil/estruturaDeDadosII/tree/particionamento)**  
  Implementação do algoritmo de Seleção Natural com reservatório para geração de partições ordenadas a partir de arquivos binários de registros que excedem o limite da memória principal.

- **[Intercalação (Árvore de Vencedores)](https://github.com/ravsil/estruturaDeDadosII/tree/intercalacao)**  
  Implementação da intercalação ótima de múltiplas partições ordenadas utilizando a estrutura de Árvore de Vencedores (*Winner Tree*) para unificar os registros em um único arquivo final ordenado.

- **[Árvore B+](https://github.com/ravsil/estruturaDeDadosII/tree/arvorebmais)**  
  Implementação de indexação em memória secundária utilizando Árvore B+, com suporte a inserção, divisão (*split*) de nós internos e folhas, persistência em arquivos binários e busca eficiente por chave.

- **[Endereçamento Aberto (Tabelas Hash)](https://github.com/ravsil/estruturaDeDadosII/tree/enderecamento_aberto)**  
  Análise experimental e comparativa de métodos de tratamento de colisão em tabelas hash com endereçamento aberto (**Tentativa Linear**, **Tentativa Quadrática** e **Dispersão Dupla**), avaliando o número de colisões e o tempo médio de busca sob diferentes tamanhos de tabela e fatores de carga.
