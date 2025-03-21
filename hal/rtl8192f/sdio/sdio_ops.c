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
#define _SDIO_OPS_C_

#include <rtl8192f_hal.h>

/* #define SDIO_DEBUG_IO 1 */
#define CONFIG_RTW_SDIO_REG_FORCE_CMD52 0

/*
 * Description:
 *	The following mapping is for SDIO host local register space.
 *
 * Creadted by Roger, 2011.01.31.
 *   */
static void HalSdioGetCmdAddr8192FSdio(
		PADAPTER			padapter,
		u8				DeviceID,
		u32				Addr,
		u32				*pCmdAddr
)
{
	switch (DeviceID) {
	case SDIO_LOCAL_DEVICE_ID:
		*pCmdAddr = ((SDIO_LOCAL_DEVICE_ID << 13) | (Addr & SDIO_LOCAL_MSK));
		break;

	case WLAN_IOREG_DEVICE_ID:
		*pCmdAddr = ((WLAN_IOREG_DEVICE_ID << 13) | (Addr & WLAN_IOREG_MSK));
		break;

	case WLAN_TX_HIQ_DEVICE_ID:
		*pCmdAddr = ((WLAN_TX_HIQ_DEVICE_ID << 13) | (Addr & WLAN_FIFO_MSK));
		break;

	case WLAN_TX_MIQ_DEVICE_ID:
		*pCmdAddr = ((WLAN_TX_MIQ_DEVICE_ID << 13) | (Addr & WLAN_FIFO_MSK));
		break;

	case WLAN_TX_LOQ_DEVICE_ID:
		*pCmdAddr = ((WLAN_TX_LOQ_DEVICE_ID << 13) | (Addr & WLAN_FIFO_MSK));
		break;

	case WLAN_RX0FF_DEVICE_ID:
		*pCmdAddr = ((WLAN_RX0FF_DEVICE_ID << 13) | (Addr & WLAN_RX0FF_MSK));
		break;

	default:
		break;
	}
}

static u8 get_deviceid(u32 addr)
{
	u8 devideId;
	u16 pseudoId;


	pseudoId = (u16)(addr >> 16);
	switch (pseudoId) {
	case 0x1025:
		devideId = SDIO_LOCAL_DEVICE_ID;
		break;

	case 0x1026:
		devideId = WLAN_IOREG_DEVICE_ID;
		break;

	/*		case 0x1027:
	 *			devideId = SDIO_FIRMWARE_FIFO;
	 *			break; */

	case 0x1031:
		devideId = WLAN_TX_HIQ_DEVICE_ID;
		break;

	case 0x1032:
		devideId = WLAN_TX_MIQ_DEVICE_ID;
		break;

	case 0x1033:
		devideId = WLAN_TX_LOQ_DEVICE_ID;
		break;

	case 0x1034:
		devideId = WLAN_RX0FF_DEVICE_ID;
		break;

	default:
		/*			devideId = (u8)((addr >> 13) & 0xF); */
		devideId = WLAN_IOREG_DEVICE_ID;
		break;
	}

	return devideId;
}

/*
 * Ref:
 *	HalSdioGetCmdAddr8192FSdio()
 */
static u32 _cvrt2ftaddr(const u32 addr, u8 *pdeviceId, u16 *poffset)
{
	u8 deviceId;
	u16 offset;
	u32 ftaddr;


	deviceId = get_deviceid(addr);
	offset = 0;

	switch (deviceId) {
	case SDIO_LOCAL_DEVICE_ID:
		offset = addr & SDIO_LOCAL_MSK;
		break;

	case WLAN_TX_HIQ_DEVICE_ID:
	case WLAN_TX_MIQ_DEVICE_ID:
	case WLAN_TX_LOQ_DEVICE_ID:
		offset = addr & WLAN_FIFO_MSK;
		break;

	case WLAN_RX0FF_DEVICE_ID:
		offset = addr & WLAN_RX0FF_MSK;
		break;

	case WLAN_IOREG_DEVICE_ID:
	default:
		deviceId = WLAN_IOREG_DEVICE_ID;
		offset = addr & WLAN_IOREG_MSK;
		break;
	}
	ftaddr = (deviceId << 13) | offset;

	if (pdeviceId)
		*pdeviceId = deviceId;
	if (poffset)
		*poffset = offset;

	return ftaddr;
}

u8 sdio_read8(struct intf_hdl *pintfhdl, u32 addr)
{
	u32 ftaddr;
	u8 device_id;
	u16 offset;
	u8 val;
	u8 sus_leave = _FALSE;

	ftaddr = _cvrt2ftaddr(addr, &device_id, &offset);

	if (device_id == WLAN_IOREG_DEVICE_ID && offset < 0x100)
		sus_leave = sdio_chk_hci_resume(pintfhdl);

	if(device_id == WLAN_IOREG_DEVICE_ID){
		u8 bMacPwrCtrlOn;
		PADAPTER padapter = pintfhdl->padapter;
		bMacPwrCtrlOn = _FALSE;
		rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
		if(bMacPwrCtrlOn == _FALSE){
			#ifdef CONFIG_SDIO_INDIRECT_ACCESS
			if(offset < 0x100)
				val = sdio_iread8(pintfhdl, ftaddr);
			else
			#endif/*CONFIG_SDIO_INDIRECT_ACCESS*/
				val = sd_read8(pintfhdl, ftaddr, NULL);
		} else {
			#if(!CONFIG_RTW_SDIO_REG_FORCE_CMD52)
			if(rtw_is_sdio30(padapter))
				val = (u8)cmd53_4byte_alignment(pintfhdl, ftaddr);
			else
			#endif /*not CONFIG_RTW_SDIO_REG_FORCE_CMD52*/
				val = sd_read8(pintfhdl, ftaddr, NULL);
		}
	}else
		val = sd_read8(pintfhdl, ftaddr, NULL);

	if (sus_leave == _TRUE)
		sdio_chk_hci_suspend(pintfhdl);

	return val;
}

u16 sdio_read16(struct intf_hdl *pintfhdl, u32 addr)
{
	u32 ftaddr;
	u8 device_id;
	u16 offset;
	u16 val;
	u8 sus_leave = _FALSE;

	ftaddr = _cvrt2ftaddr(addr, &device_id, &offset);

	if (device_id == WLAN_IOREG_DEVICE_ID && offset < 0x100)
		sus_leave = sdio_chk_hci_resume(pintfhdl);

	val = 0;
	if (device_id == WLAN_IOREG_DEVICE_ID) {
		u8 bMacPwrCtrlOn;
		PADAPTER padapter = pintfhdl->padapter;
		bMacPwrCtrlOn = _FALSE;
		rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
		if(bMacPwrCtrlOn == _FALSE ){
			#ifdef CONFIG_SDIO_INDIRECT_ACCESS
			if(offset < 0x100)
				val = sdio_iread16(pintfhdl, ftaddr);
			else
			#endif /*CONFIG_SDIO_INDIRECT_ACCESS*/
				sd_cmd52_read(pintfhdl, ftaddr, 2, (u8 *)&val);
		} else {
			#if(!CONFIG_RTW_SDIO_REG_FORCE_CMD52)
			if(rtw_is_sdio30(padapter))
				val = (u16)cmd53_4byte_alignment(pintfhdl, ftaddr);
			else
			#endif /*not CONFIG_RTW_SDIO_REG_FORCE_CMD52*/
				sd_cmd52_read(pintfhdl, ftaddr, 2, (u8 *)&val);
		}
	} else
		sd_cmd52_read(pintfhdl, ftaddr, 2, (u8 *)&val);
	val = le16_to_cpu(val);

	if (sus_leave == _TRUE)
		sdio_chk_hci_suspend(pintfhdl);

	return val;
}

