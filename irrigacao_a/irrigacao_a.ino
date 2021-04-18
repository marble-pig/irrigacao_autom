#include <LiquidCrystal.h>
#include <ezButton.h>
#include <RTClib.h>

//variaveis globais
int tela = 1;
int DataHorAtual[5]; //[Hora, Minuto, Dia, Mes, Ano]
int HorAbreValv[2][2], HorFechValv[2][2]; //[Valv1[Hora, Minuto], Valv2[Hora, Minuto]]
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

void telaAjusteIni() {
	int posAtual, HHMM = 0, valorMax, valorMin;
	bool mudaHHMM = true;

	Serial.println("Tela Ajuste 1");
	lcd.setCursor(0, 1);
	lcd.print("                ");
	//lcd.cursor();
	lcd.blink();
	while (!btnSair.isPressed()) {
		loopBtn();
		if (mudaHHMM) {
			switch (HHMM)
			{
			case 0:
				valorMin = 0;
				valorMax = 23;
				posAtual = 1;
				break;
			case 1:
				valorMin = 0;
				valorMax = 59;
				posAtual = 4;
				break;
			case 2:
				valorMin = 1;
				if (DataHorAtual[3] == 1 || DataHorAtual[3] == 3 || DataHorAtual[3] == 5 || DataHorAtual[3] == 7 || DataHorAtual[3] == 8 || DataHorAtual[3] == 10 || DataHorAtual[3] == 12)
					valorMax = 31;
				else if (DataHorAtual[3] == 4 || DataHorAtual[3] == 6 || DataHorAtual[3] == 9 || DataHorAtual[3] == 11)
					valorMax = 30;
				else
					valorMax = 29;
				posAtual = 7;
				break;
			case 3:
				valorMin = 1;
				valorMax = 12;
				posAtual = 10;
				break;
			case 4:
				valorMin = 2020;
				valorMax = 2100;
				posAtual = 15;
				break;
			default:
				HHMM = 0;
				mudaHHMM = true;
				break;
			}
			mudaHHMM = false;
		}

		lcd.setCursor(posAtual, 0);
		if (btnCima.isPressed()) {
			DataHorAtual[HHMM] ++;
			if (DataHorAtual[HHMM] > valorMax) DataHorAtual[HHMM] = valorMin;
			lcd.setCursor(0, 0);
			escreveHora(DataHorAtual[0], DataHorAtual[1]);
			lcd.write(" ");
			escreveData(DataHorAtual[2], DataHorAtual[3], DataHorAtual[4]);
		}
		else if (btnBaixo.isPressed()) {
			DataHorAtual[HHMM] --;
			if (DataHorAtual[HHMM] < valorMin) DataHorAtual[HHMM] = valorMax;
			lcd.setCursor(0, 0);
			escreveHora(DataHorAtual[0], DataHorAtual[1]);
			lcd.write(" ");
			escreveData(DataHorAtual[2], DataHorAtual[3], DataHorAtual[4]);
		}
		else if (btnSelect.isPressed()) {
			HHMM++;
			mudaHHMM = true;
		}
	}
	lcd.noBlink();
	//lcd.noCursor();

	/* Escrever hora no módulo RTC */
	rtc.adjust(DateTime(DataHorAtual[4], DataHorAtual[3], DataHorAtual[2], DataHorAtual[0], DataHorAtual[1], 0));
}

void telaAjusteValv(int p_numValv) {
	int posAtual = 1, HHMM = 0, valorMax;

	Serial.println("Tela Ajuste 2");
	//lcd.cursor();
	lcd.blink();
	while (!btnSair.isPressed()){
		loopBtn();
		lcd.setCursor(posAtual, 1);

		if (HHMM == 0)
			valorMax = 23;
		else
			valorMax = 59;
		if (btnCima.isPressed()) {
			if (posAtual < 10) {
				HorAbreValv[p_numValv - 1][HHMM]++;
				if (HorAbreValv[p_numValv - 1][HHMM] > valorMax)
					HorAbreValv[p_numValv - 1][HHMM] = 0;
			}
			else {
				HorFechValv[p_numValv - 1][HHMM]++;
				if (HorFechValv[p_numValv - 1][HHMM] > valorMax)
					HorFechValv[p_numValv - 1][HHMM] = 0;
			}
			lcd.setCursor(0, 1);
			escreveHora(HorAbreValv[p_numValv - 1][0], HorAbreValv[p_numValv - 1][1]);
			lcd.write(" <-> ");
			escreveHora(HorFechValv[p_numValv - 1][0], HorFechValv[p_numValv - 1][1]);
		}
		else if (btnBaixo.isPressed()) {
			if (posAtual < 10) {
				HorAbreValv[p_numValv - 1][HHMM]--;
				if (HorAbreValv[p_numValv - 1][HHMM] < 0)
					HorAbreValv[p_numValv - 1][HHMM] = valorMax;
			}
			else {
				HorFechValv[p_numValv - 1][HHMM]--;
				if (HorFechValv[p_numValv - 1][HHMM] < 0)
					HorFechValv[p_numValv - 1][HHMM] = valorMax;
			}
			lcd.setCursor(0, 1);
			escreveHora(HorAbreValv[p_numValv - 1][0], HorAbreValv[p_numValv - 1][1]);
			lcd.write(" <-> ");
			escreveHora(HorFechValv[p_numValv - 1][0], HorFechValv[p_numValv - 1][1]);
		}
		else if (btnSelect.isPressed()){
			switch (posAtual)
			{
			case 1:
				posAtual = 4;
				HHMM = 1;
				break;
			case 4:
				posAtual = 11;
				HHMM = 0;
				break;
			case 11:
				posAtual = 14;
				HHMM = 1;
				break;
			case 14:
				posAtual = 1;
				HHMM = 0;
				break;
			default:
				posAtual = 1;
				HHMM = 0;
				break;
			}
		}
		pegaHorario();
		confereProg();
	}
	lcd.noBlink();
	//lcd.noCursor();
	novaTela = true;
}

void telaIni() {
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

	if (btnSelect.isPressed()) telaAjusteIni();
}

void telaValv(int p_numValv) {
	/* Mostrar tela de válvulas
	Opções de ajuste */
	if (novaTela) {
		Serial.print("Tela válvula ");
		Serial.println(p_numValv);
		lcd.clear();
		novaTela = false;
		lcd.setCursor(0, 0);
		lcd.print("Valvula ");
		lcd.print(p_numValv);
		lcd.setCursor(0, 1);
		escreveHora(HorAbreValv[p_numValv - 1][0], HorAbreValv[p_numValv - 1][1]);
		lcd.write(" <-> ");
		escreveHora(HorFechValv[p_numValv - 1][0], HorFechValv[p_numValv - 1][1]);
	}

	if(btnSelect.isPressed()) telaAjusteValv(p_numValv);
}

void confereProg() {
	int HHMMatual, HHMMabreV1, HHMMfechaV1, HHMMabreV2, HHMMfechaV2;

	HHMMatual = DataHorAtual[0] * 100 + DataHorAtual[1];
	HHMMabreV1 = HorAbreValv[0][0] * 100 + HorAbreValv[0][1];
	HHMMfechaV1 = HorFechValv[0][0] * 100 + HorFechValv[0][1];
	HHMMabreV2 = HorAbreValv[1][0] * 100 + HorAbreValv[1][1];
	HHMMfechaV2 = HorFechValv[1][0] * 100 + HorFechValv[1][1];

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
		telaIni();
		break;
	case 2:
		telaValv(1);
		break;
	case 3:
		telaValv(2);
		break;
	
	default:
		tela = 1;
		break;
	}

	confereProg();
}