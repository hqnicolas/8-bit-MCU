/*
##############################################################################################################
# Projeto de Nicolas Pereira
# Desenvolvido em C para aplicação em PIC 8bits
# Criciuma, quarta-feira, 10 de Novembro de 2010
##############################################################################################################
*/
#include <16F877A.h>
#device adc=10
#use delay(clock=4000000) 
#fuses XT,NOWDT,NOPUT,NOPROTECT,NOBROWNOUT,NOLVP,NOCPD,NOWRT  

   int8 filtro,cesegundos,desegundos,segundos,parado,editando,ir,menuprince,car1=0,car2=0,car3=0,car4=0,senha1=0,senha2=9,senha3=1,senha4=1;
   int1 senhar=0,comandar=0,click=0,esquerda=0,direita=0,entra=0,sai=0,cima=0,baixo=0,logado=0,abrir=0,fechar=0,barreira=0;
   char buffer[1];
   
   #locate parado= 0x20
   #locate ir= 0x21
   #locate menuprince= 0x22
   #locate segundos= 0x23

#int_timer1
void tempo(timer1)
{
set_timer1(15535);
cesegundos++;

   if (cesegundos==2)
   {
   desegundos++;
   (cesegundos=0);
   }
   if (desegundos==10)
   {
   segundos++;
   (desegundos=0);
   }
   
    if (!input(pin_b0))
   {
   output_low(pin_c0);
   }
   if (!input(pin_b2))
   {
   output_low(pin_c1);
   }
   
   if ((!input(pin_b7))&&(input(pin_b0))&&(input(pin_b2)))
   {
   (output_low(pin_c0));
   (output_low(pin_c1));
   (output_high(pin_c0));
   }
   if ((segundos)>(59))
   {
   segundos=0;
   }
}

void pulso() 
{
   output_high(PIN_C7); // Seta pino de "Enable"...
   delay_cycles(10); // Espera 10 ciclos de máquina...
   output_low(PIN_C7); // Reseta pino de "Enable"...
}

void limpa() 
{
   output_low(PIN_C4); // Seleciona display para comandos (resetando-se o pino RS do LCD)...
   output_d(0x00);
   pulso();
   output_d(0x10);
   pulso();
   delay_ms(2); // Envia o comando 0x01 para o LCD e espera 2 ms (necessário para resposta do display)...
}

void posicao(int x,y) 
{
   int z,w; // Variáveis temporárias locais...
   z=0;
   output_low(PIN_C4); // Seleciona display para comandos...
   switch (y) // Tabela tipo case para determinar em qual linha deverá ser posicionado o cursor...
   {
      case 0: z=0x80; // Posiciona na linha 0...
      break;
      case 1: z=0xC0; // Posiciona na linha 1...
      break;
   }
   switch (x) // Tabela tipo case para determinar em qual coluna deverá ser posicionado o cursor...
   {
      case 0: w=0x00; // Posiciona na coluna 0...
      break;
      case 1: w=0x10; // Posiciona na coluna 1...
      break;
      case 2: w=0x20; // Posiciona na coluna 2...
      break;
      case 3: w=0x30; // Posiciona na coluna 3...
      break;
      case 4: w=0x40; // Posiciona na coluna 4...
      break;
      case 5: w=0x50; // Posiciona na coluna 5...
      break;
      case 6: w=0x60; // Posiciona na coluna 6...
      break;
      case 7: w=0x70; // Posiciona na coluna 7...
      break;
      case 8: w=0x80; // Posiciona na coluna 8...
      break;
      case 9: w=0x90; // Posiciona na coluna 9...
      break;
      case 10: w=0xA0; // Posiciona na coluna 10...
      break;
      case 11: w=0xB0; // Posiciona na coluna 11...
      break;
      case 12: w=0xC0; // Posiciona na coluna 12...
      break;
      case 13: w=0xD0; // Posiciona na coluna 13...
      break;
      case 14: w=0xE0; // Posiciona na coluna 14...
      break;
      case 15: w=0xF0; // Posiciona na coluna 15...
      break;
   }
   output_d(z);
   pulso();
   output_d(w);
   pulso();
   delay_us(40); // Envia o comando para o LCD e espera 40 us para resposta do display...
}

