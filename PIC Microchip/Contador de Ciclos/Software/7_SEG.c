/*
##############################################################################################################
# Projeto de Nicolas Pereira
# Desenvolvido em C para aplica��o em PIC 8bits
# Joinville, quarta-feira, ?11? de ?janeiro? de ?2012
##############################################################################################################
*/
#include <16F876.h>                                                                                // Inclui a biblioteca para o microcontrolador 16F876
#use delay(clock=4000000)
#fuses HS,NOWDT,PUT,NOBROWNOUT,NOLVP                                      // Configura��es do microcontrolador
int conta=0;                                                                                              // Reseta a Vari�vel para controlar a contagem
int filtroR=0, filtro=0;
int32 i;
int8 i1, i2, i3, i4;
int1 contando=1, reset=0, foiR=0, foi=0;                                                 // Vari�veis booleanas
int16 d1, d2, d3, d4 ,d5, d6, d7, d8;                                                         // Vari�veis para os d�gitos
int digito[11] =
               {0b11000000,    //D�gito 0
                0b11111001,    //D�gito 1
                0b10100100,    //D�gito 2
                0b10110000,    //D�gito 3
                0b10011001,    //D�gito 4
                0b10010010,    //D�gito 5
                0b10000010,    //D�gito 6
                0b11111000,    //D�gito 7
                0b10000000,    //D�gito 8
                0b10010000,    //D�gito 9
                0b10111111     //D�gito -
               };
#locate filtro=0x20                                                                                   // Define a localiza��o da vari�vel 'filtro' na mem�ria
#locate i=0x21					 	 // Define a localiza��o da vari�vel 'i' na mem�ria
#INT_TIMER0						// Declara��o de uma interrup��o do temporizador 0
#priority RTCC,eeprom,ext					// Configura��o de prioridade de interrup��o
void trata_t0()						// Fun��o que trata o timer T0
{
conta++;							// Incrementa a vari�vel 'conta'
if (conta>8) conta = 1;
switch(conta)
          {
         case 1: OUTPUT_c(0X01);				// Define a sa�da C0
                 OUTPUT_B(digito[d1]);				// Exibe o d�gito d1 no display
                 delay_us(750);
                 OUTPUT_B(0b11111111);
                       break;
         case 2: OUTPUT_c(0X02);				
                 OUTPUT_B(digito[d2]);				// Exibe o d�gito d2 no display
                 delay_us(750);
                 OUTPUT_B(0b11111111);
                       break;
         case 3: OUTPUT_c(0X04);				
                 OUTPUT_B(digito[d3]);				// Exibe o d�gito d3 no display
                 delay_us(750);
                 OUTPUT_B(0b11111111);
                       break;
         case 4: OUTPUT_c(0X08);
                 OUTPUT_B(digito[d4]);				// Exibe o d�gito d4 no display
                 delay_us(750);
                 OUTPUT_B(0b11111111);
                       break;
         case 5: OUTPUT_c(0X10);
                 OUTPUT_B(digito[d5]);				// Exibe o d�gito d5 no display
                 delay_us(750);
                 OUTPUT_B(0b11111111);
                       break;
         case 6: OUTPUT_c(0X20);
                 OUTPUT_B(digito[d6]);				// Exibe o d�gito d6 no display
                 delay_us(750);
                 OUTPUT_B(0b11111111);
                       break;
         case 7: OUTPUT_c(0X40);
                 OUTPUT_B(digito[d7]);				// Exibe o d�gito d7 no display
                 delay_us(750);
                 OUTPUT_B(0b11111111);
                       break; 
         case 8: OUTPUT_c(0X80);
                 OUTPUT_B(digito[d8]);				// Exibe o d�gito d8 no display
                 delay_us(750);
                 OUTPUT_B(0b11111111);
                       break;
         } 
SET_TIMER0(50); 						// Configura o temporizador 0 para interromper novamente ap�s 50 unidades de tempo
}
void main()
{
ENABLE_INTERRUPTS(GLOBAL | INT_TIMER0);
SETUP_TIMER_0(RTCC_INTERNAL | RTCC_DIV_16);
SET_TIMER0(100);
i4 = read_eeprom(0x00);					// L� dados da mem�ria EEPROM
i3 = read_eeprom(0x08);
i2 = read_eeprom(0x10);
i1 = read_eeprom(0x18);
i = Make32(i1,i2,i3,i4);					// Combina os valores lidos em uma vari�vel de 32 bits
while(true)
     {
         if (i>99999998)					//Configura o estouro da contagem
         {
            i = 0;
         } 
         if (!input(pin_a2))
         {
            if ((filtroR)>(0))
            {
               filtroR--;
               if ((filtroR)<(3))
                  {
                     ((foiR)=(false));
                  }
              }
         }
         if (input(pin_a2))
         {
            if (!foiR)
            {
                 if ((filtroR)<(10))
                  {
                   filtroR++;
                  }
                  if (((filtroR)>=(10))&&(!foiR)&&(!reset))
                  {
                     contando=0;					//Entrada do bot�o Reset com filtro
                     foiR= true;
                     reset= true;
                  }
                  if (((filtroR)>=(10))&&(!foiR)&&(reset))
                  {
                     contando=1;
                     i=0;
                     foiR= true;
                     reset= false;
                  }
              }
          }
         if (!input(pin_a5))
         {
            if ((filtro)>(0))
            {
               filtro--;
               if ((filtro)<(3))
                  {
                     ((foi)=(false));
                  }
            }
         }
         if (input(pin_a5))
         {
            if (!foi)
            {
                 if ((filtro)<(10))
                  {
                   filtro++;						// Cria um filtro para a entrada digital
                  }
                  if ((filtro)>=(10))
                  {
                   i++;
                   foi= true;
                   write_eeprom(0x00,make8(i,0));			// Escreve o valor de 'i' na mem�ria EEPROM
                   write_eeprom(0x08,make8(i,1));
                   write_eeprom(0x10,make8(i,2));
                   write_eeprom(0x18,make8(i,3)); 
                  }
              }
          }
    if (contando)						// Se estiver contando
      {
          d8 = i/10000000;					// Divide 'i' para exibir em d�gitos
          d7 = (i/1000000) % 10;
          d6 = (i/100000) % 10;
          d5 = (i/10000) % 10;
          d4 = (i/1000) % 10;
          d3 = (i/100) % 10;
          d2 = (i/10) % 10;
          d1 = (i/1) % 10;
      }
   if (!contando)						// Se n�o estiver contando
     {
          d8 = (10);						// Define todos os d�gitos como 10
          d7 = (10);
          d6 = (10);
          d5 = (10);
          d4 = (10);
          d3 = (10);
          d2 = (10);
          d1 = (10);
     }
   }
 }

