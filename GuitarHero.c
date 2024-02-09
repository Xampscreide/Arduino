#include "LedControl.h"
#include <LiquidCrystal_I2C.h>

#define endereco 0x27
#define colunas 16
#define linhas 2

LedControl lc = LedControl(12, 11, 10, 4);
LiquidCrystal_I2C lcd(endereco, colunas, linhas);

int tamanho = 5;
int patern[6][8] =
{ 
	{1, 1, 1, 1, 2, 2, 2, 2},
	{13, 13, 13, 13, 23, 23, 23, 23},
	{13, 13, 13, 13, 23, 23, 23, 23},
	{13, 13, 13, 13, 23, 23, 23, 23},
	{13, 13, 13, 13, 23, 23, 23, 23},
	{-2,-2,-2,-2,-2,-2,-2,-2}
};
int posicao[5];

void inicializar()//Seta todos indicadores de posicao para a posicao no patern de valor -2
{
    for(int i = 0; i < 5; i++)
    {
        posicao[i] = tamanho;
    }
}

void gerar()//Move todos indicadores de posicao para frente e gera um novo indicador
{
	for (int i = 4; i > 0; i--)
	{
		posicao[i] = posicao[i - 1];
	}
	posicao[0] = random(300);
	posicao[0] %= tamanho;
}

void setup() 
{
	lc.shutdown(0, false);
	lc.shutdown(1, false);
	lc.shutdown(2, false);
	lc.shutdown(3, false);
	lc.setIntensity(0, 2);
	lc.setIntensity(1, 2);
	lc.setIntensity(2, 2);
	lc.setIntensity(3, 2);
	lc.clearDisplay(0);
	pinMode(4, INPUT);
	pinMode(5, INPUT);
	pinMode(6, INPUT);
	pinMode(7, INPUT);
	pinMode(3, INPUT);
	inicializar();
	Serial.begin(9600);
	lcd.init();
	lcd.backlight();
	lcd.clear();
	lcd.setCursor(8,0);
	lcd.print("Press");
} 


void ligarLeds(int j)//Recebe um valor de 0 a 7 representando quantas posicoes o display tem que descer
{
    lc.clearDisplay(0);
    lc.clearDisplay(1);
    lc.clearDisplay(2);
    lc.clearDisplay(3);
    for(int l=0;l<4;l++)//Passa pelos 4 displays
    {
        for(int i=0; i<8-j; i++)//Liga os leds que estao descendo do display atual
        {
            if(patern[posicao[l+1]][i+j]>9)//Se o valor no patern for maior que 9 significa que seram dois leds nessa linha
            {
                int p = patern[posicao[l+1]][i+j]%10;
                int s = patern[posicao[l+1]][i+j]/10;
                lc.setLed(l,p+1,i,true);
                lc.setLed(l,s+1,i,true);
            }
            else
                lc.setLed(l,patern[posicao[l+1]][i+j]+1,i,true);
        }
        for(int i=8-j,k = 0; i<8; i++,k++)//"Simula" os leds que ja desceram do display de cima
        {
            if(patern[posicao[l]][k]>9)//Se o valor no patern for maior que 9 significa que seram dois leds nessa linha
            {
                int p = patern[posicao[l]][k]%10;
                int s = patern[posicao[l]][k]/10;
                lc.setLed(l,p+1,i,true);
                lc.setLed(l,s+1,i,true);
            }
            else
                lc.setLed(l,patern[posicao[l]][k]+1,i,true);
        }
    }
}
 

long int vezes = -1;
long int tempo;
int resposta1=-1,resposta2=-1;
int gabarito1=1,gabarito2=1;
int press[4]={0,0,0,0};

int antigo[4]={0,0,0,0};
int novo[4];
long int pontos = 0;
int comecou = 1;

//Config
int acerto = 20;
int erro = 2;
int quantidade = 12;
int intervalo_ms = 500;

void loop()
{
	while(comecou)//Espera até certo botao ser apertado
	{
		if(digitalRead(3))
		{
			lcd.clear();
			comecou = 0;
		}
	}
	
    tempo = millis()/intervalo_ms;//Calcula o contador
	
    if(tempo > vezes)//Verifica se um novo ciclo deve ser gerado
    {
		vezes++;
		
		if(resposta1!=-3)//Debaunce para o inicio do jogo
		{
			if(gabarito1==resposta1)//Verifica se nao ouve acerto
			{
				Serial.println("Deixou de responder");
				pontos -= erro;
			}
		}
		
		if(resposta2!=-2)//Verifica se é o caso onde sao 2 leds em uma linha e serve de debaunce para o inicio do jogo
		{
			if(gabarito2==resposta2)//Verifica se nao ouve acerto
			{
				Serial.println("Deixou de responder");
				pontos -= erro;
			}
		}
		
        int resto = vezes%8;//Calcula a nova posicao do display (0 a 7)
        if(resto==0)//Verifica se uma nova posicao deve ser gerada
        {
            gerar();
        }
		
        ligarLeds(resto);//Liga os leds
		
        resposta1 = (patern[posicao[4]][resto]%10)-1;//Guarda as respostas
        resposta2 = (patern[posicao[4]][resto]/10);//Guarda as respostas
        if(!resposta2)
            resposta2 = -2;
		else
			resposta2 -= 1;
		
        gabarito1 = resposta1;//Copia a resposta pra um gabarito
        gabarito2 = resposta2;//Copia a resposta pra um gabarito
        Serial.println(pontos);
    }

    for(int i=0;i<4;i++)//Passa pelos 4 botoes
    {
        novo[i] = digitalRead(i+4);//Le se o botao esta ativo
        if(novo[i]!=antigo[i])//Debaunce para identificar se um botao esta sendo segurado ou nao
        {
            if(novo[i])//Se ele esta sendo ativado
            {
				if(resposta1!=-2)//Debaunce para o inicio do jogo
				{
					if(i==resposta1)//Verifica se o botao pressionado corresponde a resposta correta
					{
						lc.setLed(3,i+2,0,false);//Desliga o led da linha que foi acertada
						Serial.println("Acertou");
						pontos += acerto;
						resposta1 = -1;//Atualiza a resposta para ser comparada com o gabarito e para nao ser acertada mais de uma vez
					}
					else
					{
						Serial.println("Errou");
						pontos -= erro;
					}
				}
                if(resposta2!=-2)//Debaunce para o inicio do jogo ou se somente 1 led por linha
                {
                    if(i==resposta2)//Verifica se o botao pressionado corresponde a resposta correta
                    {
						lc.setLed(3,i+2,0,false);//Desliga o led da linha que foi acertada
                        Serial.println("Acertou");
                        pontos += acerto;
                        resposta2 = -1;//Atualiza a resposta para ser comparada com o gabarito e para nao ser acertada mais de uma vez
                    }
                    else
                    {
                        Serial.println("Errou");
                        pontos -= erro;
                    }
                }
            }
            antigo[i] = novo[i];//Debaunce do botao
        }
    }
	
	lcd.print(pontos);//Mostra os pontos no lcd
	lcd.setCursor(0,0);
	
	if(vezes>(quantidade*8))//Verifica se o jogo acabou
	{
		for(int a = 0;a<4;a++)
		{
			for(int i=0;i<8;i++)
			{
				for(int j=0;j<8;j++)
				{
					lc.setLed(a,i,j,false);
				}
			}
		}
		comecou = 1;
		vezes = -1;
		lcd.setCursor(0,0);
		lcd.print("Acabou");
		lcd.setCursor(8,0);
		lcd.print(pontos);
		pontos = 0;
	}
}