void escreve(char x)  
{
   char y; // Variável temporária local...
   y=x<<4; // Separa o caracter em dois nibbles...
   output_high(PIN_C4); // Seleciona display para dados...
   output_d(x);
   pulso();
   output_d(y);
   pulso();
   delay_us(40); // Envia o dado para o LCD e espera 40 us para resposta do display...
}

void lcd_ini() 
{
   delay_ms(15); // Espera 15 ms para energização do display e do seu controlador...
   output_low(PIN_C7); // Reseta pino de "Enable" do LCD...
   output_low(PIN_C4); // Seleciona display para comandos...
   output_d(0x30); // Envia comando 0x30 para o LCD...
   pulso();
   delay_ms(4); // Aguarda 4 ms para garantir o término da operação...
   pulso(); // Reenvia comando 0x30 para o LCD...
   delay_us(100); // Aguarda 100 us...
   pulso(); // Reenvia comando 0x30 para o LCD...
   delay_us(40); // Aguarda 40 us...
   output_d(0x20); // Estabele comunicação em 4 vias...
   pulso();
   delay_us(40); // Espera 40 us...
   output_d(0x20); // Como a comunicação é de 4 vias envia-se o nibble mais significativo primeiro...
   pulso();
   output_d(0x80); // Estabelece condições de utilização: display de 2 linhas e matriz 7X5...
   pulso();
   delay_us(40); // Aguarda 40 us...
   output_d(0x00);
   pulso();
   output_d(0x10); // Limpa o display (iqual função "limpa()")...
   pulso();
   delay_ms(2); // Aguarda 2 ms...
   output_d(0x00);
   pulso();
   output_d(0xC0); // Comando para ligar display sem cursor...
   pulso();
   delay_us(40); // Aguarda 40 us...
   output_d(0x00);
   pulso();
   output_d(0x60); // Estabelece modo de operação: Deslocamento automático do cursor para direita...
   pulso();
   delay_us(40); // Aguarda 40 us...
}

void abrir()
{
if ((logado)==(1)) 
{
    posicao(0,0);
    escreve("    ABRINDO      ");
    posicao (0,1);
    escreve("                ");
    (output_high(pin_c5));
    delay_ms(300);
    posicao (0,1);
    escreve(".               ");
    (output_low(pin_c5));
    delay_ms(300);
    posicao (0,1);
    escreve("..              ");
    (output_high(pin_c5));
    delay_ms(300);
    posicao (0,1);
    escreve("...             ");
    (output_low(pin_c5));
    delay_ms(300);
    (output_high(pin_c5));
    escreve("                ");
    if (input(pin_b0))
    {
    (output_high(pin_c0));
    (output_low(pin_c1));
    }
    if (!input(pin_b0))
    {
    posicao(0,0);
    escreve(" PORTAL ABERTO ");
    delay_ms(600);
    escreve("                ");
    delay_ms(600);
    posicao(0,1);
    escreve(" PORTAL ABERTO ");
    delay_ms(600);
    posicao(0,1);
    escreve("                ");
    delay_ms(600);
    }

}
 menuprince=0; 
 ir=0;
}

void fechar()
{
if ((logado)==(1)) 
{
    posicao(0,0);
    escreve("   FECHANDO     ");
    posicao (0,1);
    escreve(".               ");
    (output_low(pin_c5));
    delay_ms(300);
    posicao (0,1);
    escreve("..              ");
    (output_high(pin_c5));
    delay_ms(300);
    posicao (0,1);
    escreve("...             ");
    (output_low(pin_c5));
    delay_ms(300);
    posicao (0,1);
    escreve("                ");
    (output_high(pin_c5));
    delay_ms(300);
    (output_low(pin_c5));
    if (input(pin_b2))
    {
    (output_high(pin_c1));
    (output_low(pin_c0));
    }
    if (!input(pin_b2))
    {
    posicao(0,0);
    escreve(" PORTAL FECHADO ");
    delay_ms(600);
    posicao(0,0);
    escreve("                ");
    delay_ms(600);
    posicao(0,1);
    escreve(" PORTAL FECHADO ");
    delay_ms(600);
    posicao(0,1);
    escreve("                ");
    delay_ms(600);
    }
}

menuprince=0;
ir=0;
}

