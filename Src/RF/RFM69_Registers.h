/*! \file
*               $Revision: 16923 $
*
*               $Author: mstewart $
*
*               $Date: 2014-01-16 15:40:40 +0000 (Thu, 16 Jan 2014) $
*
*               $HeadURL: https://selacvs01.schrader.local:8443/svn/ECU_Software/LF_TOOL_GEN2/trunk/Src/HAL/RF/RF.h $
*
*   \brief      Public header file for RF module.
*/
/* COPYRIGHT NOTICE
* ==================================================================================================
*
* The contents of this document are protected under copyright and contain commercially and/or
* technically confidential information. The content of this document must not be used other than
* for the purpose for which it was provided nor may it be disclosed or copied (by the authorised
* recipient or otherwise) without the prior written consent of an authorised officer of Schrader
* Electronics Ltd.
*
*         (C) $Date:: 2014#$ Schrader Electronics Ltd.
*
****************************************************************************************************
*/
#ifndef RFM69_REGISTERS_H
#define RFM69_REGISTERS_H

/***************************************************************************************************
**                              Includes                                                          **
***************************************************************************************************/
#include "C_defs.h"
#include "COMPILER_defs.h"
#include "RFM69.h"



/***************************************************************************************************
**                              Defines                                                           **
***************************************************************************************************/


/***************************************************************************************************
**                              Exported Globals                                                  **
***************************************************************************************************/
/* Default register set 915Mhz */
const RFM69_register_data_st RFM69_default_register_set[] =
{
    { REGOPMODE        ,0x04 },         
    { REGDATAMODUL     ,0x00 },         
    { REGBITRATEMSB    ,0x1A },         
    { REGBITRATELSB    ,0x0B },         
    { REGFDEVMSB       ,0x00 },         
    { REGFDEVLSB       ,0x52 },         
    { REGFRFMSB        ,0xE4 },         
    { REGFRFMID        ,0xC0 },         
    { REGFRFLSB        ,0x00 },         
    { REGOSC1          ,0x41 },         
    { REGAFCCTRL       ,0x00 },         
	{ RESERVED0C       ,0x0C },         
    { REGLISTEN1       ,0x92 },         
    { REGLISTEN2       ,0xF5 },         
    { REGLISTEN3       ,0x20 },         
	{ REGVERSION       ,0x24 },         
    { REGPALEVEL       ,0x9F },         
    { REGPARAMP        ,0x09 },         
    { REGOCP           ,0x1A },         
	{ RESERVED14       ,0x40 },         
	{ RESERVED15       ,0xB0 },         
	{ RESERVED16       ,0x7B },         
	{ RESERVED17       ,0x9B },         
    { REGLNA           ,0x88 },         
    { REGRXBW          ,0x55 },         
    { REGAFCBW         ,0x8B },         
    { REGOOKPEAK       ,0x40 },         
    { REGOOKAVG        ,0x80 },         
    { REGOOKFIX        ,0x06 },         
    { REGAFCFEI        ,0x10 },         
	{ REGAFCMSB		   ,0x00 },         
	{ REGAFCLSB		   ,0x00 },         
	{ REGFEIMSB        ,0x00 },         
	{ REGFEILSB        ,0x00 },         
    { REGRSSICONFIG    ,0x02 },         
	{ REGRSSIVALUE     ,0xFF },         
    { REGDIOMAPPING1   ,0x00 },         
    { REGDIOMAPPING2   ,0x07 },         
    { REGIRQFLAGS1     ,0x80 },         
    { REGIRQFLAGS2     ,0x00 },         
    { REGRSSITHRESH    ,0xE4 },         
    { REGRXTIMEOUT1    ,0x00 },         
    { REGRXTIMEOUT2    ,0x00 },         
    { REGPREAMBLEMSB   ,0x00 },         
    { REGPREAMBLELSB   ,0x03 },         
    { REGSYNCCONFIG    ,0x98 },         
    { REGSYNCVALU1     ,0x01 },         
    { REGSYNCVALU2     ,0x01 },         
    { REGSYNCVALU3     ,0x01 },         
    { REGSYNCVALU4     ,0x01 },         
    { REGSYNCVALU5     ,0x01 },         
    { REGSYNCVALU6     ,0x01 },         
    { REGSYNCVALU7     ,0x01 },         
    { REGSYNCVALU8     ,0x01 },         
    { REGPACKETCONFIG1 ,0x10 },         
    { REGPAYLOADLENGTH ,0x40 },         
    { REGNODEADRS      ,0x00 },         
    { REGBROADCASTADRS ,0x00 },         
    { REGAUTOMODES     ,0x00 },         
    { REGFIFOTHRESH    ,0x8F },         
    { REGPACKETCONFIG2 ,0x02 },         
    { REGAESKEY1       ,0x00 },         
    { REGAESKEY2       ,0x00 },         
    { REGAESKEY3       ,0x00 },         
    { REGAESKEY4       ,0x00 },         
    { REGAESKEY5       ,0x00 },         
    { REGAESKEY6       ,0x00 },         
    { REGAESKEY7       ,0x00 },         
    { REGAESKEY8       ,0x00 },         
    { REGAESKEY9       ,0x00 },         
    { REGAESKEY10      ,0x00 },         
    { REGAESKEY11      ,0x00 },         
    { REGAESKEY12      ,0x00 },
    { REGAESKEY13      ,0x00 },
    { REGAESKEY14      ,0x00 },
    { REGAESKEY15      ,0x00 },
    { REGAESKEY16      ,0x00 },
    { REGTEMP1         ,0x01 },
	{ REGTEMP2         ,0x00 },
};



