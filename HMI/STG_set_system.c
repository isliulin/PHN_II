#include "HMI_striped_background.h"
#include "sdhDef.h"
#include "ModelFactory.h"
#include "system.h"
#include <string.h>
#include "utils/log.h"

#include "Communication/smart_bus.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------
static int SysStrategy_entry(int row, int col, void *pp_text);
static void SYS_build_component(void *arg);

static int Sys_key_up(void *arg);
static int Sys_key_dn(void *arg);
static int Sys_key_lt(void *arg);
static int Sys_key_rt(void *arg);
static int Sys_key_er(void *arg);
static int Sys_init(void *arg);
static int Sys_get_focusdata(void *pp_data,  strategy_focus_t *p_in_syf);
static int Sys_commit(void *arg);
static void SYS_Exit(void);

strategy_t	g_sys_strategy = {
	SysStrategy_entry,
	Sys_init,
	SYS_build_component,
	Sys_key_up,
	Sys_key_dn,
	Sys_key_lt,
	Sys_key_rt,
	Sys_key_er,
	Sys_get_focusdata,
	Sys_commit,
	SYS_Exit,

};
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
enum {
	row_ver,
	row_set_time,
	row_set_psd,
	row_num_chn,
	row_rcd_time_gap,
	row_brk_cpl,
	row_brk_rss,
	row_cmm_method,
	row_baud,
	row_addr,
	row_modify_param,
	row_cold_end_way,
	row_cold_end_tempure,
	row_chn_status,
	row_beep,
	row_factory_reset,
	row_num
	
	
}sys_rows;

#define STG_NUM_VRAM			(row_num + 1)	//STG_TIPS_VRAM_NUM ��������
#define STG_TIPS_VRAM_NUM	 row_num
#define STG_RUN_VRAM_NUM	 row_num + 1