u32 sdio_read32(struct intf_hdl *pintfhdl, u32 addr)
{
	PADAPTER padapter;
	u8 bMacPwrCtrlOn;
	u8 deviceId;
	u16 offset;
	u32 ftaddr;
	u8 shift;
	u32 val;
	s32 err;


	padapter = pintfhdl->padapter;
	ftaddr = _cvrt2ftaddr(addr, &deviceId, &offset);

	bMacPwrCtrlOn = _FALSE;
	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (((deviceId == WLAN_IOREG_DEVICE_ID) && (offset < 0x100))
		|| (bMacPwrCtrlOn == _FALSE)
		#ifdef CONFIG_LPS_LCLK
		|| (adapter_to_pwrctl(padapter)->bFwCurrentInPSMode == _TRUE)
		#endif
		|| CONFIG_RTW_SDIO_REG_FORCE_CMD52
	) {
		u8 sus_leave = _FALSE;

		if (deviceId == WLAN_IOREG_DEVICE_ID && offset < 0x100)
			sus_leave = sdio_chk_hci_resume(pintfhdl);

		val = 0;
		if(bMacPwrCtrlOn == _FALSE ) {
			#ifdef CONFIG_SDIO_INDIRECT_ACCESS
			if(offset < 0x100)
				err = sdio_iread(padapter, ftaddr, 4, (u8 *)&val);
			else
			#endif /*CONFIG_SDIO_INDIRECT_ACCESS*/
				err = sd_cmd52_read(pintfhdl, ftaddr, 4, (u8 *)&val);
		} else {
			#if(!CONFIG_RTW_SDIO_REG_FORCE_CMD52)
				shift = ftaddr & 0x3;
				if (shift == 0)
					val = sd_read32(pintfhdl, ftaddr, &err);
				else
					RTW_ERR("%s: no 4 byte aligment, Read FAIL! addr=0x%x\n", __func__, addr);
			#else
				err = sd_cmd52_read(pintfhdl, ftaddr, 4, (u8 *)&val);
			#endif /*not CONFIG_RTW_SDIO_REG_FORCE_CMD52*/
		}

		if (sus_leave == _TRUE)
			sdio_chk_hci_suspend(pintfhdl);

#ifdef SDIO_DEBUG_IO
		if (!err) {
#endif
			val = le32_to_cpu(val);
			return val;
#ifdef SDIO_DEBUG_IO
		}

		RTW_ERR("%s: Mac Power off, Read FAIL(%d)! addr=0x%x\n", __func__, err, addr);
		return SDIO_ERR_VAL32;
#endif
	}

	/* 4 bytes alignment */
	shift = ftaddr & 0x3;
	if (shift == 0)
		val = sd_read32(pintfhdl, ftaddr, NULL);
	else {
		u8 *ptmpbuf;

		ptmpbuf = (u8 *)rtw_malloc(8);
		if (NULL == ptmpbuf) {
			RTW_ERR("%s: Allocate memory FAIL!(size=8) addr=0x%x\n", __func__, addr);
			return SDIO_ERR_VAL32;
		}

		ftaddr &= ~(u16)0x3;
		err = sd_read(pintfhdl, ftaddr, 8, ptmpbuf);
		if (err)
			return SDIO_ERR_VAL32;
		_rtw_memcpy(&val, ptmpbuf + shift, 4);
		val = le32_to_cpu(val);

		rtw_mfree(ptmpbuf, 8);
	}


	return val;
}

s32 sdio_readN(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *pbuf)
{
	PADAPTER padapter;
	u8 bMacPwrCtrlOn;
	u8 deviceId;
	u16 offset;
	u32 ftaddr;
	u8 shift;
	s32 err;


	padapter = pintfhdl->padapter;
	err = 0;

	ftaddr = _cvrt2ftaddr(addr, &deviceId, &offset);

	bMacPwrCtrlOn = _FALSE;
	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (((deviceId == WLAN_IOREG_DEVICE_ID) && (offset < 0x100))
		|| (bMacPwrCtrlOn == _FALSE)
		#ifdef CONFIG_LPS_LCLK
		|| (adapter_to_pwrctl(padapter)->bFwCurrentInPSMode == _TRUE)
		#endif
		|| CONFIG_RTW_SDIO_REG_FORCE_CMD52) {
		u8 sus_leave = _FALSE;

		if (deviceId == WLAN_IOREG_DEVICE_ID && offset < 0x100)
			sus_leave = sdio_chk_hci_resume(pintfhdl);

		err = sd_cmd52_read(pintfhdl, ftaddr, cnt, pbuf);

		if (sus_leave == _TRUE)
			sdio_chk_hci_suspend(pintfhdl);

		return err;
	}

	/* 4 bytes alignment */
	shift = ftaddr & 0x3;
	if (shift == 0)
		err = sd_read(pintfhdl, ftaddr, cnt, pbuf);
	else {
		u8 *ptmpbuf;
		u32 n;

		ftaddr &= ~(u16)0x3;
		n = cnt + shift;
		ptmpbuf = rtw_malloc(n);
		if (NULL == ptmpbuf)
			return -1;
		err = sd_read(pintfhdl, ftaddr, n, ptmpbuf);
		if (!err)
			_rtw_memcpy(pbuf, ptmpbuf + shift, cnt);
		rtw_mfree(ptmpbuf, n);
	}


	return err;
}

s32 sdio_write8(struct intf_hdl *pintfhdl, u32 addr, u8 val)
{
	u32 ftaddr;
	u8 device_id;
	u16 offset;
	s32 err;
	u8 sus_leave = _FALSE;

	ftaddr = _cvrt2ftaddr(addr, &device_id, &offset);

	if (device_id == WLAN_IOREG_DEVICE_ID && offset < 0x100)
		sus_leave = sdio_chk_hci_resume(pintfhdl);

	err = 0;
	#ifdef CONFIG_SDIO_INDIRECT_ACCESS
	if(device_id == WLAN_IOREG_DEVICE_ID ) {
		u8 bMacPwrCtrlOn;
		PADAPTER padapter = pintfhdl->padapter;
		bMacPwrCtrlOn = _FALSE;
		rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
		if((bMacPwrCtrlOn == _FALSE) && (offset < 0x100))
			err = sdio_iwrite8(pintfhdl, ftaddr, val);
		else
			sd_write8(pintfhdl, ftaddr, val, &err);
	 } else
	 #endif /*CONFIG_SDIO_INDIRECT_ACCESS*/
		sd_write8(pintfhdl, ftaddr, val, &err);

	if (sus_leave == _TRUE)
		sdio_chk_hci_suspend(pintfhdl);

	return err;
}

s32 sdio_write16(struct intf_hdl *pintfhdl, u32 addr, u16 val)
{
	u32 ftaddr;
	u8 device_id;
	u16 offset;
	s32 err;
	u8 sus_leave = _FALSE;

	ftaddr = _cvrt2ftaddr(addr, &device_id, &offset);

	if (device_id == WLAN_IOREG_DEVICE_ID && offset < 0x100)
		sus_leave = sdio_chk_hci_resume(pintfhdl);

	val = cpu_to_le16(val);
	#ifdef CONFIG_SDIO_INDIRECT_ACCESS
	if (device_id == WLAN_IOREG_DEVICE_ID) {
		u8 bMacPwrCtrlOn;
		PADAPTER padapter = pintfhdl->padapter;
		bMacPwrCtrlOn = _FALSE;
		rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
		if((bMacPwrCtrlOn == _FALSE) && (offset < 0x100))
			err = sdio_iwrite16(pintfhdl, ftaddr, val);
		else
			err = sd_cmd52_write(pintfhdl, ftaddr, 2, (u8 *)&val);
	}else
	#endif /*CONFIG_SDIO_INDIRECT_ACCESS*/
		err = sd_cmd52_write(pintfhdl, ftaddr, 2, (u8 *)&val);

	if (sus_leave == _TRUE)
		sdio_chk_hci_suspend(pintfhdl);

	return err;
}

