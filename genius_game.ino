/************************\
**     GENIUS GAME      **
*                        *
** Made by Lucas Canejo **
\************************/

//Adiciona biblioteca para EEPROM
#include <EEPROM.h>

//defines das portas 
#define RLED 2
#define RBOT 10
#define BLED 3
#define BBOT 11
#define YLED 4
#define YBOT 12
#define GLED 5
#define GBOT 13
#define BUZZER 6


//quantidade de cores a serem piscadas
#define QTD 50

//Enumeração de estados possiveis do jogo
enum Estados{
  PRONTO_PARA_PROX_RODADA,
  USUARIO_RESPONDENDO,
  JOGO_FINALIZADO_SUCESSO,
  JOGO_FINALIZADO_FALHA
};

//Variavel com a quantidade de pontos da jogada
int pontos = 0;

//Variavel que salva o recorde
int recorde = 0;

// Vetor que define a sequencia de leds
int sequencia[QTD];

// Variavel do numero da rodada
int rodada = 0;

// Variavel que conta quantos leds foram respondidos na rodada
int leds_respondidos = 0;

//Roda uma vez ao programa iniciar
void setup() {
  Serial.begin(9600);
  defPortas();
  inicia();
  limpa();
  carregaRecorde();
}


//Roda varias vezes durante o programa
void loop() {
  switch(estadoAtual()){
    case PRONTO_PARA_PROX_RODADA:
      Serial.println("pronto para proxima rodada");
      preparaNovaRodada();
      break;
    case USUARIO_RESPONDENDO:
      Serial.println("usuario respondendo");
      processaResposta();
      break;
    case JOGO_FINALIZADO_SUCESSO:
      sucesso();
      break;
    case JOGO_FINALIZADO_FALHA:
      falha();
      break;
  }
  checaRecorde();
  delay(500);
}

// Função para tocar os leds em sequencia
void tocaLeds(){
  for(int i = 0; i < rodada; i++){
    piscaLed(sequencia[i]);
  }
}


// Função que prepara uma nova rodada para ser jogada
void preparaNovaRodada(){
  rodada++;
  if(rodada <= QTD){
  tocaLeds();
  }
  leds_respondidos = 0;    
}

// Função que processa a reposta do usuario
void processaResposta(){
    int resposta = checkresposta();
    if(resposta == -1){
      return;
    }
    if(resposta == sequencia[leds_respondidos]){
    leds_respondidos++;
    pontos++;
    }else{
      Serial.println("Resposta errada");
      rodada = QTD + 2;
    }
}

//Computa estado atual
int estadoAtual(){
  if(rodada <= QTD){
    if(leds_respondidos == rodada){
     return PRONTO_PARA_PROX_RODADA; 
    }else{
      return USUARIO_RESPONDENDO;
    } 
  }else if(rodada == QTD+1){
    return JOGO_FINALIZADO_SUCESSO;   
  }else{
    return JOGO_FINALIZADO_FALHA;
  }
}

//Função que dita a sequencia do jogo 
void inicia(){
  int jogo = analogRead(0);
  randomSeed(jogo);
  for(int i = 0; i < QTD; i++){
    sequencia[i] = sorteio(); 
  } 
}

//Função para limpar a memoria do EEPROM, porém ainda falta algumas alteracoes nela
void limpa(){
  if(digitalRead(BBOT) == LOW && digitalRead(GBOT) == LOW){
    Serial.print("Limpando...\n");
    for (int i = 0 ; i < EEPROM.length() ; i++) {
      EEPROM.write(i, 0);
    }
  Serial.print("Memoria limpa\n");   
  }
 
}

//Função que carrega o recorde da memoria
void carregaRecorde(){
  recorde = EEPROM.read(0); 
  Serial.print("Recorde atual: ");
  Serial.print(recorde);
  Serial.print(" pontos\n"); 
}


//Função que checa o recorde e confere se voce bateu o recorde atual
void checaRecorde(){
  if(pontos > recorde){
    Serial.print("Voce bateu o recorde\n");
    recorde = pontos;
    EEPROM.write(0,recorde);
  }
}

//Função que retorna os leds que irão piscar por sorteio
int sorteio(){
  return random(RLED, GLED + 1); // 2 a 5
}

//função que define as portas digitais
void defPortas(){
  pinMode(RLED,OUTPUT);
  pinMode(RBOT,INPUT_PULLUP);
  pinMode(BLED,OUTPUT);
  pinMode(BBOT,INPUT_PULLUP);
  pinMode(YLED,OUTPUT);
  pinMode(YBOT,INPUT_PULLUP);
  pinMode(GLED,OUTPUT);
  pinMode(GBOT,INPUT_PULLUP);
  pinMode(BUZZER,OUTPUT);
}


// Função que checa cada botão e retorna o estado do mesmo 
int checkresposta(){
  if(digitalRead(RBOT) == LOW)
  {
    return piscaLed(RLED);
  }
  if(digitalRead(BBOT) == LOW)
  {
    return piscaLed(BLED);
  }
  if(digitalRead(YBOT) == LOW)
  {
    return piscaLed(YLED);
  }
  if(digitalRead(GBOT) == LOW)
  {
    return piscaLed(GLED);
  }

  return -1;
}

// Função que faz todas leds piscarem juntos e depois apagarem em caso de sucesso do jogador
void sucesso(){
  digitalWrite(BLED,HIGH);
  digitalWrite(RLED,HIGH);
  digitalWrite(YLED,HIGH);
  digitalWrite(GLED,HIGH);
  tone(BUZZER,300);
  delay(2000);
  noTone(BUZZER);
  digitalWrite(BLED,LOW);
  digitalWrite(RLED,LOW);
  digitalWrite(YLED,LOW);
  digitalWrite(GLED,LOW);
  delay(500);
  Serial.print(pontos);
  Serial.print(" pontos\n");  
}

// Pisca o led vermelho e toca o buzzer quando o jogador falhar
void falha(){
  Serial.println("ERROUUUUUUUU otaro");
  piscaLed(RLED);
  tone(BUZZER,200);
  delay(1000);
  noTone(BUZZER);
  tone(BUZZER,5000);
  delay(1000);
  noTone(BUZZER);
  Serial.print(recorde);
  Serial.print("\n");
  Serial.print(pontos);
  Serial.print(" pontos\n");  
}

// Pisca qualquer led escolhido
int piscaLed(int Led){
  digitalWrite(Led,HIGH);
  delay(1000);
  digitalWrite(Led,LOW);
  delay(500);
  return Led;
}