void comandos()
{
  
   
  if ((logado)==(0))
   {
   menuprince=0;
   ir=0;
   }
  
  if ((logado)==(1))
  {
  
   posicao (0,0);
   escreve (" Abrir    Fechar   ");
  
    if ((comandar)==0)
    {
    posicao (0,1);
    escreve (" ^               ");
    if (((entra)==(1))&&((click)==(0)))
    {
    entra=0;
    click=1;
    abrir();
    
    }
    }
    
    if ((comandar)==1)
    {
    posicao (0,1);
    escreve ("          ^      ");
    if (((entra)==(1))&&((click)==(0)))
    {
    entra=0;
    click=1;
    fechar();
    
    }
    }
      
    if (((direita)==(1))&&(click==(0)))
     {
     comandar++;
     click=1;
     direita=0;
     }
     
     if (((sai)==(1))&&(click==(0)))
     {
     menuprince=0;
     ir=0;
     click=1;
     sai=0;
     }
  
  if (((esquerda)==(1))&&(click==(0)))
     {
     comandar--;
     click=1;
     esquerda=0;
     }

  }
}

void cadastro()
{
if ((logado)==(0))
{
menuprince=0;
}

if ((sai)==(1)&&((click)==(0)))
  {
  sai=0;
  click=1;
  menuprince=0;
  }
  
  if ((entra)==(1)&&((click)==(0)))
  {
  entra=0;
  click=1;
  ((senha1)=(car1));
  ((senha2)=(car2));
  ((senha3)=(car3));
  ((senha4)=(car4));
  
  logado=0;
  menuprince=0;
  ir=0;  
  car1=0;
  car2=0;
  car3=0;
  car4=0;
  }
  
if ((logado)==(1)) 
{
  if (((direita)==(1))&&(click==(0)))
     {
     editando++;
     click=1;
     direita=0;
     }
  
  if (((esquerda)==(1))&&(click==(0)))
     {
     editando--;
     click=1;
     esquerda=0;
     }
 
   posicao (0,0);
   escreve("Nova Senha:");
   posicao (11,0);
   sprintf (buffer,"%01u",car1);
   escreve (buffer[0]);
   posicao (12,0);
   sprintf(buffer,"%01u",car2);   
   escreve(buffer[0]);
   posicao (13,0);
   sprintf(buffer,"%01u",car3);  
   escreve(buffer[0]);
   posicao (14,0);
   sprintf(buffer,"%01u",car4);  
   escreve(buffer[0]);
   posicao (0,1);
   escreve("           ");
   posicao (15,1);
   escreve("  ");


   if ((editando)==(0))
   {
   if (((cima)==(1))&&((click)==(0)))
   {
   car1++;
   click=1;
   cima=0;
   }
   if (((baixo)==(1))&&((click)==(0)))
   {
   car1--;
   click=1;
   baixo=0;
   }
   if ((car1)==(255))
   {
   ((car1)=(9));
   }
   posicao (11,1);
   escreve ("^   ");
   }
   
   
   if ((editando)==(1))
   {
   if (((cima)==(1))&&((click)==(0)))
   {
   car2++;
   click=1;
   cima=0;
   }
   if (((baixo)==(1))&&((click)==(0)))
   {
   car2--;
   click=1;
   baixo=0;
   }
   if ((car2)==(255))
   {
   ((car2)=(9));
   }
   posicao (11,1);
   escreve (" ^  ");
   }
   
   if ((editando)==(2))
   {
   if (((baixo)==(1))&&((click)==(0)))
   {
   car3--;
   click=1;
   baixo=0;
   }
   if (((cima)==(1))&&((click)==(0)))
   {
   car3++;
   click=1;
   cima=0;
   }
   if ((car3)==(255))
   {
   ((car3)=(9));
   }
   posicao (11,1);
   escreve ("  ^ ");
   }
   
   if ((editando)==(3))
   {
   if (((baixo)==(1))&&((click)==(0)))
   {
   car4--;
   click=1;
   baixo=0;
   }
   if (((cima)==(1))&&((click)==(0)))
   {
   car4++;
   click=1;
   cima=0;
   }
   if ((car4)==(255))
   {
   ((car4)=(9));
   }
   posicao (11,1);
   escreve ("   ^");
   }
   
   if ((editando)==(4))
   {
   ((editando)=(0));
   }
   
   if ((editando)==(255))
   {
   ((editando)=(3));
   }   

  if ((car1)>(9))
  {
  car1=0;
  }
  if ((car2)>(9))
  {
  car2=0;
  }
  if ((car3)>(9))
  {
  car3=0;
  }
  if ((car4)>(9))
  {
  car4=0;
  } 


}
}

