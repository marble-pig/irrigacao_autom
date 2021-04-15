#include <LiquidCrystal.h>
#include <ezButton.h>
#include <RTClib.h>

//variaveis globais
int tela = 1;
int DataHorAtual[5]; //[Hora, Minuto, Dia, Mes, Ano]
int HorAbreValv1[2], HorFechValv1[2], HorAbreValv2[2], HorFechValv2[2]; //[Hora, Minuto]
bool statusV1 = false, statusV2 = false; //false = 0 = fechado / true = 1 = aberto
bool vira0HV1, vira0HV2; //false = abre e fecha no mesmo dia / true = abre em um dia e fecha em outro
bool novaTela = false;

//entradas e saídas
const int ent_bCima = 6, ent_bBaixo = 7, ent_bEsq = 8, ent_bDir = 9;
const int saiV1 = 0, saiV2 = 1;
ezButton btnCima(ent_bCima);
ezButton btnBaixo(ent_bBaixo);
ezButton btnSelect(ent_bDir);
ezButton btnSair(ent_bEsq);

//Configuração RTC
RTC_DS1307 rtc;

//declara tela LCD
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void horaFalsa(){
	DataHorAtual[0] = 9;
	DataHorAtual[1] = 42;
	DataHorAtual[2] = 15;
	DataHorAtual[3] = 4;
	DataHorAtual[4] = 2021;
}

void setup() {
	Serial.begin(9600);
	lcd.begin(16, 2);
	lcd.noCursor();

	if (! rtc.begin()) {
		lcd.print("RTC NAO INICIALIZADO");
		horaFalsa();
		//while(1);
	}
	else lcd.print("Inicializando...");

	pinMode(saiV1, OUTPUT); //valvula 1
	pinMode(saiV2, OUTPUT); //valvula 2
	btnCima.setDebounceTime(50);
	btnBaixo.setDebounceTime(50);
	btnSelect.setDebounceTime(50);
	btnSair.setDebounceTime(50);

	//rtc.adjust(DateTime(2021, 4, 15, 18, 0, 0));
}

void loopBtn(){
	//inicialização necessária dos botões
	btnCima.loop();
	btnBaixo.loop();
	btnSelect.loop();
	btnSair.loop();
}

void AlteraValv(int p_valv, int p_comando) {
	/* Comandos pra abrir e fechar válvulas */
}

void pegaHorario() {
	/* Busca hora gravada no módulo RTC */
	DateTime agora = rtc.now();

	DataHorAtual[0] = agora.hour();
	DataHorAtual[1] = agora.minute();
	DataHorAtual[2] = agora.day();
	DataHorAtual[3] = agora.month();
	DataHorAtual[4] = agora.year();
}

void escreveHora(int p_Hora, int p_Minuto) {
	if (p_Hora < 10) lcd.write("0");
	lcd.print(p_Hora);
	lcd.write(":");
	if (p_Minuto < 10) lcd.write("0");
	lcd.print(p_Minuto);
}

void escreveData(int p_Dia, int p_Mes, int p_Ano) {
	if (p_Dia < 10) lcd.write("0");
	lcd.print(p_Dia);
	lcd.write("/");
	if (p_Mes < 10) lcd.write("0");
	lcd.print(p_Mes);
	lcd.write("/");
	lcd.print(p_Ano);
}

