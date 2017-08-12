#include <16f877a.h>
#use delay(clock = 20000000)
#fuses HS,NOWDT,PUT,NOPROTECT,NOBROWNOUT,NOLVP,NOCPD,NOWRT,XT,NODEBUG
#use fast_io(A)
#use fast_io(B)
#define sensor_d PIN_A0
#org 0x1F00, 0x1FFF void loader16F877(void) {} //for the 8k 16F876/7
/*tabla para PWM, ciclo de trabajo
     102 -> 10%
     205 -> 20%
     306 -> 30%
     409 -> 40%
     512 -> 50%
     614 -> 60%
     716 -> 70%
     818 -> 80%
     921 -> 90%
     1023-> 100%
*/
void avanza_p(void){
   output_d(0xa0);//salia a motores -> 1010 0000 
   set_pwm2_duty(75); //ccp1 = 40%(motor izquierdo)
   set_pwm1_duty(60); //ccp2 = 40%(motor derecho)
   delay_ms(200);
   set_pwm2_duty(0); //ccp1 = 40%(motor izquierdo)
   set_pwm1_duty(0); //ccp2 = 40%(motor derecho)
}
void giro_90_der(void){
   int sens;
      do{
         output_d(0x60);//salia a motores -> 0110 0000
         set_pwm1_duty(60); //ccp2 (motor derecha)
         set_pwm2_duty(75); //ccp1 (motor izquierda)
         sens=input_a();
      }while(sens!=0B00000100);
        
      
}
void giro_90_izq(void){
   int sen;
      avanza_p();
      sen=input_a();
      if(sen==0B00000000){
         do{
            output_d(0x90);//salia a motores -> 1001 0000
            set_pwm1_duty(60); //ccp2 (motor derecha)
            set_pwm2_duty(75); //ccp1 (motor izquierda)
            sen=input_a();
         }while(sen!=0B00000100);
        
      }else{
         avanza_p();
      }
      
}
void giro_180(void){
    int sen;
    do{
      output_d(0x60);//salia a motores -> 0110 0000
      set_pwm1_duty(60); //ccp2 (motor derecha)
      set_pwm2_duty(75); //ccp1 (motor izquierda)
      sen=input_a();
    }while(sen!=0B00000100);
}
void game_over(void){

      output_d(0x00);//salia a motores -> 0000 0000
      set_pwm1_duty(0); //ccp2 (motor derecha)
      set_pwm2_duty(0); //ccp1 (motor izquierda)
      output_b(0x00);
      output_high(pin_d2);
      while(true){}

}


void main(){
   int inicio=0;
   int sensores;
   //config puertos
   set_tris_a(0xff);
   set_tris_b(0x00);
   set_tris_d(0x00);
   set_tris_c(0x00);
   set_tris_e(0xff);
   //PWM
   //config. TMR2 para frec=1.2khz 
   setup_timer_2(T2_DIV_BY_16,255,1);  //preescalador(T2_DIV_BY_16)=16,PR2=255, Postescalador=1
   setup_ccp1(CCP_PWM);
   setup_ccp2(CCP_PWM);
   set_pwm1_duty(0); //ccp2 (motor derecha)
   set_pwm2_duty(0); //ccp1 (motor izquierda)
   
   output_low(pin_d2);
   while(true){
      sensores = input_a();
      output_b(sensores);
      inicio=input_e();
      if(inicio==0B00000001)
         break;
   }
   
   while(true){
    sensores = input_a();
    output_b(sensores);
    
    //adelante
   if(sensores==0B00000100){
         output_d(0xa0);//salia a motores -> 1010 0000 
         set_pwm2_duty(75); //ccp1 = 40%(motor izquierdo)
         set_pwm1_duty(60); //ccp2 = 40%(motor derecho)
   }
  
    //Izquierda baja
      if(sensores==0B00001000 || sensores==0B00001100){
         output_d(0xa0);//salia a motores -> 1010 0000 
         set_pwm2_duty(53); //ccp1 = 40%(motor izquierdo)
         set_pwm1_duty(75); //ccp2 = 40%(motor derecho)
      }
      
      //Izquierda media
      if(sensores==0B00010000){
         output_d(0xa0);//salia a motores -> 1010 0000
         set_pwm2_duty(46); //ccp1 = 40%(motor izquierdo)
         set_pwm1_duty(75); //ccp2 = 40%(motor derecho)
      }
      //Derecha baja
      if(sensores==0B00000010|| sensores==0B00000110){
         output_d(0xa0);//salia a motores -> 1010 0000
         set_pwm2_duty(75); //ccp1 = 40%(motor izquierdo)
         set_pwm1_duty(53); //ccp2 = 40%(motor derecho)
      }
      
      //Derecha media
      if(sensores==0B00000001){
         output_d(0xa0);//salia a motores -> 1010 0000
         set_pwm2_duty(75); //ccp1 = 40%(motor izquierdo)
         set_pwm1_duty(46); //ccp2 = 40%(motor derecho)
      }
      //caso giro 90 o fin de juego
      if(sensores==0B00011111){
         avanza_p();
         sensores=input_a();
 
         if(sensores==0B00011111){
            game_over();
         }else{
            giro_90_der();
         }
        
         
      }
      //caso camino sin salida
      if(sensores==0B00000000){
         giro_180();
      }
      //caso camino solo a la derecha 
      if(sensores==0B00000111 || sensores==0B00000011){
         avanza_p();
         sensores=input_a();
         if(sensores==0B00000000){
            giro_90_der();
         }
         if(sensores==0B00000000){
            giro_90_der();
         }
         
      }
      //caso camino solo izq o hacia enfrente
      if(sensores==0B00011100 || sensores==0B00011000){
         giro_90_izq();
      }
      
}

}