void verificar()
{
   posicao (0,0);
   escreve ("verificando.    ");
   posicao (0,1);
   escreve ("                ");
   delay_ms(200);
   posicao (0,0);
   escreve ("verificando..   ");
   delay_ms(200);
   escreve ("verificando...  ");
   delay_ms(200);
   escreve ("verificando.... ");
  
  if (((car1)==(senha1))&&((car2)==(senha2))&&((car3)==(senha3))&&((car4)==(senha4)))
  {
  logado=1;
  }
  else
  {
  logado=0;
  posicao (0,0);
  escreve ("Senha Incorreta  ");
  delay_ms(600);
  limpa();
  delay_ms(600);
  posicao (0,1);
  escreve ("Senha Incorreta  ");
  delay_ms(600);
  limpa();
  delay_ms(600);
  }
  
}

void login()
{
 if ((sai)==(1)&&((click)==(0)))
  {
  sai=0;
  click=1;
  menuprince=0;
  ir=0;
  }
  
 if ((logado)==(1))
 {
 ((menuprince)=(2));
 }
 if ((logado)==(0))
 { 
 
  if ((entra)==(1)&&((click)==(0)))
  {
  entra=0;
  click=1;
  verificar();
  }
  
  if (((direita)==(1))&&(click==(0)))
     {
     editando++;
     click=1;
     direita=0;
     }
  
  if (((esquerda)==(1))&&(click==(0)))
     {
     editando--;
     click=1;
     esquerda=0;
     }
 
   posicao (0,0);
   escreve("   Senha:");
   posicao (9,0);
   sprintf (buffer,"%01u",car1);
   escreve (buffer[0]);
   posicao (10,0);
   sprintf(buffer,"%01u",car2);   
   escreve(buffer[0]);
   posicao (11,0);
   sprintf(buffer,"%01u",car3);  
   escreve(buffer[0]);
   posicao (12,0);
   sprintf(buffer,"%01u",car4);  
   escreve(buffer[0]);
   posicao (13,0);  
   escreve("   ");
   posicao (0,1);
   escreve("         ");
   posicao (13,1);
   escreve("   ");

   if ((editando)==(0))
   {
   if (((cima)==(1))&&((click)==(0)))
   {
   car1++;
   click=1;
   cima=0;
   }
   if (((baixo)==(1))&&((click)==(0)))
   {
   car1--;
   click=1;
   baixo=0;
   }
   if ((car1)==(255))
   {
   ((car1)=(9));
   }
   posicao (9,1);
   escreve ("^   ");
   }
   
   
   if ((editando)==(1))
   {
   if (((cima)==(1))&&((click)==(0)))
   {
   car2++;
   click=1;
   cima=0;
   }
   if (((baixo)==(1))&&((click)==(0)))
   {
   car2--;
   click=1;
   baixo=0;
   }
   if ((car2)==(255))
   {
   ((car2)=(9));
   }
   posicao (9,1);
   escreve (" ^  ");
   }
   
   
   
   if ((editando)==(2))
   {
   if (((baixo)==(1))&&((click)==(0)))
   {
   car3--;
   click=1;
   baixo=0;
   }
   if (((cima)==(1))&&((click)==(0)))
   {
   car3++;
   click=1;
   cima=0;
   }
   if ((car3)==(255))
   {
   ((car3)=(9));
   }
   posicao (9,1);
   escreve ("  ^ ");
   }
   
   if ((editando)==(3))
   {
   if (((baixo)==(1))&&((click)==(0)))
   {
   car4--;
   click=1;
   baixo=0;
   }
   if (((cima)==(1))&&((click)==(0)))
   {
   car4++;
   click=1;
   cima=0;
   }
   if ((car4)==(255))
   {
   ((car4)=(9));
   }
   posicao (9,1);
   escreve ("   ^");
   }
   
   if ((editando)==(4))
   {
   ((editando)=(0));
   }
   
   if ((editando)==(255))
   {
   ((editando)=(3));
   }   

 
 
  if ((car1)>(9))
  {
  car1=0;
  }
  if ((car2)>(9))
  {
  car2=0;
  }
  if ((car3)>(9))
  {
  car3=0;
  }
  if ((car4)>(9))
  {
  car4=0;
  } 
}    
}

