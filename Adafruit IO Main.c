//******************************************************************************
/* 
 * File:   Adafruit IO.c
 * Author: Daniel
 *
 * Created on August 18, 2021, 08:24 PM
 */
//******************************************************************************
// Importación de Librerías
//******************************************************************************

#pragma config FOSC = XT        // Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
#define _XTAL_FREQ 8000000

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "USART header.h"

//******************************************************************************
// Prototipos de funciones
//******************************************************************************
void Setup(void);

//******************************************************************************
// Variables
//******************************************************************************
char Valor[10];

int ContadorPlus = 0;
int ContadorMinus = 0;

int DatoRecibido;
unsigned char Contador;
float DatoEnviado;

int espacio = 32;
int enter = 13;
int borrar = 8;

__bit ANTIREBOTE_CONTADOR1;
__bit ANTIREBOTE_CONTADOR2;

//******************************************************************************
// Vector de interrupcion
//******************************************************************************

void __interrupt() ISR(void){
//****************************************************************
//      INTERRUPCION UART
//**************************************************************** 
    if(RCIF == 1){
        InterruptReciboUSART(&DatoRecibido);
        PORTD = DatoRecibido;
        RCIF = 0;
    }
//******************************************************************************
//      INTERRUPCION PUERTO B
//******************************************************************************    
    if(RBIF == 1){
        //Contador ascendente
        if (RB0 == 1) {
            ANTIREBOTE_CONTADOR1 = 1;
        }
        if (ANTIREBOTE_CONTADOR1 == 1 && RB0 == 0){
            
//            for (int j=0;j<4;j++){
//                EnvioSerial(borrar);
//            }
            
            Contador = Contador + 1; 
            
            ANTIREBOTE_CONTADOR1 = 0;
        }
        //Contador descendente
         if (RB1 == 1) {
            ANTIREBOTE_CONTADOR2 = 1;
         }
        if (ANTIREBOTE_CONTADOR2 == 1 && RB1 == 0){
            
//            for (int j=0;j<4;j++){
//                EnvioSerial(borrar);
//            }
            
            Contador = Contador - 1;
            
            ANTIREBOTE_CONTADOR2 = 0;
        }
        
        RBIF = 0;
    }
}

//******************************************************************************
// Ciclo principal
//******************************************************************************
void main(void) {
  Setup(); 
  
  while(1)
  {
        DatoEnviado = Contador*1;
        sprintf(Valor, "%0.0f,%0.0f,%0.0f,", DatoEnviado, DatoEnviado, DatoEnviado); 
        UART_Write_Text(Valor);   
        
        __delay_us(500);
  }
    return;
}
//******************************************************************************
// Configuración
//******************************************************************************
void Setup(void){     
    PORTA = 0x00;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;

    TRISA = 0x00;
    TRISC = 0b10000000;
    TRISD = 0x00;
    TRISE = 0x00; 

    TRISB = 0xFF;  
    PORTB = 0;
    
    ANSEL = 0;
    ANSELH = 0;
    
    //Configuración de la comunicación, recepción y transmisión
    
    SPBRG = 12; //baud rate 9600
    SYNC = 0; //comunicación asíncrona
    SPEN = 1; //habilita comunicación
    CREN = 1;
    TXEN = 1;
    //Limpiamos bandera de recepción
    RCIF = 0;
    RCIE = 1; //Habilita interrupción de recepción
 
    IOCB0 = 1; //Habilita interrupción por cambio
    IOCB1 = 1;
    
    RBIE=1;    //Habilita interrupción puerto B
    RBIF=0;
    
    PEIE=1;     //Habilita interrupciones perifericas
    GIE=1;    //Habilita interrupciones globales
    
    IRCF0 = 1; //Oscilador a 8MHz
    IRCF1 = 1;
    IRCF2 = 1;
    SCS = 1;
}