s32 sdio_write32(struct intf_hdl *pintfhdl, u32 addr, u32 val)
{
	PADAPTER padapter;
	u8 bMacPwrCtrlOn;
	u8 deviceId;
	u16 offset;
	u32 ftaddr;
	u8 shift;
	s32 err;


	padapter = pintfhdl->padapter;
	err = 0;

	ftaddr = _cvrt2ftaddr(addr, &deviceId, &offset);

	bMacPwrCtrlOn = _FALSE;
	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (((deviceId == WLAN_IOREG_DEVICE_ID) && (offset < 0x100))
		|| (bMacPwrCtrlOn == _FALSE)
		#ifdef CONFIG_LPS_LCLK
		|| (adapter_to_pwrctl(padapter)->bFwCurrentInPSMode == _TRUE)
		#endif
		|| CONFIG_RTW_SDIO_REG_FORCE_CMD52
	) {
		u8 sus_leave = _FALSE;

		if (deviceId == WLAN_IOREG_DEVICE_ID && offset < 0x100)
			sus_leave = sdio_chk_hci_resume(pintfhdl);

		val = cpu_to_le32(val);
		#ifdef CONFIG_SDIO_INDIRECT_ACCESS
		if((bMacPwrCtrlOn == _FALSE) && (offset < 0x100))
			err = sdio_iwrite32(pintfhdl, ftaddr, val);
		else
		#endif /*CONFIG_SDIO_INDIRECT_ACCESS*/
			err = sd_cmd52_write(pintfhdl, ftaddr, 4, (u8 *)&val);

		if (sus_leave == _TRUE)
			sdio_chk_hci_suspend(pintfhdl);

		return err;
	}

	/* 4 bytes alignment */
	shift = ftaddr & 0x3;
#if 1
	if (shift == 0)
		sd_write32(pintfhdl, ftaddr, val, &err);
	else {
		val = cpu_to_le32(val);
		err = sd_cmd52_write(pintfhdl, ftaddr, 4, (u8 *)&val);
	}
#else
	if (shift == 0)
		sd_write32(pintfhdl, ftaddr, val, &err);
	else {
		u8 *ptmpbuf;

		ptmpbuf = (u8 *)rtw_malloc(8);
		if (NULL == ptmpbuf)
			return -1;

		ftaddr &= ~(u16)0x3;
		err = sd_read(pintfhdl, ftaddr, 8, ptmpbuf);
		if (err) {
			rtw_mfree(ptmpbuf, 8);
			return err;
		}
		val = cpu_to_le32(val);
		_rtw_memcpy(ptmpbuf + shift, &val, 4);
		err = sd_write(pintfhdl, ftaddr, 8, ptmpbuf);

		rtw_mfree(ptmpbuf, 8);
	}
#endif


	return err;
}

s32 sdio_writeN(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *pbuf)
{
	PADAPTER padapter;
	u8 bMacPwrCtrlOn;
	u8 deviceId;
	u16 offset;
	u32 ftaddr;
	u8 shift;
	s32 err;


	padapter = pintfhdl->padapter;
	err = 0;

	ftaddr = _cvrt2ftaddr(addr, &deviceId, &offset);

	bMacPwrCtrlOn = _FALSE;
	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (((deviceId == WLAN_IOREG_DEVICE_ID) && (offset < 0x100))
		|| (bMacPwrCtrlOn == _FALSE)
		#ifdef CONFIG_LPS_LCLK
		|| (adapter_to_pwrctl(padapter)->bFwCurrentInPSMode == _TRUE)
		#endif
		|| CONFIG_RTW_SDIO_REG_FORCE_CMD52
	) {
		u8 sus_leave = _FALSE;

		if (deviceId == WLAN_IOREG_DEVICE_ID && offset < 0x100)
			sus_leave = sdio_chk_hci_resume(pintfhdl);

		err = sd_cmd52_write(pintfhdl, ftaddr, cnt, pbuf);

		if (sus_leave == _TRUE)
			sdio_chk_hci_suspend(pintfhdl);

		return err;
	}

	shift = ftaddr & 0x3;
	if (shift == 0)
		err = sd_write(pintfhdl, ftaddr, cnt, pbuf);
	else {
		u8 *ptmpbuf;
		u32 n;

		ftaddr &= ~(u16)0x3;
		n = cnt + shift;
		ptmpbuf = rtw_malloc(n);
		if (NULL == ptmpbuf)
			return -1;
		err = sd_read(pintfhdl, ftaddr, 4, ptmpbuf);
		if (err) {
			rtw_mfree(ptmpbuf, n);
			return err;
		}
		_rtw_memcpy(ptmpbuf + shift, pbuf, cnt);
		err = sd_write(pintfhdl, ftaddr, n, ptmpbuf);
		rtw_mfree(ptmpbuf, n);
	}


	return err;
}

u8 sdio_f0_read8(struct intf_hdl *pintfhdl, u32 addr)
{
	u32 ftaddr;
	u8 val;

	val = sd_f0_read8(pintfhdl, addr, NULL);


	return val;
}

void sdio_read_mem(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *rmem)
{
	s32 err;


	err = sdio_readN(pintfhdl, addr, cnt, rmem);

}

void sdio_write_mem(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *wmem)
{

	sdio_writeN(pintfhdl, addr, cnt, wmem);

}

/*
 * Description:
 *	Read from RX FIFO
 *	Round read size to block size,
 *	and make sure data transfer will be done in one command.
 *
 * Parameters:
 *	pintfhdl	a pointer of intf_hdl
 *	addr		port ID
 *	cnt			size to read
 *	rmem		address to put data
 *
 * Return:
 *	_SUCCESS(1)		Success
 *	_FAIL(0)		Fail
 */
static u32 sdio_read_port(
	struct intf_hdl *pintfhdl,
	u32 addr,
	u32 cnt,
	u8 *mem)
{
	PADAPTER padapter;
	PHAL_DATA_TYPE phal;
	u32 oldcnt;
#ifdef SDIO_DYNAMIC_ALLOC_MEM
	u8 *oldmem;
#endif
	s32 err;


	padapter = pintfhdl->padapter;
	phal = GET_HAL_DATA(padapter);

	HalSdioGetCmdAddr8192FSdio(padapter, addr, phal->SdioRxFIFOCnt++, &addr);

	oldcnt = cnt;
	cnt = rtw_sdio_cmd53_align_size(adapter_to_dvobj(padapter), cnt);

	if (oldcnt != cnt) {
#ifdef SDIO_DYNAMIC_ALLOC_MEM
		oldmem = mem;
		mem = rtw_malloc(cnt);
		if (mem == NULL) {
			RTW_WARN("%s: allocate memory %d bytes fail!\n",
				 __func__, cnt);
			mem = oldmem;
			oldmem == NULL;
		}
#else
		/* in this case, caller should gurante the buffer is big enough */
		/* to receive data after alignment */
#endif
	}

	err = _sd_read(pintfhdl, addr, cnt, mem);

#ifdef SDIO_DYNAMIC_ALLOC_MEM
	if ((oldcnt != cnt) && (oldmem)) {
		_rtw_memcpy(oldmem, mem, oldcnt);
		rtw_mfree(mem, cnt);
	}
#endif

	if (err)
		return _FAIL;
	return _SUCCESS;
}

/*
 * Description:
 *	Write to TX FIFO
 *	Align write size block size,
 *	and make sure data could be written in one command.
 *
 * Parameters:
 *	pintfhdl	a pointer of intf_hdl
 *	addr		port ID
 *	cnt			size to write
 *	wmem		data pointer to write
 *
 * Return:
 *	_SUCCESS(1)		Success
 *	_FAIL(0)		Fail
 */
static u32 sdio_write_port(
	struct intf_hdl *pintfhdl,
	u32 addr,
	u32 cnt,
	u8 *mem)
{
	PADAPTER padapter;
	s32 err;
	struct xmit_buf *xmitbuf = (struct xmit_buf *)mem;

	padapter = pintfhdl->padapter;

#ifndef CONFIG_DLFW_TXPKT
	if (!rtw_is_hw_init_completed(padapter)) {
		RTW_INFO("%s [addr=0x%x cnt=%d] hw_init_completed is FALSE\n",
			 __func__, addr, cnt);
		return _FAIL;
	}
#endif

	cnt = _RND4(cnt);
	HalSdioGetCmdAddr8192FSdio(padapter, addr, cnt >> 2, &addr);

	cnt = rtw_sdio_cmd53_align_size(adapter_to_dvobj(padapter), cnt);

	err = sd_write(pintfhdl, addr, cnt, xmitbuf->pdata);

	rtw_sctx_done_err(&xmitbuf->sctx,
		  err ? RTW_SCTX_DONE_WRITE_PORT_ERR : RTW_SCTX_DONE_SUCCESS);

	if (err)
		return _FAIL;
	return _SUCCESS;
}

