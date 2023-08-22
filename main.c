#include <main.h>
unsigned int mode2;
// External Interrupt 2 service routine
interrupt [EXT_INT2] void ext_int2_isr(void)
{
    delay_ms(200);
    if(!(PINB & 0x04)){
        mode1++;   // mode1 tang 1 dv sau moi lan bam   
    } 
 
    mode2 = mode1;
    if (mode1 == 8) mode1 = 0;
}
void set_nguong(){
    // kiem tra gia tri mode 1 va set cac gia tri nguong
    /* thuc hien dieu khien: khi nhan button
    - lan 1: dieu khien nhiet do
        + up: tang nhiet do nguong
        + down: giam nhiet do nguong
        + mode1 = 1
    - lan 2: dieu khien anh sang : mode1 = 2
    - lan 3: dieu khien bao chay : mode1  = 3
    - lan 4: quay ve che do hien thi trang thai
    */     
    if (mode1 == 1){ 
        but_ton_nhiet();
        Temp_0 = nguong_nhiet;
        lcd_gotoxy(0,0);
        lcd_puts("NHIET DO:");       
        sprintf(LCD_Buffer,"NGUONG : %4i",Temp_0);   
        lcd_gotoxy(0,1);
        lcd_puts(LCD_Buffer);
      
    }       

    if (mode1 == 4){
        but_ton_khoi_1();
        khoi_0_1 = nguong_khoi_1;
        lcd_gotoxy(0,0);
        lcd_puts("CANH BAO CHAY:");       
        sprintf(LCD_Buffer,"NGUONG 1 : %4i",khoi_0_1);   
        lcd_gotoxy(0,1);
        lcd_puts(LCD_Buffer);  
    }
    
    if (mode1 == 5){
        but_ton_khoi_2();
        khoi_0_2 = nguong_khoi_2;
        lcd_gotoxy(0,0);
        lcd_puts("CANH BAO CHAY:");       
        sprintf(LCD_Buffer,"NGUONG 2 : %4i",khoi_0_2);   
        lcd_gotoxy(0,1);
        lcd_puts(LCD_Buffer); 

    }  
    
    if (mode1 == 2){
        but_ton_as_1();
        anh_sang_0_1 = nguong_as_1;
        lcd_gotoxy(0,0);
        lcd_puts("ANH SANG:");       
        sprintf(LCD_Buffer,"NGUONG 1: %4i",anh_sang_0_1);   
        lcd_gotoxy(0,1);
        lcd_puts(LCD_Buffer);  
    }
    if (mode1 == 3){
        but_ton_as_2();
        anh_sang_0_2 = nguong_as_2;
        lcd_gotoxy(0,0);
        lcd_puts("ANH SANG:");       
        sprintf(LCD_Buffer,"NGUONG 2: %4i",anh_sang_0_2);   
        lcd_gotoxy(0,1);
        lcd_puts(LCD_Buffer);  
    }
    
    if (mode1 == 6){ 
        lcd_clear();
        lcd_gotoxy(0,0);
        lcd_puts("WAITING FOR");       
        lcd_gotoxy(0,1);
        lcd_puts("SETTING UP"); 
        delay_ms(3000);
        mode1++; 
        lcd_clear();
    } 

}  