#define STG_SELF						g_sys_strategy
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	uint8_t 		cur_page;
	uint8_t			flag_conf_change;
	uint8_t			set_clt_failed;		//��������¶�ʧ�ܱ�־
	uint8_t			none;
	system_conf_t	temp_sys_conf;		
	
}sys_run_t;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

 static char *const arr_p_sys_entry[row_num] = {"�汾��","ʱ���趨", "�û�����", "ͨ����Ŀ", "��¼���", \
	"��ż����", "���账��", "ͨ�ŷ�ʽ", "������", "������ַ", "���ڲ����޸�","��˷�ʽ",  "����¶�", "ͨ��״̬��ʾ", \
	 "��������", "�ָ���������"};
 

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void Sys_update_syf(strategy_focus_t *p_syf);
static int Sys_update_content(int op, int weight);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int SysStrategy_entry(int row, int col, void *pp_text)
{
	char 	**pp = (char **)pp_text;
	Model	*model;
	sys_run_t	*p_run = (sys_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	system_conf_t	*p_sys_conf;
	
	strategy_focus_t *p_syf = &STG_SELF.sf;
	
	p_sys_conf = &p_run->temp_sys_conf;
	
	if(row >= row_num)
			return 0;
	if(col == 0) {
		
		*pp = arr_p_sys_entry[row];
		return strlen(arr_p_sys_entry[row]);
	} else if(col == 1){
		
		if(row >= STRIPE_MAX_ROWS)
			p_run->cur_page = 1;
		else
			p_run->cur_page = 0;
		
		
		switch(row) {
			case row_ver:
				sprintf(arr_p_vram[row], "%d.%d", phn_sys.major_ver, phn_sys.minor_ver);
				break;
			case row_set_time:
			
				model = Create_model("time");
				model->to_string(model, 1, arr_p_vram[row]);
				p_syf->num_byte = strlen(arr_p_vram[row]);
				break;
			case row_set_psd:
				Print_sys_param(&p_sys_conf->password, arr_p_vram[row], 48, es_psd);
				break;
			case row_num_chn:
				sprintf(arr_p_vram[row], "%d", p_sys_conf->num_chn);
				break;
			case row_rcd_time_gap:
				sprintf(arr_p_vram[row], "%d", p_sys_conf->record_gap_s);
				break;
			case row_brk_cpl:
				Print_sys_param(&p_sys_conf->break_couple, arr_p_vram[row], 48, es_brk_cpl);
				break;
			case row_brk_rss:
				Print_sys_param(&p_sys_conf->break_resistor, arr_p_vram[row], 48, es_brk_rss);
				break;
			case row_cmm_method:
				Print_sys_param(&p_sys_conf->communication_mode, arr_p_vram[row], 48, es_cmn_md);
				break;
			case row_baud:
				sprintf(arr_p_vram[row], "%d", p_sys_conf->baud_rate);
				break;
			case row_addr:
				sprintf(arr_p_vram[row], "%d", p_sys_conf->id);
				break;
			case row_modify_param:
				Print_sys_param(&p_sys_conf->disable_modify_adjust_paramter, arr_p_vram[row], 48, es_mdfy_prm);
				break;
			case row_cold_end_way:
				Print_sys_param(&p_sys_conf->cold_end_way, arr_p_vram[row], 48, es_cold_end_way);
				break;
			case row_cold_end_tempure:
				Print_sys_param(p_sys_conf, arr_p_vram[row], 48, es_CJC);
				break;
			case row_chn_status:
				
				Print_sys_param(&p_sys_conf->show_chn_status, arr_p_vram[row], 48, es_vcs);
				break;
			case row_beep:
				Print_sys_param(&p_sys_conf->enable_beep, arr_p_vram[row], 48, es_beep);
				break;
			case row_factory_reset:
				
				sprintf(arr_p_vram[row], "....");
			
//				p_syf->f_row = STRIPE_MAX_ROWS;
//				Sys_update_syf(p_syf);
				break;
			default:
				goto exit;
		}
		
		//������ҳ��ʱ��Ҫ�ѽ���Ҳ�����л�
		//Ŀǰֻ������2ҳ�����
		if(row == 0)
		{
			p_syf->f_row = 0;
			Sys_update_syf(p_syf);
		}
		else if(row == STRIPE_MAX_ROWS)
		{
			p_syf->f_row= STRIPE_MAX_ROWS;
			Sys_update_syf(p_syf);
		}
		
		*pp = arr_p_vram[row];
		return strlen(*pp);
		
	}
	exit:
	return 0;
}

static int Sys_init(void *arg)
{
	int i = 0;
	sys_run_t *p_run;
	int flag = *(int *)arg;
	if(flag)
		return 0;	
	
	memset(&STG_SELF.sf, 0, sizeof(STG_SELF.sf));
	STG_SELF.sf.f_col = 1;
	STG_SELF.sf.f_row = 0;
	STG_SELF.sf.num_byte = 1;
	
	HMI_Ram_init();
	
	//��14��������ʾ��ʾ
	for(i = 0; i < STG_NUM_VRAM; i++) {
		
		arr_p_vram[i] = HMI_Ram_alloc(48);
		memset(arr_p_vram[i], 0, 48);
	}
	STG_SELF.total_col = 2;
	STG_SELF.total_row = row_num;
	
	arr_p_vram[STG_RUN_VRAM_NUM] = HMI_Ram_alloc(sizeof(sys_run_t));
	p_run = (sys_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	memcpy(&p_run->temp_sys_conf, &phn_sys.sys_conf, sizeof(phn_sys.sys_conf));
	p_run->cur_page = 0;
	
	return RET_OK;
}
static void SYS_Exit(void)
{
	
}
static int Sys_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf)
{
	strategy_focus_t *p_syf = &STG_SELF.sf;
	char		**pp_vram = (char **)pp_data;
	int ret = 0;
	
	if(p_in_syf)
		p_syf = p_in_syf;
	
	
	if(p_syf->f_row < row_num) {
		*pp_vram = arr_p_vram[p_syf->f_row] + p_syf->start_byte;
		p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
		ret = p_syf->num_byte;
	}
	else 
		ret = -1;
	
	
	return ret;
	
}



static int Sys_key_up(void *arg)
{
//	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
//	strategy_focus_t *p_syf = &STG_SELF.sf;
	int 			ret = RET_OK;
//	
//	if(arg) {
//		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
//		
//	}
//	if(kt.key_type == SY_KEYTYPE_LONGPUSH) {
//		phn_sys.key_weight += 10;
//		
//	} else {
//		phn_sys.key_weight = 1;
//	}

	


	ret = Sys_update_content(OP_ADD, phn_sys.key_weight);
	return ret;
}


 static void	SYS_Btn_hdl(void *self, uint8_t	btn_id)
 {
	 sys_run_t	*p_run = (sys_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	uint8_t			sbub_buf[32] = {0};
	I_dev_Char 			*I_uart3 = NULL;
	uint8_t 			i, set_cld_failed = 0;


	Dev_open(DEVID_UART3, ( void *)&I_uart3);
	 
	 if(btn_id == BTN_TYPE_SAVE)
	 {
		 
		 //sundh  ��˲�����ʽ����ⲿ
		 if((p_run->temp_sys_conf.cold_end_way == 0)	&& (phn_sys.sys_conf.cold_end_way == 1))
		 {
			 
			 i = SmBus_Set_cold_tmpr(0xff, sbub_buf, 32);
			 I_uart3->write(I_uart3, sbub_buf, i);
			 delay_ms(100);

			 i = I_uart3->read(I_uart3, sbub_buf, 32);
			if(i <= 0)
			{
				set_cld_failed = 1;
				
			}
		 }
		 //sundh 181212 ����Ϊ�趨,���趨ֵ�仯ʱ��������������¶ȵ�ͨ����
		 else if((p_run->temp_sys_conf.cold_end_way)	&& (p_run->temp_sys_conf.CJC != phn_sys.sys_conf.CJC))
		 {
			 i = SmBus_Set_cold_tmpr(p_run->temp_sys_conf.CJC, sbub_buf, 32);
			 I_uart3->write(I_uart3, sbub_buf, i);
			 delay_ms(100);
			 i = I_uart3->read(I_uart3, sbub_buf, 32);
			if(i <= 0)
			{
				set_cld_failed = 1;

			}
			 
		 }
		 
		 
		 p_run->set_clt_failed = set_cld_failed;
		
		if(p_run->flag_conf_change)
		{
			
			memcpy(&phn_sys.sys_conf, &p_run->temp_sys_conf, sizeof(p_run->temp_sys_conf));
			
			if(SYS_Commit() == RET_OK)
			{
				if(set_cld_failed)
				{
					sprintf(arr_p_vram[STG_TIPS_VRAM_NUM],"ͨ����Ӧ��");
					Win_content(arr_p_vram[STG_TIPS_VRAM_NUM]);
					STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
					
				}
				else
				{
					sprintf(arr_p_vram[STG_TIPS_VRAM_NUM],"ϵͳ����д��ɹ�");
					Win_content(arr_p_vram[STG_TIPS_VRAM_NUM]);
					STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
				}
			}
			else
			{
				
				sprintf(arr_p_vram[STG_TIPS_VRAM_NUM],"ϵͳ����д��ʧ��");
				Win_content(arr_p_vram[STG_TIPS_VRAM_NUM]);
				STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
			}
				
			
			p_run->flag_conf_change = 0;;
		}
			
			
		

		 
	 }
	
	 
 }
static void SYS_build_component(void *arg)
{
	Button			*p_btn = BTN_Get_Sington();
	p_btn->build_each_btn(0, BTN_TYPE_MENU, Setting_btn_hdl, arg);
	p_btn->build_each_btn(3, BTN_TYPE_SAVE, SYS_Btn_hdl, arg);
		
	
	
}
static int Sys_key_dn(void *arg)
{
	
//	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
//	strategy_focus_t *p_syf = &STG_SELF.sf;
	int 			ret = RET_OK;
//	
//	if(arg) {
//		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
//		
//	}
//	if(kt.key_type == SY_KEYTYPE_LONGPUSH) {
//		phn_sys.key_weight += 10;
//		
//	} else {
//		phn_sys.key_weight = 1;
//	}

	


	ret = Sys_update_content(OP_SUB, phn_sys.key_weight);


	
	
	return ret;
}

static int Sys_key_rt(void *arg)
{
	strategy_focus_t *p_syf = &STG_SELF.sf;
	sys_run_t	*p_run = (sys_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	int ret = RET_OK;

	if(p_run->cur_page == 0) {
		
		p_syf->f_row = Operate_in_range(p_syf->f_row, OP_ADD, 1, 0, STRIPE_MAX_ROWS - 1);
		
		if(p_syf->f_row == 0)
		{
			//˵����귢���˷�ת
			//ֻ�г�����Χ�Żᷴת
			ret = -1;
		} else if(p_syf->f_row == row_num_chn)		{//ͨ����Ŀ����������
		
			p_syf->f_row ++;
			
		}
		
	} else {
		p_syf->f_row = Operate_in_range(p_syf->f_row, OP_ADD, 1, STRIPE_MAX_ROWS, row_num - 1);
		
		if(p_syf->f_row == STRIPE_MAX_ROWS)
		{
			//˵����귢���˷�ת
			//ֻ�г�����Χ�Żᷴת
			
			ret = -1;
		}
	}
	
	
	
	Sys_update_syf(p_syf);
	return ret;
}

static int Sys_key_lt(void *arg)
{
	strategy_focus_t *p_syf = &STG_SELF.sf;
	sys_run_t	*p_run = (sys_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	int ret = RET_OK;

	
	if(p_run->cur_page == 0) {
		if(p_syf->f_row == 0)
		{
			//˵����귢���˷�ת
			//ֻ�г�����Χ�Żᷴת
			ret = -1;
			goto exit;	//ֱ�������������´����أ�������ǵ�һ��λ��
		}
		p_syf->f_row = Operate_in_range(p_syf->f_row, OP_SUB, 1, 0, STRIPE_MAX_ROWS - 1);
		
		if(p_syf->f_row == row_num_chn)		{//ͨ����Ŀ����������
		
			p_syf->f_row --;
			
		}
		
	} else {
		if(p_syf->f_row == STRIPE_MAX_ROWS)
		{
			//˵����귢���˷�ת
			//ֻ�г�����Χ�Żᷴת
			ret = -1;
			goto exit;
		}
		
		p_syf->f_row = Operate_in_range(p_syf->f_row, OP_SUB, 1, STRIPE_MAX_ROWS, row_num - 1);
		
		
	}
	Sys_update_syf(p_syf);
exit:

	return ret;
}



static int Sys_key_er(void *arg)
{
	//�����е�������д��ģ��

	strategy_focus_t 	*p_syf = &STG_SELF.sf;
	int								ret = RET_OK;
	switch(p_syf->f_row) {
	case row_set_time:	
		STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_time, arr_p_vram[p_syf->f_row]);
		break;
	case row_set_psd:
		STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_psd, arr_p_vram[p_syf->f_row]);
		break;
	case row_factory_reset:
		//�ָ��������ã�Ӧ�ò�ֻ�ǻָ�ϵͳ���ã�����ͨ�����õȣ�Ӧ��ҲҪ�ָ�
		Win_content("ȷ�ϻָ��������ã����ݽ������!");
		STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, arr_p_vram[p_syf->f_row]);
		
		break;
	default:
		ret = ERR_OPT_FAILED;
		break;
	
	
	}
	

	
	
	return ret;
}

static int Sys_commit(void *arg)
{
	//�����е�������д��ģ��
	
	strategy_focus_t 	*p_syf = &STG_SELF.sf;
	Model							*model;
	int								ret = RET_OK;
	sys_run_t	*p_run = (sys_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	
	
	switch(p_syf->f_row) {
	case row_set_time:

		model = Create_model("time");
		ret = model->set_by_string(model, 1, arr_p_vram[p_syf->f_row]);
			
		break;
	case row_set_psd:
		Str_set_password(arr_p_vram[p_syf->f_row], p_run->temp_sys_conf.password);
		break;
	case row_factory_reset:
		LOG_Add(LOG_Factory_Reset);
		SYS_Reset(0, NUM_CHANNEL);
		break;
//	default:
//		ret = ERR_OPT_FAILED;
//		break;
	
	
	
	}

//	
	if(p_run->set_clt_failed)
		ret = -1;
	
	return ret;
}

static void Sys_update_syf(strategy_focus_t *p_syf)
{
	p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	
//	//�ѵ�λ�޳���
//	switch(p_syf->f_row)
//	{
//		case 6:		//x M
//		case 7:		//x S
//		case 8:		//x %
//			p_syf->num_byte -= 2;
//			break;
//		
//	}
//	
	
}


static int Sys_update_content(int op, int weight)
{
	strategy_focus_t 	*p_syf = &STG_SELF.sf;
	int					ret = RET_OK;
	sys_run_t			*p_run = (sys_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	system_conf_t 		*p_cfg = &p_run->temp_sys_conf;
	
	p_run->flag_conf_change = 1;
	switch(p_syf->f_row) {
	case row_set_time:		
		STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_time, arr_p_vram[p_syf->f_row]);
		ret = 1;
		break;
	case row_set_psd:
		STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_psd, arr_p_vram[p_syf->f_row]);
		ret = 1;
		break;
	case row_num_chn:		//ͨ����Ŀ����������
		p_run->flag_conf_change = 0;
		break;
	case row_rcd_time_gap:
		Str_set_sys_param(p_cfg,arr_p_vram[p_syf->f_row], es_rcd_t_s, op, weight);
		break;
	case row_brk_cpl:
		Str_set_sys_param(p_cfg,arr_p_vram[p_syf->f_row], es_brk_cpl, op, weight);
		break;
	case row_brk_rss:
		Str_set_sys_param(p_cfg,arr_p_vram[p_syf->f_row], es_brk_rss, op, weight);
		break;
	case row_cmm_method:
		//sundh 181212 ͨ�ŷ�ʽ����������
//		Str_set_sys_param(p_cfg,arr_p_vram[p_syf->f_row], es_cmn_md, op, weight);
		break;
	case row_baud:
		Str_set_sys_param(p_cfg,arr_p_vram[p_syf->f_row], es_baud, op, weight);
		break;
	case row_addr:
		Str_set_sys_param(p_cfg,arr_p_vram[p_syf->f_row], es_id, op, weight);
		break;
	case row_modify_param:
//		Str_set_sys_param(p_cfg,arr_p_vram[p_syf->f_row], es_mdfy_prm, op, weight);
		break;
	case row_cold_end_way:
		Str_set_sys_param(p_cfg,arr_p_vram[p_syf->f_row], es_cold_end_way, op, weight);
		break;
	case row_cold_end_tempure:
		Str_set_sys_param(p_cfg,arr_p_vram[p_syf->f_row], es_CJC, op, weight);
		break;
	case row_chn_status:
		Str_set_sys_param(p_cfg,arr_p_vram[p_syf->f_row], es_vcs, op, weight);
		break;
	case row_beep:
		Str_set_sys_param(p_cfg,arr_p_vram[p_syf->f_row], es_beep, op, weight);
		break;
	default:
		p_run->flag_conf_change = 0;
		break;
	
	
	}
	return ret;
}