void sdio_set_intf_ops(_adapter *padapter, struct _io_ops *pops)
{

	pops->_read8 = &sdio_read8;
	pops->_read16 = &sdio_read16;
	pops->_read32 = &sdio_read32;
	pops->_read_mem = &sdio_read_mem;
	pops->_read_port = &sdio_read_port;

	pops->_write8 = &sdio_write8;
	pops->_write16 = &sdio_write16;
	pops->_write32 = &sdio_write32;
	pops->_writeN = &sdio_writeN;
	pops->_write_mem = &sdio_write_mem;
	pops->_write_port = &sdio_write_port;

	pops->_sd_f0_read8 = sdio_f0_read8;
	#ifdef CONFIG_SDIO_INDIRECT_ACCESS
	pops->_sd_iread8 = sdio_iread8;
	pops->_sd_iread16 = sdio_iread16;
	pops->_sd_iread32 = sdio_iread32;
	pops->_sd_iwrite8 = sdio_iwrite8;
	pops->_sd_iwrite16 = sdio_iwrite16;
	pops->_sd_iwrite32 = sdio_iwrite32;
	#endif

}

/*
 * Todo: align address to 4 bytes.
 */
s32 _sdio_local_read(
	PADAPTER	padapter,
	u32			addr,
	u32			cnt,
	u8			*pbuf)
{
	struct intf_hdl *pintfhdl;
	u8 bMacPwrCtrlOn;
	s32 err;
	u8 *ptmpbuf;
	u32 n;


	pintfhdl = &padapter->iopriv.intf;

	HalSdioGetCmdAddr8192FSdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);

	bMacPwrCtrlOn = _FALSE;
	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (_FALSE == bMacPwrCtrlOn
		|| CONFIG_RTW_SDIO_REG_FORCE_CMD52) {
		err = _sd_cmd52_read(pintfhdl, addr, cnt, pbuf);
		return err;
	}

	n = RND4(cnt);
	ptmpbuf = (u8 *)rtw_malloc(n);
	if (!ptmpbuf)
		return -1;

	err = _sd_read(pintfhdl, addr, n, ptmpbuf);
	if (!err)
		_rtw_memcpy(pbuf, ptmpbuf, cnt);

	if (ptmpbuf)
		rtw_mfree(ptmpbuf, n);

	return err;
}

/*
 * Todo: align address to 4 bytes.
 */
s32 sdio_local_read(
	PADAPTER	padapter,
	u32			addr,
	u32			cnt,
	u8			*pbuf)
{
	struct intf_hdl *pintfhdl;
	u8 bMacPwrCtrlOn;
	s32 err;
	u8 *ptmpbuf;
	u32 n;

	pintfhdl = &padapter->iopriv.intf;

	HalSdioGetCmdAddr8192FSdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);

	bMacPwrCtrlOn = _FALSE;
	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if ((_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
		|| (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode)
#endif
		|| CONFIG_RTW_SDIO_REG_FORCE_CMD52
	   ) {
		err = sd_cmd52_read(pintfhdl, addr, cnt, pbuf);
		return err;
	}

	n = RND4(cnt);
	ptmpbuf = (u8 *)rtw_malloc(n);
	if (!ptmpbuf)
		return -1;

	err = sd_read(pintfhdl, addr, n, ptmpbuf);
	if (!err)
		_rtw_memcpy(pbuf, ptmpbuf, cnt);

	if (ptmpbuf)
		rtw_mfree(ptmpbuf, n);

	return err;
}

/*
 * Todo: align address to 4 bytes.
 */
s32 _sdio_local_write(
	PADAPTER	padapter,
	u32			addr,
	u32			cnt,
	u8			*pbuf)
{
	struct intf_hdl *pintfhdl;
	u8 bMacPwrCtrlOn;
	s32 err;
	u8 *ptmpbuf;

	if (addr & 0x3)
		RTW_INFO("%s, address must be 4 bytes alignment\n", __func__);

	if (cnt  & 0x3)
		RTW_INFO("%s, size must be the multiple of 4\n", __func__);

	pintfhdl = &padapter->iopriv.intf;

	HalSdioGetCmdAddr8192FSdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);

	bMacPwrCtrlOn = _FALSE;
	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if ((_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
		|| (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode)
#endif
		|| CONFIG_RTW_SDIO_REG_FORCE_CMD52
	   ) {
		err = _sd_cmd52_write(pintfhdl, addr, cnt, pbuf);
		return err;
	}

	ptmpbuf = (u8 *)rtw_malloc(cnt);
	if (!ptmpbuf)
		return -1;

	_rtw_memcpy(ptmpbuf, pbuf, cnt);

	err = _sd_write(pintfhdl, addr, cnt, ptmpbuf);

	if (ptmpbuf)
		rtw_mfree(ptmpbuf, cnt);

	return err;
}

/*
 * Todo: align address to 4 bytes.
 */
s32 sdio_local_write(
	PADAPTER	padapter,
	u32		addr,
	u32		cnt,
	u8		*pbuf)
{
	struct intf_hdl *pintfhdl;
	u8 bMacPwrCtrlOn;
	s32 err;
	u8 *ptmpbuf;

	if (addr & 0x3)
		RTW_INFO("%s, address must be 4 bytes alignment\n", __func__);

	if (cnt  & 0x3)
		RTW_INFO("%s, size must be the multiple of 4\n", __func__);

	pintfhdl = &padapter->iopriv.intf;

	HalSdioGetCmdAddr8192FSdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);

	bMacPwrCtrlOn = _FALSE;
	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if ((_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
		|| (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode)
#endif
		|| CONFIG_RTW_SDIO_REG_FORCE_CMD52
	   ) {
		err = sd_cmd52_write(pintfhdl, addr, cnt, pbuf);
		return err;
	}

	ptmpbuf = (u8 *)rtw_malloc(cnt);
	if (!ptmpbuf)
		return -1;

	_rtw_memcpy(ptmpbuf, pbuf, cnt);

	err = sd_write(pintfhdl, addr, cnt, ptmpbuf);

	if (ptmpbuf)
		rtw_mfree(ptmpbuf, cnt);

	return err;
}

u8 SdioLocalCmd52Read1Byte(PADAPTER padapter, u32 addr)
{
	u8 val = 0;
	struct intf_hdl *pintfhdl = &padapter->iopriv.intf;

	HalSdioGetCmdAddr8192FSdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);
	sd_cmd52_read(pintfhdl, addr, 1, &val);

	return val;
}

u16 SdioLocalCmd52Read2Byte(PADAPTER padapter, u32 addr)
{
	u16 val = 0;
	struct intf_hdl *pintfhdl = &padapter->iopriv.intf;

	HalSdioGetCmdAddr8192FSdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);
	sd_cmd52_read(pintfhdl, addr, 2, (u8 *)&val);

	val = le16_to_cpu(val);

	return val;
}

u32 SdioLocalCmd52Read4Byte(PADAPTER padapter, u32 addr)
{
	u32 val = 0;
	struct intf_hdl *pintfhdl = &padapter->iopriv.intf;

	HalSdioGetCmdAddr8192FSdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);
	sd_cmd52_read(pintfhdl, addr, 4, (u8 *)&val);

	val = le32_to_cpu(val);

	return val;
}

u32 SdioLocalCmd53Read4Byte(PADAPTER padapter, u32 addr)
{

	u8 bMacPwrCtrlOn;
	u32 val = 0;
	struct intf_hdl *pintfhdl = &padapter->iopriv.intf;

	HalSdioGetCmdAddr8192FSdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);
	bMacPwrCtrlOn = _FALSE;
	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if ((_FALSE == bMacPwrCtrlOn)
#ifdef CONFIG_LPS_LCLK
		|| (_TRUE == adapter_to_pwrctl(padapter)->bFwCurrentInPSMode)
#endif
		|| CONFIG_RTW_SDIO_REG_FORCE_CMD52
	   ) {
		sd_cmd52_read(pintfhdl, addr, 4, (u8 *)&val);
		val = le32_to_cpu(val);
	} else
		val = sd_read32(pintfhdl, addr, NULL);

	return val;
}

void SdioLocalCmd52Write1Byte(PADAPTER padapter, u32 addr, u8 v)
{
	struct intf_hdl *pintfhdl = &padapter->iopriv.intf;

	HalSdioGetCmdAddr8192FSdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);
	sd_cmd52_write(pintfhdl, addr, 1, &v);
}

