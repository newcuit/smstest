/**************************************************************************************
 **** Copyright (C), 2017, xx xx xx xx info&tech Co., Ltd.

 * File Name     : ucs2_gb2312.h
 * Author        :
 * Date          : 2017-03-15
 * Description   : .C file function description
 * Version       : 1.0
**************************************************************************************/
#ifndef _UCS2_GB2312_H_
#define _UCS2_GB2312_H_

#include "stdio.h"
#include "string.h"

typedef  unsigned char uint8_t;
typedef  unsigned short uint16_t;
typedef  unsigned int uint32_t;
typedef  int int32_t;

/******************************************************************************************
* FunctionName   : ucs2_to_gb2312()
* Description    : ucs2编码转换成gb2312编码
* EntryParameter : gb2312，指向转换后的GB2312数据串， ucs2,指向需要转换编码的UCS2数据串，len，长度
* ReturnValue    : 返回gb2312的长度
*******************************************************************************************/
uint16_t ucs2_to_gb2312(uint8_t *gb2312, uint8_t *ucs2, int length);

/******************************************************************************************
 * * FunctionName   : gsm7bit_to_ascii()
 * * Description    : gsm7bit编码转换成ascii编码
 * * EntryParameter : gsm7bit，指向gsm7bit数据串， ascii,指向ascii数据串，bitlen，长度
 * * ReturnValue    : 返回gb2312的长度
 * *******************************************************************************************/
void gsm7bit_to_ascii(uint8_t *gsm7bit, int bitlen, uint8_t *ascii);

#endif /* _UCS2_GB2312_H_ */