void menu()
{ 
   
   if (((baixo)==(1))&&((click)==(0))&&((ir)==(0))&&((menuprince)==(0)))
   {
   filtro++;
   if (((baixo)==(1))&&((click)==(0))&&((ir)==(0))&&((menuprince)==(0))&&((filtro)>(20)))
   {
   filtro=0;
   click=1;
   baixo=0;
   abrir();
   }
   }
   
   if (((sai)==(1))&&((click)==(0))&&((ir)==(0))&&((menuprince)==(0)))
   {
   filtro++;
   if (((sai)==(1))&&((click)==(0))&&((ir)==(0))&&((menuprince)==(0))&&((filtro)>(20)))
   {
   filtro=0;
   click=1;
   sai=0;
   fechar();
   }
   }
   
   if (((entra)==(1))&&((click)==(0)))
   {
   click=1;
   entra=0;
   ((menuprince)=(ir));
   }
   
   if (((esquerda)==(1))&&((click)==(0)))
   {
     ir--;
     click=1;
     esquerda=0;
     if ((ir)==(255))
     {
     ir=1;
     }
   }
   
   if (((direita)==(1))&&((click)==(0)))
   {
     ir++;
     click=1;
     direita=0;
     if ((ir)==(2))
     {
     ir=0;
     }
   }
   
   
   if (((menuprince)==(0))&&((ir)==(0))&&((logado)==(1)))
    {
    posicao(0,0);
    escreve("     ONLINE     ");
    if ((segundos)<(5))
    {
    posicao (0,1);
    escreve("SAIR PARA FECHAR");
    }
    if ((segundos)>(5))
    {
    posicao (0,1);
    escreve("BAIXO PARA ABRIR");
    }
    }
    
   if (((menuprince)==(0))&&((ir)==(0))&&((logado)==(0)))
    {
    posicao(0,0);
    escreve("BEM VINDO       ");
    posicao (0,1);
    escreve("         OFFLINE");
    }
   
   if (((menuprince)==(0))&&((ir)==(1))&&((logado)==(0)))
   {
   posicao (0,0);
   escreve("EFETUAR         ");
   posicao (0,1);
   escreve("           LOGIN");
   }
  
   if (((menuprince)==(0))&&((ir)==(1))&&((logado)==(1)))
   {
   posicao (0,0);
   escreve("COMANDAR       ");
   posicao (0,1);
   escreve("          LOGADO");
   }
   
   
}