void SdioLocalCmd52Write2Byte(PADAPTER padapter, u32 addr, u16 v)
{
	struct intf_hdl *pintfhdl = &padapter->iopriv.intf;

	HalSdioGetCmdAddr8192FSdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);
	v = cpu_to_le16(v);
	sd_cmd52_write(pintfhdl, addr, 2, (u8 *)&v);
}

void SdioLocalCmd52Write4Byte(PADAPTER padapter, u32 addr, u32 v)
{
	struct intf_hdl *pintfhdl = &padapter->iopriv.intf;
	HalSdioGetCmdAddr8192FSdio(padapter, SDIO_LOCAL_DEVICE_ID, addr, &addr);
	v = cpu_to_le32(v);
	sd_cmd52_write(pintfhdl, addr, 4, (u8 *)&v);
}

#if 0
void
DumpLoggedInterruptHistory8192Sdio(
	PADAPTER		padapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	u32				DebugLevel = DBG_LOUD;

	if (DBG_Var.DbgPrintIsr == 0)
		return;

	DBG_ChkDrvResource(padapter);



}

void
LogInterruptHistory8192Sdio(
	PADAPTER			padapter,
	PRT_ISR_CONTENT	pIsrContent
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);

	if ((pHalData->IntrMask[0] & SDIO_HIMR_RX_REQUEST_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_RX_REQUEST))
		pHalData->InterruptLog.nISR_RX_REQUEST++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_AVAL_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_AVAL))
		pHalData->InterruptLog.nISR_AVAL++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_TXERR_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_TXERR))
		pHalData->InterruptLog.nISR_TXERR++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_RXERR_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_RXERR))
		pHalData->InterruptLog.nISR_RXERR++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_TXFOVW_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_TXFOVW))
		pHalData->InterruptLog.nISR_TXFOVW++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_RXFOVW_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_RXFOVW))
		pHalData->InterruptLog.nISR_RXFOVW++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_TXBCNOK_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_TXBCNOK))
		pHalData->InterruptLog.nISR_TXBCNOK++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_TXBCNERR_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_TXBCNERR))
		pHalData->InterruptLog.nISR_TXBCNERR++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_BCNERLY_INT_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_BCNERLY_INT))
		pHalData->InterruptLog.nISR_BCNERLY_INT++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_C2HCMD_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_C2HCMD))
		pHalData->InterruptLog.nISR_C2HCMD++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_CPWM1_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_CPWM1))
		pHalData->InterruptLog.nISR_CPWM1++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_CPWM2_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_CPWM2))
		pHalData->InterruptLog.nISR_CPWM2++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_HSISR_IND_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_HSISR_IND))
		pHalData->InterruptLog.nISR_HSISR_IND++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_GTINT3_IND_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_GTINT3_IND))
		pHalData->InterruptLog.nISR_GTINT3_IND++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_GTINT4_IND_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_GTINT4_IND))
		pHalData->InterruptLog.nISR_GTINT4_IND++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_PSTIMEOUT_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_PSTIMEOUT))
		pHalData->InterruptLog.nISR_PSTIMEOUT++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_OCPINT_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_OCPINT))
		pHalData->InterruptLog.nISR_OCPINT++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_ATIMEND_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_ATIMEND))
		pHalData->InterruptLog.nISR_ATIMEND++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_ATIMEND_E_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_ATIMEND_E))
		pHalData->InterruptLog.nISR_ATIMEND_E++;
	if ((pHalData->IntrMask[0] & SDIO_HIMR_CTWEND_MSK) &&
	    (pIsrContent->IntArray[0] & SDIO_HISR_CTWEND))
		pHalData->InterruptLog.nISR_CTWEND++;

}

void
DumpHardwareProfile8192Sdio(
		PADAPTER		padapter
)
{
	DumpLoggedInterruptHistory8192Sdio(padapter);
}
#endif

static s32 ReadInterrupt8192FSdio(PADAPTER padapter, u32 *phisr)
{
	u32 hisr, himr;
	u8 val8, hisr_len;


	if (phisr == NULL)
		return _FALSE;

	himr = GET_HAL_DATA(padapter)->sdio_himr;

	/* decide how many bytes need to be read */
	hisr_len = 0;
	while (himr) {
		hisr_len++;
		himr >>= 8;
	}

	hisr = 0;
	while (hisr_len != 0) {
		hisr_len--;
		val8 = SdioLocalCmd52Read1Byte(padapter, SDIO_REG_HISR + hisr_len);
		hisr |= (val8 << (8 * hisr_len));
	}

	*phisr = hisr;

	return _TRUE;
}

/*
 *	Description:
 *		Initialize SDIO Host Interrupt Mask configuration variables for future use.
 *
 *	Assumption:
 *		Using SDIO Local register ONLY for configuration.
 *
 *	Created by Roger, 2011.02.11.
 *   */
void InitInterrupt8192FSdio(PADAPTER padapter)
{
	PHAL_DATA_TYPE pHalData;


	pHalData = GET_HAL_DATA(padapter);
	pHalData->sdio_himr = (u32)(
				      SDIO_HIMR_RX_REQUEST_MSK			|
#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
				      SDIO_HIMR_AVAL_MSK					|
#endif
				      /*								SDIO_HIMR_TXERR_MSK				|
				       *								SDIO_HIMR_RXERR_MSK				|
				       *								SDIO_HIMR_TXFOVW_MSK				|
				       *								SDIO_HIMR_RXFOVW_MSK				|
				       *								SDIO_HIMR_TXBCNOK_MSK				|
				       *								SDIO_HIMR_TXBCNERR_MSK			|
				       *								SDIO_HIMR_BCNERLY_INT_MSK			|
				       *								SDIO_HIMR_C2HCMD_MSK				| */
#if defined(CONFIG_LPS_LCLK) && !defined(CONFIG_DETECT_CPWM_BY_POLLING)
				      SDIO_HIMR_CPWM1_MSK				|
				      /*								SDIO_HIMR_CPWM2_MSK				| */
#endif /* CONFIG_LPS_LCLK && !CONFIG_DETECT_CPWM_BY_POLLING
 *								SDIO_HIMR_HSISR_IND_MSK			|
 *								SDIO_HIMR_GTINT3_IND_MSK			|
 *								SDIO_HIMR_GTINT4_IND_MSK			|
 *								SDIO_HIMR_PSTIMEOUT_MSK			|
 *								SDIO_HIMR_OCPINT_MSK				|
 *								SDIO_HIMR_ATIMEND_MSK				|
 *								SDIO_HIMR_ATIMEND_E_MSK			|
 *								SDIO_HIMR_CTWEND_MSK				| */
				      0);
}

/*
 *	Description:
 *		Initialize System Host Interrupt Mask configuration variables for future use.
 *
 *	Created by Roger, 2011.08.03.
 *   */
void InitSysInterrupt8192FSdio(PADAPTER padapter)
{
	PHAL_DATA_TYPE pHalData;


	pHalData = GET_HAL_DATA(padapter);

	pHalData->SysIntrMask = (
					/*							HSIMR_GPIO12_0_INT_EN			|
					 *							HSIMR_SPS_OCP_INT_EN			|
					 *							HSIMR_RON_INT_EN				|
					 *							HSIMR_PDNINT_EN				|
					 *							HSIMR_GPIO9_INT_EN				| */
					0);
}

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
/*
 *	Description:
 *		Clear corresponding SDIO Host ISR interrupt service.
 *
 *	Assumption:
 *		Using SDIO Local register ONLY for configuration.
 *
 *	Created by Roger, 2011.02.11.
 *   */
void ClearInterrupt8192FSdio(PADAPTER padapter)
{
	PHAL_DATA_TYPE pHalData;
	u8 *clear;
	u8 val_8 = 0;
	u32 val_32 = 0;

	if (rtw_is_surprise_removed(padapter))
		return;

	pHalData = GET_HAL_DATA(padapter);
	clear = rtw_zmalloc(4);

	/* Clear corresponding HISR Content if needed */
	val_8 = SdioLocalCmd52Read1Byte(padapter, SDIO_REG_HISR + 2);

	val_32 = pHalData->sdio_hisr & MASK_SDIO_HISR_CLEAR;

	if (val_8 && BIT3)
		*(u32 *)clear = cpu_to_le32(val_32 | SDIO_HISR_CPWM2);
	else
		*(u32 *)clear = cpu_to_le32(val_32);

	if (*(u32 *)clear) {
		/* Perform write one clear operation */
		sdio_local_write(padapter, SDIO_REG_HISR, 4, clear);
	}

	rtw_mfree(clear, 4);
}
#endif