void main(void){
    char Data;
    // ADC
    ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS0);     //enable ADC, khong dung interrupt
    ADMUX=ADC_VREF_TYPE;                         //chon kieu dien ap tham chieu   
    // input,output port
    PORTC=0xFF;
    DDRC=0xff;
    DDRA = 0x00;
    DDRB=0x83;
  
    
    DDRD = 0xff;

    // USART initialization
    // Communication Parameters: 8 Data, 1 Stop, No Parity
    // USART Receiver: On
    // USART Transmitter: On
    // USART Mode: Asynchronous
    // USART Baud Rate: 38400
    UCSRA=0x00;
    UCSRB=0x18;
    UCSRC=0x86;
    UBRRH=0x00;
    UBRRL=0x0C;
    
    // External Interrupt(s) initialization
    // INT0: Off
    // INT1: Off
    // INT2: On
    // INT2 Mode: Falling Edge
    GICR|=0x20;
    MCUCR=0x00;
    MCUCSR=0x00;
    GIFR=0x20;
    
    TCCR1A=0x22;   // CAU HINH CHO CAC CHE DO CHO TIMER   
    TCCR1B=0x1A;
    TCNT1H=0x00;
    TCNT1L=0x00;
    
    // PWM KENH A =====================================================
    ICR1H=0x4e;
    ICR1L=0x20;
    //ICR1 = 20000;

    //Bo chia Timer: sau 1us, TCNT1 tang 1 don vi
    //ICR1 = 20000 --> CHU KY cua xung tin hieu T = 20000us = 20ms

    OCR1AH=0x00;
    OCR1AL=0x00;
    OCR1B = 3000;
    //OCR1 = 10000 --> Thoi gian tin hieu o muc 1 T1 = 10000us =10000ms
    //Vay D = T1/T = 50%
    // PWM KENH B =====================================================
    //OCR1BH=0x00;
    //OCR1BL=0x00;
    
        
    // Global enable interrupts
    #asm("sei")    // Cho phep ngat toan cuc    
    
    // khoi tao DS18B20
    w1_init(); // cau hinh su dung 1 wire 
    ds18b20_init(T,0,0,DS18B20_9BIT_RES);  // Khoi tao DS18b20, do phan giai 12bit

    // khoi tao LCD
    lcd_init(16);
    lcd_clear();

    lcd_gotoxy(0,0);
    lcd_puts("BTL DLTDDK");
    lcd_gotoxy(0,1);
    lcd_puts("BY: SCNT GROUP");   
    delay_ms(2000);
    lcd_clear(); 
    
    lcd_gotoxy(0,0);
    lcd_puts("SET UP");
    lcd_gotoxy(0,1);
    lcd_puts("PLEASE!");   
    while (1){  
        
        while (mode1<6){
            set_nguong();
            
 
        }       
        read_parameter(); // doc cac thong so   
        dis_PC_nguong();
        //-------------------------hien thi PC----------------------------------
        if (Temp>=0){ 
            if ((Temp != Temp1)||(khoi != khoi1) || (anh_sang != anh_sang1)){                     
                dis_PC_parameter();
                dis_PC_cb();
            }
            Temp1=Temp;    // doc gia tri nhiet do va gan vao  bien Temp
            khoi1 = khoi;    // doc gia tri khoi
            anh_sang1 =  anh_sang;// doc gia tri cuong do anh sang         
            //-------------------------so sanh nguong---------------------------------- 
            so_sanh();  
            //--------------------------bao tren LCD----------------------------------- 
            if ((Temp<Temp_0) && (khoi<khoi_0_1) && (anh_sang>anh_sang_0_1)) { 
                lcd_clear(); 
                lcd_gotoxy(0,0);
                lcd_puts("AN TOAN"); // neu ca 3 thong so =1 thi an toan  
            }
            else {
                dis_LCD_cb();
            }

            //-------------------------dieu khien--------------------------------------
            control();
            if ((anh_sang >= anh_sang_0_1)&&( anh_sang <= anh_sang_0_2)) {
                cbi(L298, EN); //rem khong hoat dong
                m=0;
                n=0;
                OCR1B = 0;
            }
            else { 
                
                if (anh_sang < anh_sang_0_1) { 
                    if (m==0){
                        // keo rem  
                        OCR1B = 2500;   
                        sbi(L298, EN); //rem khong hoat dong
                        cbi(L298, IN1);
                        //cbi(L298, IN2);
                        delay_ms(900); 
                    }
                    m++; 
                    //tat rem
                    cbi(L298, EN); //rem khong hoat dong
                    //OCR1B = 0;
                }
               
                else  { 
                    if (n==0){
                        // keo rem   
                        OCR1B = 18500;
                        sbi(L298, EN); //rem khong hoat dong
                        sbi(L298, IN1);
                        //sbi(L298, IN2);
                        delay_ms(7000); 
                    }
                    n++;
                    //tat rem
                    cbi(L298, EN); //rem khong hoat dong   
                    //OCR1B = 0;
                }              
            }
        }  
    }        
}
