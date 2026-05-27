#include <stdlib.h>
#include <string.h>
#include <Servo.h>
#include <AFMotor.h>

#define LINE_BUFFER_LENGTH 512

// Define o tipo de movimento do motor
#define STEP DOUBLE

// Posição Up e Down do servo
const int penZUp = 30;
const int penZDown = 90;

// Servo controlado pelo PWM no pino 10
const int penServoPin = 10 ;

// Quantidade de passos a cada volta dos motores de passo
const int stepsPerRevolution = 48;

//  Configs de desenho
float StepInc = 1;
int StepDelay = 2;
int LineDelay = 0;
int penDelay = 50;

// Limites do desenho, em mm
int Xmin = 0;
int Xmax = 40;
int Ymin = 0;
int Ymax = 40;
int Zmin = 0;
int Zmax = 1;

int Xpos = Xmin;
int Ypos = Ymin;
int Zpos = Zmax;

char line[LINE_BUFFER_LENGTH];
char c;
int lineIndex = 0;
bool lineSemiColon = false;
bool lineIsComment = false;

const int penCtrlPin1 = 15;
const int penCtrlPin2 = 16;

/* Structures, global variables    */
struct point {
  float x;
  float y;
  float z;
};

// Posição atual do cabeçote
struct point actuatorPos;

// Cria objeto para controlar o servo
Servo penServo;

// O motor avança 1 milímetro por passo.
// Use um esboço de teste para percorrer 100 passos. Meça o comprimento da linha.
// Calcular passos por mm. Insira aqui.
float StepsPerMillimeterX = 6.875;
float StepsPerMillimeterY = 7.1875;

// Inicialize os motores de passo dos eixos X e Y usando estes pinos do Arduino para a ponte H L293D.
AF_Stepper myStepperY(stepsPerRevolution, 2);
AF_Stepper myStepperX(stepsPerRevolution, 1);

void setup() {
  //  Setup

  Serial.begin(9600);
  pinMode(penCtrlPin1, OUTPUT);
  pinMode(penCtrlPin2, OUTPUT);

  // Inicializa o servo motor na posição Up
  penServo.attach(penServoPin);
  penServo.write(penZUp);
  delay(100);

  // Configura a velocidade dos motores de passo
  myStepperX.setSpeed(30);
  myStepperY.setSpeed(30);


  // Mover cabeçote para a posição inicial
  // TBD

  //  Notifications!!!
  Serial.println("Mini CNC Plotter alive and kicking!");
  Serial.print("X range is from ");
  Serial.print(Xmin);
  Serial.print(" to ");
  Serial.print(Xmax);
  Serial.println(" mm.");
  Serial.print("Y range is from ");
  Serial.print(Ymin);
  Serial.print(" to ");
  Serial.print(Ymax);
  Serial.println(" mm.");
}

void loop()
{
  delay(100);
    // Looping principal para recepção dos comandos via serial. Mostly from Grbl, added semicolon support

    //  Comandos g-code
    //  G1: Comando de movimento
    //  G4: Comando de espera
    //  M300: Comando de controle do servo
    //  M114: Leitura da posição atual do cabeçote
    //  Descarta qualquer comando com (
    //  Descarta qualquer outro comando
    
    //  Ex:
    //      G1 X60 Y30  - Movimenta 60 em x e 30 em y
    //      G4 P300     - Aguarda 150ms
    //      M300 S30    - Abaixa o cabeçote
    //      M300 S50    - Levanta o cabeçote    
    
    while (Serial.available() > 0) {                  // Aguarda recebimento de char               
      c = Serial.read();                              // Lê um caractere da serial
      
      if ((c == '\n') || (c == '\r')) {               // Fim da linha!
        if (lineIndex > 0) {                          // Leitura da linha completa. Então, executa-a.
          line[lineIndex] = '\0';                     // Adiciona a terminação na string
          processIncomingLine(line, lineIndex);
          lineIndex = 0;
        }
        lineIsComment = false;
        lineSemiColon = false;
        Serial.println("ok");
      }
      else {
        if (lineIsComment || lineSemiColon) {   // A partir desse caractere, os demais são parte de um comentário
          if (c == ')')  lineIsComment = false;     // Fim do comentário, que ocorre entre parênteses ("comentário em g-code")
        }
        else {
          if (c <= ' ') {}                             // Descarta caracteres de espaço e de controle
          else if (c == '/') {}                        // O programa não suporta esse recurso, então apenas ignora
          else if (c == '(') lineIsComment = true;     // Habilita a flag de comentário e ignora todos os caracteres até encontrar ')' ou EOL.
          else if (c == ';') lineSemiColon = true;     // Início de um comentário

          else if (lineIndex >= LINE_BUFFER_LENGTH - 1) {    // Verifica se o buffer de comando sofreu overflow.
            Serial.println("ERRO - lineBuffer overflow");
            lineIsComment = false;
            lineSemiColon = false;
          }
          else if (c >= 'a' && c <= 'z') {        // Converte caractere minúsculo em maiúsculo
            line[lineIndex++] = c - 'a' + 'A';
          }
          else {
            line[lineIndex++] = c;                // Armazena o comando recebido
          }
        }
      }
    }
}

