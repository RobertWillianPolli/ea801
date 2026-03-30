# Importa bibliotecas necessárias para hardware e tempo
from machine import Pin, PWM, I2C
import utime
import ssd1306
import neopixel

# ===== OLED =====
# Inicializa comunicação I2C (pinos 3 = SCL, 2 = SDA) para counicar com o display oled
i2c = I2C(1, scl=Pin(3), sda=Pin(2))

# Cria objeto do display OLED (128x64 pixels)
oled = ssd1306.SSD1306_I2C(128, 64, i2c)

# ===== BOTÕES =====
# Define botões como entradas e com pull-up interno
btnC = Pin(10, Pin.IN, Pin.PULL_UP)
btnA = Pin(5, Pin.IN, Pin.PULL_UP)
btnB = Pin(6, Pin.IN, Pin.PULL_UP)

# ===== BUZZER =====
# Configura buzzer com PWM no pino 21
buzzer = PWM(Pin(21))

# ===== MATRIZ DE LEDs =====
NUM_LEDS = 25  # quantidade de LEDs da matriz
np = neopixel.NeoPixel(Pin(7), NUM_LEDS)

# ===== ESTADOS DO SISTEMA =====
CONFIG = 0           # modo de configuração
RUNNING = 1          # contagem regressiva
ALARM = 2            # alarme disparado
estado = CONFIG      # inicia em modo configuração

# ===== VARIÁVEIS =====
minutos = 0
segundos = 30
campo = 0       # seleciona minutos ou segundos
tempo = 0
tempo_total = 0
ultimo_tick = utime.ticks_ms()  # Cria instância para controle de tempo

# ===== INTERRUPÇÃO DO BOTÃO C =====
flag_btnC = False
last_irq = 0

def irq_btnC(pin):                  # Função de tratamento da interrupção pelo botão C
    global flag_btnC, last_irq
    agora = utime.ticks_ms()

    # Condicional de debounce (evita múltiplos acionamentos)
    if utime.ticks_diff(agora, last_irq) > 200:
        flag_btnC = True            # Flag para tratamento da interrupção no loop principal 
        last_irq = agora

# Associa interrupção ao botão C (quando pressionado)
btnC.irq(trigger=Pin.IRQ_FALLING, handler=irq_btnC)

# ===== FUNÇÕES =====

# Liga o buzzer em uma frequência
def beep(freq):
    buzzer.freq(freq)
    buzzer.duty_u16(30000)

# Desliga o buzzer
def beep_off():
    buzzer.duty_u16(0)

# Apaga todos os LEDs
def clear_leds():
    for i in range(NUM_LEDS):
        np[i] = (0, 0, 0)
    np.write()

# Define cor de um LED específico
def set_led(i, c):
    if 0 <= i < NUM_LEDS:
        np[i] = c

# Atualiza matriz de LEDs
def show():
    np.write()

# Animação tipo "explosão"
def anim_explosao():
    frames = [
        [12],  # centro
        [6,7,8,11,13,16,17,18],  # intermediário
        [0,1,2,3,4,5,9,10,14,15,19,20,21,22,23,24]  # externo
    ]
    for f in frames:
        clear_leds()
        for i in f:
            set_led(i, (50,0,0))  # vermelho
        show()
        utime.sleep_ms(120)

# Som de alarme variando frequência
def som_alarme():
    for f in range(1000, 3000, 200):
        beep(f)
        utime.sleep_ms(30)
    for f in range(3000, 1000, -200):
        beep(f)
        utime.sleep_ms(30)

# Mostra progresso da contagem nos LEDs da matriz de led
def progresso_leds():
    if tempo_total == 0:
        return

    # calcula quantos LEDs devem estar acesos
    n = int((tempo / tempo_total) * NUM_LEDS)

    for i in range(NUM_LEDS):
        np[i] = (0,0,50) if i < n else (0,0,0)
    np.write()

# ===== CONFIGURAÇÃo DAS TELAS DO DISPLAY OLED =====

# Tela de configuração
def tela_config():
    oled.fill(0)
    oled.text("CONFIGURAR", 10, 0)

    # indica campo selecionado
    oled.text(">" if campo==0 else " ", 0, 20)
    oled.text(">" if campo==1 else " ", 0, 40)

    oled.text("Min: {:02}".format(minutos), 10, 20)
    oled.text("Seg: {:02}".format(segundos), 10, 40)
    oled.text("A:+ B:- C:OK", 0, 55)
    oled.show()

# Tela durante contagem
def tela_running():
    oled.fill(0)
    m = tempo // 60
    s = tempo % 60
    oled.text("CONTAGEM", 10, 0)
    oled.text("{:02}:{:02}".format(m, s), 30, 30)
    oled.text("C = cancelar", 0, 55)
    oled.show()

# Tela de alarme
def tela_alarm():
    oled.fill(0)
    oled.text("TEMPO!", 30, 15)
    oled.text("FINALIZADO", 10, 30)
    oled.text("C = parar", 20, 50)
    oled.show()

# ===== LOOP PRINCIPAL =====
while True:

    # Verificação do estado atual.

    # ===== MODO CONFIGURAÇÃO =====
    if estado == CONFIG:
        clear_leds()
        tela_config()

        # botão A incrementa
        if not btnA.value():
            if campo == 0:
                minutos = (minutos + 1) % 100
            else:
                segundos = (segundos + 1) % 60
            utime.sleep_ms(200)

        # botão B decrementa
        if not btnB.value():
            if campo == 0:
                minutos = max(0, minutos - 1)
            else:
                segundos = max(0, segundos - 1)
            utime.sleep_ms(200)

        # botão C troca campo / inicia
        if flag_btnC:                # Flag de acionamento do botão "cancelar"
            flag_btnC = False
            campo += 1

            if campo > 1:
                tempo = minutos * 60 + segundos
                if tempo > 0:
                    tempo_total = tempo
                    estado = RUNNING                    # Altera o estado para modo contagem
                    ultimo_tick = utime.ticks_ms()
                campo = 0

    # ===== MODO CONTAGEM =====
    elif estado == RUNNING:
        tela_running()
        progresso_leds()

        agora = utime.ticks_ms()

        # decrementa a cada 1 segundo
        if utime.ticks_diff(agora, ultimo_tick) >= 1000:
            ultimo_tick = agora
            tempo -= 1

        # se acabar o tempo → alarme
        if tempo <= 0:
            estado = ALARM

        # botão C cancela
        if flag_btnC:
            flag_btnC = False
            estado = CONFIG
            minutos = 0
            segundos = 30
            clear_leds()

    # ===== MODO ALARME =====
    elif estado == ALARM:
        tela_alarm()

        anim_explosao()
        som_alarme()
        beep_off()

        # botão C reinicia
        if flag_btnC:
            flag_btnC = False
            beep_off()
            clear_leds()
            estado = CONFIG
            minutos = 0
            segundos = 30
