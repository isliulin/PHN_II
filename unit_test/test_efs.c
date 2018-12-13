//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
/*
�����ļ�ϵͳ�Ĺ��ܣ�
1 �ļ������Ƿ���ȷ��
	�����Ƿ���ƻ������ļ���
	���Բ����Ƿ�������ļ��������˲�����û���κ���©��

*/

#include "cfg_test.h"
#include "unit_test.h"
#if UNT_EFS == 1
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "fs/easy_fs.h"

#include "test_w25q.h"

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

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
#define DATA_FILE_SIZE					2 * 1024 * 1024
#define NUM_DATA_FILE						6	
#define FILL_DATA								0x5a
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
//static flash_t					*tdd_fsh;
static int 							row_num;
static uint8_t*						my_buf;
static int 							my_buf_size;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int TEFS_Check_area_val(int fd, uint32_t start_addr, uint32_t end_addr, uint8_t val);
static int TEFS_Fill_data(int fd, uint8_t *buf, uint32_t size);
static int TEFS_erase_area(char n, int *all_fd, uint32_t start, uint32_t erase_size);
static void TEFS_Erase(int *arr_fd, uint32_t start, uint32_t size);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//


int Init_test(char	*test_buf, int size)
{
	int i;
	
	my_buf = (uint8_t *)test_buf;
	if(size > 256)
		size = 256;
	my_buf_size = size;
	
	EFS_Reset();
	
	row_num = 0;
//	UNT_Disp_text("�ļ�ϵͳ����",0, 0);
	
//	//������Ҫ���Ե��ļ�,����ʵ�ʻ��õ����ļ����д���,�����ļ��Ĵ�С����һ��׼ȷ
//	phn_sys.fs.fs_open(0, "sys.cfg", "rw", 256);
//	STRG_SYS.fs.fs_open(STRG_RCD_FSH_NUM, "alm_lost_pwr", "rw", 1024);
//	STRG_SYS.fs.fs_open(STRG_RCD_FSH_NUM, "SDH_LOG.CSV", "rw", 3600);
	for(i = 0 ; i < NUM_DATA_FILE; i ++)
	{

		sprintf(test_buf, "mod_chn_%d", i);
		phn_sys.fs.fs_open(1, test_buf, "rw", DATA_FILE_SIZE);
		
		
	}
	
	
	
	
	row_num = 0;
	return 0;
}

