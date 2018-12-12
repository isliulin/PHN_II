//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
/*
�ṩһЩʱ��ֵ�Ϸ��Ե��ж�

*/
#include <string.h>

#include "sdhDef.h"

#include "time_func.h"
#include "PHN_string.h"

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
//const	char	g_moth_day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
const unsigned short daytab[13]={0,31,59,90,120,151,181,212,243,273,304,334,365};//�������·��ۻ�����
const unsigned short daytab1[13]={0,31,60,91,121,152,182,213,244,274,305,335,366};//�����·��ۻ�����
#define    xMINUTE     (60)					/*1 ????*/
#define    xHOUR         (60*xMINUTE)			/*1 ?????*/
#define    xDAY           (24*xHOUR)			/*1 ????*/
#define    xYEAR         (365*xDAY)			/*1 ???? */
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------



//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//������ݺ��·ݣ����ص�ǰ�µ�������
//ʧ�ܷ���0
int TMF_Get_num_mday(int year, int mon)
{
	char	moth_day[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	char	moth_day_l[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if((mon < 1) || (mon > 12))
		return 0;
	
	if(year < 100)
		year += 2000;
	if(year % 4 == 0)
		return moth_day_l[mon - 1];
	else
		return moth_day[mon - 1];
}

//���tm�ĺϷ���
int TMF_Check_tm(struct  tm	*p_t)
{
	
	if((p_t->tm_mon < 1) || (p_t->tm_mon > 12))
		return RET_FAILED;
	if((p_t->tm_mday < 1) || (p_t->tm_mday > TMF_Get_num_mday(p_t->tm_year, p_t->tm_mon)))
		return RET_FAILED;
	if(p_t->tm_hour >= 24)
		return RET_FAILED;
	if(p_t->tm_min >= 60)
		return RET_FAILED;
	if(p_t->tm_sec >= 60)
		return RET_FAILED;


	return RET_OK;
	
}




//--------------------------------------------------------------------------------
//�����2000��1��1��0ʱ0��0�뵽���ڵ�����
//��ݵ�ֵ��ֻ����λ����������㲻��
//----------------------------------------------------------------------------------
uint32_t    Time_2_u32(struct  tm	*tm_2_sec)
{

	unsigned char a;
	unsigned short b;
	unsigned long seconds;

	a=tm_2_sec->tm_year/4;
	seconds=(unsigned long)1461*a*xDAY;	//��ȥ����������
	a=tm_2_sec->tm_year%4;
	if(a==0)
	{
		a = tm_2_sec->tm_mon - 1;
		b = daytab1[a];
	}
	else
	{
		b=366;
		while(--a)
		{
			b=b+365;
		}
		a = tm_2_sec->tm_mon - 1;
		b=b+daytab[a];
	}
	seconds +=xDAY*(b+tm_2_sec->tm_mday-1);//���ϱ����ѹ�������������
	seconds += xHOUR*tm_2_sec->tm_hour;				//���ϱ����ѹ���Сʱ
	seconds += xMINUTE*tm_2_sec->tm_min;			//���ϱ������Ѿ���ȥ������
	seconds += tm_2_sec->tm_sec;					//??????
	return seconds;

	
	
}




int TMF_Str_2_tm(char *s_time, struct  tm *p_t)
{
	char *p = s_time;
	struct  tm	t = {0};
	uint8_t		err = 0;
	uint16_t	i;
	t.tm_year = Get_str_data(p, "/", 0, &err);
	if(err)
		return ERR_PARAM_BAD;
	t.tm_mon = Get_str_data(p, "/", 1, &err);
	if(err)
		return ERR_PARAM_BAD;
	t.tm_mday = Get_str_data(p, "/", 2, &err);
	if(err)
		return ERR_PARAM_BAD;

		
	
	i = strcspn(p, " ");
	p += i;
	
	t.tm_hour = Get_str_data(p, ":", 0, &err);
	if(err)
		return ERR_PARAM_BAD;
	t.tm_min = Get_str_data(p, ":", 1, &err);
	if(err)
		return ERR_PARAM_BAD;
	t.tm_sec = Get_str_data(p, ":", 2, &err);
	if(err)
		return ERR_PARAM_BAD;
	
	if(TMF_Check_tm(&t) != RET_OK)
		return ERR_PARAM_BAD;
	
	
	
	
	p_t->tm_year = t.tm_year;
	p_t->tm_mon = t.tm_mon;
	p_t->tm_mday = t.tm_mday;
	p_t->tm_hour = t.tm_hour;
	p_t->tm_min = t.tm_min;
	p_t->tm_sec = t.tm_sec;
	
	return RET_OK;
}

uint32_t  Str_time_2_u32(char *s_time)
{
//	uint32_t sec = 0;
	struct  tm	t = {0};
	
	
	if(TMF_Str_2_tm(s_time, &t) != RET_OK)
		return 0xffffffff;
	return Time_2_u32(&t);	
	
}



//����ֵת����������ʱ����
extern int Sec_2_tm(uint32_t seconds, struct  tm *time)
{
	
	
	unsigned char a,c,i;
	unsigned short b,d;
	unsigned long x;

		
	x=(unsigned long)24*3600;
	b=(unsigned short)((seconds/x)&0xffff);
	a=(unsigned char)((b/1461)&0xff);
	a=a*4;
	b=b%1461;
	if(b<366)
	{	
		c=0;
		d=b-0;
		time->tm_year = a + c;
		i=1;
		while (d>daytab1[i]-1)
		{
			i++;
		}
		time->tm_mon = i;
		d=d-daytab1[i-1]+1;
		time->tm_mday = d;
	}
	else
	{
	  if((b>=366) && (b<731))
		{c=1;d=b-366;}
	  if((b>=731) && (b<1096))
		{c=2;d=b-731;}
	  if((b>=1096) && (b<1461))
		{c=3;d=b-1096;}
		a=a+c;
		time->tm_year = a;
		i=1;
		while (d>daytab[i]-1)
		{
			i++;
		}
		time->tm_mon = i;
		d=d-daytab[i-1]+1;
		time->tm_mday = d;
	}
	x=(unsigned long)24*3600;
	x=seconds%x;
	time->tm_hour = x / 3600;
	b=x%3600;
	time->tm_min = b / 60;
	time->tm_sec = b % 60;
	
	return RET_OK;
}

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