void processIncomingLine(char* line, int charNB) {
  int currentIndex = 0;
  char buffer[64];                                 
  struct point newPos;
  
  newPos.x = actuatorPos.x;
  newPos.y = actuatorPos.y;

  while (currentIndex < charNB) {
    switch (line[currentIndex++]) {              
      case 'G':                                    // Comando de controle
        buffer[0] = line[currentIndex++];          // Lê o número do comando de controle
        buffer[1] = '\0';

        switch (atoi(buffer)) {                    // Converte o comando, de str para int
          case 0:                                  // G00 & G01 - Movimento linear
          case 1:
            // Supõe que a posição X vem antes de Y
            char* indexX = strchr(line + currentIndex, 'X'); // Procura o caractere "X" na string line a partir a posição atual
            char* indexY = strchr(line + currentIndex, 'Y'); // Procura o caractere "Y" na string line a partir a posição atual
            
            if (indexX != NULL) newPos.x = atof(indexX + 1);
            if (indexY != NULL) newPos.y = atof(indexY + 1);
            
            drawLine(newPos.x, newPos.y);   // Desenha a posição lida
            
            actuatorPos.x = newPos.x;       //
            actuatorPos.y = newPos.y;       // Atualiza a posição atual
            break;
        }
        break;
      
      case 'M':
        for (int i = 0; i < 3; i++) buffer[i] = line[currentIndex++];
        buffer[3] = '\0';

        int mCmd = atoi(buffer);
        
        switch (mCmd) {
          case 300:                // Comando de movimentação do cabeçote
            {
              char* indexS = strchr(line + currentIndex, 'S');

              if (indexS != NULL){
                float Spos = atoi(indexS + 1);
              
                if (Spos == 30) {
                  penDown();
                }
                else if (Spos == 50) {
                  penUp();
                }
              }
              break;
            }
          case 114:                // Comando de report da posição do cabeçote
            Serial.print("Absolute position : X = " );
            Serial.print(actuatorPos.x );
            Serial.print("  -  Y = " );
            Serial.println(actuatorPos.y);
            break;
          default:
            Serial.print("Commando não reconhecido : M");
            Serial.println(buffer);
        }
      break;
    }
  }
}

void drawLine(float x11, float y11) {

  //  Limitando a atuação do cabeçote
  if (x11 > Xmax) x11 = Xmax;
  if (x11 < Xmin) x11 = Xmin;
  if (y11 > Ymax) y11 = Ymax;
  if (y11 < Ymin) y11 = Ymin;

  //  Converte coordenadas em passos
  int x1 = (int)(x11 * StepsPerMillimeterX);
  int y1 = (int)(y11 * StepsPerMillimeterY);
  int x0 = Xpos;
  int y0 = Ypos;

  //  Calcula a variação das coordenadas e a direção do giro do motor
  long dx = abs(x1 - x0);
  long dy = abs(y1 - y0);
  uint8_t sx = x0 < x1 ? BACKWARD: FORWARD;
  uint8_t sy = y0 < y1 ? BACKWARD: FORWARD;

  long i;
  long over = 0;

  if (dx > dy) {
    for (i = 0; i < dx; ++i) {
      myStepperX.onestep(sx, STEP);
      over += dy;
      if (over >= dx) {
        over -= dx;
        myStepperY.onestep(sy, STEP);
      }
      delay(StepDelay);
    }
  }
  else {
    for (i = 0; i < dy; ++i) {
      myStepperY.onestep(sy, STEP);
      over += dx;
      if (over >= dy) {
        over -= dy;
        myStepperX.onestep(sx, STEP);
      }
      delay(StepDelay);
    }
  }
 
  //  Delay before any next lines are submitted
  delay(LineDelay);
  //  Atualiza a posição atual do cabeçote
  Xpos = x1;
  Ypos = y1;
}

//  Levanta o cabeçote
void penUp() {
  penServo.write(penZUp);
  delay(penDelay);
  
  Zpos = Zmax;
  digitalWrite(penCtrlPin1, LOW);
  digitalWrite(penCtrlPin2, HIGH);
}
//  Abaixa o cabeçote
void penDown() {
  penServo.write(penZDown);
  delay(penDelay);
  
  Zpos = Zmin;
  digitalWrite(penCtrlPin1, HIGH);
  digitalWrite(penCtrlPin2, LOW);
}
