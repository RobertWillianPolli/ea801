#include <SPI.h>  
#include <Pixy.h>

// Cria instância para comunicação com a Pixycam
Pixy pixy;

void setup()
{
  // Configura a comunicação serial e inicia a Pixycam
  Serial.begin(9600);
  pixy.init();
}

void loop()
{
  static int i = 0;
  uint16_t blocks;

  // Recebe dados lidos pela câmera
  blocks = pixy.getBlocks();
  
  if (blocks)
  {
    i++;
    
    // Reduz frequência de prints
    if (i % 30 == 0)
    {
      int j;

      int xmin = 10000, xmax = 0;
      int ymin = 10000, ymax = 0;

      // achar limites
      for (j = 0; j < blocks; j++)
      {
        int x = pixy.blocks[j].x; // Lê a posição no eixo x do bloco
        int y = pixy.blocks[j].y; // Lê a posição no eixo y do bloco

        if (x < xmin) xmin = x; //
        if (x > xmax) xmax = x; //
        if (y < ymin) ymin = y; //
        if (y > ymax) ymax = y; // Define as posições limites dos blocos lidos 
      }

      float dx = (xmax - xmin) / 3.0;  //
      float dy = (ymax - ymin) / 3.0;  // Calcula o tamanho de cada cor 

      char face[3][3];    // Cria a matriz da face

      // Inicializa a matriz com '?'
      for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
          face[r][c] = '?';

      // Preencher matriz
      for (j = 0; j < blocks; j++)
      {
        int x = pixy.blocks[j].x;  // Lê a posição no eixo x da cor
        int y = pixy.blocks[j].y;  // Lê a posição no eixo y da cor

        int col = (int)((x - xmin) / dx);  // Estima a coluna da cor lida
        int row = (int)((y - ymin) / dy);  // Estima a linha da cor lida

        if (col < 0) col = 0;
        if (col > 2) col = 2;
        if (row < 0) row = 0;
        if (row > 2) row = 2;

        char cor;

        // Converte o dado lido pela sua cor correspondente
        switch (pixy.blocks[j].signature)
        {
          case 1: cor = 'G'; break; // verde
          case 2: cor = 'Y'; break; // amarelo
          case 3: cor = 'O'; break; // laranja
          case 4: cor = 'R'; break; // vermelho
          case 5: cor = 'B'; break; // azul
          case 6: cor = 'W'; break; // branco  
          default: cor = 'W';       // branco como default
        }

        face[row][col] = cor;  // Preenche a matriz com as respectivas cores
      }

      // Converte a matriz 3x3 em uma string
      char resultado[10];
      int k = 0;

      for (int r = 0; r < 3; r++)
      {
        for (int c = 0; c < 3; c++)
        {
          resultado[k++] = face[r][c];
        }
      }
      // Adiciona caractere de finalização da string
      resultado[k] = '\0';  

      // Envia o resultado pela serial
      Serial.println(resultado);
    }
  }
}



