      #include "Teste_Gerador_Teste_Vida11.h"
      
      int16   aux;
    
      Unsigned int8 conta, filtro ;
    
      
      #priority TIMER1 , RTCC
       
      #int_TIMER1
       
      void  TIMER1_isr(void )
      { 
      
       if (aux>=3035)
            {
            set_timer1 (aux);
            output_toggle(pin_A2);
         
            }
         else 
            {
            output_low(pin_A2);
           } 
      }
      
      #int_RTCC
      
      void RTCC_isr(void)  //entra nesta interrupção a cada 33mS
      
      {
         conta++;
         if (conta>=15)
            {
            output_toggle(PIN_A1); 
            conta=0;
            }
      
      }

           
      
      void main()
      {
         
         
       
         setup_adc_ports(sAN0|VSS_VDD);
         setup_adc(ADC_CLOCK_DIV_64);
         setup_timer_0(RTCC_INTERNAL|RTCC_DIV_128);
         
         setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);
         setup_vref(FALSE);
         enable_interrupts(INT_TIMER1);
         enable_interrupts(GLOBAL);
        // set_tris_a (0b00001111);
      
      
        int16 FreqSelecionada;
        int16 tempo;
        int8 incrementa,  setup=0, pic_desligado=0, parte1, parte2 ;
        
        while(true)
                
        {
            if (pic_desligado==0)
               { 
                                    
                  parte2 = read_eeprom(0x00);
                  parte1 = read_eeprom(0x08);
                  aux = Make16(parte1,parte2);
                  pic_desligado=1;
               } 
               
                FreqSelecionada=0; //força esta varialver para 0 antes de entrar no for                            
            for ( incrementa=0; incrementa<10; incrementa++)//Faz um loop para média
             {
                FreqSelecionada = FreqSelecionada + read_adc();//soma porta analogica 10 vezes
             } 
              
               FreqSelecionada = FreqSelecionada/10;// Faz a média
               
           if (FreqSelecionada>=250)FreqSelecionada=250;
           if (setup==2) setup=0 ;    
           if (setup==1)
                  {
                  tempo = 62500/FreqSelecionada;
                  aux = 65535 - tempo;
                 
                                                   
                   write_eeprom(0x00,make8(aux,0));
                   write_eeprom(0x08,make8(aux,1));
                                
                  conta++;
                  }
                     
                 if ((input(pin_a3))&&(!filtro))
                                 
                  {
                       filtro=1;             
                  }
                  else if (!input(pin_a3)&&(filtro))
                     {
                       filtro=0;                 
                       setup++; 
                     }
                 
                 
               
         }   
           
      }  
      
