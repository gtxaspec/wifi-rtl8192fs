/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifndef __RTL8192F_CMD_H__
#define __RTL8192F_CMD_H__

/* ---------------------------------------------------------------------------------------------------------
 * ----------------------------------    H2C CMD DEFINITION    ------------------------------------------------
 * --------------------------------------------------------------------------------------------------------- */

enum h2c_cmd_8192F {
	/* Common Class: 000 */
	H2C_8192F_RSVD_PAGE = 0x00,
	H2C_8192F_MEDIA_STATUS_RPT = 0x01,
	H2C_8192F_SCAN_ENABLE = 0x02,
	H2C_8192F_KEEP_ALIVE = 0x03,
	H2C_8192F_DISCON_DECISION = 0x04,	
	H2C_8192F_PSD_OFFLOAD = 0x05,	
	H2C_8192F_AP_OFFLOAD = 0x08,	
	H2C_8192F_BCN_RSVDPAGE = 0x09,	
	H2C_8192F_PROBERSP_RSVDPAGE = 0x0A,	
	H2C_8192F_FCS_RSVDPAGE = 0x10,	
	H2C_8192F_FCS_INFO = 0x11,	
	H2C_8192F_AP_WOW_GPIO_CTRL = 0x13,

	/* PoweSave Class: 001 */
	H2C_8192F_SET_PWR_MODE = 0x20,
	H2C_8192F_PS_TUNING_PARA = 0x21,
	H2C_8192F_PS_TUNING_PARA2 = 0x22,
	H2C_8192F_P2P_LPS_PARAM = 0x23,	
	H2C_8192F_P2P_PS_OFFLOAD = 0x24,	
	H2C_8192F_PS_SCAN_ENABLE = 0x25,	
	H2C_8192F_SAP_PS_ = 0x26,
	H2C_8192F_INACTIVE_PS_ = 0x27,/* Inactive_PS */
	H2C_8192F_FWLPS_IN_IPS_ = 0x28,

	/* Dynamic Mechanism Class: 010 */
	H2C_8192F_MACID_CFG = 0x40,	
	H2C_8192F_TXBF = 0x41,	
	H2C_8192F_RSSI_SETTING = 0x42,	
	H2C_8192F_AP_REQ_TXRPT = 0x43,	
	H2C_8192F_INIT_RATE_COLLECT = 0x44,	
	H2C_8192F_RA_PARA_ADJUST = 0x46,

	/* BT Class: 011 */
	H2C_8192F_B_TYPE_TDMA = 0x60,
	H2C_8192F_BT_INFO = 0x61,
	H2C_8192F_FORCE_BT_TXPWR = 0x62,
	H2C_8192F_BT_IGNORE_WLANACT = 0x63,
	H2C_8192F_DAC_SWING_VALUE = 0x64,
	H2C_8192F_ANT_SEL_RSV = 0x65,
	H2C_8192F_WL_OPMODE = 0x66,
	H2C_8192F_BT_MP_OPER = 0x67,
	H2C_8192F_BT_CONTROL = 0x68,
	H2C_8192F_BT_WIFI_CTRL = 0x69,
	H2C_8192F_BT_FW_PATCH = 0x6A,
	H2C_8192F_BT_WLAN_CALIBRATION = 0x6D,

	/* WOWLAN Class: 100 */
	H2C_8192F_WOWLAN = 0x80,
	H2C_8192F_REMOTE_WAKE_CTRL = 0x81,
	H2C_8192F_AOAC_GLOBAL_INFO = 0x82,	
	H2C_8192F_AOAC_RSVD_PAGE = 0x83,	
	H2C_8192F_AOAC_RSVD_PAGE2 = 0x84,
	H2C_8192F_D0_SCAN_OFFLOAD_CTRL = 0x85,
	H2C_8192F_D0_SCAN_OFFLOAD_INFO = 0x86,
	H2C_8192F_CHNL_SWITCH_OFFLOAD = 0x87,
	H2C_8192F_P2P_OFFLOAD_RSVD_PAGE = 0x8A,	
	H2C_8192F_P2P_OFFLOAD = 0x8B,

