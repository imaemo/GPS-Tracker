#include "type.h"

uchar g_rcv_start 	= 0;
uchar g_rcv_ok 		= 0;
uchar g_rcv_cnt 	= 0;
uchar g_rcv_buf[80]	= {0};
uchar g_comma[15] 	= {0};
uchar g_comma_cnt 	= 0;

typedef struct msg_gprmc
{
	uchar		utc_time[9];			// utc time hhmmss.ss
	uchar 		nv_status;				// char 'A'--valid 'V'--invalid
	uchar		latitude[10];			// xxxx.xxxxx 纬度
	uchar		ns_indicator;			// char 'N'--north  'S'--south
	uchar		longitude[11];			// xxxxx.xxxxx
	uchar 		ew_indicator;			// char 'E'--east	'W'--West
	uchar		speed[5];				// speed over ground, the Unit is Knots, NOT Km/h x.xxx
	uchar 		course[5];				// course over ground xx.xx
	uchar 		date[6];				// ddmmyy
} msg_gprmc;

msg_gprmc g_rmc_data;


msg_gprmc rmc_dec(uchar buf[], uchar comma[])
{
	uchar i=0;
	msg_gprmc rmc_data = {
							.utc_time		={0},
							.nv_status		=0,
							.latitude		={0},
							.ns_indicator	=0,
							.longitude		={0},
							.ew_indicator	=0,
							.speed			={0},
							.course			=0,
							.date			=0
						};
	
	if (9 == (comma[1]-comma[0]-1))								//get utc time hhmmss.ss
	{
		for (i=0; i<9; i++)
		rmc_data.utc_time[i] = buf[comma[0]+1+i];
	}
	
	rmc_data.nv_status 		= buf[comma[2]-1];				//get navigation status, 'A'--valid, 'V'--invalid
	
	if (10 == (comma[3]-comma[2]-1))
	{
		for (i=0; i<10; i++)
		rmc_data.latitude[i] 	= buf[comma[2]+1+i];			//get latitude xxxx.xxxxx
	}
	
	rmc_data.ns_indicator 	= buf[comma[4]-1];				//get indicator N/S North or South
	
	if (11 ==(comma[5]-comma[4]-1))
	{
		for (i=0; i<11; i++)
		rmc_data.longitude[i] = buf[comma[4]+1+i];			//get longitude xxxxx.xxxxx
	}
	
	rmc_data.ew_indicator 	= buf[comma[6]-1];				//get indicator E/W East or West
	
	if (5 == (comma[7]-comma[6]-1))								//get speed
	{
		for (i=0; i<5; i++)
		rmc_data.speed[i]		= buf[comma[6]+1+i];			
	}
	
	if (5 == (comma[8]-comma[7]-1))								//get course
	{
		for (i=0; i<5; i++)
		rmc_data.course[i]	= buf[comma[7]+1+i];	
	}
	
	if (6 == comma[9]-comma[8]-1)								//get date ddmmyy
	{
		for (i=0; i<6; i++)
		rmc_data.date[i] 		= buf[comma[8]+1+i];
	}
	
	return rmc_data;
}



uchar	get_msg_type(uchar buf[], uchar comma[])				//get msg type 			
{
	if (('R'==buf[2]) && ('M'==buf[3]) && ('C'==buf[4]))		//1 -- GPRMC
	{
		return 1;
	}
	
	return 0;
}

