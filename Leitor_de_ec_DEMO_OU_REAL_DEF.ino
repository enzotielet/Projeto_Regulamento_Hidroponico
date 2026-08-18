#include <WiFiS3.h>
//#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Servo.h>
#define DEMONSTRACAO 0 
//================ WIFI =================
char ssid[] = "Internet...";
char pass[] = "aaaa3333";

//================ NTP =================
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -3 * 3600, 60000);

//================ SERVOS =================
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

//================ MEDIA MOVEL =================
const int N = 10;
float buffer[N] = {0};
int posBuffer = 0;
float soma = 0;

//================ CONTROLE HORARIO =================
int ultimoResultadoEC = 0;
int ultimoMinutoVerificado = -1;
int ultimoDiaServo = -1;
int Hora_Ativacao = 20;
int Minuto_Ativacao = 0;

const int horariosVerificacao[10][2] = {
  {17, 15},
  {17, 16},
  {17, 17},
  {17, 18},
  {17, 19},
  {17, 20},
  {17, 21},
  {17, 22},
  {17, 23},
  {17, 24}
};

//====================================================

void setup() {

  Serial.begin(115200);
  Serial.println("bound setado");

  servo1.attach(9);
  servo2.attach(10);
  servo3.attach(11);
  servo4.attach(12);

  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);
  Serial.println("servos setados");

  //================ WIFI CONNECT =================
  WiFi.begin(ssid, pass);
  Serial.println("loggin e senha passado para funcao");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("loop wifi conexao");
  }

  Serial.println("Conectado ao WiFi");

  //================ NTP START =================
  timeClient.begin();
  timeClient.update();
}

//====================================================

void loop() {
//Serial.println("entrou no loop");
  timeClient.update();

  int hora = timeClient.getHours();
  int minuto = timeClient.getMinutes();

  // Como NTPClient não fornece dia direto confiável sem parsing,
  // usamos timestamp bruto
  unsigned long epoch = timeClient.getEpochTime();
  int dia = (epoch / 86400L) % 30; // aproximação de controle de dia

  //================ VERIFICA EC =================
  for (int i = 0; i < 10; i++) {

    if (hora == horariosVerificacao[i][0] &&
        minuto == horariosVerificacao[i][1] &&
        ultimoMinutoVerificado != minuto) {

      ultimoResultadoEC = verificarEC();

      Serial.print("Resultado EC = ");
      Serial.println(ultimoResultadoEC);

      ultimoMinutoVerificado = minuto;
    }
  }

  // reset do controle de minuto
  static int minutoAnterior = -1;

  if (minuto != minutoAnterior) {
    minutoAnterior = minuto;

    if (minuto != ultimoMinutoVerificado) {
      ultimoMinutoVerificado = -1;
    }
  }

  //================ EXECUÇÃO DOS SERVOS =================
#ifdef DEMONSTRACAO
if(le_btt(A1)){
  executarServos(1);
  }
#else
if(hora == Hora_Ativacao && minuto == Minuto_Ativacao && ultimoDiaServo != dia) {
    ultimoDiaServo = dia;
    executarServos(ultimoResultadoEC);
  }
#endif
  delay(1000);
}

//====================================================
// LEITURA EC
//====================================================

int verificarEC() {
  Serial.println("entrou no ec");
  float somaEC = 0;

  for (int i = 0; i < 20; i++) {

    int leitura = analogRead(A0);

    float tensao = leitura * 5.0 / 1023.0;

    float ec = 1.8014 * tensao - 0.0453786;

    somaEC += ec;

    delay(100);
  }

  float ecMedio = somaEC / 20.0;

  Serial.print("EC medio = ");
  Serial.println(ecMedio);

  if (ecMedio < 2.0)
    return 1; // EC baixo

  else if (ecMedio > 3.0)
    return 2; // EC alto

  else
    return 0; // EC OK
}

//====================================================
// EXECUÇÃO DOS SERVOS
//====================================================

void executarServos(int resultado) {

  switch (resultado) {

    case 0:
      Serial.println("EC OK");
      break;

    case 1:
      Serial.println("EC baixo - adicionando nutrientes");

      AtuaServo(servo1, 1000);
      AtuaServo(servo2, 1000);
      AtuaServo(servo3, 1000);
      AtuaServo(servo4, 1000);
      break;

    case 2:
      Serial.println("EC alto");
      break;

    default:
      Serial.println("Resultado desconhecido");
      break;
  }
}

//====================================================

void AtuaServo(Servo &servo, int dl) {

  servo.write(30);
  delay(dl);
  servo.write(0);
}

//====================================================
bool le_btt(int btt) {
int a[5];
a[0]=0;
a[4]=0;
a[2]=0;
a[3]=0;
a[1]=0;
a[0]=analogRead(btt);
a[4] = 0.5*a[3];
a[2] = 1.2*a[1];
a[3] = 0.8*a[2];
a[1] = 1.4*a[0];

int result = ((a[0]+a[1]+a[2]+a[3])/15);
if(result>10){
return true;
}else{
return false;
}
}