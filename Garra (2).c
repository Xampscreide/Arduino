#include <Servo.h>

Servo garra,base,direita,esquerda;

#define Pot1 A0
#define Pot2 A1
#define Pot3 A2
#define Pot4 A3

#define Garra 8
#define Base 6
#define Direita 3
#define Esquerda 2

#define Repetir 10
#define Lembrar 11

void setup()
{
	Serial.begin(9600);
	delay(2000);
	garra.attach(Garra);
	base.attach(Base);
	direita.attach(Direita);
	esquerda.attach(Esquerda);
	pinMode(Repetir,INPUT);
	pinMode(Lembrar,INPUT);
}

int i = 0;
int Angulos[4][6];

void loop()
{
	//Le os angulos atraves dos potenciometros
	int AGarra = map(analogRead(Pot1),0,1023,100,160);
	int ABase = map(analogRead(Pot2),0,1023,0,180);
	int ADireita = map(analogRead(Pot3),0,1023,105,155);
	int AEsquerda = map(analogRead(Pot4),0,1023,90,173);
	
	//Move a Garra
	garra.write(AGarra);
	base.write(ABase);
	direita.write(ADireita);
	esquerda.write(AEsquerda);
	
	if(digitalRead(Lembrar)==HIGH && i<6)
	{
		Serial.print("Gravando\n");
		//Grava as posicoes atuais no vetor
		Angulos[0][i] = AGarra;
		Angulos[1][i] = ABase;
		Angulos[2][i] = ADireita;
		Angulos[3][i] = AEsquerda;
		i++;
		while(digitalRead(Lembrar)==HIGH)
			delay(100);
		delay(600);
		Serial.print("Parou De Gravar\n");
	}
	
	if(digitalRead(Repetir)==HIGH && i>0)
	{
		Serial.print("Iniciou Repeticao\n");
		//Calcula a diferenca em 40 partes
		double Dif[4] = {(AGarra-Angulos[0][0])/40.0,(ABase-Angulos[1][0])/40.0,(ADireita-Angulos[2][0])/40.0,(AEsquerda-Angulos[3][0])/40.0};
		for(int j=1;j<40;j++)//Incrementa as 40 partes
		{
			garra.write(AGarra-(int)ceil(j*Dif[0]));
			delay(18);
			base.write(ABase-(int)ceil(j*Dif[1]));
			delay(18);
			direita.write(ADireita-(int)ceil(j*Dif[2]));
			delay(18);
			esquerda.write(AEsquerda-(int)ceil(j*Dif[3]));
			delay(18);
		}
		
		//Garante a Posicao Final Caso Imprecisao do Double
		garra.write(Angulos[0][0]);
		delay(12);
		base.write(Angulos[1][0]);
		delay(12);
		direita.write(Angulos[2][0]);
		delay(12);
		esquerda.write(Angulos[3][0]);
		delay(12);
		
		
		for(int j=1;j<i;j++)
		{
			//Calcula a diferenca em 40 partes
			double Dif2[4] = {(Angulos[0][j-1]-Angulos[0][j])/40.0,(Angulos[1][j-1]-Angulos[1][j])/40.0,(Angulos[2][j-1]-Angulos[2][j])/40.0,(Angulos[3][j-1]-Angulos[3][j])/40.0};
			for(int k=1;k<40;k++)//Incrementa as 40 partes
			{
				garra.write(Angulos[0][j-1]-(int)ceil(k*Dif2[0]));
				delay(18);
				base.write(Angulos[1][j-1]-(int)ceil(k*Dif2[1]));
				delay(18);
				direita.write(Angulos[2][j-1]-(int)ceil(k*Dif2[2]));
				delay(18);
				esquerda.write(Angulos[3][j-1]-(int)ceil(k*Dif2[3]));
				delay(18);
			}
			
			//Garante a Posicao Final Caso Imprecisao do Double
			garra.write(Angulos[0][j]);
			delay(12);
			base.write(Angulos[1][j]);
			delay(12);
			direita.write(Angulos[2][j]);
			delay(12);
			esquerda.write(Angulos[3][j]);
			delay(12);
		}
		while(digitalRead(Repetir)==HIGH)
			delay(100);
		delay(600);
		Serial.print("Finalizou Repeticao\n");
	}
	delay(50);
}