/*
 *	Description:
 *		Clear corresponding system Host ISR interrupt service.
 *
 *
 *	Created by Roger, 2011.02.11.
 *   */
void ClearSysInterrupt8192FSdio(PADAPTER padapter)
{
	PHAL_DATA_TYPE pHalData;
	u32 clear;


	if (rtw_is_surprise_removed(padapter))
		return;

	pHalData = GET_HAL_DATA(padapter);

	/* Clear corresponding HISR Content if needed */
	clear = pHalData->SysIntrStatus & MASK_HSISR_CLEAR;
	if (clear) {
		/* Perform write one clear operation */
		rtw_write32(padapter, REG_HSISR, clear);
	}
}

/*
 *	Description:
 *		Enalbe SDIO Host Interrupt Mask configuration on SDIO local domain.
 *
 *	Assumption:
 *		1. Using SDIO Local register ONLY for configuration.
 *		2. PASSIVE LEVEL
 *
 *	Created by Roger, 2011.02.11.
 *   */
void EnableInterrupt8192FSdio(PADAPTER padapter)
{
	PHAL_DATA_TYPE pHalData;
	u32 himr;

	pHalData = GET_HAL_DATA(padapter);

	himr = cpu_to_le32(pHalData->sdio_himr);
	sdio_local_write(padapter, SDIO_REG_HIMR, 4, (u8 *)&himr);


	/* Update current system IMR settings */
	himr = rtw_read32(padapter, REG_HSIMR);
	rtw_write32(padapter, REG_HSIMR, himr | pHalData->SysIntrMask);


	/* */
	/* <Roger_Notes> There are some C2H CMDs have been sent before system interrupt is enabled, e.g., C2H, CPWM. */
	/* So we need to clear all C2H events that FW has notified, otherwise FW won't schedule any commands anymore. */
	/* 2011.10.19. */
	/* */
	rtw_write8(padapter, REG_C2HEVT_CLEAR, C2H_EVT_HOST_CLOSE);
}

/*
 *	Description:
 *		Disable SDIO Host IMR configuration to mask unnecessary interrupt service.
 *
 *	Assumption:
 *		Using SDIO Local register ONLY for configuration.
 *
 *	Created by Roger, 2011.02.11.
 *   */
void DisableInterrupt8192FSdio(PADAPTER padapter)
{
	u32 himr;

	himr = cpu_to_le32(SDIO_HIMR_DISABLED);
	sdio_local_write(padapter, SDIO_REG_HIMR, 4, (u8 *)&himr);

}

/*
 *	Description:
 *		Using 0x100 to check the power status of FW.
 *
 *	Assumption:
 *		Using SDIO Local register ONLY for configuration.
 *
 *	Created by Isaac, 2013.09.10.
 *   */
u8 CheckIPSStatus(PADAPTER padapter)
{
	RTW_INFO("%s(): Read 0x100=0x%02x 0x86=0x%02x\n", __func__,
		 rtw_read8(padapter, 0x100), rtw_read8(padapter, 0x86));

	if (rtw_read8(padapter, 0x100) == 0xEA)
		return _TRUE;
	else
		return _FALSE;
}

#ifdef CONFIG_WOWLAN
void DisableInterruptButCpwm28192FSdio(PADAPTER padapter)
{
	u32 himr, tmp;

	sdio_local_read(padapter, SDIO_REG_HIMR, 4, (u8 *)&tmp);
	RTW_INFO("DisableInterruptButCpwm28192FSdio(): Read SDIO_REG_HIMR: 0x%08x\n", tmp);

	himr = cpu_to_le32(SDIO_HIMR_DISABLED) | SDIO_HIMR_CPWM2_MSK;
	sdio_local_write(padapter, SDIO_REG_HIMR, 4, (u8 *)&himr);

	sdio_local_read(padapter, SDIO_REG_HIMR, 4, (u8 *)&tmp);
	RTW_INFO("DisableInterruptButCpwm28192FSdio(): Read again SDIO_REG_HIMR: 0x%08x\n", tmp);
}
#endif /* CONFIG_WOWLAN
 *
 *	Description:
 *		Update SDIO Host Interrupt Mask configuration on SDIO local domain.
 *
 *	Assumption:
 *		1. Using SDIO Local register ONLY for configuration.
 *		2. PASSIVE LEVEL
 *
 *	Created by Roger, 2011.02.11.
 *   */
void UpdateInterruptMask8192FSdio(PADAPTER padapter, u32 AddMSR, u32 RemoveMSR)
{
	HAL_DATA_TYPE *pHalData;

	pHalData = GET_HAL_DATA(padapter);

	if (AddMSR)
		pHalData->sdio_himr |= AddMSR;

	if (RemoveMSR)
		pHalData->sdio_himr &= (~RemoveMSR);

	DisableInterrupt8192FSdio(padapter);
	EnableInterrupt8192FSdio(padapter);
}

#ifdef CONFIG_MAC_LOOPBACK_DRIVER
static void sd_recv_loopback(PADAPTER padapter, u32 size)
{
	PLOOPBACKDATA ploopback;
	u32 readsize, allocsize;
	u8 *preadbuf;


	readsize = size;
	RTW_INFO("%s: read size=%d\n", __func__, readsize);
	allocsize = _RND(readsize, rtw_sdio_get_block_size(adapter_to_dvobj(padapter)));

	ploopback = padapter->ploopback;
	if (ploopback) {
		ploopback->rxsize = readsize;
		preadbuf = ploopback->rxbuf;
	} else {
		preadbuf = rtw_malloc(allocsize);
		if (preadbuf == NULL) {
			RTW_INFO("%s: malloc fail size=%d\n", __func__, allocsize);
			return;
		}
	}

	/*	rtw_read_port(padapter, WLAN_RX0FF_DEVICE_ID, readsize, preadbuf); */
	sdio_read_port(&padapter->iopriv.intf, WLAN_RX0FF_DEVICE_ID, readsize, preadbuf);

	if (ploopback)
		_rtw_up_sema(&ploopback->sema);
	else {
		u32 i;

		RTW_INFO("%s: drop pkt\n", __func__);
		for (i = 0; i < readsize; i += 4) {
			RTW_INFO("%08X", *(u32 *)(preadbuf + i));
			if ((i + 4) & 0x1F)
				printk(KERN_ERR " ");
			else
				printk(KERN_ERR "\n");
		}
		printk(KERN_ERR "\n");
		rtw_mfree(preadbuf, allocsize);
	}
}
#endif /* CONFIG_MAC_LOOPBACK_DRIVER */

#ifdef CONFIG_SDIO_RX_COPY
static u32 sd_recv_rxfifo(PADAPTER padapter, u32 size,
			  struct recv_buf **recvbuf_ret)
{
	u32 readsize, ret;
	u8 *preadbuf;
	struct recv_priv *precvpriv;
	struct recv_buf	*precvbuf;

	*recvbuf_ret = NULL;

#if 0
	readsize = size;
#else
	/* Patch for some SDIO Host 4 bytes issue */
	/* ex. RK3188 */
	readsize = RND4(size);
#endif

	/* 3 1. alloc recvbuf */
	precvpriv = &padapter->recvpriv;
	precvbuf = rtw_dequeue_recvbuf(&precvpriv->free_recv_buf_queue);
	if (precvbuf == NULL) {
		if(0)
			RTW_INFO("%s: recvbuf unavailable\n", __func__);
		ret = RTW_RBUF_UNAVAIL;
		goto exit;
	}

