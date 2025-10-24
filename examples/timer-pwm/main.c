/*!
    \file  main.c
    \brief setup the TIMER0 peripheral for pulsating the RED color in the RGB LED
    
    \version 20200710
*/

/*
    Copyright (c) 2020, IAR Systems AB.

    See LICENSE.md for detailed license information.
*/

#include "iar-risc-v-gd32v-eval.h"
#include "systick.h"

typedef enum 
{
    INHALE = SET,
    EXHALE = RESET,
} breathe_t;

/* function prototypes */
void gpio_config(void);
void timer_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void main(void)
{
    int32_t i = 0;
    breathe_t breathe_flag = INHALE;
    
    /* configure the GPIO ports */
    gpio_config();
    
    /* configure the TIMER peripheral */
    timer_config();
    
    while(1)
    {
        delay_1ms(1);

        i = (INHALE == breathe_flag) ? i + 10 : i - 10;
        breathe_flag = (500 < i) ? EXHALE: breathe_flag;
        breathe_flag = (0 >= i)  ? INHALE: breathe_flag;
        
        /* configure the timer channel output pulse value */
        timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, i);
    }
}

/**
    \brief      configure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
  */
void gpio_config(void)
{
    rcu_periph_clock_enable(LEDR_GPIO_CLK);

    /* Configure PA11(TIMER0_CH3) to its alternate function */
    gpio_init(LEDR_GPIO_PORT, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, LEDR_PIN);
}

/*!
    \brief      TIMER peripheral configuration
    \param[in]  none
    \param[out] none
    \retval     none
*/
void timer_config(void)
{
    /* TIMER0 configuration: generate PWM signals with different duty cycles:
       TIMER0CLK = SystemCoreClock / 108 = 1MHz */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER0);
    timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 107;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 500;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0,&timer_initpara);

     /* CH3 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0, TIMER_CH_3, &timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_3, 250);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_3, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_3, TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(TIMER0, ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    timer_enable(TIMER0);
}