/* 433Mhz OOK register set */
const RFM69_register_data_st RFM69_433Mhz_OOK_set[] =
{
    { REGOPMODE        ,0x04 },			//Operating mode 				1
    { REGDATAMODUL     ,0x08 },         //Packet Mode | OOK
    { REGBITRATEMSB    ,0x1A },         //4.8Kbps Bitrate
    { REGBITRATELSB    ,0x0B },         //4.8Kbps Bitrate
    { REGFDEVMSB       ,0x00 },
    { REGFDEVLSB       ,0x52 },
    { REGFRFMSB        ,0x6C },         //433Mhz
    { REGFRFMID        ,0x40 },         //433Mhz
    { REGFRFLSB        ,0x00 },         //433Mhz
    { REGOSC1          ,0x41 },
    { REGAFCCTRL       ,0x00 },
    { RESERVED0C	   ,0x00 },
    { REGLISTEN1       ,0xA0 },
    { REGLISTEN2       ,0xF5 },
    { REGLISTEN3       ,0x20 },
	{ REGVERSION       ,0x24 },
	{ REGPALEVEL       ,0x85 },			//Medium output power on PA0
    { REGPARAMP        ,0x09 },
    { REGOCP           ,0x1A },         //OCP
	{ RESERVED14       ,0x40 }, 
	{ RESERVED15       ,0xB0 }, 
	{ RESERVED16       ,0x7B }, 
	{ RESERVED17       ,0x9B }, 
    { REGLNA           ,0x88 },
    { REGRXBW          ,0x42 },
    { REGAFCBW         ,0x8B },
    { REGOOKPEAK       ,0x40 },
    { REGOOKAVG        ,0x80 },
    { REGOOKFIX        ,0x06 },
    { REGAFCFEI        ,0x10 },
	{ REGAFCMSB		   ,0x00 },
	{ REGAFCLSB		   ,0x00 },
	{ REGFEIMSB        ,0x00 },
	{ REGFEILSB        ,0x00 },
    { REGRSSICONFIG    ,0x02 },
	{ REGRSSIVALUE     ,0xFF }, 
    { REGDIOMAPPING1   ,0x00 },
    { REGDIOMAPPING2   ,0x07 },
    { REGIRQFLAGS1     ,0x80 },
    { REGIRQFLAGS2     ,0x00 },
    { REGRSSITHRESH    ,0xDC },
    { REGRXTIMEOUT1    ,0x00 },
    { REGRXTIMEOUT2    ,0x00 },
    { REGPREAMBLEMSB   ,0x00 },
    { REGPREAMBLELSB   ,0x03 },
    { REGSYNCCONFIG    ,0x90 },
    { REGSYNCVALU1     ,0xAA },			//Keep this as the default sync value meaning 0xAA must be present on all nodes and receiver
    { REGSYNCVALU2     ,0xAA },			// Will be replaced by network ID ( default to 0xAA )
    { REGSYNCVALU3     ,0x01 },
    { REGSYNCVALU4     ,0x01 },
    { REGSYNCVALU5     ,0x01 },
    { REGSYNCVALU6     ,0x01 },
    { REGSYNCVALU7     ,0x01 },
    { REGSYNCVALU8     ,0x01 },
    { REGPACKETCONFIG1 ,0x90 },         //variable length packets | CRC OFF | NO Address filtering
    { REGPAYLOADLENGTH ,0x42 },
    { REGNODEADRS      ,0x00 },
    { REGBROADCASTADRS ,0x00 },
    { REGAUTOMODES     ,0x00 },
    { REGFIFOTHRESH    ,0x8F },
    { REGPACKETCONFIG2 ,0x03 },         //AES ON
    { REGAESKEY1       ,0x00 },
    { REGAESKEY2       ,0x00 },
    { REGAESKEY3       ,0x00 },
    { REGAESKEY4       ,0x00 },
    { REGAESKEY5       ,0x00 },
    { REGAESKEY6       ,0x00 },
    { REGAESKEY7       ,0x00 },
    { REGAESKEY8       ,0x00 },
    { REGAESKEY9       ,0x00 },
    { REGAESKEY10      ,0x00 },
    { REGAESKEY11      ,0x00 },
    { REGAESKEY12      ,0x00 },
    { REGAESKEY13      ,0x00 },
    { REGAESKEY14      ,0x00 },
    { REGAESKEY15      ,0x00 },
    { REGAESKEY16      ,0x00 },
    { REGTEMP1         ,0x01 },
	{ REGTEMP2         ,0x00 },
};


#define DEFAULT_CONFIGURATION_SIZE ( sizeof( RFM69_default_register_set ) / sizeof( RFM69_register_data_st ) )
#define RFM69_433Mhz_CONFIGURATION_SIZE ( sizeof( RFM69_433Mhz_OOK_set ) / sizeof( RFM69_register_data_st ) )


const RFM69_static_configuration_st RFM69_config_c[ 2 ] =
{
    { (RFM69_register_data_st * )RFM69_default_register_set, DEFAULT_CONFIGURATION_SIZE },
    { (RFM69_register_data_st * )RFM69_433Mhz_OOK_set,       RFM69_433Mhz_CONFIGURATION_SIZE }
};


#endif /* RFM69_H multiple inclusion guard */

/****************************** END OF FILE *******************************************************/