	H2C_8192F_RESET_TSF = 0xC0,
	H2C_8192F_MAXID,
};

/* ---------------------------------------------------------------------------------------------------------
 * ----------------------------------    H2C CMD CONTENT    --------------------------------------------------
 * ---------------------------------------------------------------------------------------------------------
 * _RSVDPAGE_LOC_CMD_0x00 */
#define SET_8192F_H2CCMD_RSVDPAGE_LOC_PROBE_RSP(__pH2CCmd, __Value)			SET_BITS_TO_LE_1BYTE(__pH2CCmd, 0, 8, __Value)
#define SET_8192F_H2CCMD_RSVDPAGE_LOC_PSPOLL(__pH2CCmd, __Value)				SET_BITS_TO_LE_1BYTE((__pH2CCmd)+1, 0, 8, __Value)
#define SET_8192F_H2CCMD_RSVDPAGE_LOC_NULL_DATA(__pH2CCmd, __Value)			SET_BITS_TO_LE_1BYTE((__pH2CCmd)+2, 0, 8, __Value)
#define SET_8192F_H2CCMD_RSVDPAGE_LOC_QOS_NULL_DATA(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE((__pH2CCmd)+3, 0, 8, __Value)
#define SET_8192F_H2CCMD_RSVDPAGE_LOC_BT_QOS_NULL_DATA(__pH2CCmd, __Value)	SET_BITS_TO_LE_1BYTE((__pH2CCmd)+4, 0, 8, __Value)

/*_MEDIA_STATUS_RPT_PARM_CMD_0x01*/
#define SET_8192F_H2CCMD_MSRRPT_PARM_OPMODE(__pH2CCmd, __Value)				SET_BITS_TO_LE_1BYTE(__pH2CCmd, 0, 1, __Value)
#define SET_8192F_H2CCMD_MSRRPT_PARM_MACID_IND(__pH2CCmd, __Value)			SET_BITS_TO_LE_1BYTE(__pH2CCmd, 1, 1, __Value)
#define SET_8192F_H2CCMD_MSRRPT_PARM_MACID(__pH2CCmd, __Value)				SET_BITS_TO_LE_1BYTE(__pH2CCmd+1, 0, 8, __Value)
#define SET_8192F_H2CCMD_MSRRPT_PARM_MACID_END(__pH2CCmd, __Value)			SET_BITS_TO_LE_1BYTE(__pH2CCmd+2, 0, 8, __Value)
/* _PWR_MOD_CMD_0x20 */
#define SET_8192F_H2CCMD_PWRMODE_PARM_MODE(__pH2CCmd, __Value)				SET_BITS_TO_LE_1BYTE(__pH2CCmd, 0, 8, __Value)
#define SET_8192F_H2CCMD_PWRMODE_PARM_RLBM(__pH2CCmd, __Value)				SET_BITS_TO_LE_1BYTE((__pH2CCmd)+1, 0, 4, __Value)
#define SET_8192F_H2CCMD_PWRMODE_PARM_SMART_PS(__pH2CCmd, __Value)			SET_BITS_TO_LE_1BYTE((__pH2CCmd)+1, 4, 4, __Value)
#define SET_8192F_H2CCMD_PWRMODE_PARM_BCN_PASS_TIME(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE((__pH2CCmd)+2, 0, 8, __Value)
#define SET_8192F_H2CCMD_PWRMODE_PARM_ALL_QUEUE_UAPSD(__pH2CCmd, __Value)	SET_BITS_TO_LE_1BYTE((__pH2CCmd)+3, 0, 8, __Value)
#define SET_8192F_H2CCMD_PWRMODE_PARM_BCN_EARLY_C2H_RPT(__pH2CCmd, __Value)	SET_BITS_TO_LE_1BYTE((__pH2CCmd)+3, 2, 1, __Value)
#define SET_8192F_H2CCMD_PWRMODE_PARM_PWR_STATE(__pH2CCmd, __Value)			SET_BITS_TO_LE_1BYTE((__pH2CCmd)+4, 0, 8, __Value)

