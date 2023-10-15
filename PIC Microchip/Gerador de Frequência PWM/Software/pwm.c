/*
OBJETIVO
Gerador de PWM do Boost para controle de Freio.
****************************************************
*/

#include <16F628A.h>
#FUSES NOWDT                 	//No Watch Dog Timer
#FUSES INTRC_IO              	//Internal RC Osc, no CLKOUT
#FUSES NOPUT                   	//Power Up Timer
#FUSES NOPROTECT             	//Code not protected from reading
#FUSES NOBROWNOUT            	//No brownout reset
#FUSES NOMCLR                 	//Master Clear pin used for I/O
#FUSES NOLVP                 	//No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOCPD                 	//No EE protection
#use delay(clock=4000000,RESTART_WDT)

#priority timer1,rtcc,rb
/*
****************************************
/*
Variaveis Globais
/*
**************************************
*/

int				leds,dsp1,dsp2,dsp3,dsp4; //Display
signed  int		soCont=-1;				  //contador de uso geral
boolean			DP1,DP2,DP3,DP4;	      //indica se o digital point esta ligado ON = 0 OFF = 1 	
boolean			atualizar_display;
int				cont; 					  //contador de uso geral	
int				inFlag;
int				fCont[4]; //contadores do fltro
int 			status = 0;
int16		pwmcont = 1;
int16		temp; 
/*****************************************
/*Constantes, Bits e Bytes 
/****************************************/
#use	fast_io(a)
#use	fast_io(b)
#byte	tris_a = 0x85
#byte	tris_b = 0x86

//IO
#define		outEn		pin_a7 //controle do 4093
#define		clk			pin_b0 //controle do 4093
#define		data		pin_a4 //controle do 4093	
#define		bus0		pin_a0
#define		bus1		pin_a1
#define		bus2		pin_a2
#define		bus3		pin_a3
#define		bus4		pin_b4
#define		bus5		pin_b5
#define		bus6		pin_a6
#define		bus7		pin_b7	
#define		fre_pin		pin_b3  //pino da frequência


#define		out_a		0b10110000
#define		out_b		0b01001111

#define 	um			0b11111001
#define 	dois		0b10100100
#define 	tres		0b10110000
#define 	quatro		0b10011001
#define 	cinco		0b10010010
#define 	seis		0b10000010
#define 	sete		0b11111000
#define 	oito		0b10000000
#define		nove		0b10010000
#define		zero		0b11000000
#define		off			0b11111111

// referencia ao registrador leds
#define		led6		0b11111101	
#define		led2		0b11110111
#define		led3		0b11101111
#define		led4		0b11011111
#define		L1_L2		0b11111110	

//Posição física da tecla em relação ao PORT
#define		sw1			1
#define		sw3			3
#define		sw4			4
#define		sw5			5

//Posição lógica da tecla no Flag "inFlag"
#define		sw1b		0
#define		sw3b		1
#define		sw4b		2
#define		sw5b		3

/*****************************************
/*Protótipos
/****************************************/
void output_bus(int8 bus,boolean DPst );
int	filtro (int flag);
int	retorna_display(int16 numero);
void	at_display();

/*****************************************
/*Interupção Timer0 
/*Utilizada como timer do "Sistema Operacional"
/****************************************/
#int_RTCC
RTCC_isr() 
{
	atualizar_display = true;

}//int_RTCC

#int_TIMER1
TIMER1_isr() 
{
	#asm //debug
		nop
	#endasm
		set_timer1(temp);

}
/*****************************************
/*Função Main
/****************************************/
void main(){
  
  int16		retorno,retorno2;  		
//configurações
  
//TIMER 0	
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_8);
   enable_interrupts(INT_RTCC);

//TIMER 1
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);	
   enable_interrupts(INT_TIMER1);

//TIMER 2  
 setup_timer_2(T2_DIV_BY_4, 10, 1);
 setup_ccp1 (ccp_pwm);
 set_pwm1_duty (0); 

//COMPARADOR
   setup_comparator(NC_NC_NC_NC);
   setup_vref(False);

//OSCILADOR
   setup_oscillator(OSC_4MHZ);
//MODULO PWM
  setup_ccp1(CCP_PWM);   
 
//TRISTATE REG
	set_tris_b(0B011110111);

enable_interrupts(GLOBAL); 

// inicialização de variaveis
	status = 0;
	fCont[0] = 0;
	fCont[1] = 0;
	fCont[2] = 0;
	fCont[3] = 0;
 	atualizar_display = false; 
	leds = 0xff;//led6&&led2 ;
    dsp1 = off;
    dsp2 = off;
    dsp3 = off;
    dsp4 = off;
	DP1 = true;
	DP2 = true;
    DP3 = true;
	DP4 = true;

