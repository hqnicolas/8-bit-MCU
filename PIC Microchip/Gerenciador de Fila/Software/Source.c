/*
##############################################################################################################
# Projeto de Nicolas Pereira
# Desenvolvido em C para aplicação em PIC 8bits
# Criciuma, quarta-feira, 22 de Fevereiro de 2011
##############################################################################################################
*/

#include <16F877A.h> // Arquivo de include do microcontrolador usado...
#use delay(clock=4000000) // Estipula valor de clock para as rotinas de delay...
#use fast_io(b) // Usa modo rápido de entrada e saída...
#fuses XT,NOWDT,NOPUT,NOPROTECT,NOBROWNOUT,NOLVP,NOCPD,NOWRT   /* Acerta os bits de configuração:
                                                                Oscilador do tipo XT; watchdog timer desligado; reset de
                                                                power-up desligado; proteção de código desativada; reset
                                                                de brown-out desligado; programação por nível baixo de
                                                                tensão desativada, pino B3 liberado para I/O; proteção de
                                                                leitura da E2Prom interna desativada; escrita na memória
                                                                flash desativada...
                                                               */
int16 senha=0;
int16 senha_aux=0;
int16 beep=0;
int8 centena, dezena, unidade =0;
int8 senhahigh, senhalow;
int1 aux_beep=0;
int1 aux1, aux2, aux3;

#locate senha=0x20
#locate senha_aux=0x22
#locate senhahigh=0x23
#locate senhalow=0x24
#locate milesimo=0x25
#locate decimo=0x26
#locate beep=0x27

void varre_display()  # Função para converter e exibir a senha em um display de sete segmentos.
{
      centena=0;
      dezena=0;
      unidade=0;
      senha_aux=senha;
      while(senha_aux>=100)
      {
         senha_aux=(senha_aux-100);
         centena++;
      }
      while(senha_aux>=10)
      {
         senha_aux=(senha_aux-10);
         dezena++;
      }
      unidade=senha_aux;

      output_c(centena);
      output_low(pin_d0);
      output_high(pin_d0);
      output_c(dezena);
      output_low(pin_d1);
      output_high(pin_d1);
      output_c(unidade);
      output_low(pin_d2);
      output_high(pin_d2);
}

void main()
{
   setup_adc_ports(NO_ANALOGS);
   setup_psp(PSP_DISABLED);
   setup_spi(FALSE);
   setup_counters(RTCC_INTERNAL,RTCC_DIV_2);
   setup_timer_1(T1_DISABLED);
   setup_timer_2(T2_DISABLED,0,1);
   
   port_b_pullups(true);
   set_tris_b(0b11111111);
   output_b(0x00);
   set_tris_c(0b11110000);
   output_c(0x00);
   set_tris_d(0b01100000);
   output_d(0x00);
   
   if ((read_eeprom(3))!=347)
   {
      write_eeprom(3,347);
      write_eeprom(1,0);
      write_eeprom(2,0);
   }
   senhalow=read_eeprom(1);
   senhahigh=read_eeprom(2);
   senha=make16(senhahigh,senhalow);
   senhahigh=0;
   senhalow=0;

   while(true)
   {
      varre_display();
      
      if((((!input(pin_b0))||(!input(pin_b1))) && (aux1==true)) && (senha<999))
      {
         senha++;
         aux1=false;
         senhalow=make8(senha,0);
         senhahigh=make8(senha,1);
         write_eeprom(1,senhalow);
         write_eeprom(2,senhahigh);
         if(!input(pin_b0)) {output_high(pin_d4); output_low(pin_d5);}
         if(!input(pin_b1)) {output_high(pin_d5); output_low(pin_d4);}
         beep=3000;
      }
      if((input(pin_b0))&&(input(pin_b1))) aux1=true;

      if((((!input(pin_b2))||(!input(pin_b3))) && (aux2==true)) && (senha>0))
      {
         senha--;
         aux2=false;
         senhalow=make8(senha,0);
         senhahigh=make8(senha,1);
         write_eeprom(1,senhalow);
         write_eeprom(2,senhahigh);
         if(!input(pin_b2)) {output_high(pin_d4); output_low(pin_d5);}
         if(!input(pin_b3)) {output_high(pin_d5); output_low(pin_d4);}
         beep=3000;
      }
      if((input(pin_b2))&&(input(pin_b3))) aux2=true;
      
      if((!input(pin_b4)) && (aux3==true))
      {
         senha=0;
         aux3=false;
         senhalow=make8(senha,0);
         senhahigh=make8(senha,1);
         write_eeprom(1,senhalow);
         write_eeprom(2,senhahigh);
         output_low(pin_d4);
         output_low(pin_d5);
         beep=3000;
      }
      if(input(pin_b4)) aux3=true;
      
      /*if((beep>1000)&&(beep!=0))
      {
         output_high(pin_d3);
         delay_us(250);
         output_low(pin_d3);
         delay_us(250);
         beep--;
      }
      if((beep<=1000)&&(beep!=0))
      {
         output_high(pin_d3);
         delay_us(500);
         output_low(pin_d3);
         delay_us(500);
         beep--;
      }
      if(beep==0) output_low(pin_d3);*/
   }

}


