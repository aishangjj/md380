/*************** (C) COPYRIGHT 2012 Co., Ltd****************
* File Name          : f_dspcan.c
* Author             : 	
* Version            : V0.0.1
* Date               : 08/09/2012
* Description        : DSP CAN总线底层驱动库

********************************************************************************/
#include "DSP28x_Project.h"     							// DSP2803x Headerfile Include File	
//#include "main.h"											// 包含头文件

#include "f_funcCode.h"
#include "f_dspcan.h"



#define DEBUG_F_CAN              1



#if DEBUG_F_CAN

#if (DSP_CLOCK == 100)
	#define		DSPCAN_CLK		100000
#else
	#define		DSPCAN_CLK		30000
#endif

const	CAN_BAUD	eCanBaud[CAN_BAUD_SUM] = {
									{(DSPCAN_CLK/20/20)-1, 3, 14},	// 20Kbps	
									{(DSPCAN_CLK/20/50)-1, 3, 14},	// 50Kbps		
									{(DSPCAN_CLK/20/100)-1, 3, 14},	// 100Kbps	
									{(DSPCAN_CLK/20/125)-1, 3, 14},	// 125Kbps		3+14+ 2 +1 = 20
									{(DSPCAN_CLK/20/250)-1, 3, 14},	// 250Kbps		3+14+ 2 +1 = 20
									{(DSPCAN_CLK/20/500)-1, 3, 14},	// 500Kbps		3+14+ 2 +1 = 20
									{(DSPCAN_CLK/10/1000)-1, 1, 6} //  1Mbps		1+6 + 2 +1 = 10
								};

Uint32 eCanTranEnFlag;// = 0;
Uint32 eCanReEnFlag;// = 0;
	
