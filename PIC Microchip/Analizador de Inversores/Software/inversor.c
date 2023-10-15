#include <16F876A.h>
#device adc=8
#use delay(clock=20000000)

   //Configurações de gravação
   #fuses NOWDT                		    //No Watch Dog Timer
   #fuses XT                    		    //Oscillator
   #fuses PUT                   		    //Power Up Timer
   #fuses PROTECT              		    //Code protected from reading
   #fuses BROWNOUT                  	    //Reset when brownout detected
   #fuses NOCPD              		    //No EE protection
   #fuses NOWRT                   		    //Program memory not write protected
   #fuses NODEBUG                 	    //No Debug mode for ICD
 
   //Limites de corrente
   #define egmin 151
   #define egmax 182
   #define emmin 98
   #define emmax 122
   
   //Limite de driver
   #define driver_max 20
   
   //Limites de cada modo de operação
   #define freq_max 126
   #define freq_min 114
   #define drop_max 150
   #define drop_min 49

   //Vetor padrão de sequência de fases
   int pattern[] = {0b110, 0b100, 0b101, 0b001, 0b011, 0b010};      
   
   //Declaração de variáveis globais
   boolean control, motor, inicia_teste;
      //Control = 1 --> Controle por frequencia
      //Control = 0 --> Controle por drop-in
      //Motor = 1 --> Motor do tipo EG
      //Motor = 0 --> Motor do tipo EM
      //Inicia_teste = 1 --> Realizar teste
      //Inicia_teste = 0 --> Em espera
   boolean error = 0;
      //Error = 1 --> Deu erro
      //Error = 0 --> Sem erro
   boolean acabou = 0;
      //Acabou = 1 --> Teste encerrado
      //Acabou = 0 --> Executar testes
   boolean begin = 1;
      //Begin = 0 --> Já leu pinos de tipo do compressor e modo de operação
      //Begin = 1 --> Aguarda start e faz leitura dos switches de modelo e modo
   int teste = 0;
      //Teste = 0 --> Corrente de partida
      //Teste = 1 --> Velocidade de operação
      //Teste = 2 --> Sequência de fases
      //Teste = 3 --> Driver
      //Teste = 4 --> Tempo de execução
   
   //Variáveis para limitar tempo máximo de execução
   long int conta_segundo;
   
   //Variável para piscar led on enquanto executa teste
   long int pisca_led;
   
   //Variáveis do teste de corrente de partida
   static int current_c;
   long int current_rep;
   long int current;
   long int aux;
   
   //Variáveis do teste de velocidade de estabilização
   boolean y;
   int time;
   int status_change;
   
   //Variáveis do teste de sequência de fases
   int phase, newphase;
   
   //Variáveis do teste de erro de driver
   long int driver_rep;
   long int driver_c;
   int z;

//----------------------------------
//Função que verifica se a corrente se encontra na faixa de valor desejado.
void current_check(long int current)
{
   if(motor)                          
   {
      if(current > egmin && current < egmax)
         teste = 1;
      else
         error = 1;
   }
   else 
   {
      if(current > emmin && current < emmax)
         teste = 1;
      else
         error = 1;
   }
}

//-----------------------------------
//Funçao que verifica se a frequência da corrente da saída do inversor
//está adequada.
void speed_test(int t)
{
   int freq;
   freq = 5000/t;                            

   if(control)
   {
      if(freq > freq_min && freq < freq_max)
      {
         teste = 2;
         phase = 0b100*input(pin_A3)+0b10*input(pin_A4)+input(pin_A5);
      }
      else
         error = 1;
   }
   else
   {
      if(freq > drop_min && freq < drop_max)
      {
         teste = 2;
         phase = 0b100*input(pin_A3)+0b10*input(pin_A4)+input(pin_A5);
      }
      else
         error = 1;
   }
}

//----------------------------------
//Função que verifica se a ordem das fases está correta.
void phase_order_test(int phase, int newphase)
{
   int i;
   int j;
   
   i = 0; j = 0;
   
   while (phase != pattern[i])
   {
      i++;
   }

   j = (i+1)%6;
   
   if (newphase == pattern[j])
      teste = 3;
   else
      error = 1;
}

//----------------------------------
//Função que verifica o funcionamento dos drivers do inversor
void driver_failure_test(int z)
{
   if(z > driver_max)
      acabou = 1;
   else
      error = 1;
}