void main()
{
   
   setup_adc_ports(RA0_RA1_RA3_ANALOG); // Configura conversor A/D para usar Pinos A0,A1,A3 com referência ao Vcc...
   setup_adc(ADC_CLOCK_INTERNAL); // Configura conversor A/D para usar clock interno...
   setup_psp(PSP_DISABLED); // Desabilita porta paralela...
   setup_spi(FALSE); // Desliga comunicação serial SPI...
   setup_counters(RTCC_INTERNAL,RTCC_DIV_2); // Configura timer 0 com clock interno e prescaler 1:2...
  // setup_timer_1(T1_DISABLED); // Desliga timer 1...
  // setup_timer_2(T2_DISABLED,0,1); // Desliga timer 2...
   lcd_ini(); // Inicializa dislpay LCD...
   set_adc_channel(1); // Seleciona canal 1 do conversor A/D (Pino A1)...
   delay_us(20); // Aguarda 20 us para estabilização do conversor A/D...
   limpa();
   setup_timer_1(t1_internal|t1_div_by_1);
   enable_interrupts(int_timer1);
   enable_interrupts(global);
   set_timer1(15535);
   port_b_pullups(true); // Habilita resistores de pull-up dp Port B...
   set_tris_b(0b01111111); // Configura Port B... 
   set_tris_c(0b01101100);
   output_b(0x00);

   
   while(true)
 {
 
   if (((logado)==(0))&&((click)==(0)))
   {
   if (((segundos)==(0))||((segundos)==(5))||((segundos)==(10))||((segundos)==(15))||((segundos)==(20))||((segundos)==(25))||((segundos)==(30))||((segundos)==(35))||((segundos)==(40))||((segundos)==(45))||((segundos)==(50))||((segundos)==(55)))
   {
   click=1;
   (output_high(pin_c5));
   delay_ms(10);
   (output_low(pin_c5));
   }
   }
   if ((segundos)>(40))
   {
   logado=0;
   car1=0;
   car2=0;
   car3=0;
   car4=0;
   }
   
   if (((menuprince)==(0))&&(!input(pin_b3))&&(!input(pin_b4))&&(!input(pin_b6))&&(!input(pin_b5))&&((click)==(0)))
   {
   click=1;
   menuprince=99;
   }
   
   if ((input(pin_b3))&&(input(pin_b4))&&(input(pin_b6))&&(input(pin_b5))&&((click)==(1)))
   {
   parado++;
      if ((parado)>(30))
      {
      click=0;
      parado=0;
      direita=0;
      esquerda=0;
      cima=0;
      baixo=0;
      entra=0;
      sai=0;      
      }
   }
   
      if ((!input(pin_b6))&&(input(pin_b5)))
   {
    if ((!input(pin_b6))&&(input(pin_b5))&&((click)==(0)))
     {
     segundos=0;
     direita=1;
     }
   } 
   
   if ((input(pin_b6))&&(!input(pin_b5)))
   {
    if ((input(pin_b6))&&(!input(pin_b5))&&((click)==(0)))
     {
     sai=1;
     segundos=0;
     }
   } 
   
    if ((!input(pin_b6))&&(!input(pin_b5)))
   {
    if ((!input(pin_b6))&&(!input(pin_b5))&&((click)==(0)))
     {
     entra=1;
     segundos=0;
     }
   }
   
    if ((!input(pin_b3))&&(input(pin_b4)))
   {
    if ((!input(pin_b3))&&(input(pin_b4))&&((click)==(0)))
     {
     esquerda=1;
     segundos=0;
     }
   }
   
   if ((!input(pin_b3))&&(!input(pin_b4)))
   {
    if ((!input(pin_b3))&&(!input(pin_b4))&&((click)==(0)))
     {
     cima=1;
     segundos=0;
     }
   }
   
   if ((input(pin_b3))&&(!input(pin_b4)))
   {
    if ((input(pin_b3))&&(!input(pin_b4))&&((click)==(0)))
     {
     baixo=1;
     segundos=0;
     }
   }
   
   if (!input(pin_b1))
   {
   filtro++;
   if ((!input(pin_b1))&&((filtro)>(20)))
   {
   filtro=0;
    if ((!input(pin_b0))&&(input(pin_b2)))
   {
   logado=1;
   fechar();
   logado=0;
   }
   if ((input(pin_b0))&&(!input(pin_b2)))
   {
   logado=1;
   abrir();
   logado=0;
   }
   }
   }
   
   if ((menuprince)==(0))
   {
   menu();
   }  
    
   if ((menuprince)==(1))
   {
   login();
   }
   
   if ((menuprince)==(2))
   {
   comandos();
   }
   if ((menuprince)==(99))
   {
   cadastro();
   }
     
 }
}