	/* 3 2. alloc skb */
	if (precvbuf->pskb == NULL) {
		SIZE_PTR tmpaddr = 0;
		SIZE_PTR alignment = 0;

		precvbuf->pskb = rtw_skb_alloc(MAX_RECVBUF_SZ + RECVBUFF_ALIGN_SZ);
		if (precvbuf->pskb == NULL) {
			RTW_INFO("%s: alloc_skb fail! read=%d\n", __func__, readsize);
			rtw_enqueue_recvbuf(precvbuf, &precvpriv->free_recv_buf_queue);
			ret = RTW_RBUF_PKT_UNAVAIL;
			goto exit;
		}

		precvbuf->pskb->dev = padapter->pnetdev;

		tmpaddr = (SIZE_PTR)precvbuf->pskb->data;
		alignment = tmpaddr & (RECVBUFF_ALIGN_SZ - 1);
		skb_reserve(precvbuf->pskb, (RECVBUFF_ALIGN_SZ - alignment));
	}

	/* 3 3. read data from rxfifo */
	preadbuf = precvbuf->pskb->data;
	/*	rtw_read_port(padapter, WLAN_RX0FF_DEVICE_ID, readsize, preadbuf); */
	ret = sdio_read_port(&padapter->iopriv.intf, WLAN_RX0FF_DEVICE_ID, readsize, preadbuf);
	if (ret == _FAIL) {
		rtw_enqueue_recvbuf(precvbuf, &precvpriv->free_recv_buf_queue);
		goto exit;
	}

	/* 3 4. init recvbuf */
	precvbuf->len = size;
	precvbuf->phead = precvbuf->pskb->head;
	precvbuf->pdata = precvbuf->pskb->data;
	skb_set_tail_pointer(precvbuf->pskb, size);
	precvbuf->ptail = skb_tail_pointer(precvbuf->pskb);
	precvbuf->pend = skb_end_pointer(precvbuf->pskb);

	*recvbuf_ret = precvbuf;
exit:
	return ret;
}
#else /* !CONFIG_SDIO_RX_COPY */
static struct recv_buf *sd_recv_rxfifo(PADAPTER padapter, u32 size)
{
	u32 readsize, allocsize, ret;
	u8 *preadbuf;
	_pkt *ppkt;
	struct recv_priv *precvpriv;
	struct recv_buf	*precvbuf;


#if 0
	readsize = size;
#else
	/* Patch for some SDIO Host 4 bytes issue */
	/* ex. RK3188 */
	readsize = RND4(size);
#endif

	/* 3 1. alloc skb */
	/* align to block size */
	allocsize = rtw_sdio_cmd53_align_size(adapter_to_dvobj(padapter), readsize);

	ppkt = rtw_skb_alloc(allocsize);

	if (ppkt == NULL) {
		return NULL;
	}

	/* 3 2. read data from rxfifo */
	preadbuf = skb_put(ppkt, size);
	/*	rtw_read_port(padapter, WLAN_RX0FF_DEVICE_ID, readsize, preadbuf); */
	ret = sdio_read_port(&padapter->iopriv.intf, WLAN_RX0FF_DEVICE_ID, readsize, preadbuf);
	if (ret == _FAIL) {
		rtw_skb_free(ppkt);
		return NULL;
	}

	/* 3 3. alloc recvbuf */
	precvpriv = &padapter->recvpriv;
	precvbuf = rtw_dequeue_recvbuf(&precvpriv->free_recv_buf_queue);
	if (precvbuf == NULL) {
		rtw_skb_free(ppkt);
		RTW_ERR("%s: alloc recvbuf FAIL!\n", __func__);
		return NULL;
	}

	/* 3 4. init recvbuf */
	precvbuf->pskb = ppkt;

	precvbuf->len = ppkt->len;

	precvbuf->phead = ppkt->head;
	precvbuf->pdata = ppkt->data;
	precvbuf->ptail = skb_tail_pointer(precvbuf->pskb);
	precvbuf->pend = skb_end_pointer(precvbuf->pskb);

	return precvbuf;
}
#endif /* !CONFIG_SDIO_RX_COPY */

static void sd_rxhandler(PADAPTER padapter, struct recv_buf *precvbuf)
{
	struct recv_priv *precvpriv;
	_queue *ppending_queue;


	precvpriv = &padapter->recvpriv;
	ppending_queue = &precvpriv->recv_buf_pending_queue;

	/* 3 1. enqueue recvbuf */
	rtw_enqueue_recvbuf(precvbuf, ppending_queue);

	/* 3 2. trigger recv hdl */
#ifdef CONFIG_RECV_THREAD_MODE
	_rtw_up_sema(&precvpriv->recv_sema);
#else
	#ifdef PLATFORM_LINUX
	tasklet_schedule(&precvpriv->recv_tasklet);
	#endif /* PLATFORM_LINUX */
#endif /* CONFIG_RECV_THREAD_MODE */
}
#ifdef CONFIG_RECV_THREAD_MODE
static u32 sdio_recv_and_drop(PADAPTER adapter, u32 size)
{
	struct dvobj_priv *d;
	u32 readsz, bufsz;
	u8 *rbuf;
	s32 ret = _SUCCESS;

	 RTW_INFO("%s\n", __FUNCTION__);
	d = adapter_to_dvobj(adapter);

	/*
	 * Patch for some SDIO Host 4 bytes issue
	 * ex. RK3188
	 */
	readsz = RND4(size);

	/* round to block size */
	bufsz = rtw_sdio_cmd53_align_size(d, readsz);

	rbuf = rtw_zmalloc(bufsz);
	if (NULL == rbuf) {
		ret = _FAIL;
		goto _exit;
	}

	ret = sdio_read_port(&adapter->iopriv.intf,WLAN_RX0FF_DEVICE_ID, bufsz, rbuf);
	if (_FAIL == ret)
		RTW_ERR("%s: read port FAIL!\n", __FUNCTION__);

	if (NULL != rbuf)
		rtw_mfree(rbuf, bufsz);

_exit:
	return ret;
}
#endif/*CONFIG_RECV_THREAD_MODE*/
void sd_int_dpc(PADAPTER padapter)
{
	PHAL_DATA_TYPE phal;
	struct dvobj_priv *dvobj;
	struct intf_hdl *pintfhdl = &padapter->iopriv.intf;
	struct pwrctrl_priv *pwrctl;


	phal = GET_HAL_DATA(padapter);
	dvobj = adapter_to_dvobj(padapter);
	pwrctl = dvobj_to_pwrctl(dvobj);

#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
		if (phal->sdio_hisr & SDIO_HISR_AVAL) {
		/* _irqL irql; */
		u8	freepage[8];

		_sdio_local_read(padapter, SDIO_REG_FREE_TXPG1_8192F, 8, freepage);
		#ifdef DBG_TX_FREE_PAGE
		RTW_INFO("SDIO_HISR_AVAL, Tx Free Page = H:%u, M:%u, L:%u, P:%u\n",
			freepage[0], freepage[2], freepage[4], freepage[6]);
		#endif
		_rtw_up_sema(&(padapter->xmitpriv.xmit_sema));
	}
#endif
	if (phal->sdio_hisr & SDIO_HISR_CPWM1) {
		struct reportpwrstate_parm report;

#ifdef CONFIG_LPS_RPWM_TIMER
		_cancel_timer_ex(&(pwrctl->pwr_rpwm_timer));
#endif /* CONFIG_LPS_RPWM_TIMER */

		report.state = SdioLocalCmd52Read1Byte(padapter, SDIO_REG_HCPWM1_8192F);

#ifdef CONFIG_LPS_LCLK
		/* cpwm_int_hdl(padapter, &report); */
		_set_workitem(&(pwrctl->cpwm_event));
#endif
	}

	if (phal->sdio_hisr & SDIO_HISR_TXERR) {
		u8 *status;
		u32 addr;

		status = rtw_malloc(4);
		if (status) {
			addr = REG_TXDMA_STATUS;
			HalSdioGetCmdAddr8192FSdio(padapter, WLAN_IOREG_DEVICE_ID, addr, &addr);
			_sd_read(pintfhdl, addr, 4, status);
			_sd_write(pintfhdl, addr, 4, status);
			RTW_INFO("%s: SDIO_HISR_TXERR (0x%08x)\n", __func__, le32_to_cpu(*(u32 *)status));
			rtw_mfree(status, 4);
		} else
			RTW_INFO("%s: SDIO_HISR_TXERR, but can't allocate memory to read status!\n", __func__);
	}
	if (phal->sdio_hisr & SDIO_HISR_SDIO_CRCERR)
		RTW_INFO("%s: SDIO_HISR_SDIO_CRCERR\n", __func__);
	
	if (phal->sdio_hisr & SDIO_HISR_HSISR3_IND_INT)
		RTW_INFO("%s: SDIO_HISR_HSISR3_IND_INT\n", __func__);

	if (phal->sdio_hisr & SDIO_HISR_HSISR2_IND_INT)
		RTW_INFO("%s: Sdio command/data CRC error\n", __func__);
	
	if (phal->sdio_hisr & SDIO_HISR_HEISR_IND_INT)
		RTW_INFO("%s: Sdio command/data CRC error\n", __func__);
	
	if (phal->sdio_hisr & SDIO_HISR_TXBCNOK)
		RTW_INFO("%s: SDIO_HISR_TXBCNOK\n", __func__);

	if (phal->sdio_hisr & SDIO_HISR_TXBCNERR)
		RTW_INFO("%s: SDIO_HISR_TXBCNERR\n", __func__);

#ifdef CONFIG_FW_C2H_REG
	if (phal->sdio_hisr & SDIO_HISR_C2HCMD) {
		RTW_INFO("%s: C2H Command\n", __func__);
		sd_c2h_hisr_hdl(padapter);
	}
#endif

	if (phal->sdio_hisr & SDIO_HISR_RXFOVW)
		RTW_INFO("%s: Rx Overflow\n", __func__);
	if (phal->sdio_hisr & SDIO_HISR_RXERR)
		RTW_INFO("%s: Rx Error\n", __func__);

	if (phal->sdio_hisr & SDIO_HISR_RX_REQUEST) {
		struct recv_buf *precvbuf;
		int alloc_fail_time = 0;
		u32 rx_cnt = 0, ret = 0;

		phal->sdio_hisr ^= SDIO_HISR_RX_REQUEST;
		do {
				u8 data[4] = {0};
				phal->SdioRxFIFOSize = 0;

				_sdio_local_read(padapter, SDIO_REG_RX0_REQ_LEN, 4, data);
				phal->SdioRxFIFOSize = (le32_to_cpu(*(u32 *)data)) & 0x1FFFF;
				/*RTW_INFO("%s: phal->SdioRxFIFOSize =%u\n", __func__, phal->SdioRxFIFOSize);*/
			if (phal->SdioRxFIFOSize != 0) {
#ifdef CONFIG_MAC_LOOPBACK_DRIVER
				sd_recv_loopback(padapter, phal->SdioRxFIFOSize);
#else /*!CONFIG_MAC_LOOPBACK_DRIVER*/
				ret = sd_recv_rxfifo(padapter, phal->SdioRxFIFOSize, &precvbuf);
				if (precvbuf) {
					sd_rxhandler(padapter, precvbuf);
					phal->SdioRxFIFOSize = 0;
					rx_cnt++;
				} else {
					alloc_fail_time++;
			#ifdef CONFIG_RECV_THREAD_MODE
					if (alloc_fail_time >= 10) {
						if (_FAIL == sdio_recv_and_drop(padapter, phal->SdioRxFIFOSize))
							break;
						alloc_fail_time = 0;
					}
					else {
						rtw_msleep_os(1);
						continue;
					}
			#else/* !CONFIG_RECV_THREAD_MODE */
					if (ret == RTW_RBUF_UNAVAIL || ret == RTW_RBUF_PKT_UNAVAIL)
						rtw_msleep_os(10);
					else {
						RTW_INFO("%s: recv fail!(time=%d)\n", __func__, alloc_fail_time);
						phal->SdioRxFIFOSize = 0;
					}
					if (alloc_fail_time >= 10 && rx_cnt != 0)
						break;
			#endif/*!CONFIG_RECV_THREAD_MODE*/

				}
#endif
			} else
				break;
		} while (1);

		if (alloc_fail_time >= 10)
			RTW_DBG("%s: exit because recv failed more than 10 times!\n", __func__);
	}
}