void telaAjusteN1() {
	int posAtual = 1, HHMM = 0;

	Serial.println("Tela Ajuste 1");
	lcd.setCursor(0, 1);
	lcd.print("                ");
	lcd.cursor();
	lcd.blink();
	while (!btnSair.isPressed()) {
		loopBtn();

		lcd.setCursor(posAtual, 0);
		if (btnCima.isPressed()) {
			DataHorAtual[HHMM] ++;
			if (DataHorAtual[1] > 59) DataHorAtual[1] = 0;
			else if (DataHorAtual[0] > 23) DataHorAtual[0] = 0;
			else if (DataHorAtual[2] > 31) DataHorAtual[2] = 1;
			else if (DataHorAtual[3] > 12) DataHorAtual[3] = 1;
			lcd.setCursor(0, 0);
			escreveHora(DataHorAtual[0], DataHorAtual[1]);
			lcd.write(" ");
			escreveData(DataHorAtual[2], DataHorAtual[3], DataHorAtual[4]);
		}
		else if (btnBaixo.isPressed()) {
			DataHorAtual[HHMM] --;
			if (DataHorAtual[HHMM] < 1) {
				switch (HHMM)
				{
				case 0:
					if (DataHorAtual[0] < 0) DataHorAtual[0] = 23;
					break;
				case 1:
					if (DataHorAtual[1] < 0) DataHorAtual[1] = 59;
					break;
				case 2:
					DataHorAtual[2] = 31;
					break;
				case 3:
					DataHorAtual[3] = 12;
				default:
					break;
				}
			}
			lcd.setCursor(0, 0);
			escreveHora(DataHorAtual[0], DataHorAtual[1]);
			lcd.write(" ");
			escreveData(DataHorAtual[2], DataHorAtual[3], DataHorAtual[4]);
		}
		else if (btnSelect.isPressed()) {
			HHMM++;
			switch (HHMM)
			{
			case 0: //hora
				posAtual = 1;
				break;
			case 1: //minuto
				posAtual = 4;
			case 2: //dia
				posAtual = 7;
			case 3: //mes
				posAtual = 10;
			case 4: //ano
				posAtual = 13;
			default:
				HHMM = 0;
				posAtual = 1;
				break;
			}
		}
	}
	lcd.noBlink();
	lcd.noCursor();

	/* Escrever hora no módulo RTC */
	rtc.adjust(DateTime(DataHorAtual[4], DataHorAtual[3], DataHorAtual[2], DataHorAtual[0], DataHorAtual[1], 0));
}

void telaAjusteN2() {
}

void telaAjusteN3(){
}

void telaN1() {
	/* Mostrar tela 1
	Opções de ajuste */
	char charV1[4], charV2[4];
	String strLig = "Lig", strDesl = "Desl";

	if (statusV1) strLig.toCharArray(charV1, 4);
	else strDesl.toCharArray(charV1, 4);
	if (statusV2) strLig.toCharArray(charV2, 4);
	else strDesl.toCharArray(charV2, 4);

	if (novaTela) {
		Serial.println("Tela 1");
		lcd.clear();
		novaTela = false;
	}
	lcd.setCursor(0, 0);
	escreveHora(DataHorAtual[0], DataHorAtual[1]);
	lcd.write(" ");
	escreveData(DataHorAtual[2], DataHorAtual[3], DataHorAtual[4]);
	lcd.setCursor(0, 1);
	lcd.print("V1:");
	lcd.setCursor(3, 1);
	lcd.print(charV1);
	lcd.setCursor(9, 1);
	lcd.print("V2:");
	lcd.setCursor(12, 1);
	lcd.print(charV2);

	if (btnSelect.isPressed()) telaAjusteN1();
}

void telaN2() {
	/* Mostrar tela 2
	Opções de ajuste */
	if (novaTela) {
		Serial.println("Tela 2");
		lcd.clear();
		novaTela = false;
	}
	lcd.setCursor(0, 0);
	lcd.print("Valvula 1");
	lcd.setCursor(0, 1);
	escreveHora(HorAbreValv1[0], HorAbreValv1[1]);
	lcd.write(" <-> ");
	escreveHora(HorFechValv1[0], HorFechValv1[1]);

	if(btnSelect.isPressed()) telaAjusteN2();
}

void telaN3() {
	/* Mostrar tela 3
	Opções de ajuste */
	if (novaTela) {
		Serial.println("Tela 3");
		lcd.clear();
		novaTela = false;
	}
	lcd.setCursor(0, 0);
	lcd.print("Valvula 2");
	lcd.setCursor(0, 1);
	escreveHora(HorAbreValv2[0], HorAbreValv2[1]);
	lcd.write(" <-> ");
	escreveHora(HorFechValv2[0], HorFechValv2[1]);

	if(btnSelect.isPressed()) telaAjusteN3();
}

void confereProg() {
	int HHMMatual, HHMMabreV1, HHMMfechaV1, HHMMabreV2, HHMMfechaV2;

	HHMMatual = DataHorAtual[0] * 100 + DataHorAtual[1];
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

void testeBotoes() {
	if(btnSair.isPressed()) Serial.println("Botão Sair pressionado");
	else if(btnCima.isPressed()) Serial.println("Botão Cima pressionado");
	else if(btnBaixo.isPressed()) Serial.println("Botão Baixo pressionado");
	else if(btnSelect.isPressed()) Serial.println("Botão Select pressionado");
}

void mudaTela() {
	if(btnCima.isPressed()) {
		tela++;
		novaTela = true;
	}
	else if(btnBaixo.isPressed()) {
		tela--;
		novaTela = true;
	}
}

void loop() {
	loopBtn();
	//testeBotoes();

	pegaHorario();

	mudaTela();
	switch (tela)
	{
	case 0:
		tela = 3;
		break;
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
