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

#include "vmd642.h"
#include "vmd642_uart.h"

/*VMD642-A的emifa的设置结构*/
EMIFA_Config g_dm642ConfigA ={
	   0x00052078,/*gblctl EMIFA(B)global control register value */
	   			  /*将CLK6、4、1使能；将MRMODE置1；使能EK2EN,EK2RATE*/
	   0xffffffd3,/*cectl0 CE0 space control register value*/
	   			  /*将CE0空间设为SDRAM*/
	   0x73a28e01,/*cectl1 CE1 space control register value*/
	   			  /*Read hold: 1 clock;
	   			    MTYPE : 0000,选择8位的异步接口
	   			    Read strobe ：001110；14个clock宽度
	   			    TA：2 clock; Read setup 2 clock;
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
	   0x00,/*寄存器IER,关所有中断*/
	   0x57,/*寄存器FCR,队列初始化*/
	   0x03,/*寄存器LCR,字长=8bit*/
	   0x01,/*寄存器MCR,控制RTS输出*/
};

extern far void vectors();

Uint8 g_ioBuf;
Uint16 g_uartBuf;
VMD642_UART_Handle g_uartHandleA;

/* Movement controls */
Uint8 turnLeft[7]  = {0xFF, 0x01, 0x00, 0x04, 0x3F, 0x00, 0x44};
Uint8 turnRight[7] = {0xFF, 0x01, 0x00, 0x02, 0x3F, 0x00, 0x42};

void main()
{
    Uint8 i;
    
/*-------------------------------------------------------*/
/* perform all initializations                           */
/*-------------------------------------------------------*/
	/*Initialise CSL，初始化CSL库*/
	CSL_init();
/*----------------------------------------------------------*/
	/*EMIFA的初始化，将CE0设为SDRAM空间，CE1设为异步空间
	 注，DM642支持的是EMIFA，而非EMIF*/
	EMIFA_config(&g_dm642ConfigA);
/*----------------------------------------------------------*/
	/*中断向量表的初始化*/
	//Point to the IRQ vector table
    IRQ_setVecs(vectors);

#if 0
/*----------------------------------------------------------*/
/*测试VMD642-A的数字输入与输出*/

    /*输出测试值*/
    VMD642_rset(VMD642_IOOUT, 0x55);
    /*延时1ms时间*/
    VMD642_waitusec(1);
    /*读回测试值*/
    g_ioBuf = VMD642_rget(VMD642_IOOUT);
    /*延时1ms时间*/
    VMD642_waitusec(1);

    /*输出测试值*/
    VMD642_rset(VMD642_IOOUT, 0xaa);
    /*延时1ms时间*/
    VMD642_waitusec(1);
    /*读回测试值*/
    g_ioBuf = VMD642_rget(VMD642_IOOUT);
    /*延时1ms时间*/
    VMD642_waitusec(1);
#endif

/*----------------------------------------------------------*/
/*测试串口A*/
    /* Open UART */
    g_uartHandleA = VMD642_UART_open(VMD642_UARTB,
    									  UARTHW_VMD642_BAUD_9600,
    									  &g_uartConfig);

    for (;;)
  	{
        /*Turn left*/
        for (i = 0; i < 7; i++)
        {
            VMD642_UART_putChar(g_uartHandleA, turnLeft[i]);
        }
		VMD642_waitusec(1000);
        
        /*Turn Right*/
        for (i = 0; i < 7; i++)
        {
            VMD642_UART_putChar(g_uartHandleA, turnRight[i]);
        }
        VMD642_waitusec(1000);
    }
}
