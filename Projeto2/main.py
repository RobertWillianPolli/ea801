import serial
import time
import kociemba

# Ajuste a porta conforme seu sistema
# Windows: COM3, COM4...
# Linux/Mac: /dev/ttyUSB0 ou /dev/ttyACM0
porta = 'COM10'  # ou '/dev/ttyUSB0'
baud_rate = 9600

arduino = serial.Serial(porta, baud_rate, timeout=1)

time.sleep(2)  # aguarda inicialização do Arduino

arduino.write(b'\n')  # manda ENTER inicial


for faces in ["Superior", "Direita", "Frontal", "Inferior", "Esquerda", "Traseira"]:
        
    print(f"Posicione a face {faces} em frente à câmera e pressione ENTER")
    input()
        
    if arduino.in_waiting > 0:
        dado_lido = arduino.readline().decode('utf-8').strip()
            
        facesDado += dado_lido        
        
solution = kociemba.solve(scrambled_cube)
print(f"Solution: {solution.split(' ')}")
