#include <12F675.h>
#device adc=8


#FUSES INTRC_IO
#FUSES NOWDT                    //No Watch Dog Timer
#FUSES HS                       //High speed Osc (> 4mhz for PCM/PCH) (>10mhz for PCD)
#FUSES NOPROTECT                //Code not protected from reading
#FUSES NOMCLR                   //Master Clear pin used for I/O
#FUSES NOPUT                    //No Power Up Timer
//#FUSES NOBROWNOUT               //No brownout reset
#FUSES XT
#FUSES LP
#FUSES INTRC
#FUSES CPD
#FUSES BROWNOUT                //Reset when brownout detected
 
#use delay(clock=4000000)