void sd_int_hdl(PADAPTER padapter)
{
	PHAL_DATA_TYPE phal;


	if (RTW_CANNOT_RUN(padapter))
		return;

	phal = GET_HAL_DATA(padapter);

	if (!phal->sdio_himr) {
		RTW_WARN("%s: unexpected interrupt!\n", __FUNCTION__);
		return;
	}

	phal->sdio_hisr = 0;
	ReadInterrupt8192FSdio(padapter, &phal->sdio_hisr);

	if (phal->sdio_hisr & phal->sdio_himr) {
		u32 v32;

		phal->sdio_hisr &= phal->sdio_himr;

		/* clear HISR */
		v32 = phal->sdio_hisr & MASK_SDIO_HISR_CLEAR;
		if (v32)
			SdioLocalCmd52Write4Byte(padapter, SDIO_REG_HISR, v32);

		sd_int_dpc(padapter);
	} else {
		if (0)
			RTW_INFO("%s: HISR(0x%08x) and HIMR(0x%08x) not match!\n",
				__FUNCTION__, phal->sdio_hisr, phal->sdio_himr);
	}
}

/*
 *	Description:
 *		Query SDIO Local register to query current the number of Free TxPacketBuffer page.
 *
 *	Assumption:
 *		1. Running at PASSIVE_LEVEL
 *		2. RT_TX_SPINLOCK is NOT acquired.
 *
 *	Created by Roger, 2011.01.28.
 *   */
u8 HalQueryTxBufferStatus8192FSdio(PADAPTER padapter)
{
	PHAL_DATA_TYPE phal;
	u32 NumOfFreePage;
	/* _irqL irql; */


	phal = GET_HAL_DATA(padapter);

	/* _enter_critical_bh(&phal->SdioTxFIFOFreePageLock, &irql); */
	NumOfFreePage = SdioLocalCmd53Read4Byte(padapter, SDIO_REG_FREE_TXPG1_8192F);
	phal->SdioTxFIFOFreePage[HI_QUEUE_IDX] = NumOfFreePage & 0x00000fff;
	phal->SdioTxFIFOFreePage[MID_QUEUE_IDX] = (NumOfFreePage & 0x0fff0000) >> 16;
	NumOfFreePage = SdioLocalCmd53Read4Byte(padapter, SDIO_REG_FREE_TXPG2_8192F);
	phal->SdioTxFIFOFreePage[LOW_QUEUE_IDX] = NumOfFreePage & 0x00000fff;
	phal->SdioTxFIFOFreePage[PUBLIC_QUEUE_IDX] = (NumOfFreePage & 0x0fff0000) >> 16;

	/* _exit_critical_bh(&phal->SdioTxFIFOFreePageLock, &irql); */

	return _TRUE;
}

/*
 *	Description:
 *		Query SDIO Local register to get the current number of TX OQT Free Space.
 *   */
u8 HalQueryTxOQTBufferStatus8192FSdio(PADAPTER padapter)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);

	pHalData->SdioTxOQTFreeSpace = SdioLocalCmd52Read1Byte(padapter, SDIO_REG_AC_OQT_FREEPG_8192F);
	return _TRUE;
}

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
u8 RecvOnePkt(PADAPTER padapter)
{
	struct recv_buf *precvbuf;
	struct dvobj_priv *psddev;
	PSDIO_DATA psdio_data;
	struct sdio_func *func;
	u32 tmp = 0;
	u16 len = 0;
	u8 res = _FALSE;

	if (padapter == NULL) {
		RTW_ERR("%s: padapter is NULL!\n", __func__);
		return _FALSE;
	}

	psddev = adapter_to_dvobj(padapter);
	psdio_data = &psddev->intf_data;
	func = psdio_data->func;

	/* If RX_DMA is not idle, receive one pkt from DMA */
	res = sdio_local_read(padapter,
			  SDIO_REG_RX0_REQ_LEN, 4, (u8 *)&tmp);
	len = (le32_to_cpu(tmp)) & 0x1FFFF;
	RTW_INFO("+%s: size: %d+\n", __func__, len);

	if (len) {
		sdio_claim_host(func);
		res = sd_recv_rxfifo(padapter, len, &precvbuf);

		if (precvbuf) {
			/* printk("Completed Recv One Pkt.\n"); */
			sd_rxhandler(padapter, precvbuf);
			res = _TRUE;
		} else
			res = _FALSE;
		sdio_release_host(func);
	}
	RTW_INFO("-%s-\n", __func__);
	return res;
}
#endif /* CONFIG_WOWLAN */