#define GET_8192F_H2CCMD_PWRMODE_PARM_MODE(__pH2CCmd)					LE_BITS_TO_1BYTE(__pH2CCmd, 0, 8)

/* _PS_TUNE_PARAM_CMD_0x21 */
#define SET_8192F_H2CCMD_PSTUNE_PARM_BCN_TO_LIMIT(__pH2CCmd, __Value)			SET_BITS_TO_LE_1BYTE(__pH2CCmd, 0, 8, __Value)
#define SET_8192F_H2CCMD_PSTUNE_PARM_DTIM_TIMEOUT(__pH2CCmd, __Value)			SET_BITS_TO_LE_1BYTE(__pH2CCmd+1, 0, 8, __Value)
#define SET_8192F_H2CCMD_PSTUNE_PARM_ADOPT(__pH2CCmd, __Value)			SET_BITS_TO_LE_1BYTE(__pH2CCmd+2, 0, 1, __Value)
#define SET_8192F_H2CCMD_PSTUNE_PARM_PS_TIMEOUT(__pH2CCmd, __Value)			SET_BITS_TO_LE_1BYTE(__pH2CCmd+2, 1, 7, __Value)
#define SET_8192F_H2CCMD_PSTUNE_PARM_DTIM_PERIOD(__pH2CCmd, __Value)			SET_BITS_TO_LE_1BYTE(__pH2CCmd+3, 0, 8, __Value)

/* _MACID_CFG_CMD_0x40 */
#define SET_8192F_H2CCMD_MACID_CFG_MACID(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd, 0, 8, __Value)
#define SET_8192F_H2CCMD_MACID_CFG_RAID(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd+1, 0, 5, __Value)
#define SET_8192F_H2CCMD_MACID_CFG_SGI_EN(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd+1, 7, 1, __Value)
#define SET_8192F_H2CCMD_MACID_CFG_BW(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd+2, 0, 2, __Value)
#define SET_8192F_H2CCMD_MACID_CFG_NO_UPDATE(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd+2, 3, 1, __Value)
#define SET_8192F_H2CCMD_MACID_CFG_VHT_EN(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd+2, 4, 2, __Value)
#define SET_8192F_H2CCMD_MACID_CFG_DISPT(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd+2, 6, 1, __Value)
#define SET_8192F_H2CCMD_MACID_CFG_DISRA(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd+2, 7, 1, __Value)
#define SET_8192F_H2CCMD_MACID_CFG_RATE_MASK0(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd+3, 0, 8, __Value)
#define SET_8192F_H2CCMD_MACID_CFG_RATE_MASK1(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd+4, 0, 8, __Value)
#define SET_8192F_H2CCMD_MACID_CFG_RATE_MASK2(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd+5, 0, 8, __Value)
#define SET_8192F_H2CCMD_MACID_CFG_RATE_MASK3(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd+6, 0, 8, __Value)

/* _RSSI_SETTING_CMD_0x42 */
#define SET_8192F_H2CCMD_RSSI_SETTING_MACID(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd, 0, 8, __Value)
#define SET_8192F_H2CCMD_RSSI_SETTING_RSSI(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd+2, 0, 7, __Value)
#define SET_8192F_H2CCMD_RSSI_SETTING_ULDL_STATE(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd+3, 0, 8, __Value)

/* _AP_REQ_TXRPT_CMD_0x43 */
#define SET_8192F_H2CCMD_APREQRPT_PARM_MACID1(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd, 0, 8, __Value)
#define SET_8192F_H2CCMD_APREQRPT_PARM_MACID2(__pH2CCmd, __Value)		SET_BITS_TO_LE_1BYTE(__pH2CCmd+1, 0, 8, __Value)