/*******************************************************************************
* 函数名称          : Uint16 InitdspECan(Uint16 baud)
* 入口参数			: CAN接口波特率，
* 出口				：CAN_INIT_TIME	 初始化进行中
*					  CAN_INIT_SUCC  初始化成功
*					  CAN_INIT_TIMEOUT 初始化超时
*					  CAN_INIT_BAUD_ERR 波特率出错
* 创建	            : 	
* 版本		        : V0.0.1
* 时间              : 07/29/2012
* 说明				: 初始化DSP Ecan接口
********************************************************************************/
#define		IINIT_CAN_TIME				3
Uint16 InitdspECan(Uint16 baud)		// Initialize eCAN-A module
{
	struct ECAN_REGS ECanaShadow;							// 声明一个影子寄存器，某些寄存器只能使用32位操作
	Uint32 *MsgCtrlPi;										// 初始化引用指针
	Uint16	i;												// 循环变量
	static	Uint16 con = 0;
	static	Uint16 count = 0;								// 超时计数器
	
	if (baud >= CAN_BAUD_SUM)
		return CAN_INIT_BAUD_ERR;							// 波特率出错
	if (count > IINIT_CAN_TIME)								// 初始化超时出错
		return  CAN_INIT_TIMEOUT;
	
	EALLOW;
	if (con == 0)
	{
		GpioCtrlRegs.GPAPUD.bit.GPIO30 = 0;	    // Enable pull-up for GPIO30 (CANRXA)
		GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;	    // Enable pull-up for GPIO31 (CANTXA)

	/* Set qualification for selected CAN pins to asynch only */
	// Inputs are synchronized to SYSCLKOUT by default.  
	// This will select asynch (no qualification) for the selected pins.

		GpioCtrlRegs.GPAQSEL2.bit.GPIO30 = 3;   // Asynch qual for GPIO30 (CANRXA)   

	/* Configure eCAN-A pins using GPIO regs*/
	// This specifies which of the possible GPIO pins will be eCAN functional pins.

		GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 1;	// Configure GPIO30 for CANTXA operation
		GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 1;	// Configure GPIO31 for CANRXA operation
	
	/* Configure eCAN RX and TX pins for eCAN transmissions using eCAN regs*/  
		ECANREGS.CANTIOC.bit.TXFUNC = 1;
		ECANREGS.CANRIOC.bit.RXFUNC = 1;  

	/* Configure eCAN for HECC mode - (reqd to access mailboxes 16 thru 31) */
										// HECC mode also enables time-stamping feature
		ECanaShadow.CANMC.all = 0;
		ECanaShadow.CANMC.bit.SRES = 1;
		ECANREGS.CANMC.all = ECanaShadow.CANMC.all;			// 软件复位CAN模块
		
		ECanaShadow.CANMC.all = ECANREGS.CANMC.all;			// 读取CAN主控制寄存器
		ECanaShadow.CANMC.bit.SCB = 1;						// eCAN模式				
		ECanaShadow.CANMC.bit.SUSP = 1;						// 外设不受调试影响
//		ECanaShadow.CANMC.bit.DBO = 1;						// 首先 最低有效位 高字节在前
		ECanaShadow.CANMC.bit.CCR = 1;						// CPU请求修改波特率或全局屏蔽寄存器
		ECanaShadow.CANMC.bit.ABO = 1;						// 自动恢复总线使能
		ECANREGS.CANMC.all = ECanaShadow.CANMC.all;			// 回写控制寄存器
		
	/* Initialize all bits of 'Master Control Field' to zero */
	// Some bits of MSGCTRL register come up in an unknown state. For proper operation,
	// all bits (including reserved bits) of MSGCTRL must be initialized to zero
		MsgCtrlPi = (Uint32 *)(&ECANMBOXES.MBOX0.MSGCTRL);	// 消息控制器指针
		for (i=0; i<32; i++)
		{
			MsgCtrlPi[i<<2] = 0x00000000;					// 清零所有消息控制寄存器
		}
		MsgCtrlPi = (Uint32 *)(&ECANLAMS.LAM0);				// 息控制器指针
		for (i=0; i<32; i++)								// 清空所有屏蔽寄存器
		{
			MsgCtrlPi[i] = 0x00000000;						// 
		}
		
	/* 
		ECanaMboxes.MBOX0.MSGCTRL.all = 0x00000000;
		..........
		ECanaMboxes.MBOX31.MSGCTRL.all = 0x00000000;
	*/    
	// TAn, RMPn, GIFn bits are all zero upon reset and are cleared again
	//	as a matter of precaution. 
		ECANREGS.CANTRR.all	= 0xFFFFFFFF;					// 复位发送请求，取消正在进行的发送
		ECANREGS.CANTA.all	= 0xFFFFFFFF;					// 清零发送响应寄存器/* Clear all TAn bits */      
		ECANREGS.CANRMP.all = 0xFFFFFFFF;					// 接收消息挂起寄存器/* Clear all RMPn bits */      
		ECANREGS.CANGIF0.all = 0xFFFFFFFF;					// 全局中断标志/* Clear all interrupt flag bits */ 
		ECANREGS.CANGIF1.all = 0xFFFFFFFF;
		ECANREGS.CANOPC.all = 0;							// 所有邮箱可被覆盖
	/* Configure bit timing parameters for eCANA
		ECanaShadow.CANMC.all = ECANREGS.CANMC.all;
		ECanaShadow.CANMC.bit.CCR = 1 ;            			// CPU请求修改波特率或全局屏蔽寄存器
		ECANREGS.CANMC.all = ECanaShadow.CANMC.all;
	*/	
		con = 1;											// 第一阶段完成
	}
    if (con == 1)
	{
		ECanaShadow.CANES.all = ECANREGS.CANES.all;
		if (ECanaShadow.CANES.bit.CCE != 1 ) 				// Wait for CCE bit to be set..
		{
			count++;
			EDIS;
			return CAN_INIT_TIME;							// 初始化进行中
		}
		else
			con = 2;
	}
	
    if (con == 2)
	{
		ECanaShadow.CANBTC.all = 0;                         // 初始化波特率
		ECanaShadow.CANBTC.bit.BRPREG = eCanBaud[baud].BRPREG;
		ECanaShadow.CANBTC.bit.TSEG2REG = eCanBaud[baud].TSEG2REG;
		ECanaShadow.CANBTC.bit.TSEG1REG = eCanBaud[baud].TSEG1REG; 
		ECanaShadow.CANBTC.bit.SAM = 0;
		ECANREGS.CANBTC.all = ECanaShadow.CANBTC.all;
		
		ECanaShadow.CANMC.all = ECANREGS.CANMC.all;
		ECanaShadow.CANMC.bit.CCR = 0 ;            			// 波特率设置完成 Set CCR = 0
		ECANREGS.CANMC.all = ECanaShadow.CANMC.all;
		con = 3;
    }
	if (con == 3)
	{
		ECanaShadow.CANES.all = ECANREGS.CANES.all;
		if (ECanaShadow.CANES.bit.CCE != 0 ) 				// Wait for CCE bit to be  cleared..
		{
			count++;
			EDIS;
			return CAN_INIT_TIME;		
		}
	}
/* Disable all Mailboxes  */

	con = 0;
	count = 0;
 	ECANREGS.CANME.all = 0;									// Required before writing the MSGIDs

    EDIS;
	
	eCanTranEnFlag = 0;                                     // 清空邮箱初始化标志
	eCanReEnFlag = 0;
	return CAN_INIT_SUCC;									// 初始化成功 
}	


