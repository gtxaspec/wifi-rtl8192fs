/******************************************************************************
*
* Copyright(c) 2012 - 2017 Realtek Corporation.
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
******************************************************************************/

#ifdef CONFIG_RTL8192F

#ifndef _FW_HEADER_8192F_H
#define _FW_HEADER_8192F_H

#ifdef LOAD_FW_HEADER_FROM_DRIVER
#if (defined(CONFIG_AP_WOWLAN) || (DM_ODM_SUPPORT_TYPE & (ODM_AP)))
extern u8 array_mp_8192f_fw_ap[31156];
extern u32 array_length_mp_8192f_fw_ap;
#endif

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN)) || (DM_ODM_SUPPORT_TYPE & (ODM_CE))
extern u8 array_mp_8192f_fw_nic[31136];
extern u32 array_length_mp_8192f_fw_nic;
#ifdef CONFIG_WOWLAN
extern u8 array_mp_8192f_fw_wowlan[31344];
extern u32 array_length_mp_8192f_fw_wowlan;
#endif /*CONFIG_WOWLAN*/
#endif
#endif /* end of LOAD_FW_HEADER_FROM_DRIVER */

#endif

#endif

