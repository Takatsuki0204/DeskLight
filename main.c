/*
 * File:   main.c
 * Author: TAKATSUKI
 * MPLAB X IDE v3.61
 * XC8 v1.42
 * Created on 2018/06/13, 10:41
 * LEDライト PIC12F1822マイコン使用 PWM5段階調光
 * スペック：DC3.3V入力、最大427mAで点灯
 */

#include <xc.h>

#define _XTAL_FREQ 8000000
#define TMR0CNT 6                 // 1ms*10カウント=10ms

// グローバル変数定義
int cnt = 0;
int chat_cnt = 0;                   // チャタリングカウント
int sw_cnt = 0;                     // SWプッシュカウント

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

/* Timer0割込み処理
 * チャタリング防止(10ms毎にSWをresd)
 */
void interrupt InterTimer(void){
    if(TMR0IF == 1){                // Timer0割込み発生か?
        TMR0   = TMR0CNT;           // タイマー0の初期化
        TMR0IF = 0;                 // Timer0割込みフラグをリセット
        if(chat_cnt >= 3){          // 一定時間以上HIで
            if(RA5 == 0){           // SWがLOWになっていたら
                ++sw_cnt;           // 1段階調光する
                if(sw_cnt > 5) sw_cnt = 0;  // 光度MAXだったら光度MINに
                if(chat_cnt >= 194){        // 約2sで消灯
                    chat_cnt = 0;   // カウント0に
                    sw_cnt = 0;     // 消灯する
                }
            }
        }
        
        if(RA5 == 1){               // SWが押されたら
            ++chat_cnt;             // カウントする
        } else {                    // SWが押されてなかったら
            chat_cnt =0;            // カウントクリアする
        }        
    }
}


void main(){
    OSCCON = 0b01110010;
    ANSELA = 0b00000000;
    TRISA  = 0b00101000;
    
    OPTION_REG = 0b10000010;        // TIMER0を使用, ﾌﾟﾘｽｹｰﾗｶｳﾝﾄ値1:8, 内部プルアップ無効
    TMR0   = TMR0CNT;               // タイマー0初期化
    TMR0IF = 0;                     // タイマー0割込フラグ(TMR0IF)を0にする
    TMR0IE = 1;                     // タイマー0割込み(TMR0IE)を許可する
    GIE    = 1;                     // 全割込み処理を許可する

    CCP1SEL = 0;                    // RA2からPWM出力
    CCP1CON = 0b00001100;           // PWMシングル出力
    T2CON   = 0b00000010;           // Timer2プリスケーラ値1:16
    CCPR1L  = 0;                    // Duty初期化 (100%実際は反転)
    CCPR1H  = 0;                    // Duty初期化 HIビット
    TMR2    = 0;                    // Timer2カウンタ初期化
    PR2     = 249;                  // Timer2カウンタ初期化
    TMR2ON  = 1;                    // Timer2スタート(PWM出力)
    
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