/*******************************************************************************
* 函数名称          : void InitTranMbox(Uint16 mbox)
* 入口参数			: mbox 邮箱编号 0~31， 
*					  ID	消息标识ID
* 出口				：无
* 创建	            : 	
* 版本		        : V0.0.1
* 时间              : 07/29/2012
* 说明				: 初化CAN发送邮箱，可初始化为自动应答邮箱
********************************************************************************/
void InitTranMbox(Uint16 mbox, Uint32 msgid, Uint32 *dataPi)
{
	Uint16 id;
	Uint32 ECanaShadow, *msgIdPi;	                        //指针赋值消息ID地址

	id = mbox & 0x1f;
	eCanTranEnFlag |= 1ul <<mbox;							// 邮箱初始化发送标志

	msgIdPi = (Uint32 *)(&ECANMBOXES.MBOX0.MSGID);
	msgIdPi[id<<2] = msgid;								    // 写消息标志，确定是否为自动应答邮箱
	msgIdPi[(id<<2) +1] = 8;
	
	ECanaShadow = ECANREGS.CANMD.all;
	ECanaShadow &= ~(1ul<<id);
	ECANREGS.CANMD.all = ECanaShadow;						// 清零设置为发送邮箱

	ECanaShadow = ECANREGS.CANME.all;
	ECanaShadow |= 1ul<<id;
	ECANREGS.CANME.all = ECanaShadow;						// 使能对应邮箱

	msgIdPi[(id<<2) + 2] = *dataPi++;						// 写自动应答信息到
	msgIdPi[(id<<2) + 3] = *dataPi;	
	
}

/*******************************************************************************
* 函数名称          : void InitReMbox(Uint16 mbox, union CANMSGID_REG msgid, union CANLAM_REG lam)
* 入口参数			: mbox 邮箱编号 0~31，bit7 “1” 接收远程帧 “0”胀�?bit6 "1"覆盖保护
*					  msgid	消息标识ID
*					  lam	接收屏蔽寄存器
* 出口				：无
* 创建	            : 	
* 版本		        : V0.0.1
* 时间              : 07/29/2012
* 说明				: 初化CAN接收邮箱
********************************************************************************/
void InitRecMbox(Uint16 mbox, Uint32 msgid, Uint32 lam)
{
	Uint16 id;
	Uint32 ECanaShadow,  *pi;								// = (Uint32 *)(&ECANMBOXES.MBOX0.MSGID);
	
	id = mbox & 0x1f;
	eCanReEnFlag |= 1ul << id;
	
	pi = (Uint32 *)(&ECANMBOXES.MBOX0.MSGID);
	pi[id<<2] = msgid;										// 消息标识寄存器
	if ((mbox & 0x80) == 0x80)								// 发送远程帧传输初始化
		pi[(id<<2) +1] = 1<<4 | 8;							// 消息控制寄存器
	else
		pi[(id<<2) +1] = 8;
		
	ECanaShadow = ECANREGS.CANOPC.all;
	if ( (mbox & 0x40) == 0x40 )							// 使能覆盖保护检查，主初始化中已经将所有邮箱禁止覆盖保护
		ECanaShadow |= 1ul<<id;
	else
		ECanaShadow &= ~(1ul<<id);
	ECANREGS.CANOPC.all = ECanaShadow;
		
	ECanaShadow = ECANREGS.CANMD.all;						// 置“1”设置为接收邮箱
	ECanaShadow |= 1ul<<id;
	ECANREGS.CANMD.all = ECanaShadow;						// 
	
	ECanaShadow = ECANREGS.CANME.all;
	ECanaShadow |= 1ul<<id;
	ECANREGS.CANME.all = ECanaShadow;						// 使能对应邮箱
	
	pi = (Uint32 *)(&ECANLAMS.LAM0);						// 配置接收屏蔽寄存器
	pi[id] = lam;
}


