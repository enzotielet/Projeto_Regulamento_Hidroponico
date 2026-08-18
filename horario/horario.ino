#include <WiFiS3.h>
#include <time.h>

char ssid[] = "SEU_WIFI";
char pass[] = "SUA_SENHA";

// Armazena o último resultado da medição de EC
int ultimoResultadoEC = 0;

// Controle para não executar mais de uma vez
int ultimoMinutoVerificado = -1;
int ultimoDiaServo = -1;

// Horários das 10 medições diárias
const int horariosVerificacao[10][2] = {
{0, 0},
{2, 24},
{4, 48},
{7, 12},
{9, 36},
{12, 0},
{14, 24},
{16, 48},
{19, 12},
{21, 36}
};

void setup() {
Serial.begin(115200);

WiFi.begin(ssid, pass);

while (WiFi.status() != WL_CONNECTED) {
delay(1000);
Serial.println("Conectando ao WiFi...");
}

configTime(-3 * 3600, 0, "pool.ntp.org");

struct tm timeinfo;
while (!getLocalTime(&timeinfo)) {
delay(1000);
}

Serial.println("Hora sincronizada.");
}

void loop() {

struct tm timeinfo;

if (!getLocalTime(&timeinfo)) {
delay(1000);
return;
}

int hora = timeinfo.tm_hour;
int minuto = timeinfo.tm_min;
int dia = timeinfo.tm_mday;

// Verificações de EC
for (int i = 0; i < 10; i++) {

if (hora == horariosVerificacao[i][0] &&
minuto == horariosVerificacao[i][1] &&
ultimoMinutoVerificado != minuto) {

ultimoResultadoEC = verificarEC();

Serial.print("Resultado EC: ");
Serial.println(ultimoResultadoEC);

ultimoMinutoVerificado = minuto;
}
}

// Reinicia o controle ao mudar o minuto
static int minutoAnterior = -1;
if (minuto != minutoAnterior) {
minutoAnterior = minuto;

if (minuto != ultimoMinutoVerificado) {
ultimoMinutoVerificado = -1;
}
}

// Executa os servos às 20:00
if (hora == 20 &&
minuto == 0 &&
ultimoDiaServo != dia) {

ultimoDiaServo = dia;

executarServos(ultimoResultadoEC);
}

delay(1000);
}


//=====================================================
// FUNÇÃO DE MEDIÇÃO DO EC
//=====================================================
int verificarEC() {

// Coloque aqui sua lógica de medição

int resultado = 0;

/*
Exemplo:

if (ec < 1.8)
resultado = 1;

else if (ec > 2.2)
resultado = 2;

else
resultado = 0;
*/

return resultado;
}


//=====================================================
// FUNÇÃO DOS SERVOS
//=====================================================
void executarServos(int resultado) {

switch (resultado) {

case 0:
Serial.println("EC OK");
break;

case 1:
Serial.println("Adicionar nutrientes");
// mover servo A
break;

case 2:
Serial.println("EC alto");
// mover servo B
break;

default:
Serial.println("Resultado desconhecido");
break;
}
}