# CNC Plotter com Interpretação de G-code em Arduino

Sistema de controle para uma mini CNC/plotter baseado em Arduino, capaz de interpretar comandos G-code enviados pela comunicação serial e controlar motores de passo nos eixos `X` e `Y`, além de um servo-motor responsável pelo movimento vertical do cabeçote (`Z`).

O projeto implementa uma interpretação simplificada de G-code, permitindo executar movimentos lineares, controle do cabeçote e monitoramento da posição atual do sistema.

---

# Funcionalidades

- Interpretação de comandos G-code via serial
- Controle de motores de passo nos eixos X e Y
- Controle de servo-motor para movimento vertical do cabeçote
- Execução de movimentos lineares
- Conversão de coordenadas em milímetros para passos
- Limitação da área útil de trabalho
- Reporte da posição atual do cabeçote
- Compatibilidade básica com softwares geradores de G-code

---

# Hardware Utilizado

## Controlador

- Arduino Uno

## Atuadores

- 2 motores de passo (eixos X e Y)
- 1 servo-motor (eixo Z)

## Drivers

- Ponte H ou driver de motor de passo compatível

## Estrutura Mecânica

- Sistema cartesiano XY
- Cabeçote móvel com servo para levantamento

---

#  Funcionamento Geral

O sistema recebe comandos G-code pela comunicação serial. Cada linha recebida é armazenada em uma string e interpretada pela função:

```cpp
processIncomingLine()
```

Essa função identifica o tipo de comando recebido e executa a ação correspondente.

---

# Comunicação Serial

A comunicação serial é utilizada para envio dos comandos G-code.

## Exemplo

```gcode
G01 X50 Y30
M300 S30
G01 X10 Y10
M300 S50
```

---

# Interpretação de G-code

## Comandos Implementados

| Comando | Função |
|---|---|
| `G00` | Movimento linear rápido |
| `G01` | Movimento linear |
| `M300 S30` | Abaixa o cabeçote |
| `M300 S50` | Levanta o cabeçote |
| `M114` | Reporta posição atual |

---

# Movimentação Linear

Os comandos `G00` e `G01` são utilizados para movimentar o cabeçote nos eixos `X` e `Y`.

Exemplo:

```gcode
G01 X40 Y25
```

A função responsável pela interpretação do comando:

```cpp
processIncomingLine()
```

realiza os seguintes passos:

1. Identifica o comando `G`
2. Extrai o número do comando (`00` ou `01`)
3. Procura os parâmetros `X` e `Y`
4. Converte os valores recebidos para `float`
5. Chama a função:

```cpp
drawLine(x, y);
```

---

# 📏 Conversão de Coordenadas

As coordenadas recebidas em milímetros são convertidas para passos do motor utilizando:

```cpp
StepsPerMillimeter
```

Conversão:

```cpp
passos = coordenada_mm * StepsPerMillimeter
```

---

# Controle do Cabeçote (Eixo Z)

O eixo Z é controlado por um servo-motor.

## Funções

### Levantar Cabeçote

```cpp
penUp();
```

### Abaixar Cabeçote

```cpp
penDown();
```

Essas funções:

- movimentam o servo
- aguardam estabilização mecânica
- atualizam a posição lógica do eixo Z
- alteram os sinais de controle

---

# Algoritmo de Movimento

A função:

```cpp
drawLine()
```

é responsável pelo cálculo da trajetória e sincronização dos motores de passo.

Ela realiza:

- Limitação da área útil
- Conversão de coordenadas
- Cálculo de deslocamento (`dx`, `dy`)
- Definição do sentido de rotação
- Interpolação linear do movimento

---

# Limitação da Área de Trabalho

O sistema impede que o cabeçote ultrapasse os limites físicos da máquina.

Variáveis utilizadas:

```cpp
Xmin
Xmax
Ymin
Ymax
```

---

# Reporte de Posição

O comando:

```gcode
M114
```

envia pela serial a posição atual do cabeçote.

Exemplo de saída:

```text
Absolute position : X = 40 - Y = 25
```

---

# Configuração de Pinos

Exemplo:

```cpp
#define X_STEP_PIN 2
#define X_DIR_PIN  3

#define Y_STEP_PIN 4
#define Y_DIR_PIN  5

#define SERVO_PIN  9
```

---

# Variáveis Importantes

| Variável | Função |
|---|---|
| `actuatorPos.x` | Posição atual em X |
| `actuatorPos.y` | Posição atual em Y |
| `StepsPerMillimeter` | Conversão mm → passos |
| `penZUp` | Ângulo do servo levantado |
| `penZDown` | Ângulo do servo abaixado |
| `Xmax`, `Ymax` | Limites máximos |
| `Xmin`, `Ymin` | Limites mínimos |

---

# Como Utilizar

## 1. Upload do código

Compile e envie o código utilizando a Arduino IDE.

## 2. Abrir comunicação serial

Configurar baud rate compatível com o firmware.

Exemplo:

```cpp
Serial.begin(115200);
```

## 3. Enviar comandos G-code

Os comandos podem ser enviados:

- Pelo Monitor Serial
- Pelo Pronterface
- Por softwares CNC
- Por scripts externos

---

# Exemplo de Sequência

```gcode
M300 S50
G01 X10 Y10
M300 S30
G01 X50 Y10
G01 X50 Y50
G01 X10 Y50
G01 X10 Y10
M300 S50
```

Esse exemplo desenha um quadrado.

---

# Tratamento de Erros

Comandos não implementados são identificados automaticamente.

Exemplo:

```text
Comando não reconhecido : M500
```

---

# Bibliotecas Utilizadas

## Servo

```cpp
#include <Servo.h>
```

---

# Possíveis Melhorias Futuras

- Implementação de aceleração
- Controle de velocidade real para G00 e G01
- Implementação de homing
- Suporte a fim de curso
- Interpolação circular (`G02` e `G03`)
- Controle de feedrate (`F`)
- Buffer de comandos
- Compatibilidade com GRBL

---

# Aplicações

- Plotter de caneta
- Pequena CNC didática
- Sistema XY automatizado
- Escrita automática
- Desenho automatizado
- Estudos de G-code
