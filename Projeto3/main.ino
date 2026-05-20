#include <Servo.h>
#include <AFMotor.h>

#define LINE_BUFFER_LENGTH 512

// Define o tipo de movimento do motor
#define STEP MICROSTEP;

// Posição Up e Down do servo
const int penZUp = 90;
const int penZDown = 50;

// Servo controlado pelo PWM no pino 10
const int penServoPin = 10 ;

// Quantidade de passos a cada volta dos motores de passo
const int stepsPerRevolution = 48;

// Cria objeto para controlar o servo
Servo penServo;

// Inicialize os motores de passo dos eixos X e Y usando estes pinos do Arduino para a ponte H L293D.
AF_Stepper myStepperY(stepsPerRevolution, 1);
AF_Stepper myStepperX(stepsPerRevolution, 2);

/* Structures, global variables    */
struct point {
  float x;
  float y;
  float z;
};

// Posição atual do cabeçote
struct point actuatorPos;

//  Configs de desenho
float StepInc = 1;
int StepDelay = 0;
int LineDelay = 0;
int penDelay = 50;

// O motor avança 1 milímetro por passo.
// Use um esboço de teste para percorrer 100 passos. Meça o comprimento da linha.
// Calcular passos por mm. Insira aqui.
float StepsPerMillimeterX = 100.0;
float StepsPerMillimeterY = 100.0;

// Limites do desenho, em mm
float Xmin = 0;
float Xmax = 40;
float Ymin = 0;
float Ymax = 40;
float Zmin = 0;
float Zmax = 1;

float Xpos = Xmin;
float Ypos = Ymin;
float Zpos = Zmax;

// Variável para controle do debug
boolean verbose = false;

