/*
 * main.c
 *
 * author:maemo
 *date:2012/07/16 11:48
 * history:
 *
 */

#include <msp430g2553.h>   
#include <stdio.h>
#include "pff.h"
#include "lcd.h"
#include "type.h"
#include "neo-6m.h"
#include "diskio.h"

/* key1 p1.0 key2 p1.7 key3 p2.7 key4 p2.6 */
#define KEY1 0x01  
#define KEY2 0x80
#define KEY3 0x80
#define  KEY4 0x40

#define LIGHT   0x20   
#define LIGHT_ON()    (P2OUT |= LIGHT)
#define LIGHT_OFF()   (P2OUT &= ~LIGHT)   
#define LIGHT_TRG()   (P2OUT ^= LIGHT)   
uchar g_record  = 0;
uchar key12_buf=0, key34_buf=0, key_trg=0;
uchar config_flag=0, record_flag=0, uart_flag=0, file_select=1, light_flag=1,time_flag=0;

extern msg_gprmc g_rmc_data;
uchar Str1[]    = {0, 1, 2, 0, 3, 4, 5};
uchar Str2[]    = {0, 6, 7, 0, 0, 9};
const uchar dat_tab1[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const uchar dat_tab2[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
void InitSysClk(void);
void InitDisplay();
void InitUart();  
void InitKey();
void InitTimerA0();
void DispUpdate();
void DispTime();


void main(void)
{
    uchar flag= 0x05;
	uchar write_cnt=0;
	FATFS fatfs;			/* File system object */
	DIR dir;				/* Directory object */
	FILINFO fno;			/* File information object */ 
    FRESULT f_open,rc;
    uchar   buff[64];
    WORD    br=0,bw=0,record=0;
    uchar wr_str[]={"ok!"};
    uchar init_flag=0;
    WDTCTL          =       WDTPW + WDTHOLD;
    
    uchar i=0;
    InitSysClk();
    InitKey();
    InitTimerA0();
    LCD_Init();   
    InitDisplay();    
    
//    InitUart();  
    
    __delay_cycles(1000);
	 __bis_SR_register(GIE);
     
    init_flag = disk_initialize();

    UCA0TXBUF		=		0x00;
    
    flag = pf_mount(&fatfs);     
    f_open = pf_open("record_1.txt");
//    rc= pf_write(wr_str, sizeof(wr_str), &bw);
//    rc = pf_write(0, 0, &bw);
//
//    pf_lseek(0);
//    rc = pf_read(&buff[0], 1, &br);
//    if (rc == 0)
//        LCD_WriteString(0, 3, "Read ok!"); 
//    if (br == 1) {
//        LCD_WriteChar(6, 5, buff[0]);
//    }
   
    
    while (1) {       

		if (g_rcv_ok ) {
            g_rcv_ok = 0;
			if (1 == get_msg_type(g_rcv_buf, g_comma)) {			// GPRMC msg
                g_rmc_data = rmc_dec(g_rcv_buf, g_comma);
                time_flag = 1;
                IE2 &= 	~UCA0RXIE;           	// clear USCI_A0 RX interrupt				                
				if (g_rmc_data.nv_status == 'A'  ) {
                    
                    g_rmc_data.nv_status = 'N';
                    if (g_rmc_data.speed[0] >='0' && g_rmc_data.speed[2] >= '3' && record_flag) {
					DispUpdate();                    
//					pf_lseek(record);
					rc = pf_write(g_rcv_buf, g_rcv_cnt, &bw);

					write_cnt++;
					LCD_WriteString(0, 3, "wr:");
					LCD_WriteChar(13, 3, write_cnt/100+'0');
					LCD_WriteChar(18, 3, write_cnt/10%10+'0');
                    LCD_WriteChar(23, 3, write_cnt%10+'0');
                    }
				}
                IE2 |= 	UCA0RXIE;           	// Enable USCI_A0 RX interrupt
			}		
		}
        
        if (time_flag) {
            time_flag = 0;
            DispTime();
        }
        
		if (config_flag) {
			switch(config_flag) {
				case 1:
//                    LIGHT_TRG();
                    if (record_flag)
                        LCD_WriteIcon(20, 0, 6);
                    else
                        LCD_WriteIcon(20, 0, 5); 
					break;
				case 2:		
//                    LIGHT_TRG();
                  if (uart_flag){ 
						InitUart();
                        LCD_WriteIcon(60, 0, 13);
                  } else { 
						UCA0CTL1 &= ~UCSWRST;
                        LCD_WriteIcon(60, 0, 12);
                  }
					break;
				case 3:	
//                  LIGHT_TRG();
//                  pf_lseek(0);              
                  if (1 == file_select) {
                        rc = pf_write(0, 0, &bw);
						f_open = pf_open("record_1.txt");
//                        bw = 0;
                        LCD_WriteIcon(40, 0, 7);                  
                  } else if (2 == file_select){     
                        rc = pf_write(0, 0, &bw);
						f_open = pf_open("record_2.txt");
//                        bw = 0;
                        LCD_WriteIcon(40, 0, 8);
                  } else if (3 == file_select){
                        rc = pf_write(0, 0, &bw);
						f_open = pf_open("record_3.txt");
  //                      bw = 0;
                        LCD_WriteIcon(40, 0, 9);               
                  } else if (4 == file_select) {
                        rc = pf_write(0, 0, &bw);
						f_open = pf_open("record_4.txt");
    //                    bw = 0;
                        LCD_WriteIcon(40, 0, 10);
                  }
                  
				  break;
                case 4:
                  if (light_flag)
                    LIGHT_ON();
                  else
                    LIGHT_OFF();
				default:break;
			}
			config_flag=0;
		}
	}
}

/*
 * function:Init System Clock
 * input: none
 * output: none
 * description: set MCLK/SMCLK source from DCO @ 1MHz
 */
void InitSysClk(void)
{
	if (CALBC1_1MHZ == 0XFF || CALDCO_1MHZ == 0XFF)
	{
		while(1);		        					/*if calibration constants erased, taps cpu*/
	}

	BCSCTL1         =       CALBC1_1MHZ;
	DCOCTL          =       CALDCO_1MHZ;			/* DCOCTL  Calibration Data for 1MHz */
//	BCSCTL3         |=      LFXT1S_0;		/* Mode 0 for LFXT1 : Normal operation */
	IFG1            &=      ~OFIFG;					/*clear OSCfault flag*/

	BCSCTL2 |= SELM_0 + DIVM_0 + DIVS_0; /*set MCLK,SMCLK source from DCO*/
}


/*
*Function Name: initi display
*
*Input        : none
*
*Output       : none
*
*description  : none
*
*author       : 2012/7/20, by Maemo
*/
void InitDisplay()
{
//    LCD_WriteCNString(0, 0, 6, Str1);       
//    LCD_WriteCNString(0, 1, 5, Str2);
//    LCD_WriteString(24, 2, "1:");
//    LCD_WriteCNString(0, 3, 5, Str2);    
//    LCD_WriteString(24, 5, "2:"); 
  
	/* GPS */
    LCD_WriteIcon(0, 5, 0); LCD_WriteIcon(8, 5, 1);  LCD_WriteIcon(16, 5, 2); 
	/* signal */
//	LCD_WriteIconX(40, 0, 0, 10);	
    LCD_WriteIconX(0, 0, 10, 10);
    /* stop */
    LCD_WriteIcon(20, 0, 5);
    /* file */
    LCD_WriteIcon(40, 0, 7);
    /* N: E: */
    LCD_WriteIcon(0, 1, 3); LCD_WriteChar(8, 1, ':'); 
	LCD_WriteIcon(0, 2, 4); LCD_WriteChar(8, 2, ':'); 
    
//	LCD_WriteChar(12, 2, '3'); LCD_WriteIconX(18, 2, 20, 4);
//	LCD_WriteChar(22, 2, '4'); LCD_WriteIconX(28, 2, 24, 2);
//	LCD_WriteChar(30, 2, '5'); LCD_WriteIconX(36, 2, 26, 4);
	
    
//    LCD_WriteString(0, 0, " GPS ", 1);
//	LCD_WriteChar(0, 1, 'N', 0); LCD_WriteChar(5, 1, ':', 0);
//	LCD_WriteChar(0, 2, 'E', 0); LCD_WriteChar(5, 2, ':', 0);    
//	LCD_WriteString(0, 3, "Speed:", 0);
//	LCD_WriteString(0, 4, "Time:", 0);	
	
}

/*
uart:  8 bits data, no parity, 1 stop bit, 1M @ 9600 baud
*/
void InitUart()
{
    UCA0CTL1		|=		UCSWRST;				//reset the USCI by set UCSWRST,init all register;
	
    P1SEL           |=      BIT1 +  BIT2;			//configure port fuction P1.1 and P1.2;
    P1SEL2          |=      BIT1 +  BIT2; 
	UCA0CTL1		|= 		UCSSEL_2;				//USCI closk source from SMCLK;	
	UCA0BR0			=		6;						//1Mhz @ 9600 baud;
	UCA0BR1			= 		0;						//1Mhz @ 9600 baud;	
	UCA0MCTL		|= 		UCOS16 + UCBRF3;		//oversampling mode is selected, 1Mhz @ 9600 baud;	
	
	UCA0CTL1		&=		~UCSWRST;				//Initialize USCI state machine	
	IE2 			|= 		UCA0RXIE;           	// Enable USCI_A0 RX interrupt
//	LCD_WriteString(0, 1,"init ok");
//    LCD_SetAddr(0, 2);
} 

#pragma		vector = USCIAB0RX_VECTOR
__interrupt void USCI_A0_RX_ISR(void)
{
	//while (!(IFG2&UCA0TXIFG));						// USCI_A0 TX buffer ready?
	//UCA0TXBUF		=		UCA0RXBUF;				// TX -> RXed character
    // if ((UCA0RXBUF != 0x0D) && (UCA0RXBUF != 0x0A))
	// LCD_WriteChar(UCA0RXBUF);						//display what received
	uchar tmp = 0;
					
	
	tmp	= UCA0RXBUF;							//stor the buf first	
	
	if (g_rcv_start)
	{
		if ('*' == tmp)
		{
			g_rcv_ok 	= 1;
			g_rcv_start = 0;
            g_rcv_buf[g_rcv_cnt] = '\0';
		}		
		else
		{
			g_rcv_buf[g_rcv_cnt] = tmp;
			if (',' == tmp)
			{
				g_comma[g_comma_cnt] = g_rcv_cnt;				//store the comma's count 
				g_comma_cnt++;									//in order to decode the message	
			}
			g_rcv_cnt++;
		}
	}
	
	if ('$' == tmp)
	{
		g_rcv_start = 1;
		g_rcv_cnt 	= 0;
		g_comma_cnt = 0;
        g_rcv_ok    = 0;
	}
}

void InitTimerA0()
{
	TACTL			=		TASSEL_2 ;		//Timera clock @ SMCLK 1MHz
	TACCR0			= 		50000;					
	TACCTL0			|= 		CCIE;			// enable CCIFG
}

#pragma	vector = TIMER0_A0_VECTOR
__interrupt void Timer0_ccr0_isr()
{
	uchar tmp = 0;

	if (1 == key_trg) {
		tmp = ((~P1IN) &(KEY1+KEY2));
		if (tmp == KEY1) {
			config_flag = 1;
			if  (uart_flag)					/*Ö»ÓÐÔÚuart´ò¿ªÊ±²ÅÄÜÊ¹ÓÃ¼ÇÂ¼Êý¾Ý*/
				record_flag = !record_flag;	
		} else if (tmp == KEY2) {
			config_flag = 2;				
			uart_flag = !uart_flag;			
		}
        key_trg = 0;
	} else if (2 == key_trg) {
		tmp = ((~P2IN)&(KEY3+KEY4));
		if (tmp == KEY3) {						
			config_flag = 3;
			if (!record_flag) {				/*Ö»ÓÐµ±Ã»ÓÐ¼ÇÂ¼Êý¾ÝµÄÊ±ºò²ÅÄÜ¸ü¸Ä¼ÇÂ¼ÎÄ¼þ*/
				file_select++;
				if (file_select == 5)
				file_select = 1;
			}
		} else if (tmp == KEY4) {
			config_flag = 4;
			light_flag = !light_flag;			
		}
        key_trg = 0;
	}
	TACTL &= ~MC_1;
}

void InitKey()
{
    P2SEL &= ~(KEY3+KEY4);  P2SEL2 &= ~(KEY3+KEY4);
    P1DIR &= ~(KEY1+KEY2);  P2DIR &= ~(KEY3+KEY4);
    P1REN |= (KEY1+KEY2);   P2REN |= (KEY3+KEY4);   /* enable pull-up */
    P1OUT |= (KEY1+KEY2);   P2OUT |= (KEY3+KEY4);   /* enable pull-up */
    P1IES |= (KEY1+KEY2);   P2IES |= (KEY3+KEY4);   /* high to low edge */
    P1IFG &= ~(KEY1+KEY2);  P2IFG &= ~(KEY3+KEY4); 
    P1IE |= (KEY1+KEY2);    P2IE |= (KEY3+KEY4);
}

#pragma vector=PORT1_VECTOR
__interrupt void key12_isr()
{
	key_trg = 1;
	key12_buf = P1IFG;
	P1IFG = 0;
	TACTL |= MC_1;
}

#pragma vector=PORT2_VECTOR
__interrupt void key34_isr()
{
    LCD_WriteChar(0, 3, 'o');
	key_trg = 2;
	key34_buf = P2IFG;
	P2IFG = 0;
	TACTL |= MC_1;
}
	




void DispUpdate()
{
    LCD_WriteIconX(0, 0, 0, 10);
	LCD_WriteCharX(12, 1, 2, &g_rmc_data.latitude[0]);  	/* dd */
	LCD_WriteIconX(22, 1, 20, 4);							/* åº¦ */
	LCD_WriteCharX(26, 1, 2, &g_rmc_data.latitude[2]);		/* mm */
	LCD_WriteIconX(36, 1, 24, 2);							/* åˆ†' */
	LCD_WriteCharX(38, 1, 3, &g_rmc_data.latitude[5]);       /* ss */
	
	LCD_WriteCharX(12, 2, 3, &g_rmc_data.longitude[0]);  	/* dd */
	LCD_WriteIconX(27, 2, 20, 4);							/* åº¦ */
	LCD_WriteCharX(31, 2, 2, &g_rmc_data.longitude[3]);		/* mm */
	LCD_WriteIconX(42, 2, 24, 2);							/* åˆ†' */
	LCD_WriteCharX(45, 2, 3, &g_rmc_data.longitude[6]);       /* ss */
	

}

void DispTime()
{
	uchar tmp_min =0,tmp_hou=0, tmp_day=0, tmp_year=0, tmp_month=0;
	
	tmp_min = (g_rmc_data.utc_time[2]-'0')*10+g_rmc_data.utc_time[3]-'0';	/*get the minutes */
	tmp_hou	= (g_rmc_data.utc_time[0]-'0')*10 + g_rmc_data.utc_time[1]-'0'; /* get the hour */
	tmp_day = (g_rmc_data.date[0]-'0')*10+ g_rmc_data.date[1]-'0'; 	/* get the day */
	tmp_month = (g_rmc_data.date[2]-'0')*10+g_rmc_data.date[3]-'0';	/* get the month */
	tmp_year = (g_rmc_data.date[4]-'0')*10+g_rmc_data.date[5]-'0';	/* get thd year */	
	
	tmp_hou  = tmp_hou + 8;	
	if (tmp_hou>= 24) {				/* the date need to change */
		tmp_hou = tmp_hou - 24;
		if (0==(tmp_year%4)) {		/* run nian*/
			if (tmp_day == dat_tab2[tmp_month-1]) { /* judge if it is end of the month */
				tmp_day = 1;
				tmp_month = tmp_month+1;
				if (13 == tmp_month)
					tmp_month = 1;
			} else
				tmp_day = tmp_day+1;
		} else {
			if (tmp_day == dat_tab1[tmp_month-1]) {
				tmp_day = 1;
				tmp_month = tmp_month+1;
				if (13 == tmp_month)
					tmp_month = 1;
			} else 
				tmp_day = tmp_day+1;
		}		
	}
	/* month*/
	LCD_WriteChar(30, 5, tmp_month/10+'0'); LCD_WriteChar(35, 5, tmp_month%10+'0');  		
	LCD_WriteChar(40, 5, '-');								/* - */
	/* day */
	LCD_WriteChar(45, 5, tmp_day/10+'0'); LCD_WriteChar(50, 5, tmp_day%10+'0');			
	/* hour */
	LCD_WriteChar(60, 5, tmp_hou/10+'0'); LCD_WriteChar(65, 5, tmp_hou%10+'0');		
	LCD_WriteChar(70, 5, ':');								/* : */
	/* min */
	LCD_WriteChar(74, 5, tmp_min/10+'0'); LCD_WriteChar(79, 5, tmp_min%10+'0');		
	
}







