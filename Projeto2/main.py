import serial
import time
import kociemba

# Ajuste da porta de comunicação
porta = 'COM10'         # Windows
baud_rate = 9600        # Taxa de comunicação

# Cria instância do Arduino 
arduino = serial.Serial(porta, baud_rate, timeout = 1)       

time.sleep(2)  # aguarda inicialização do Arduino

for faces in ["Superior", "Direita", "Frontal", "Inferior", "Esquerda", "Traseira"]:
        
    print(f"Posicione a face {faces} em frente à câmera e pressione ENTER")
    input()
        
    if arduino.in_waiting > 0:  # Aguarda recebimento de bytes do Arduino      

        # Lê o dado do Arduino, decodifica e remove caracteres especiais
        dado_lido = arduino.readline().decode('utf-8').strip()  
        # Atualiza string de faces do cubo     
        facesDado += dado_lido        

# Envia a sequência de cores para o solver
solution = kociemba.solve(scrambled_cube)
# Exibe a solução
print(f"Solution: {solution.split(' ')}")
