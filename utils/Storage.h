#ifndef __INC_Storage_H
#define __INC_Storage_H
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include "basis/macros.h"
#include "utils/time.h"
#include <stdint.h>
#include "ModelFactory.h"
#include "system.h"

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define STG_CHN_CONF(n)					(n)
#define STG_SYS_CONF					0x10
#define STG_CHN_DATA(n)					(0x20 + n)
#define STG_CHN_ALARM(n)				(0x30 + n)
#define STG_CHN_SUM(n)				(0x40 + n)
#define STG_LOSE_PWR					(0x50)
#define STG_LOG							(0x60)

#define	IS_CHN_CONF(type)				(type < NUM_CHANNEL)
#define	IS_SYS_CONF(type)				(type == 0x10)
#define	IS_CHN_DATA(type)				((type & 0xf0) == 0x20)
#define	IS_CHN_ALARM(type)			((type & 0xf0) == 0x30)
#define	IS_CHN_SUM(type)			((type & 0xf0) == 0x40)
#define	IS_LOSE_PWR(type)				(type == 0x50)
#define	IS_LOG(type)				(type == 0x60)

#define STG_GET_CHN(type)				(type & 0x0f)

#define STG_DRC_READ					0
#define STG_DRC_WRITE					1
#define STG_SUR_READ					2

#define	STG_STOP									0
#define	STG_COVER									1
#define	STG_ERASE									2		//���ڼ�¼�࣬ʹ��������ʽ�Ͳ��ؿ��ǻ����ˣ�ֻ��������
#define STG_RCD_FULL_ACTION				STG_ERASE


#define STG_DEF_FILE_SIZE 		0


#define STG_MAX_NUM_CHNALARM			24		//ÿ��ͨ������󱨾�����	
#define STG_MAX_NUM_LST_PWR				24

#define STG_LOG_FILE_SIZE					3600		//FM25L64��ʣ�ಿ��ȫ�����ڴ洢��־		

//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef struct {
	uint32_t				rcd_maxcount;
	uint32_t				rcd_count;
	uint32_t				file_size;		//��¼����ʷ�����ļ��Ĵ�С���ļ�д������֮�����´����ļ�ʱʹ��
}rcd_mgr_t;

typedef struct {
	uint8_t			flag;
	uint8_t			alm_pwr_type;
	uint16_t		rcd_num;	//��ż�¼������ֻ��Ҫ�ڵ�һ����¼�ĵط��������������
	uint32_t		happen_time_s;
	uint32_t		disapper_time_s;
}rcd_alm_pwr_t;



typedef struct {
	char			enable_sum;
	uint8_t		sum_start_year;
	uint8_t		sum_start_month;
	uint8_t		sum_start_day;
	
	uint8_t		sum_year;
	uint8_t		sum_month;
	uint16_t	accumlated_remain;			/*�ۻ�����*/
	
	//�ۼ�ֵ��1λС�������ֽ���ǰ������ˡ�
	uint16_t	accumlated_day[31][3];		/*???*/
	uint16_t	accumlated_month[12][3];	/*???*/
	uint16_t	accumlated_year[3];			/*???*/
	uint16_t	accumlated_total[3];			/*���ۻ�*/
	
}rcd_chn_accumlated_t;


typedef struct
{
	mdl_chn_save_t		mdlchn[NUM_CHANNEL];
	system_conf_t			sys_cfg;
	
	
}conf_file_t;

typedef struct {
	//180521 ����ֶ���Ӧ���� w25q flash������Լ�����Ǹ�flashʹ�õ�
	uint8_t			use_by_flash;		
	uint8_t			decimal_places;
	int16_t			rcd_val;

	
	uint32_t		rcd_time_s;


}data_in_fsh_t;

CLASS(Storage)
{
//	IMPLEMENTS( mdl_observer);

	
	rcd_mgr_t		arr_rcd_mgr[NUM_CHANNEL];
	int					(*init)(Storage *self);
	int					(*rd_stored_data)(Storage *self, uint8_t	cfg_type, void *buf, int len);
	int					(*wr_stored_data)(Storage *self, uint8_t	cfg_type, void *buf, int len);
	void				(*shutdown)(Storage *self);
	int					(*open_file)(uint8_t	cfg_type, uint32_t file_size);
//	uint8_t			arr_rcd_fd[NUM_CHANNEL];
//	uint8_t			alarm_fd;
//	uint8_t			lose_pwr_fd;
	
};
//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
Storage		*Get_storage();


//��ȡָ��ʱ��ļ�¼,�����ظļ�¼��λ��,sec = 0���ͷ��������������¼, sec = 0xffffffff,�������ļ�¼
//pos=0��ʱ�򣬾���ͷ��ʼ���ң���֪�ʹ�posָ����λ�ÿ�ʼ����
uint32_t STG_Read_data_by_time(uint8_t	chn, uint32_t sec, uint32_t pos, data_in_fsh_t *r);
//��Լ�¼���ݵ�����ӿ�
//��ȡĳ��ʱ���ڵļ�¼���ݣ�����ֵ�Ƕ�ȡ�������ݵĳ���
//���Ұ�����ת��csv��ʽ
//int	STG_Read_rcd_by_time(uint8_t	chn, uint32_t start_sec, uint32_t end_sec, char *buf, int buf_size, uint32_t *rd_sec);

int STG_Read_rcd(uint8_t	chn, uint8_t	*buf,  uint16_t size);

//chn 0 ~ NUM_CHANNELNUM �Ǳ�����Ϣ,0xff�ǵ�����Ϣ
int	STG_Read_alm_pwr(uint8_t	chn_pwr,short start, char *buf, int buf_size, uint32_t *rd_count);			
uint16_t STG_Get_alm_pwr_num(uint8_t	chn_pwr);
int	STG_Set_alm_pwr_num(uint8_t	chn_pwr, uint16_t new_num);

int	STG_Set_file_position(uint8_t	file_type, uint8_t rd_or_wr, int position);
void STG_Erase_file(uint8_t	file_type);
void STG_Resize(uint8_t	file_type, uint32_t	new_size);

void STG_Run(void);
void STG_Reset(void);

#endif