/********************************************************************/
/*  Copyright 2006 by VisionMagic Ltd.								*/
/*  All rights reserved. Property of VisionMagic Ltd.				*/
/*  Restricted rights to use, duplicate or disclose this code are	*/
/*  granted through contract.									    */
/********************************************************************/
#include <csl.h>
#include <csl_emifa.h>
#include <csl_irq.h>
#include <csl_chip.h>
#include <csl_timer.h>

#include "vmd642.h"
#include "vmd642_uart.h"

/*VMD642-A��emifa�����ýṹ*/
EMIFA_Config g_dm642ConfigA ={
	   0x00052078,/*gblctl EMIFA(B)global control register value */
	   			  /*��CLK6��4��1ʹ�ܣ���MRMODE��1��ʹ��EK2EN,EK2RATE*/
	   0xffffffd3,/*cectl0 CE0 space control register value*/
	   			  /*��CE0�ռ���ΪSDRAM*/
	   0x73a28e01,/*cectl1 CE1 space control register value*/
	   			  /*Read hold: 1 clock;
	   			    MTYPE : 0000,ѡ��8λ���첽�ӿ�
	   			    Read strobe ��001110��14��clock���
	   			    TA��2 clock; Read setup 2 clock;
	   			    Write hold :2 clock; Write strobe: 14 clock
	   			    Write setup :7 clock
	   			    --					 ---------------
	   			  	  \		 14c		/1c
	   			 	   \----------------/ */
	   0x22a28a22, /*cectl2 CE2 space control register value*/
       0x22a28a42, /*cectl3 CE3 space control register value*/
	   0x57226000, /*sdctl SDRAM control register value*/
	   0x0000081b, /*sdtim SDRAM timing register value*/
	   0x001faf4d, /*sdext SDRAM extension register value*/
	   0x00000002, /*cesec0 CE0 space secondary control register value*/
	   0x00000002, /*cesec1 CE1 space secondary control register value*/
	   0x00000002, /*cesec2 CE2 space secondary control register value*/
	   0x00000073 /*cesec3 CE3 space secondary control register value*/
};

VMD642_UART_Config g_uartConfig ={
	   0x00,/*�Ĵ���IER,�������ж�*/
	   0x57,/*�Ĵ���FCR,���г�ʼ��*/
	   0x03,/*�Ĵ���LCR,�ֳ�=8bit*/
	   0x01,/*�Ĵ���MCR,����RTS���*/
};

TIMER_Config timerConfig = {
    0x00000280, /* interal clock, reset counter and hold */
    0x007270E0, /* interrupt every 0.1s */
    0x00000000  /* start from 0 */
};

extern far void vectors();

Uint8 g_ioBuf;
Uint16 g_uartBuf;
TIMER_Handle hTimer;
Uint32 TimerEventId;
VMD642_UART_Handle g_uartHandleA;

/* Movement controls */
Uint8 turnLeft[7]  = {0xFF, 0x01, 0x00, 0x04, 0x3F, 0x00, 0x44};
Uint8 turnRight[7] = {0xFF, 0x01, 0x00, 0x02, 0x3F, 0x00, 0x42};
Uint8 stay[7]      = {0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01};

/* Remaining period for the next movement */
Uint8 controlPeriod = 1;

/* Next moving function */
Uint8 controlMove = HOLDER_MOV_STAY;

/* State of moving */
Uint8 hasMoved = 0;

void main()
{
    //Uint8 i;
    
/*-------------------------------------------------------*/
/* perform all initializations                           */
/*-------------------------------------------------------*/
	/*Initialise CSL����ʼ��CSL��*/
	CSL_init();
/*----------------------------------------------------------*/
	/*EMIFA�ĳ�ʼ������CE0��ΪSDRAM�ռ䣬CE1��Ϊ�첽�ռ�
	 ע��DM642֧�ֵ���EMIFA������EMIF*/
	EMIFA_config(&g_dm642ConfigA);
    
/*----------------------------------------------------------*/
    /*TIMER��ʼ��������TIMER0*/
    hTimer = TIMER_open(TIMER_DEV0, 0);
    TimerEventId = TIMER_getEventId(hTimer);
    TIMER_config(hTimer, &timerConfig);
    
/*----------------------------------------------------------*/
	/*�ж�������ĳ�ʼ��*/
	//Point to the IRQ vector table
    IRQ_setVecs(vectors);
    IRQ_globalEnable();
    IRQ_nmiEnable();

    IRQ_map(TimerEventId, 14);
    IRQ_reset(TimerEventId);
    IRQ_enable(TimerEventId);

/*----------------------------------------------------------*/
/*�ڴ���B - RS485��������ź�*/
    /* Open UART */
    g_uartHandleA = VMD642_UART_open(VMD642_UARTB,
    									  UARTHW_VMD642_BAUD_9600,
    									  &g_uartConfig);

    /* Open Timer */
    TIMER_start(hTimer);
    
    for (;;)
  	{

    }
}

interrupt void c_int14(void)
{
    printf("Interrupt successfully!\n");
}
