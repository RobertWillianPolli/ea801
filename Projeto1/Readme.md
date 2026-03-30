Backlog - https://miro.com/app/board/uXjVG1r_7C8=/?focusWidget=3458764639215524031

# Timer Doméstico Multifuncional

Projeto de um **temporizador digital interativo** utilizando MicroPython, com interface visual em display OLED, feedback sonoro via buzzer e indicação de progresso com matriz de LEDs NeoPixel.

---

## Funcionalidades

* Configuração de tempo (minutos e segundos)
* Contagem regressiva em tempo real
* Exibição no display OLED
* Barra de progresso com LEDs
* Alarme sonoro ao final
* Animação visual de “explosão”
* Interação com 3 botões físicos

---

## Hardware utilizado

* Microcontrolador compatível com MicroPython (ex: Raspberry Pi Pico)
* Display OLED SSD1306 (I2C - 128x64)
* Matriz de LEDs NeoPixel (25 LEDs)
* Buzzer (PWM)
* 3 botões (com pull-up)

---

## Pinagem

| Componente | Pino |
| ---------- | ---- |
| OLED SCL   | GP3  |
| OLED SDA   | GP2  |
| Botão A    | GP5  |
| Botão B    | GP6  |
| Botão C    | GP10 |
| NeoPixel   | GP7  |
| Buzzer     | GP21 |

---

## Como funciona

O sistema é baseado em uma **máquina de estados**, com três modos principais:

### 1. CONFIG (Configuração)

* Usuário define minutos e segundos
* Botões:

  * **A**: incrementa valor
  * **B**: decrementa valor
  * **C**: alterna campo (min/seg) e inicia

---

### 2. RUNNING (Contagem regressiva)

* Tempo decrementa a cada 1 segundo

* Display mostra o tempo restante

* LEDs indicam progresso (barra)

* Botão:

  * **C**: cancela e retorna ao início

---

### 3. ALARM (Alarme)

* Dispara:

  * Animação de LEDs (explosão)
  * Som de sirene no buzzer

* Botão:

  * **C**: para o alarme e reinicia

---

## Fluxo do sistema

```
CONFIG → RUNNING → ALARM → CONFIG
```

---

## Lógica de funcionamento

### Controle de tempo

Utiliza:

```python
utime.ticks_ms()
```

Para garantir precisão sem travar o loop principal.

---

### Botão com interrupção

O botão C usa interrupção (IRQ):

* Resposta imediata
* Debounce por software (200 ms)

---

### LEDs (NeoPixel)

* Durante execução:

  * LEDs representam o progresso do tempo restante
* No alarme:

  * Executam animação de expansão ("explosão")

---

### Buzzer

* Controlado por PWM
* Frequência variável cria efeito de sirene

---

## Como executar

1. Instale MicroPython no microcontrolador
2. Copie o código para o dispositivo
3. Certifique-se de ter as bibliotecas:

   * `ssd1306.py`
   * `neopixel.py`
4. Execute o script principal

---

## Possíveis melhorias

* Adicionar botão de **pausa**
* Ajuste de brilho dos LEDs
* Diferentes cores para progresso
* Alarme com padrões diferentes
* Interface com encoder rotativo
* Salvamento do último tempo configurado
