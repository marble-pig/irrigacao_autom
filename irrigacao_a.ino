#include <LiquidCrystal.h>

//variaveis globais
int tela = 1;
int btnW, btnA, btnS, btnD;
int HorAtual[3]; //[Hora, Minuto, Segundo]
int HorValv1[4], HorValv2[4]; //[Hora abrir, Minuto abrir, Hora fechar, Minuto fechar]
bool statusV1, statusV2;

//entradas e saídas
const int ent_bCima = 6, ent_bBaixo = 7, ent_bEsq = 8, ent_bDir = 9;
const int saiV1 = 0, saiV2 = 1;

//declara tela LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
	lcd.begin(16, 2);

	pinMode(saiV1, OUTPUT); //valvula 1
	pinMode(saiV2, OUTPUT); //valvula 2
	pinMode(ent_bCima, INPUT_PULLUP); //botao para cima
	pinMode(ent_bBaixo, INPUT_PULLUP); //botao para baixo
	pinMode(ent_bEsq, INPUT_PULLUP); //botao para esquerda
	pinMode(ent_bDir, INPUT_PULLUP); ///botao para direita
}

void AlteraValv(int p_valv, int p_comando) {
	/* Comandos pra abrir e fechar válvulas */
}

void pegaHorario() {
	/* Busca hora gravada no módulo RTC */
}

void telaAjusteN1() {
	int posAtual = 0, HHMMSS = 0;

	lcd.setCursor(0, 0);
	lcd.blink();
	while (!btnA) {
		lcd.setCursor(0, posAtual);
		if (btnW) HorAtual[HHMMSS] ++;
		if (btnS) HorAtual[HHMMSS] --;
		
		if (btnD) {
			posAtual++;
			if (posAtual == 2 || posAtual == 5) {
				posAtual ++;
				HHMMSS ++;
			}
			else if (posAtual == 8) {
				posAtual = 0;
				HHMMSS = 0;
			}
		}
	}
}

void telaN1() {
	/* Mostrar tela 1 */
	/* Opções de ajuste */
	char charV1, charV2;

	if (statusV1) charV1 = 'A';
	else charV1 = 'F';
	if (statusV2) charV2 = 'A';
	else charV2 = 'F';

	lcd.setCursort(0, 0);
	lcd.setCursor(1, 1);
	lcd.print("V1 -");
	lcd.setCursor(1, 5);
	lcd.write(charV1);
	lcd.setCursor(1, 8);
	lcd.print("V2 -");
	lcd.setCursor(1, 13);
	lcd.write(charV2);

	if (btnD) telaAjusteN1();
}

void telaN2() {
	/* Mostrar tela 2 */
	/* Opções de ajuste */
}

void telaN3() {
	/* Mostrar tela 3 */
	/* Opções de ajuste */
}

void confereProg() {
	if (HorAtual[0] == HorValv1[0] && HorAtual[1] == HorValv1[1]) {
		AlteraValv(1, 1); //Abrir valvula 1
	}
	else if (HorAtual[0] == HorValv1[2] && HorAtual[1] == HorValv1[3]) {
		AlteraValv(1, 0); //Fechar valvula 1
	}

	if (HorAtual[0] == HorValv2[0] && HorAtual[1] == HorValv2[1]) {
		AlteraValv(2, 1); //Abrir valvula 2
	}
	else if (HorAtual[0] == HorValv2[2] && HorAtual[1] == HorValv2[3]) {
		AlteraValv(2, 0); //Fechar valvula 2
	}
}

void loop() {
	pegaHorario();
	btnW = digitalRead(ent_bCima);
	btnA = digitalRead(ent_bBaixo);
	btnS = digitalRead(ent_bEsq);
	btnD = digitalRead(ent_bDir);

	switch (tela)
	{
	case 1:
		telaN1();
		break;
	case 2:
		telaN2();
		break;
	case 3:
		telaN3();
		break;
	
	default:
		tela = 1;
		break;
	}

	confereProg();
}

/*
	Criar interrupt pra pegar o status dos botoes.
*/