/* _FORCE_BT_TXPWR_CMD_0x62 */
#define SET_8192F_H2CCMD_BT_PWR_IDX(__pH2CCmd, __Value)							SET_BITS_TO_LE_1BYTE(__pH2CCmd, 0, 8, __Value)

/* _FORCE_BT_MP_OPER_CMD_0x67 */
#define SET_8192F_H2CCMD_BT_MPOPER_VER(__pH2CCmd, __Value)							SET_BITS_TO_LE_1BYTE(__pH2CCmd, 0, 4, __Value)
#define SET_8192F_H2CCMD_BT_MPOPER_REQNUM(__pH2CCmd, __Value)							SET_BITS_TO_LE_1BYTE(__pH2CCmd, 4, 4, __Value)
#define SET_8192F_H2CCMD_BT_MPOPER_IDX(__pH2CCmd, __Value)							SET_BITS_TO_LE_1BYTE(__pH2CCmd+1, 0, 8, __Value)
#define SET_8192F_H2CCMD_BT_MPOPER_PARAM1(__pH2CCmd, __Value)							SET_BITS_TO_LE_1BYTE(__pH2CCmd+2, 0, 8, __Value)
#define SET_8192F_H2CCMD_BT_MPOPER_PARAM2(__pH2CCmd, __Value)							SET_BITS_TO_LE_1BYTE(__pH2CCmd+3, 0, 8, __Value)
#define SET_8192F_H2CCMD_BT_MPOPER_PARAM3(__pH2CCmd, __Value)							SET_BITS_TO_LE_1BYTE(__pH2CCmd+4, 0, 8, __Value)

/* _BT_FW_PATCH_0x6A */
#define SET_8192F_H2CCMD_BT_FW_PATCH_SIZE(__pH2CCmd, __Value)					SET_BITS_TO_LE_2BYTE((u8 *)(__pH2CCmd), 0, 16, __Value)
#define SET_8192F_H2CCMD_BT_FW_PATCH_ADDR0(__pH2CCmd, __Value)					SET_BITS_TO_LE_1BYTE((__pH2CCmd)+2, 0, 8, __Value)
#define SET_8192F_H2CCMD_BT_FW_PATCH_ADDR1(__pH2CCmd, __Value)					SET_BITS_TO_LE_1BYTE((__pH2CCmd)+3, 0, 8, __Value)
#define SET_8192F_H2CCMD_BT_FW_PATCH_ADDR2(__pH2CCmd, __Value)					SET_BITS_TO_LE_1BYTE((__pH2CCmd)+4, 0, 8, __Value)
#define SET_8192F_H2CCMD_BT_FW_PATCH_ADDR3(__pH2CCmd, __Value)					SET_BITS_TO_LE_1BYTE((__pH2CCmd)+5, 0, 8, __Value)

/* ---------------------------------------------------------------------------------------------------------
 * -------------------------------------------    Structure    --------------------------------------------------
 * --------------------------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------------------------------------
 * ----------------------------------    Function Statement     --------------------------------------------------
 * --------------------------------------------------------------------------------------------------------- */

/* host message to firmware cmd */
void rtl8192f_set_FwPwrMode_cmd(PADAPTER padapter, u8 Mode);
void rtl8192f_set_FwJoinBssRpt_cmd(PADAPTER padapter, u8 mstatus);
/* s32 rtl8192f__set_lowpwr_lps_cmd(PADAPTER padapter, u8 enable); */
void rtl8192f_set_FwPsTuneParam_cmd(PADAPTER padapter);
void rtl8192f_download_rsvd_page(PADAPTER padapter, u8 mstatus);
#ifdef CONFIG_BT_COEXIST
void rtl8192f_download_BTCoex_AP_mode_rsvd_page(PADAPTER padapter);
#endif /* CONFIG_BT_COEXIST */
#ifdef CONFIG_P2P
void rtl8192f_set_p2p_ps_offload_cmd(PADAPTER padapter, u8 p2p_ps_state);
#endif /* CONFIG_P2P */