void setup() {
  //  Setup

  Serial.begin(9600);

  // Inicializa o servo motor na posição Up
  penServo.attach(penServoPin);
  penServo.write(penZUp);
  delay(100);

  // Configura a velocidade dos servos motores
  myStepperX.setSpeed(600);
  myStepperY.setSpeed(600);


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
  char line[LINE_BUFFER_LENGTH];
  char c;
  int lineIndex;
  bool lineIsComment, lineSemiColon;

  lineIndex = 0;
  lineSemiColon = false;
  lineIsComment = false;

  while (1) {
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
          if (verbose) {
            Serial.print("Received : ");
            Serial.println(line);
          }
          processIncomingLine(line, lineIndex);
          lineIndex = 0;
        }
        else {
          // Empty or comment line. Skip block.
        }
        lineIsComment = false;
        lineSemiColon = false;
        Serial.println("ok");
      }
      else {
        if ((lineIsComment) || (lineSemiColon)) {   // A partir desse caractere, os demais são parte de um comentário
          if (c == ')')  lineIsComment = false;     // Fim do comentário, que ocorre entre parênteses ("comentário em g-code")
        }
        else {
          if (c <= ' ') {                           // Descarta caracteres de espaço e de controle
          }
          else if (c == '/') {                      // O programa não suporta esse recurso, então apenas ignora
          }
          else if (c == '(') {                      // Habilita a flag de comentário e ignora todos os caracteres até encontrar ')' ou EOL.
            lineIsComment = true;
          }
          else if (c == ';') {                      // Início de um comentário
            lineSemiColon = true;
          }
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
}

void processIncomingLine(char* line, int charNB) {
  int currentIndex = 0;
  char buffer[64];                                 
  struct point newPos;

  newPos.x = 0.0;
  newPos.y = 0.0;

  while (currentIndex < charNB) {
    switch (line[currentIndex++]) {              // Caso o g-code converter gere comandos mais simples para movimentação da caneta (TESTAR!)
      case 'U':
        penUp();
        break;
      
      case 'D':
        penDown();
        break;
      
      case 'G':                                    // Comando de controle
        buffer[0] = line[currentIndex++];          // Lê o número do comando de controle
        buffer[1] = '\0';

        switch (atoi(buffer)) {                    // Converte o comando, de str para int
          case 0:                                  // G00 & G01 - Movimento linear
          case 1:
            // Supõe que a posição X vem antes de Y
            char* indexX = strchr(line + currentIndex, 'X'); // Procura o caractere "X" na string line a partir a posição atual
            char* indexY = strchr(line + currentIndex, 'Y'); // Procura o caractere "Y" na string line a partir a posição atual
            
            if (indexY <= 0) {              // Comando apenas para o movimento no eixo X
              newPos.x = atof(indexX + 1);  // Converte a string lida em float
              newPos.y = actuatorPos.y;     // Mantém a posição em y
            }
            else if (indexX <= 0) {         // Comando apenas para o movimento no eixo y
              newPos.y = atof(indexY + 1);  // Converte a string lida em float
              newPos.x = actuatorPos.x;     // Mantém a posição em x
            }
            else {
              newPos.y = atof(indexY + 1);  // Converte a string lida em float
              newPos.x = atof(indexX + 1);  // Converte a string lida em float
            }
            drawLine(newPos.x, newPos.y);   // Desenha a posição lida

            actuatorPos.x = newPos.x;       //
            actuatorPos.y = newPos.y;       // Atualiza a posição atual
            break;
        }
        break;
      case 'M':
        buffer[0] = line[currentIndex++];
        buffer[1] = line[currentIndex++];
        buffer[2] = line[currentIndex++];
        buffer[3] = '\0';
        switch (atoi(buffer)) {
          case 300:                // Comando de movimentação do cabeçote
            {
              char* indexS = strchr(line + currentIndex, 'S');
              float Spos = atof(indexS + 1);
              //         Serial.println("ok");
              if (Spos == 30) {
                penDown();
              }
              if (Spos == 50) {
                penUp();
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
    }
  }
}

void drawLine(float x1, float y1) {
  if (verbose)
  {
    Serial.print("fx1, fy1: ");
    Serial.print(x1);
    Serial.print(",");
    Serial.print(y1);
    Serial.println("");
  }

  //  Limitando a atuação do cabeçote
  if (x1 >= Xmax) {
    x1 = Xmax;
  }
  if (x1 <= Xmin) {
    x1 = Xmin;
  }
  if (y1 >= Ymax) {
    y1 = Ymax;
  }
  if (y1 <= Ymin) {
    y1 = Ymin;
  }

  if (verbose)
  {
    Serial.print("Xpos, Ypos: ");
    Serial.print(Xpos);
    Serial.print(",");
    Serial.print(Ypos);
    Serial.println("");
  }

  if (verbose)
  {
    Serial.print("x1, y1: ");
    Serial.print(x1);
    Serial.print(",");
    Serial.print(y1);
    Serial.println("");
  }

  //  Converte coordenadas em passos
  x1 = (int)(x1 * StepsPerMillimeterX);
  y1 = (int)(y1 * StepsPerMillimeterY);
  float x0 = Xpos;
  float y0 = Ypos;

  //  Calcula a variação das coordenadas e a direção do giro do motor
  long dx = abs(x1 - x0);
  long dy = abs(y1 - y0);
  int sx = x0 < x1 ? StepInc : -StepInc;
  int sy = y0 < y1 ? StepInc : -StepInc;

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

  if (verbose)
  {
    Serial.print("dx, dy:");
    Serial.print(dx);
    Serial.print(",");
    Serial.print(dy);
    Serial.println("");
  }

  if (verbose)
  {
    Serial.print("Going to (");
    Serial.print(x0);
    Serial.print(",");
    Serial.print(y0);
    Serial.println(")");
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
  digitalWrite(15, LOW);
  digitalWrite(16, HIGH);
  
  if (verbose) {
    Serial.println("Cabeçote up!");
  }
}
//  Abaixa o cabeçote
void penDown() {
  penServo.write(penZDown);
  delay(penDelay);
  
  Zpos = Zmin;
  digitalWrite(15, HIGH);
  digitalWrite(16, LOW);
  
  if (verbose) {
    Serial.println("Cabeçote down.");
  }
}
