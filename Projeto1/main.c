from machine import Pin, PWM, I2C
import utime
import ssd1306
import neopixel

# ===== OLED =====
i2c = I2C(1, scl=Pin(3), sda=Pin(2))
oled = ssd1306.SSD1306_I2C(128, 64, i2c)

# ===== BOTÕES =====
btnC = Pin(10, Pin.IN, Pin.PULL_UP)
btnA = Pin(5, Pin.IN, Pin.PULL_UP)
btnB = Pin(6, Pin.IN, Pin.PULL_UP)

# ===== BUZZER =====
buzzer = PWM(Pin(21))

# ===== MATRIZ =====
NUM_LEDS = 25
np = neopixel.NeoPixel(Pin(7), NUM_LEDS)

# ===== ESTADOS =====
CONFIG = 0
RUNNING = 1
ALARM = 2
estado = CONFIG

# ===== VARIÁVEIS =====
minutos = 0
segundos = 30
campo = 0
tempo = 0
tempo_total = 0
ultimo_tick = utime.ticks_ms()

# ===== INTERRUPÇÃO =====
flag_btnC = False
last_irq = 0

def irq_btnC(pin):
    global flag_btnC, last_irq
    agora = utime.ticks_ms()

    # debounce simples
    if utime.ticks_diff(agora, last_irq) > 200:
        flag_btnC = True
        last_irq = agora

btnC.irq(trigger=Pin.IRQ_FALLING, handler=irq_btnC)

# ===== FUNÇÕES =====
def beep(freq):
    buzzer.freq(freq)
    buzzer.duty_u16(30000)

def beep_off():
    buzzer.duty_u16(0)

def clear_leds():
    for i in range(NUM_LEDS):
        np[i] = (0, 0, 0)
    np.write()

def set_led(i, c):
    if 0 <= i < NUM_LEDS:
        np[i] = c

def show():
    np.write()

def anim_explosao():
    frames = [
        [12],
        [6,7,8,11,13,16,17,18],
        [0,1,2,3,4,5,9,10,14,15,19,20,21,22,23,24]
    ]
    for f in frames:
        clear_leds()
        for i in f:
            set_led(i, (50,0,0))
        show()
        utime.sleep_ms(120)

def som_alarme():
    for f in range(1000, 3000, 200):
        beep(f)
        utime.sleep_ms(30)
    for f in range(3000, 1000, -200):
        beep(f)
        utime.sleep_ms(30)

def progresso_leds():
    if tempo_total == 0:
        return

    n = int((tempo / tempo_total) * NUM_LEDS)
    for i in range(NUM_LEDS):
        np[i] = (0,0,50) if i < n else (0,0,0)
    np.write()

# ===== OLED =====
def tela_config():
    oled.fill(0)
    oled.text("CONFIGURAR", 10, 0)

    oled.text(">" if campo==0 else " ", 0, 20)
    oled.text(">" if campo==1 else " ", 0, 40)

    oled.text("Min: {:02}".format(minutos), 10, 20)
    oled.text("Seg: {:02}".format(segundos), 10, 40)
    oled.text("A:+ B:- C:OK", 0, 55)
    oled.show()

def tela_running():
    oled.fill(0)
    m = tempo // 60
    s = tempo % 60
    oled.text("CONTAGEM", 10, 0)
    oled.text("{:02}:{:02}".format(m, s), 30, 30)
    oled.text("C = cancelar", 0, 55)
    oled.show()

def tela_alarm():
    oled.fill(0)
    oled.text("TEMPO!", 30, 15)
    oled.text("FINALIZADO", 10, 30)
    oled.text("C = parar", 20, 50)
    oled.show()

# ===== LOOP =====
while True:

    # ===== CONFIG =====
    if estado == CONFIG:
        clear_leds()
        tela_config()

        if not btnA.value():
            if campo == 0:
                minutos = (minutos + 1) % 100
            else:
                segundos = (segundos + 1) % 60
            utime.sleep_ms(200)

        if not btnB.value():
            if campo == 0:
                minutos = max(0, minutos - 1)
            else:
                segundos = max(0, segundos - 1)
            utime.sleep_ms(200)

        if flag_btnC:
            flag_btnC = False
            campo += 1

            if campo > 1:
                tempo = minutos * 60 + segundos
                if tempo > 0:
                    tempo_total = tempo
                    estado = RUNNING
                    ultimo_tick = utime.ticks_ms()
                campo = 0

    # ===== RUNNING =====
    elif estado == RUNNING:
        tela_running()
        progresso_leds()

        agora = utime.ticks_ms()
        if utime.ticks_diff(agora, ultimo_tick) >= 1000:
            ultimo_tick = agora
            tempo -= 1

        if tempo <= 0:
            estado = ALARM

        if flag_btnC:
            flag_btnC = False
            estado = CONFIG
            minutos = 0
            segundos = 30
            clear_leds()

    # ===== ALARM =====
    elif estado == ALARM:
        tela_alarm()

        anim_explosao()
        som_alarme()
        beep_off()

        if flag_btnC:
            flag_btnC = False
            beep_off()
            clear_leds()
            estado = CONFIG
            minutos = 0
            segundos = 30
