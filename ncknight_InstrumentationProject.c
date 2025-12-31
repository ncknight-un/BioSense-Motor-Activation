//Nolan Knight
//BME595 - BioInstrumentation
//Dr. Ken Yoshida
//Copyright included as Modified ADC code was borrowed from TI Library examples:

/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP430FR235x Demo - ADC, Sample A1, AVcc Ref, Set LED if A1 > 0.5*AVcc
//   Cash Hao
//   Texas Instruments Inc.
//   Nov. 2016
//   Built with IAR Embedded Workbench v6.50.0 & Code Composer Studio v6.2.0
//******************************************************************************

//Beginning of Modified ADC Code:
#include <msp430.h>

//Create Global Variable of ADC_Result
unsigned int ADC_Result;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                               // Stop WDT

    //Motor Control PWM Setup
    P1DIR |= BIT6;                                          // P1.6 output
    P1SEL1 |= BIT6;                                         // P1.6 options select

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    TB0CCR0 = 128;                             // PWM Period/2
    TB0CCTL1 = OUTMOD_6;                       // TBCCR1 toggle/set
    TB0CCR1 = 96;                              // TBCCR1 PWM duty cycle     //32 =25% and 96 = 75%.
    TB0CTL = TBSSEL_1 | MC_3;                  // ACLK, up-down mode


    // Configure GPIO
    P1DIR |= BIT0;                                           // Set P1.0/LED to output direction
    P1OUT &= ~BIT0;                                          // P1.0 LED off

    // Configure ADC A1 pin
    P1SEL0 |= BIT1;
    P1SEL1 |= BIT1;

    //Configure Indicator PIN
    P3DIR |= BIT0;                                          // Set P3.0 as output
    P3OUT &= ~BIT0;                                         // Clear P3.0 (set it low)

    // Configure ADC12
    ADCCTL0 |= ADCSHT_2 | ADCON;                             // ADCON, S&H=16 ADC clks
    ADCCTL1 |= ADCSHP;                                       // ADCCLK = MODOSC; sampling timer
    ADCCTL2 &= ~ADCRES;                                      // clear ADCRES in ADCCTL
    ADCCTL2 |= ADCRES_2;                                     // 12-bit conversion results
    ADCMCTL0 |= ADCINCH_1;                                   // A1 ADC input select; Vref=AVCC
    ADCIE |= ADCIE0;                                         // Enable ADC conv complete interrupt

    P3OUT |= BIT2;                                           //P3.2 is the power control pin always on.

    while(1)
    {
        ADCCTL0 |= ADCENC | ADCSC;                           // Sampling and conversion start
        __bis_SR_register(LPM0_bits | GIE);                  // LPM0, ADC_ISR will force exit
        __no_operation();                                    // For debug only
        if (ADC_Result < 0xE14) {                            //E14 or A5A Works for User: Approximately (2.25V) //990 (synthetic heart)
            P1OUT &= ~BIT0;                                  // Clear P1.0 LED off
            // Turn off the power source
            P3OUT &= ~BIT0;                                  // P3.0 is the power control pin for motor
        }
        else {
            P1OUT |= BIT0;                                   // Set P1.0 LED on
            // Turn on the power source
            P3OUT |= BIT0;                                   // P3.0 is the power control pin for motor
            _delay_cycles(50000); //5000
        }

        __delay_cycles(5000); //5000

    }
}

// ADC interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC_VECTOR))) ADC_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(ADCIV,ADCIV_ADCIFG))
    {
        case ADCIV_NONE:
            break;
        case ADCIV_ADCOVIFG:
            break;
        case ADCIV_ADCTOVIFG:
            break;
        case ADCIV_ADCHIIFG:
            break;
        case ADCIV_ADCLOIFG:
            break;
        case ADCIV_ADCINIFG:
            break;
        case ADCIV_ADCIFG:
            ADC_Result = ADCMEM0;
            __bic_SR_register_on_exit(LPM0_bits);            // Clear CPUOFF bit from LPM0
            break;
        default:
            break;
    }
}
