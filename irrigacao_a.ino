#include <LiquidCrystal.h>
#include <ezButton.h>
#include <RTClib.h>

//variaveis globais
int tela = 1;
int HorAtual[3]; //[Hora, Minuto, Segundo]
int HorAbreValv1[2], HorFechValv1[2], HorAbreValv2[2], HorFechValv2[2]; //[Hora, Minuto]
bool statusV1, statusV2; //false = 0 = aberto / true = 1 = fechado
bool vira0HV1, vira0HV2; //false = abre e fecha no mesmo dia / true = abre em um dia e fecha em outro

//entradas e saídas
const int ent_bCima = 6, ent_bBaixo = 7, ent_bEsq = 8, ent_bDir = 9;
const int saiV1 = 0, saiV2 = 1;
ezButton btnCima(ent_bCima);
ezButton btnBaixo(ent_bBaixo);
ezButton btnSelect(ent_bDir);
ezButton btnSair(ent_bEsq);

//declara tela LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
	lcd.begin(16, 2);
	lcd.noCursor();

	pinMode(saiV1, OUTPUT); //valvula 1
	pinMode(saiV2, OUTPUT); //valvula 2
	btnCima.setDebounceTime(50);
	btnBaixo.setDebounceTime(50);
	btnSelect.setDebounceTime(50);
	btnSair.setDebounceTime(50);
}

void AlteraValv(int p_valv, int p_comando) {
	/* Comandos pra abrir e fechar válvulas */
}

void pegaHorario() {
	/* Busca hora gravada no módulo RTC */
}

void escreveHora(int p_Hora, int p_Minuto) {
	if (p_Hora < 10) lcd.write("0");
	lcd.print(p_Hora);
	lcd.write(":");
	if (p_Minuto < 10) lcd.write("0");
	lcd.print(p_Minuto);
}

void telaAjusteN1() {
	int posAtual = 1, HHMM = 0;

	lcd.setCursor(0, 0);
	lcd.cursor();
	lcd.blink();
	while (!btnSair.isPressed()) {
		lcd.setCursor(0, posAtual);
		if (btnCima.isPressed()) {
			HorAtual[HHMM] ++;
			lcd.setCursor(0, 0);
			escreveHora(HorAtual[0], HorAtual[1]);
		}
		if (btnBaixo.isPressed()) {
			HorAtual[HHMM] --;
			lcd.setCursor(0, 0);
			escreveHora(HorAtual[0], HorAtual[1]);
		}
		
		if (btnSelect.isPressed()) {
			if (HHMM == 0) {
				HHMM = 1;
				posAtual = 4
			}
			else {
				HHMM = 0;
				posAtual = 1;
			}
		}
	}
	lcd.noBlink();
	lcd.noCursor();

	/* Escrever hora no módulo RTC */
}

void telaN1() {
	/* Mostrar tela 1
	Opções de ajuste */
	char charV1[4], charV2[4];

	if (statusV1) charV1 = "Lig";
	else charV1 = "Desl";
	if (statusV2) charV2 = "Lig";
	else charV2 = "Desl";

	lcd.clear();
	escreveHora(HorAtual[0], HorAtual[1]);
	lcd.setCursor(1, 0);
	lcd.print("V1:");
	lcd.setCursor(1, 3);
	lcd.print(charV1);
	lcd.setCursor(1, 9);
	lcd.print("V2:");
	lcd.setCursor(1, 12);
	lcd.print(charV2);

	if (btnSelect.isPressed()) telaAjusteN1();
}

void telaN2() {
	/* Mostrar tela 2
	Opções de ajuste */
	lcd.clear();
	lcd.print("Valvula 1");
	lcd.setCursor(1, 0);
	escreveHora(HorValv1[0], HorValv1[1]);
	lcd.write(" <-> ");
	escreveHora(HorVal1[2], HorValv1[3]);

	if(btnSelect.isPressed()) telaAjusteN2();
}

void telaN3() {
	/* Mostrar tela 3
	Opções de ajuste */
	lcd.clear();
	lcd.print("Valvula 2");
	lcd.setCursor(1, 0);
	escreveHora(HorValv2[0], HorValv2[1]);
	lcd.write(" <-> ");
	escreveHora(HorVal2[2], HorValv2[3]);

	if(btnSelect.isPressed()) telaAjusteN3();
}

void confereProg() {
	int HHMMatual, HHMMabreV1, HHMMfechaV1, HHMMabreV2, HHMMfechaV2;

	HHMMatual = HorAtual[0] * 100 + HorAtual[1];
	HHMMabreV1 = HorAbreValv1[0] * 100 + HorAbreValv1[1];
	HHMMfechaV1 = HorFechValv1[0] * 100 + HorFechValv1[1];
	HHMMabreV2 = HorAbreValv2[0] * 100 + HorAbreValv2[1];
	HHMMfechaV2 = HorFechValv2[0] * 100 + HorFechValv2[1];

	if (!vira0HV1) {
		if ((HHMMatual >= HHMMabreV1 && HHMMatual <= HHMMfechaV1) && statusV1)
			AlteraValv(1, 1); //Abrir valvula 1
		else if ((HHMMatual <= HHMMabreV1 && HHMMatual >= HHMMfechaV1) && !statusV1)
			AlteraValv(1, 0); //Fechar valvula 1
	}
	else  {
		if ((HHMMatual <= HHMMabreV1 && HHMMatual >= HHMMfechaV1) && statusV1)
			AlteraValv(1, 1); //Abrir valvula 1
		else if ((HHMMatual >= HHMMabreV1 && HHMMatual <= HHMMfechaV1) && !statusV1)
			AlteraValv(1, 0); //Fechar valvula 1
	}
	if (!vira0HV2) {
		if ((HHMMatual >= HHMMabreV2 && HHMMatual <= HHMMfechaV2) && statusV2)
			AlteraValv(2, 1); //Abrir valvula 1
		else if ((HHMMatual <= HHMMabreV2 && HHMMatual >= HHMMfechaV2) && !statusV2)
			AlteraValv(2, 0); //Fechar valvula 1
	}
	else {
		if ((HHMMatual <= HHMMabreV2 && HHMMatual >= HHMMfechaV2) && statusV2)
			AlteraValv(2, 1); //Abrir valvula 1
		else if ((HHMMatual >= HHMMabreV2 && HHMMatual <= HHMMfechaV2) && !statusV2)
			AlteraValv(2, 0); //Fechar valvula 1
	}
}

void loop() {
	//inicialização necessária dos botões
	btnCima.loop();
	btnBaixo.loop();
	btnSelect.loop();
	btnSair.loop();

	pegaHorario();

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