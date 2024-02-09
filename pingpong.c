#include "LedControl.h"
LedControl lc = LedControl(10, 9, 8, 1);
int j1=15,j2=16;

void setup() 
{
	lc.shutdown(0, false);
	lc.setIntensity(0, 2);
	lc.clearDisplay(0);
	Serial.begin(9600);
	pinMode(j1, INPUT);
	pinMode(j2, INPUT);
}

int bar1_pos = 0;
int bar2_pos = 0;
int bola_posv = 4;
int bola_posh = 4;
int dir_h = 1;
int dir_v = 1;
long int vezes = -1;
int intervalo_ms = 500;
int pontos[2] = {0,0};

void updateBar(int pos1,int pos2,bool flag)
{
	bar1_pos=pos1;
	bar2_pos=pos2;
	lc.clearDisplay(0);
	for(int i=0;i<3;i++)
	{
		lc.setLed(0,0,pos1+i,true);
		lc.setLed(0,7,pos2+i,true);
	}
	if(flag)
		lc.setLed(0,bola_posh,bola_posv,true);
}

void updateBola(int jogador)
{
	int bar_pos = bar1_pos;
	if(jogador==1) bar_pos = bar2_pos;
	int dif = bola_posv-bar_pos-1;
	if(abs(dif)>=2)
	{
		pontos[jogador]++;
		bola_posh = 4;
		bola_posv = 4;
	}
	else
	{
		dir_h *= -1;
		dir_v = dif;
	}
}

void loop()
{
	long int tempo = millis()/intervalo_ms;
	
	int read1 = analogRead(j1);
	read1 = map(read1,0,950,0,5);
	int read2 = analogRead(j2);
	read2 = map(read2,0,950,0,5);
	
	if(tempo>vezes)
	{
		vezes++;
		updateBar(read1,read2,false);
		if((bola_posh+dir_h)==7)
			updateBola(1);
		if((bola_posh+dir_h)==0)
			updateBola(2);
		if(bola_posv+dir_v==8||bola_posv+dir_v==-1)
			dir_v *= -1;
		bola_posh += dir_h;
		bola_posv += dir_v;
		lc.setLed(0,bola_posv,bola_posh,true);
	}
	else
		updateBar(read1,read2,true);
}