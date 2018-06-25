/*
 * File:   main.c
 * Author: TAKATSUKI
 * MPLAB X IDE v3.61
 * XC8 v1.42
 * Created on 2018/06/13, 10:41
 * LED���C�g PIC12F1822�}�C�R���g�p PWM5�i�K����
 * �X�y�b�N�FDC3.3V���́A�ő�427mA�œ_��
 */

#include <xc.h>

#define _XTAL_FREQ 8000000
#define TMR0CNT 6                 // 1ms*10�J�E���g=10ms

// �O���[�o���ϐ���`
int cnt = 0;
int chat_cnt = 0;                   // �`���^�����O�J�E���g
int sw_cnt = 0;                     // SW�v�b�V���J�E���g

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF       // PLL Enable (4x PLL enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = HI        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), high trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)

/* Timer0�����ݏ���
 * �`���^�����O�h�~(10ms����SW��resd)
 */
void interrupt InterTimer(void){
    if(TMR0IF == 1){                // Timer0�����ݔ�����?
        TMR0   = TMR0CNT;           // �^�C�}�[0�̏�����
        TMR0IF = 0;                 // Timer0�����݃t���O�����Z�b�g
        if(chat_cnt >= 3){          // ��莞�Ԉȏ�HI��
            if(RA5 == 0){           // SW��LOW�ɂȂ��Ă�����
                ++sw_cnt;           // 1�i�K��������
                if(sw_cnt > 5) sw_cnt = 0;  // ���xMAX����������xMIN��
                if(chat_cnt >= 194){        // ��2s�ŏ���
                    chat_cnt = 0;   // �J�E���g0��
                    sw_cnt = 0;     // ��������
                }
            }
        }
        
        if(RA5 == 1){               // SW�������ꂽ��
            ++chat_cnt;             // �J�E���g����
        } else {                    // SW��������ĂȂ�������
            chat_cnt =0;            // �J�E���g�N���A����
        }        
    }
}


void main(){
    OSCCON = 0b01110010;
    ANSELA = 0b00000000;
    TRISA  = 0b00101000;
    
    OPTION_REG = 0b10000010;        // TIMER0���g�p, ��ؽ��׶��Ēl1:8, �����v���A�b�v����
    TMR0   = TMR0CNT;               // �^�C�}�[0������
    TMR0IF = 0;                     // �^�C�}�[0�����t���O(TMR0IF)��0�ɂ���
    TMR0IE = 1;                     // �^�C�}�[0������(TMR0IE)��������
    GIE    = 1;                     // �S�����ݏ�����������

    CCP1SEL = 0;                    // RA2����PWM�o��
    CCP1CON = 0b00001100;           // PWM�V���O���o��
    T2CON   = 0b00000010;           // Timer2�v���X�P�[���l1:16
    CCPR1L  = 0;                    // Duty������ (100%���ۂ͔��])
    CCPR1H  = 0;                    // Duty������ HI�r�b�g
    TMR2    = 0;                    // Timer2�J�E���^������
    PR2     = 249;                  // Timer2�J�E���^������
    TMR2ON  = 1;                    // Timer2�X�^�[�g(PWM�o��)
    
    while(1){
        if(sw_cnt > 5) sw_cnt = 0;
        switch(sw_cnt){
            case(1):
                CCPR1L = 50;    // 20%
                break;
            case(2):
                CCPR1L = 125;   // 50%
                break;
            case(3):
                CCPR1L = 175;   // 70%
                break;
            case(4):
                CCPR1L = 200;   // 80%
                break;
            case(5):
                CCPR1L = 249;   // 100%
                break;
            case(0):
                CCPR1L = 0;
                break;
        }
    }
        
}