//----------------------------------
//Tratador de interrupção do timer0
#int_timer0
void t0()
{   
   //Parte da configuração do timer
   set_timer0(156+get_timer0());
      
   //Chamada do teste de corrente de partida
   if (teste == 0 && !error && inicia_teste)
   {
      if(current_rep < 1000)
      {
         current_c++;
         if(current_c == 50)                            //50 estouros de timer = 10ms 
         {                                              //Realiza teste durante 1000*10ms = 10seg
            current_c = 0;
            aux = read_adc();
            current_rep++;
            if (aux > current)
            {
               current = aux;
            }
         }
      }
      else
      {
         current_check(current);                       
         y = input(pin_A3);                           
      }
   }
   
   //Chamada do teste de velocidade
   if (teste == 1 && !error && inicia_teste)
   {
      if (y == input(pin_A3))     
      {
         if (status_change > 0)                          //Verifica se já mudou fase
         {                                               
            time++;
         }
      }
      else
      {  
         status_change++;
         time++;

         if (status_change > 2)
         {
            speed_test(time);
         }
      }
      y = input(pin_A3);                                 //Atualiza y                
   }

   //Chamada do teste de sequência de fases
   if (teste == 2 && !error && inicia_teste)
   {
      if(phase != 0b100*input(pin_A3)+0b10*input(pin_A4)+input(pin_A5))          
      {
         newphase = 0b100*input(pin_A3)+0b10*input(pin_A4)+input(pin_A5);
         phase_order_test(phase, newphase);
      }
   }

   //Chamada para teste de driver
   if (teste == 3 && !error && inicia_teste)
   {
      if (driver_rep < 100)
      {
         driver_c++;
         if (driver_c == 50)                             //50 estouros = 10ms
         {                                               //100*10ms = 1s
            driver_c = 0;
            z = z + input(pin_C3);                
            driver_rep++;
         }
      }
      else
         driver_failure_test(z);
   }
   
   //Piscar led = realizando teste
   //Controlar tempo máximo de execução
   ++pisca_led;
   if(pisca_led < 2500)
      output_low(pin_B0);                             
   else if (pisca_led == 5000)                         //1<->0 a cada 0.5 segundos
   {
      pisca_led = 0;
      ++conta_segundo;
   }
   else
      output_high(pin_B0);
      
   //Verificar tempo máximo de execução
   if (conta_segundo == 30)                              //30 segundos
   {
      error = 1;                                         //Ultrapassou tempo máximo
      teste = 4;
   }
      
   //Caso o inversor esteja ok
   if(acabou)
   {
      disable_interrupts(int_timer0 | global);
      output_low(pin_C4);
      output_low(pin_C5);                                //Desabilita alimentação
      output_high(pin_C6);                               //LED BICOLOR = GREEN = INVERSOR OK
      output_b(0b10000001);                              //0 => 7 seg e on ligado    
   }

   //Resultado de erros
   if (error)
   {
      disable_interrupts(int_timer0 | global);
      output_low(pin_C4);
      output_low(pin_C5);
      output_low(pin_C6);
      output_high(pin_C7);
              
      switch(teste)
      {
         case 0 : output_b(0b11110011);
                  break;
         
         case 1 : output_b(0b01001001);
                  break;
                 
         case 2 : output_b(0b01100001);
                  break;
                  
         case 3 : output_b(0b00110011);
                  break;
         
         case 4: output_b(0b00100101);
                 break;
      }
   }      
}


//----------------------------------
void main()
{
   //Configuração inicial de portas de entrada (1) e saída (0)
   //Portas não utilizadas foram setadas como entrada
   set_tris_A(0b111111);
   set_tris_B(0b00000000);
   set_tris_C(0b00001111);
   
   //Desabilita alimentação e led de status do inversor
   output_c(0);
  
   //Acende led On/Off
   output_b(0b11111111);
   
   //Inicializar variáveis
   begin = 1;
   error = 0;
   inicia_teste = 0;
   acabou = 0;
   teste = 0;
   pisca_led = 0;
   conta_segundo = 0;
   current_c = 0; current_rep = 0; current = 0; aux = 0;
   time = 0; status_change = 0; y = 0;
   phase = 0; newphase = 0;
   driver_rep = 0; driver_c = 0; z = 0;
         
   //Configurar A/D
   setup_adc(ADC_CLOCK_DIV_8);
   setup_adc_ports(AN0);
   set_adc_channel(0);
   delay_us(100);
         
   //Define tipo de controle e motor como parâmetros do teste
   //Não faz leitura até o botão start ser pressionado
   if(begin)
   {                                            
      while(!input(pin_C2))
      {
          //Wait start (pin_c2)
      }    
      
      output_high(pin_C4);                      
      inicia_teste = 1;  
         
      if(input(pin_A1))
         motor = 1;           //EG
      else 
         motor = 0;           //EM

      if(input(pin_A2))
         control = 1;         //Frequência
      else
      {
         control = 0;         //Drop-in
         output_high(pin_C5);
      }
         
      begin = 0;
   }

   //Configura timer com frequência 5000Hz
   setup_timer_0(RTCC_INTERNAL | RTCC_DIV_2);
   set_timer0(156);
   enable_interrupts(int_timer0 | global);
   
while(1);
}

