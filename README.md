# Relatório de Análise Comparativa: Métodos de Sondagem em Endereçamento Aberto

## 1. Introdução
Este relatório compara o desempenho de três métodos de tratamento de colisão em tabelas hash com endereçamento aberto: **Tentativa Linear**, **Tentativa Quadrática** e **Dispersão Dupla**.

Os testes foram feitos com três tamanhos de tabela (**M = 1.000, 100.000 e 1.000.000**) e três fatores de carga (**α ≈ 0.7, 0.8 e 0.9**).

Métricas avaliadas:
- **Número de colisões** (inserção e busca)
- **Tempo médio de busca** (µs)

---

## 2. Número de Colisões

### Observação 1 — Tentativa Linear e Agrupamento Primário
A Tentativa Linear teve o pior desempenho, especialmente com α alto.

- (M=1M, α=0.7): ~71 milhões de colisões
- (M=1M, α=0.9): ~245 milhões de colisões

O motivo é o **agrupamento primário**, onde colisões formam grandes blocos contínuos, piorando rapidamente o desempenho.

### Observação 2 — Tentativa Quadrática e Dispersão Dupla
Ambas espalham melhor as chaves:

- (M=1M, α=0.9) Quadrática: ~4.5 milhões de colisões
- (M=1M, α=0.9) Dispersão Dupla: ~1.3 milhões de colisões

A Quadrática sofre com **agrupamento secundário**.
A Dispersão Dupla evita isso usando uma segunda função hash.

### Observação 3 — Anomalia na Dispersão Dupla (α=0.9)
Diferença grande entre colisões de inserir e buscar:

- M=100k, α=0.9 → Inserir: 126.462 | Buscar: 1.326.462
- M=1M, α=0.9 → Inserir: 1.299.033 | Buscar: 80.299.033

Isso indica **inserções que falharam**, pois a Dispersão Dupla só percorre toda a tabela se **M for primo**.
Como M não era primo, algumas chaves ficaram sem posição, e as buscas de chaves inexistentes geraram muitas colisões.

---

## 3. Tempo Médio de Busca
Reflete diretamente o número de colisões:

- Linear: mais lenta em todos os cenários (ex.: 2.34 µs em M=1M, α=0.9)
- Quadrática: mais rápida que Linear (0.42 µs)
- Dispersão Dupla: melhor desempenho (0.27 µs)

Mesmo indo de α=0.7 para α=0.9:
- Linear quase dobrou o tempo
- Dispersão Dupla aumentou muito pouco

---

## 4. Conclusão

### 1. Tentativa Linear
- Simples, mas degrada muito com o aumento do fator de carga
- Não recomendada acima de **α = 0.5–0.6**

### 2. Tentativa Quadrática
- Boa alternativa intermediária
- Muito melhor que Linear

### 3. Dispersão Dupla
- Melhor desempenho geral
- Evita ambos os tipos de agrupamento
- Mais robusta mesmo com tabelas muito cheias


rafael@NB-001265:/mnt/c/Users/Colaborador/Downloads$ nvim foo.md
rafael@NB-001265:/mnt/c/Users/Colaborador/Downloads$ ls
'budai_background 2.png'   desktop.ini   foo.md   relatorio.pdf   relatoriopdf.pdf
rafael@NB-001265:/mnt/c/Users/Colaborador/Downloads$ rm -rf relatorio.pdf
rafael@NB-001265:/mnt/c/Users/Colaborador/Downloads$ mv relatoriopdf.pdf relatorio.pdf
rafael@NB-001265:/mnt/c/Users/Colaborador/Downloads$ ls
'budai_background 2.png'   desktop.ini   foo.md   relatorio.pdf
rafael@NB-001265:/mnt/c/Users/Colaborador/Downloads$ cat foo.md
# Relatório de Análise Comparativa: Métodos de Sondagem em Endereçamento Aberto

## 1. Introdução
Este relatório compara o desempenho de três métodos de tratamento de colisão em tabelas hash com endereçamento aberto: **Tentativa Linear**, **Tentativa Quadrática** e **Dispersão Dupla**.

Os testes foram feitos com três tamanhos de tabela (**M = 1.000, 100.000 e 1.000.000**) e três fatores de carga (**α ≈ 0.7, 0.8 e 0.9**).

Métricas avaliadas:
- **Número de colisões** (inserção e busca)
- **Tempo médio de busca** (µs)


## 2. Número de Colisões

### Observação 1 — Tentativa Linear e Agrupamento Primário
A Tentativa Linear teve o pior desempenho, especialmente com α alto.

- (M=1M, α=0.7): ~71 milhões de colisões
- (M=1M, α=0.9): ~245 milhões de colisões

O motivo é o **agrupamento primário**, onde colisões formam grandes blocos contínuos, piorando rapidamente o desempenho.

### Observação 2 — Tentativa Quadrática e Dispersão Dupla
Ambas espalham melhor as chaves:

- (M=1M, α=0.9) Quadrática: ~4.5 milhões de colisões
- (M=1M, α=0.9) Dispersão Dupla: ~1.3 milhões de colisões

A Quadrática sofre com **agrupamento secundário**.
A Dispersão Dupla evita isso usando uma segunda função hash.

### Observação 3 — Anomalia na Dispersão Dupla (α=0.9)
Diferença grande entre colisões de inserir e buscar:

- M=100k, α=0.9 → Inserir: 126.462 | Buscar: 1.326.462
- M=1M, α=0.9 → Inserir: 1.299.033 | Buscar: 80.299.033

Isso indica **inserções que falharam**, pois a Dispersão Dupla só percorre toda a tabela se **M for primo**.
Como M não era primo, algumas chaves ficaram sem posição, e as buscas de chaves inexistentes geraram muitas colisões.

## 3. Tempo Médio de Busca
Reflete diretamente o número de colisões:

- Linear: mais lenta em todos os cenários (ex.: 2.34 µs em M=1M, α=0.9)
- Quadrática: mais rápida que Linear (0.42 µs)
- Dispersão Dupla: melhor desempenho (0.27 µs)

Mesmo indo de α=0.7 para α=0.9:
- Linear quase dobrou o tempo
- Dispersão Dupla aumentou muito pouco


## 4. Conclusão

### 1. Tentativa Linear
- Simples, mas degrada muito com o aumento do fator de carga
- Não recomendada acima de **α = 0.5–0.6**

### 2. Tentativa Quadrática
- Boa alternativa intermediária
- Muito melhor que Linear

### 3. Dispersão Dupla
- Melhor desempenho geral
- Evita ambos os tipos de agrupamento
- Mais robusta mesmo com tabelas muito cheias