/*******************************************************************************
* 函数名称          : Uint16 eCanDataTran(Uint16 mbox, Uint16 len, Uint32 msgid, Uint32 *dataPi)
* 入口参数			: mbox 邮箱编号 0~31，
*					  ID	消息标识ID			只包含有效ID位
* 出口				：CAN_MBOX_NUM_ERROR		邮箱号出错，该邮箱未被初始化为发送邮箱
*					  CAN_MBOX_BUSY				邮箱忙
*					  CAN_MBOX_TRAN_SUCC		发送成功
* 创建	            : 	
* 版本		        : V0.0.1
* 时间              : 08/25/2012
* 说明				: 指定邮箱发送数据，邮箱必须被初始化为发送邮箱
********************************************************************************/
Uint16 eCanDataTran(Uint16 mbox, Uint16 len, Uint32 msgid, Uint32 *dataPi)
{
	Uint32 ECanaShadow, *pi;
	mbox &= 0x1f;
	if ( (eCanTranEnFlag & (1ul << mbox)) != (1ul << mbox) )
	{
		return (CAN_MBOX_NUM_ERROR);						// CAN邮箱号出错，邮箱未初始化
	}
	
	if (ECANREGS.CANTRS.all & (1ul << mbox))				// 检查上次发送是否完成，发送请求标志置位
	{
		return (CAN_MBOX_BUSY);								// CAN邮箱忙
	}
	
	ECANREGS.CANTA.all = 1ul << mbox;						// 清空发送响应标志
	
	pi = (Uint32 *)(&ECANMBOXES.MBOX0.MSGID);				// 写ID，写数据
	
	msgid &= ~(0x7ul<<29);									// 清除高三位
	msgid |= pi[mbox<<2] & (0x7ul << 29);					// 不修改ID配置位
	
	ECanaShadow = ECANREGS.CANME.all;
	ECanaShadow &= ~(1ul<<mbox);
	ECANREGS.CANME.all = ECanaShadow;						// 禁止对应邮箱
	
	pi[mbox<<2] = msgid;									// 重写ID
	pi[(mbox<<2) + 1] = len;
	pi[(mbox<<2) + 2] = *dataPi++;							// 写数据
	pi[(mbox<<2) + 3] = *dataPi;
	
	ECanaShadow |= 1ul<<mbox;
	ECANREGS.CANME.all = ECanaShadow;						// 使能对应邮箱	

	ECANREGS.CANTRS.all = 1ul << mbox;						// 使能发送
	return (CAN_MBOX_TRAN_SUCC);
}

/*******************************************************************************
* 函数名称          : Uint16 eCanDataRec(Uint16 mbox, Uint32 *dataPi)
* 入口参数			: mbox 邮箱编号 0~31，
*					  *	dataPi 接收缓存
* 出口				：CAN_MBOX_NUM_ERROR		邮箱号出错，该邮箱未被初始化为发送邮箱
*					  CAN_MBOX_EMPTY			接收邮箱空
*					  CAN_MBOX_REC_SUCC			接收数据成功
*					  CAN_MBOX_REC_OVER			接收数据溢出
* 创建	            : 	
* 版本		        : V0.0.1
* 时间              : 08/25/2012
* 说明				: 接收数据接收缓存区
********************************************************************************/
Uint16 eCanDataRec(Uint16 mbox, Uint32 *dataPi)
{
	Uint32 *pi;
	
	mbox &= 0x1f;
//	if ( (eCanReEnFlag & (1ul << mbox)) != (1ul << mbox))
//	{
//		return (CAN_MBOX_NUM_ERROR);						// CAN邮箱号出错，邮箱未初始化
//	}
	if (ECANREGS.CANRMP.all & (1ul << mbox) )				// 检查是否有接收消息挂起
	{
		pi = (Uint32 *)(&ECANMBOXES.MBOX0.MSGID);			
		*dataPi++ = pi[mbox<<2];							//  读ID，读数据
		*dataPi++ = pi[(mbox<<2) + 2];	
		*dataPi++ = pi[(mbox<<2) + 3];
        *dataPi   = pi[(mbox<<2) + 1] & 0xf;                // 读取接收数据长度

//		ECanaShadow = 1ul<<mbox;
		
		if (ECANREGS.CANRML.all & (1ul << mbox))			// 检查邮箱是否被覆盖过
		{
			ECANREGS.CANRMP.all = 1ul<<mbox;				// 清除消息挂起寄存器
			return (CAN_MBOX_REC_OVER);
		}	
		else
		{
			ECANREGS.CANRMP.all = 1ul<<mbox;				// 清除消息挂起寄存器
			return (CAN_MBOX_REC_SUCC);		
		}
	}
	else
	{
		return (CAN_MBOX_EMPTY);							// CAN邮箱空，无可读取数据		
	}
}




#elif 1



#endif




