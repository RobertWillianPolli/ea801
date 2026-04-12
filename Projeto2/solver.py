import kociemba

# Define o estado do cubo como uma string de 54 caractere.
# O cubo resolvido é na forma: "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB"
# Cada letra representa uma cor na posição da face X.
'''
             |************|
             |*U1**U2**U3*|
             |************|
             |*U4**U5**U6*|
             |************|
             |*U7**U8**U9*|
             |************|
 ************|************|************|************
 *L1**L2**L3*|*F1**F2**F3*|*R1**R2**R3*|*B1**B2**B3*
 ************|************|************|************
 *L4**L5**L6*|*F4**F5**F6*|*R4**R5**R6*|*B4**B5**B6*
 ************|************|************|************
 *L7**L8**L9*|*F7**F8**F9*|*R7**R8**R9*|*B7**B8**B9*
 ************|************|************|************
             |************|
             |*D1**D2**D3*|
             |************|
             |*D4**D5**D6*|
             |************|
             |*D7**D8**D9*|
             |************|

'''
# U   -> Giro simples da face superior no sentido horário
# U2  -> Giro duplo da face superior no sentido horário
# U'  -> Girio simples da face superior no sentido anti-horário 

# R   -> Giro simples da face direita no sentido horário
# R2  -> Giro duplo da face direita no sentido horário
# R'  -> Giro duplo da face direita no sentido anti-horário

# F   -> Giro simples da face frontal no sentido horário
# F2  -> Giro duplo da face frontal no sentido horário
# F'  -> Girio simples da face frontal no sentido anti-horário 

# D   -> Giro simples da face inferior no sentido horário
# D2  -> Giro duplo da face inferior no sentido horário
# D'  -> Giro simples da face inferior no sentido anti-horário

# L   -> Giro simples da face esquerda no sentido horário
# L2  -> Giro duplo da face esquerda no sentido horário
# L'  -> Girio simples da face esquerda no sentido anti-horário 

# B   -> Giro simples da face traseira no sentido horário
# B2  -> Giro duplo da face traseira no sentido horário
# B'  -> Giro simples da face traseira no sentido anti-horário

# Ordem = superior (U), direita (R), frontal (F), inferior (D), esquerda (L), traseira (B)

converter = {"W":"U", 
             "R":"F", 
             "G":"L", 
             "Y":"D", 
             "O":"B", 
             "B":"R"}

superior = "GYGYWBOBW"            # branco    (U)
frontal = "YRRWRGGYR"             # vermelho  (F)
esquerda = "OGBYGROWR"            # verde     (L)
inferior = "WRGOYOBGR"            # amarelo   (D)
direita = "BWOWBOYGB"             # azul      (R)
traseira = "YBWBOOYRW"            # laranja   (B)

atual = superior + direita + frontal + inferior + esquerda + traseira

scrambled_cube = ""
scrambled_cube = "".join(converter[f] for f in atual)

# Solve the cube
solution = kociemba.solve(scrambled_cube).split(' ')

print(solution)

for x in solution:
  if x == "U":
    print("SUPERIOR sentido horário")

  elif x == "U2":
    print("SUPERIOR sentido horário duas vezes")

  elif x == "U'":
    print("SUPERIOR sentido anti-horário")

  elif x == "R":
    print("DIREITA sentido horário")
  
  elif x == "R2":
    print("DIREITA sentido horário duas vezes")
  
  elif x == "R'":
    print("DIREITA sentido anti-horário")
  
  elif x == "F":
    print("FRONTAL sentido horário")
  
  elif x == "F2":
    print("FRONTAL sentido horário duas vezes")
  
  elif x == "F'":
    print("FRONTAL sentido anti-horário")

  elif x == "D":
    print("INFERIOR sentido horário")
  
  elif x == "D2":
    print("INFERIOR sentido horário duas vezes")
  
  elif x == "D'":
    print("INFERIOR sentido anti-horário")

  elif x == "L":
    print("ESQUERDA sentido horário")
  
  elif x == "L2":
    print("ESQUERDA sentido horário duas vezes")
  
  elif x == "L'":
    print("ESQUERDA sentido anti-horário")

  elif x == "B":
    print("TRASEIRA sentido horário")
  
  elif x == "B2":
    print("TRASEIRA sentido horário duas vezes")
  
  elif x == "B'":
    print("TRASEIRA sentido anti-horário")
  
  input()
