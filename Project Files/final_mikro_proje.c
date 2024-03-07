#include <final_mikro_proje.h>
#include <lcd.c>
#include <stdlib.h>
#fuses NOWDT

#use delay(clock=20MHz)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, ERRORS)

#define SELECT PIN_B4
#define UP PIN_B5
#define DOWN PIN_B6
#define EXIT PIN_B7

#define LAZER PIN_B0


#define PWM0 PIN_C0

unsigned int16 altValue = 0;
unsigned int16 azValue = 0;
unsigned int16 sensorValue = 0;


int16 max_time = 100, min_time = 50;
int16 end_time = 1000;
int16 tasma = 0, duty0 = 0;


int outOfAngle=0;
int16 sensorAngleCalculate(int16 azObject,int16 heading){
  // kuzey 0 derece 
  // servo 90 sensör yönü
  // örenk senaryo sensör 20 derece yollar
  // gözlemlemek istenen açý 50 derecedir
  // 50-20 + 90 yapýldýðýnda servonun gitmesi geren açý bulunur.
  
  // sensör 350 yolladý gözlem açýsý 80 derecede
  // 80 - -10 +90 = 180 servonun 180 dereceye mi gelmesi lazým
  // sensör açýsý+90 derece servo derecesi ile eþit
   int16 angle;
   if (heading > 180) {
        heading = heading - 360;}
   
   angle= azObject - heading +90;
   //-10   20 -170 +90
   
   
   if (angle <= 0) {
   outOfAngle=1;
   lcd_putc('\f');
   printf(lcd_putc, "Saat yonu tersi ");
   lcd_gotoxy(1, 2);
   printf(lcd_putc, "ceviriniz ");
   return 50;
   }
   
   else if (angle >= 180) {
   outOfAngle=1;
   lcd_putc('\f');
   printf(lcd_putc, "Saat yonunde");
   lcd_gotoxy(1, 2);
   printf(lcd_putc, "ceviriniz ");
   return 100;
   }
   else {
      
      int16 out;
      out =  (angle * 50 / 180) + 50;
      return out;
    }
}

#int_TIMER2
void T2_isr(void){

  if(tasma<end_time){
  tasma++;}
  else{
  tasma = 0;}
  if(tasma<=duty0){
  output_high(PWM0);}
  else{
 output_low(PWM0);}
}

signed int selectedObject = 0;
char objects[9][10] = {"Mercury", "Venus", "Mars", "Jupiter","Saturn","Uranus","Neptune","Sun","Moon"};

void loadingAnimation() {

    lcd_gotoxy(1, 2);
    printf(lcd_putc, "Loading");
    for(int i = 0; i < 8; i++) {
        printf(lcd_putc, ".");
        delay_ms(500); // 500 ms delay between each character
    }
}



int inMenu = 0;
int inObsMenu = 0;

void anaEkran() {
    lcd_putc('\f');
    lcd_gotoxy(1, 1);
    printf(lcd_putc, "Select a object:");

    lcd_gotoxy(1, 2);
    lcd_putc("> ");

    lcd_gotoxy(3, 2);
    printf(lcd_putc, "%s", objects[selectedObject]);
}


#INT_RB // Portb Change Interrupt
void kesme_isr() {
    delay_ms(20); // Debouncing için kýsa bir gecikme
    if (input(SELECT) == 1) {
        inMenu=1;
        printf("%i",selectedObject);
        
        lcd_putc('\f');
        printf(lcd_putc, "%s ", objects[selectedObject]);
        loadingAnimation();
        inObsMenu=1;
        
        
    } 
    
    if (input(UP) == 1 && inMenu != 1) {
        selectedObject++;
        if (selectedObject > 8) selectedObject = 0;
        printf("%i",selectedObject);
        anaEkran();
        
    }
    
    if (input(DOWN) == 1 && inMenu != 1) {
        selectedObject--;
        if (selectedObject < 0) selectedObject = 8;
        printf("%i",selectedObject);
        anaEkran();
            
    } 
    
    if (input(EXIT) == 1) {
        inMenu = 0;
        inObsMenu=0;
        printf("%i",selectedObject);
        anaEkran(); 
        
    }
    
}


void main()
{  

   lcd_init();
   anaEkran();
   delay_ms(500);
   
   setup_timer_2(T2_DIV_BY_1, 99, 1);
   
   duty0 = min_time;
 
   
   set_timer2(0);
   

 
   enable_interrupts(INT_TIMER2); // Timer2 Iterrupt 
   enable_interrupts(INT_RB); // Portb Change Interrupt
   enable_interrupts(GLOBAL); // Genel kesmeleri 

    char buffer[30];

   
    setup_uart(9600);

   printf("%i",selectedObject);

   output_low(LAZER); 
    
   while(TRUE)
   {
        
        

       
       if(inMenu==1){
     
       
          altValue=0; azValue=0; sensorValue=0;
       
          gets(buffer);
   
          char *p = buffer;
        
          while (*p >= '0' && *p <= '9') {
      
          altValue = altValue * 10 + (*p - '0');
   
          p++;
    
          }
          if (*p == ',') {
      
          p++;
    
          }
    
             
          while (*p >= '0' && *p <= '9') {
     
          azValue = azValue * 10 + (*p - '0');
     
          p++;
   
          }
          if (*p == ',') {
      
          p++;
    
          }
          
          while (*p >= '0' && *p <= '9') {
     
          sensorValue = sensorValue * 10 + (*p - '0');
     
          p++;
   
          }
          
          
          
          buffer[0] = '\0';
       }  
         
        
        
        //lazerin calisma fonksiyonu
        if(inObsMenu==1 && outOfAngle!=1){  
        output_high(LAZER);
        duty0=sensorAngleCalculate(azValue,sensorValue);//duty degerini sensore gore ayarla
        lcd_putc('\f');
        printf(lcd_putc, "%s ", objects[selectedObject]);
        lcd_gotoxy(1, 2);
        printf(lcd_putc, "alt %li  az %li ", altValue,azValue);
        }
         
        
          
         
   }

}