//�����ļ������������ļ���Ӱ�켰�����Ƿ���ȫ
void Run_test(void)
{
	int fd[NUM_DATA_FILE];
	file_info_t		*f_info;
	char	tmp_buf[32];
	int		ret;
	short 	i;
	short	count;
	
	
	
	UNT_Disp_text("�ȴ���̨�����ļ�...", row_num++, 0);
	
	for(i = 0; i < NUM_DATA_FILE; i++)
	{
		sprintf(tmp_buf, "mod_chn_%d", i);
		fd[i] = phn_sys.fs.fs_open(FSH_W25Q_NUM, tmp_buf, "rw", DATA_FILE_SIZE);
		
		f_info =  phn_sys.fs.fs_file_info(fd[i]);
		count = 0;
		while(f_info->file_flag & EFILE_ERASE)
		{
			//�ȴ��ļ�ϵͳ�ĺ�̨�������н�������Ϊ��һ�δ����ļ�ʱҪ�����������ļ�ϵͳ��̨����ȥʵ��ִ�е�
			UNT_Delay_ms(1000);	
			sprintf(tmp_buf, "�ļ�[%d]:�ȴ�������� %d sec", i, count++);
			UNT_Disp_text(tmp_buf, i + row_num, 0);
		}
	}
	
	
	UNT_Delay_ms(10000);
	UNT_Clean_lcd();
	row_num = 0;
	
	UNT_Disp_text("�ж��ļ�������ȫ��0xff",row_num++, 0);
	err = 0;
	for(i = 0; i < NUM_DATA_FILE; i++)
	{
		ret = TEFS_Check_area_val(fd[i], 0, DATA_FILE_SIZE, 0xff);
		if(ret == 0)
		{
			
			sprintf(tmp_buf, "check[%d] ok", i);
		}
		else
		{
			
			sprintf(tmp_buf, "check[%d] err at:%d", i, ret);
		}
		UNT_Disp_text(tmp_buf,row_num + i, 0);
	}
	
	UNT_Delay_ms(10000);
	UNT_Clean_lcd();
	
	UNT_Disp_text("�������",0, 0);
	
	//�ȶ��ļ�д���0xff������
#if 1	
	for(i = 0; i < NUM_DATA_FILE; i++)
	{
		sprintf(tmp_buf, "mod_chn_%d", i);
		
		if(TEFS_Fill_data(fd[i], my_buf, my_buf_size))
		{
			sprintf(tmp_buf,"�ļ�[%d]:���ʧ��", i);
		}
		else
		{
			ret = TEFS_Check_area_val(fd[i], 0, DATA_FILE_SIZE, FILL_DATA);
			if(ret)
				sprintf(tmp_buf,"�ļ�[%d]:У��ʧ����:%d", i, ret);
			else
				sprintf(tmp_buf,"�ļ�[%d]:���ɹ�", i);
		}
		
		UNT_Disp_text(tmp_buf, i + 1 , 0);
		
	}
	
	UNT_Delay_ms(15000);
	UNT_Clean_lcd();

	
	//�������ԣ��ļ��ڲ����ǣ���Ҫ���ļ�ͷ���ļ�β���������������Ͳ�����ÿ�β������������������
	//����1���ֽ�
	TEFS_Erase(fd, 3, 1);
	UNT_Delay_ms(15000);
	UNT_Clean_lcd();
	
	
	//����һ����ֽ�
	TEFS_Erase(fd, 3, DATA_FILE_SIZE / 2);
	UNT_Delay_ms(15000);
	UNT_Clean_lcd();
#endif	
	
	//����ȫ�����ֽ�
	TEFS_Erase(fd, 0, DATA_FILE_SIZE);
	
	
	
	UNT_Delay_ms(15000);
	
	
}


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static void TEFS_Erase(int *arr_fd, uint32_t start, uint32_t size)
{
	int i;
	char tmp_buf[48];
	
	sprintf(tmp_buf,"�����ֽ�:%d", size);
	UNT_Disp_text(tmp_buf, 0, 0);
	
	for(i = 0; i < NUM_DATA_FILE; i++)
	{
		row_num = 1 + i;
		if(TEFS_erase_area(i, arr_fd,start, size))
		{
			sprintf(tmp_buf,"����[%d : %xh] ʧ��", i, size);
			UNT_Disp_text(tmp_buf,i + 1, 0);
		}
		else
		{
			
			sprintf(tmp_buf,"����[%d:%xh] �ɹ�", i, size);
			UNT_Disp_text(tmp_buf,i + 1, 0);
		}
		
		

		
	}
	
	
}


static int TEFS_Fill_data(int fd, uint8_t* buf, uint32_t size)
{
	uint32_t addr;
	int ret = 0;


	phn_sys.fs.fs_lseek(fd, WR_SEEK_SET, 0);
	memset(my_buf, FILL_DATA, my_buf_size);
	for(addr = 0; addr < DATA_FILE_SIZE; addr += size)
	{
		
		if(phn_sys.fs.fs_raw_write(fd, my_buf, size) != size)
		{
			ret = 1;
			break;
		}
		
	}

	return ret;
}

//���ĳһ�����ڵ������Ƿ���ָ����ֵ���
//�ɹ�����0
//��ȡ���󷵻ش���ĵ�ַ + 1
static int TEFS_Check_area_val(int fd, uint32_t start_addr, uint32_t end_addr, uint8_t val)
{
	uint32_t addr;
	char tmp_buf[48];
	int j;
	int ret = 0;
	
	phn_sys.fs.fs_lseek(fd, RD_SEEK_SET, start_addr);
	
	for(addr = start_addr; addr < end_addr; addr += my_buf_size)
	{
		
		memset(my_buf, 0, my_buf_size);
		if(phn_sys.fs.fs_raw_read(fd, (uint8_t *)my_buf, my_buf_size) != my_buf_size)
		{
			
			sprintf(tmp_buf,"[%d] RDE %d",fd,  addr);
			UNT_Disp_text(tmp_buf, 10, 0);
			return addr + 1;
		}
		
		
		for(j = 0; j < my_buf_size; j++)
		{
			
			//Ҫ���ĳ��ȿ��ܻ�С��my_buf_size
			if((j + addr) >= end_addr)
				break;
			if(my_buf[j] == val)
				continue;
			
			
			sprintf(tmp_buf,"[%d] CKE [%d] %xh != %xh  ", fd, addr + j, my_buf[j], val);
			UNT_Disp_text(tmp_buf,11 , 0);
//			return addr + j + 1;
//			goto read_again;
			ret = -1;
			break;
		}
	}
	
	return ret;
}