// loop principal
	while(true){
		set_pwm1_duty (pwmcont/4);
	//exibição no display	
		retorno = pwmcont/1000 ;
		retorno2 = pwmcont%1000; 
		retorno = retorno2/100; 
		dsp1 = retorna_display(retorno);
		retorno2 = retorno2%100;
		retorno =  retorno2/10; 
		dsp2 = (retorna_display(retorno)-128);
		retorno =  retorno2%10; 
		dsp3 = retorna_display(retorno);
		bit_clear (retorno,7);
		dsp4 = 0b00101101;
	//verifica teclas
	if(bit_test(inFlag,sw1b))
		{
			bit_clear(inFlag,sw1b);
			if(pwmcont<999)
				pwmcont++;
		}
	else
		if(bit_test(inFlag,sw3b))
			{	bit_clear(inFlag,sw3b);
				if(pwmcont>0)
				pwmcont--;					
			}

	if(bit_test(inFlag,sw4b))
	{
		bit_clear(inFlag,sw4b);
		if(pwmcont<(990))
			pwmcont =	pwmcont	+ 10;
		else	
			pwmcont = 999;	
	}
	else
		if(bit_test(inFlag,sw5b))
			{	bit_clear(inFlag,sw5b);
				if(pwmcont>10)
					pwmcont =	pwmcont	- 10;
				else
					pwmcont = 0;	
			}
	//debug
	// verifica atualização do Display 
	if (atualizar_display){
		at_display();
		atualizar_display = false;
	}	
	}//while
}
/*****************************************
/*Função converte para display 7 seguimentos
/* 
/****************************************/
int	retorna_display(int16 numero){


switch (numero){
	case 	(0):return 	(zero);
	case	(1):return	(um);		 
	case	(2):return	(dois);
	case	(3):return	(tres);
	case	(4):return	(quatro);
	case	(5):return	(cinco);
	case	(6):return	(seis);
	case	(7):return	(sete);
	case	(8):return	(oito);
	case	(9):return	(nove);
	default:	return	(off);


}
}
/*****************************************
/*Função Atualiza display
/* atualiza o estado do diplsy e le teclas
/****************************************/

void	at_display(){

int		temp1,temp2; //variavels locais 

soCont++;
output_bit(outEn, 0);	
switch (soCont){
	case 0:	set_tris_a(tris_a&out_a);
		    set_tris_b(tris_b&out_b);
			
			output_bus(leds,1);//move leds para saida
			output_bit(data, 1);
			for(cont=0;cont<10;cont++) //zera shift reg 
			{	output_bit(clk,0);
				output_bit(clk,1);
			}
			output_bit(data, 0);
			output_bit(clk,0);
			output_bit(clk,1);
			output_bit(data, 1);
			output_bit(outEn, 1);
			break;
	
	case 1:	output_bit(clk,0);
			output_bit(clk,1);		
			output_bus(dsp1,DP1);
			output_bit(outEn, 1);
			break;

	case 2:	output_bit(clk,0);
			output_bit(clk,1);	
			output_bus(dsp2,DP2);
			output_bit(outEn, 1);
			break;

	case 3:	output_bit(clk,0);
			output_bit(clk,1);	
			output_bus(dsp3,DP3);
			output_bit(outEn, 1);
			break;

	case 4:	output_bit(clk,0);
			output_bit(clk,1);
			output_bus(dsp4,DP4);
			output_bit(outEn, 1);
			break;	

	case 5: set_tris_a(0b01001111);
		    set_tris_b(0b00110000);
			temp2 = 0x00;
			temp1 = input_b();
			if(!bit_test(temp1,sw4)) bit_set(temp2,sw4b);
			if(!bit_test(temp1,sw5)) bit_set(temp2,sw5b);
			temp1 = input_a();
			if(!bit_test(temp1,sw1)) bit_set(temp2,sw1b);
			if(!bit_test(temp1,sw3)) bit_set(temp2,sw3b);
			inFlag = filtro(temp2);
			soCont = -1;
			break;			

	default: break;

	}//switch
	set_timer0(100);		

}

/*****************************************
/*Função output_bus(int8 bus) 
/* Exterioriza valor lógico da dado do display e leds
/****************************************/
void output_bus(int8 bus,boolean DPst){
	output_bit(bus0,bit_test(bus,0));
	output_bit(bus1,bit_test(bus,1));
	output_bit(bus2,bit_test(bus,2));
	output_bit(bus3,bit_test(bus,3));
	output_bit(bus4,bit_test(bus,4));
	output_bit(bus5,bit_test(bus,5));
	output_bit(bus6,bit_test(bus,6));
	output_bit(bus7,bit_test(bus,7));
	//output_bit(bus7,DPst);			//pino do digital point

}
/*****************************************
/*Função filtro (int flag) 
/* Debouce das teclas
/****************************************/
int	filtro (int flag){
		int p;
		int ret = 0;
		for (p=0;p<4;p++){
			if (bit_test(flag,p))
			{	if(!bit_test(status,p))
				{	fCont[p]++;
		   			if (fCont[p] > 5){
						bit_set(status,p);
						bit_set(ret,p);
					}//if
				}//if
			}
			else
			{	fCont[p] = 0;
				if(!bit_test(inFlag,p))
					bit_clear(status,p);
			}//else


		}//for

	return (ret|inFlag);
}//função