#ifdef CONFIG_P2P_WOWLAN
void rtl8192f_set_p2p_wowlan_offload_cmd(PADAPTER padapter);
#endif

/*	AP_REQ_TXREP_CMD 0x43	*/
#define SET_8192F_H2CCMD_TXREP_PARM_STA1(__pH2CCmd, __Value)			SET_BITS_TO_LE_1BYTE(__pH2CCmd, 0, 8, __Value)
#define SET_8192F_H2CCMD_TXREP_PARM_STA2(__pH2CCmd, __Value)			SET_BITS_TO_LE_1BYTE((__pH2CCmd)+1, 0, 8, __Value)
#define SET_8192F_H2CCMD_TXREP_PARM_RTY(__pH2CCmd, __Value)			SET_BITS_TO_LE_1BYTE((__pH2CCmd)+2, 0, 2, __Value)

/*		C2H_AP_REQ_TXRPT		*/
#define	GET_8192F_C2H_TC2H_APREQ_TXRPT_MACID1(_Header)				LE_BITS_TO_1BYTE((_Header + 0), 0, 8)
#define	GET_8192F_C2H_TC2H_APREQ_TXRPT_TXOK1(_Header)				LE_BITS_TO_2BYTE((_Header + 1), 0, 16)
#define	GET_8192F_C2H_TC2H_APREQ_TXRPT_TXFAIL1(_Header)				LE_BITS_TO_2BYTE((_Header + 3), 0, 16)
#define	GET_8192F_C2H_TC2H_APREQ_TXRPT_INIRATE1(_Header)			LE_BITS_TO_1BYTE((_Header + 5), 0, 8)
#define	GET_8192F_C2H_TC2H_APREQ_TXRPT_MACID2(_Header)				LE_BITS_TO_1BYTE((_Header + 6), 0, 8)
#define	GET_8192F_C2H_TC2H_APREQ_TXRPT_TXOK2(_Header)				LE_BITS_TO_2BYTE((_Header + 7), 0, 16)
#define	GET_8192F_C2H_TC2H_APREQ_TXRPT_TXFAIL2(_Header)				LE_BITS_TO_2BYTE((_Header + 9), 0, 16)
#define	GET_8192F_C2H_TC2H_APREQ_TXRPT_INIRATE2(_Header)			LE_BITS_TO_1BYTE((_Header + 11), 0, 8)

/*		C2H_SPC_STAT			*/
#define	GET_8192F_C2H_SPC_STAT_IDX(_Header)					LE_BITS_TO_1BYTE((_Header + 0), 0, 8)
	/*	Tip :TYPE_A data3 is msb and data0 is lsb	*/
#define	GET_8192F_C2H_SPC_STAT_TYPEA_RETRY(_Header)				LE_BITS_TO_4BYTE((_Header + 1), 0, 32)
#define	GET_8192F_C2H_SPC_STAT_TYPEB_PKT1(_Header)				LE_BITS_TO_2BYTE((_Header + 1), 0, 16)
#define	GET_8192F_C2H_SPC_STAT_TYPEB_RETRY1(_Header)				LE_BITS_TO_2BYTE((_Header + 3), 0, 16)
#define	GET_8192F_C2H_SPC_STAT_TYPEB_PKT2(_Header)				LE_BITS_TO_2BYTE((_Header + 5), 0, 16)
#define	GET_8192F_C2H_SPC_STAT_TYPEB_RETRY2(_Header)				LE_BITS_TO_2BYTE((_Header + 7), 0, 16)

void rtl8192f_req_txrpt_cmd(PADAPTER, u8 macid);
s32 FillH2CCmd8192F(PADAPTER padapter, u8 ElementID, u32 CmdLen, u8 *pCmdBuffer);
u8 GetTxBufferRsvdPageNum8192F(_adapter *padapter, bool wowlan);
#endif