static int TEFS_erase_area(char n, int *all_fd, uint32_t start_addr, uint32_t erase_size)
{
	uint8_t			buf[8];
//	int					size = 8;
	int					ret = 0;
//	int 				i;
	
	//	������ļ����ڵ��������ߵ��ļ�
	int			ngh_fd[2] = {0, 2};	

	
	
	if(n == 0)
	{
		ngh_fd[0] = -1;
		ngh_fd[1] = all_fd[1];
		
	}
	else if(n == (NUM_DATA_FILE - 1))
	{
		
		ngh_fd[0] = all_fd[n - 1];
		ngh_fd[1] = -1;
	}
	else
	{
		
		ngh_fd[0] = all_fd[n - 1];
		ngh_fd[1] = all_fd[n + 1];
		
	}
	
	if(erase_size < (DATA_FILE_SIZE - start_addr))		//�ļ��ڲ��ж��Ƿ�Խ�缴��
	{
		if(start_addr < 2)		//
			start_addr = 2;
		
		phn_sys.fs.fs_erase_file(all_fd[n], start_addr, erase_size);		
		
		//�����������ǲ���0xff
		//ǰ��߽綼Ҫ���һ��
		row_num = 1 + n;
		ret = TEFS_Check_area_val(all_fd[n], start_addr, start_addr + erase_size, 0xff);
		if(ret != 0)
			goto exit;
		
		//���ǰ�߽�
		phn_sys.fs.fs_lseek(all_fd[n], RD_SEEK_SET, start_addr - 2);
		phn_sys.fs.fs_raw_read(all_fd[n], buf, 2);
		if(buf[0] != FILL_DATA)
		{
			ret = 1;
			goto exit;
		}
		if(buf[1] != FILL_DATA)
		{
				ret = 1;
				goto exit;
			}
		
		//����߽�
		phn_sys.fs.fs_lseek(all_fd[n], RD_SEEK_SET, start_addr + erase_size);
		phn_sys.fs.fs_raw_read(all_fd[n], buf, 2);
		if(buf[0] != FILL_DATA)
		{
				ret = 1;
				goto exit;
			}
		if(buf[1] != FILL_DATA)
		{
				ret = 1;
				goto exit;
			}
		
		
		
		goto exit;
			
		
	}
	
	
	//����ǰһ���ļ����ܻ�����һ�β��Ե�ʱ�򱻲���������ڴ˴����һ��
	if(ngh_fd[0] >= 0)
	{
		buf[0] = FILL_DATA;
		buf[1] = FILL_DATA;
		
		phn_sys.fs.fs_lseek(ngh_fd[0], WR_SEEK_SET, DATA_FILE_SIZE - 2);

		phn_sys.fs.fs_raw_write(ngh_fd[0], buf, 2);
	}
	
	if(ngh_fd[1] >= 0)
	{
		buf[0] = FILL_DATA;
		buf[1] = FILL_DATA;
		
		phn_sys.fs.fs_lseek(ngh_fd[1], WR_SEEK_SET, 0);

		phn_sys.fs.fs_raw_write(ngh_fd[1], buf, 2);
	}

	phn_sys.fs.fs_erase_file(all_fd[n], 0, DATA_FILE_SIZE);	
	
	

	row_num = 1 + n;	
	ret = TEFS_Check_area_val(all_fd[n], 0, DATA_FILE_SIZE, 0xff);
	if(ret != 0)
	{
		goto exit;
	}
		
	//���������ļ��Ƿ�������ƻ�


		//���ǰһ���ļ���β��
		if(ngh_fd[0] >= 0)
		{
			buf[0] = 0xff;
			buf[0] = 0xff;
			
			phn_sys.fs.fs_lseek(ngh_fd[0], RD_SEEK_SET, DATA_FILE_SIZE - 2);
			phn_sys.fs.fs_raw_read(ngh_fd[0], buf, 2);
			if(buf[0] != FILL_DATA)
			{
				ret = 1;
				goto exit;
			}
			if(buf[1] != FILL_DATA)
			{
				ret = 1;
				goto exit;
			}
			
			
		}
			
		
		//����һ���ļ���ͷ��
		if(ngh_fd[1] >= 0)
		{
			buf[0] = 0xff;
			buf[0] = 0xff;
			
			phn_sys.fs.fs_lseek(ngh_fd[1], RD_SEEK_SET, 0);
			phn_sys.fs.fs_raw_read(ngh_fd[1], buf, 2);
			if(buf[0] != FILL_DATA)
			{
				ret = 1;
				goto exit;
			}
			if(buf[1] != FILL_DATA)
			{
				ret = 1;
				goto exit;
			}
		}
	
		exit:
		return ret;
			
	
}

#endif
