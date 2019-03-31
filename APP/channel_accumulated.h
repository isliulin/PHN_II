//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_channel_accumulated_H__
#define __INC_channel_accumulated_H__
#include <stdint.h>
#include "utils/Storage.h"

//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
extern rcd_chn_accumlated_t	arr_chn_acc[NUM_CHANNEL];
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
int CNA_Init(void);
void CNA_default(int num_ch);

void CNA_Run(int cyc_ms);
void CNA_Print_enable(char *s, char	enable);
void CNA_Print_acc_val(uint16_t *p_val, char *s, char pos);
int		CNA_Commit(char chn_num);
int		CNA_Clear(char chn_num);
uint64_t CNA_arr_u16_2_u64(uint16_t *p_u16, char num_data);
void CNA_Forbid_acc(int chn_num, int forbid);
int CNA_Is_forbid(int chn_num);
#endif
