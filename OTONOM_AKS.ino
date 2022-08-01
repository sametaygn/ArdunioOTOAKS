#include <ServoTimer2.h> // ServoTimer2 kütüphanesini ekliyoruz
ServoTimer2 servo;      // Kütüphaneyi servom adıyla tanımlıyoruz
#include <avr/interrupt.h>
#include <avr/io.h>
#define dirPin 2
#define stepPin 3
#define signal_lampR 4
#define signal_lampL 5
int signal_lamp=0;
int wanted_degree;
String command="";
int motor = 9;
char temp;
int adim=200*8;
int sayac=0;
void setup(){
   servo.attach(motor);
   servo.write(0);
   pinMode(stepPin, OUTPUT);
   pinMode(dirPin, OUTPUT);
   pinMode(signal_lampR, OUTPUT);
   pinMode(signal_lampL, OUTPUT);
 
   UBRR0 = 103; // for configuring baud rate of 9600bps
   UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); 
  // Use 8-bit character sizes
   UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);  
  // Turn on the transmission, reception, and Receive interrupt  
   // Turn on the transmission, reception, and Receive interrupt

  //Timer1 Cofigrations
  TCCR1A=0b00000000; 
  TCCR1B=0b00001000;
  TCNT1=0;
  OCR1A=1000;// ADJUST MOTOR TURNING Speed 0 ==> Fastest 65536=>> Slowest
  TIMSK1|=(1<<OCIE1A);
  //Timer3 Cofigrations
  TCCR3A=0b00000000; 
  TCCR3B=0b00001010;
  TCNT3=0;
  OCR3A=1000;// ADJUST MOTOR TURNING Speed 0 ==> Fastest 65536=>> Slowest
  TIMSK3|=(1<<OCIE1A);
   sei();// enable global interrupt

  
}
ISR(TIMER1_COMPA_vect){
if(sayac<adim){
  digitalWrite(stepPin,!digitalRead(stepPin));
  digitalWrite(stepPin,!digitalRead(stepPin));
  
  sayac++;
}
else if(sayac==adim){
  TCCR1B=0b00001000;
  sayac=0;
  TCNT1=0;
  USART_TransmitString("Dönüş Yapıldı\n");
  sayac=0;
 
}
}
ISR(TIMER3_COMPA_vect){
  if(signal_lamp==0){
  digitalWrite(signal_lampR,LOW);
  digitalWrite(signal_lampR,HIGH);
  }
  else if(signal_lamp==1){
  digitalWrite(signal_lampR,!digitalRead(signal_lampR));
  }
  else if(signal_lamp==2){
  digitalWrite(signal_lampL,!digitalRead(signal_lampL));
  }
  else if(signal_lamp==3){
  digitalWrite(signal_lampL,!digitalRead(signal_lampL));
  digitalWrite(signal_lampR,!digitalRead(signal_lampL));
  }
  OCR3A=0;
}

void loop()
{
 

}

ISR(USART0_RX_vect)
{ 
  temp=UDR0;// read the received data byte in temp
  if(temp!='\n'){
  command+=temp;  
  }
  else{
    if((command.charAt(0)=='T'||command.charAt(0)=='t')&&command.length()<5)
    {
        wanted_degree=command.substring(1).toInt();
        if(command.substring(1).toInt()>0){
          USART_TransmitString("Turn Right");
          //Write Step Turn Code
        }
        else if(command.substring(1).toInt()<0){
          USART_TransmitString("Turn Left");
          //Write Step Turn Code
        }
        else if(command.substring(1).toInt()==0){
          USART_TransmitString("Wheel Center ");
          //Write Step Turn Code
        }
        else{
          USART_TransmitString("Unknow Command");
        }
    }
    else if(command.charAt(0)=='B'||command.charAt(0)=='b'){
      if(command.charAt(1)=='0'&&command.length()==2)
      {
        //Write Release Brake Function
        USART_TransmitString("Release Brake");
      }
      else if(command.charAt(1)=='1'&&command.length()==2){
        //Write Break Function
        USART_TransmitString("Break");
      }
      else{
        USART_TransmitString("Unknow Command");
      }
    }
    else if(command.charAt(0)=='S'||command.charAt(0)=='s'){
      if(command.charAt(1)=='0'&&command.length()==2)
      {
        //Write Release Brake Function
        signal_lamp=0;
      }
      else if(command.charAt(1)=='1'&&command.length()==2){
        //Write Break Function
        signal_lamp=1;
      }
      else if(command.charAt(1)=='2'&&command.length()==2){
        //Write Break Function
        signal_lamp=2;
      }
      else if(command.charAt(1)=='3'&&command.length()==2){
        //Write Break Function
        signal_lamp=3;
      }
      else{
        USART_TransmitString("Unknow Command");
      }
    }
    else{
        USART_TransmitString("Unknow Command");
    }
    USART_TransmitString("\n");
    command="";
  }
}
void USART_TransmitPolling(char DataByte)
{
  while (( UCSR0A & (1<<UDRE0)) == 0) {}; // Do nothing until UDR is ready
  UDR0 = DataByte;
}
void USART_TransmitString(String DataByte)
{
  
  for(int i=0;i<DataByte.length();i++){
    USART_TransmitPolling(DataByte.charAt(i));
  }
  
}
