#include <SPI.h>  
#include <Pixy.h>


Pixy pixy;


void setup()
{
  Serial.begin(9600);
  Serial.println("Starting...");
  pixy.init();
}


void loop()
{
  static int i = 0;
  uint16_t blocks;


  blocks = pixy.getBlocks();


  // precisa ter exatamente 9 blocos
  if (blocks == 9)
  {
    i++;


    // reduz frequência de print
    if (i % 30 == 0)
    {
      int j;


      int xmin = 10000, xmax = 0;
      int ymin = 10000, ymax = 0;


      // achar limites
      for (j = 0; j < blocks; j++)
      {
        int x = pixy.blocks[j].x;
        int y = pixy.blocks[j].y;


        if (x < xmin) xmin = x;
        if (x > xmax) xmax = x;
        if (y < ymin) ymin = y;
        if (y > ymax) ymax = y;
      }


      float dx = (xmax - xmin) / 3.0;
      float dy = (ymax - ymin) / 3.0;


      char face[3][3];


      // inicializa com '?'
      for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
          face[r][c] = '?';


      // preencher matriz
      for (j = 0; j < blocks; j++)
      {
        int x = pixy.blocks[j].x;
        int y = pixy.blocks[j].y;


        int col = (int)((x - xmin) / dx);
        int row = (int)((y - ymin) / dy);


        if (col < 0) col = 0;
        if (col > 2) col = 2;
        if (row < 0) row = 0;
        if (row > 2) row = 2;


        char cor;


        switch (pixy.blocks[j].signature)
        {
          case 1: cor = 'G'; break; // vermelho
          case 2: cor = 'Y'; break;
          case 3: cor = 'O'; break;
          case 4: cor = 'R'; break;
          case 5: cor = 'B'; break;
          case 6: cor = 'W'; break;
          default: cor = '?';
        }


        face[row][col] = cor;
      }


      // gerar string final
      char resultado[10]; // 9 + '\0'
      int k = 0;


      for (int r = 0; r < 3; r++)
      {
        for (int c = 0; c < 3; c++)
        {
          resultado[k++] = face[r][c];
        }
      }


      resultado[k] = '\0';


      // print organizado
      Serial.println("Face detectada:");
      for (int r = 0; r < 3; r++)
      {
        for (int c = 0; c < 3; c++)
        {
          Serial.print(face[r][c]);
          Serial.print(" ");
        }
        Serial.println();
      }


      Serial.print("Sequencia: ");
      Serial.println(resultado);
      Serial.println("-----------------");
    }
  }
}



