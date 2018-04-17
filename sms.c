#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "ucs2_gb2312.h"

typedef  unsigned char uint8_t;
typedef  unsigned short uint16_t;
typedef  unsigned int uint32_t;
typedef  int int32_t;

/**************************************************************************************
* Description    : 定义SMS PDU中固定长度字段的长度
**************************************************************************************/
#define SMS_FO_OFFSET               (1*2)
#define SMS_PID_OFFSET              (1*2)
#define SMS_DCS_OFFSET              (1*2)
#define SMS_SCTS_OFFSET             (7*2)
#define SMS_UDL_OFFSET              (1*2)
#define SMS_MR_OFFSET               (1*2)
#define SMS_UDL_OFFSET              (1*2)

/**************************************************************************************
* Description    : 定义SMS PDU中DCS定义
**************************************************************************************/
#define SMS_DCS_7BIT_NOCLASS                0x00
#define SMS_DCS_UCS2_NOCLASS                0x08
#define SMS_DCS_UCS2_CLASS0                 0x18
#define SMS_DCS_UCS2_CLASS1                 0x19
#define SMS_DCS_7BIT_CLASS0                 0xF0
#define SMS_DCS_7BIT_CLASS1                 0xF1
#define SMS_DCS_7BIT_CLASS2                 0xF2
#define SMS_DCS_7BIT_CLASS3                 0xF3
#define SMS_DCS_8BIT_CLASS0                 0xF4
#define SMS_DCS_8BIT_CLASS1                 0xF5
#define SMS_DCS_8BIT_CLASS2                 0xF6
#define SMS_DCS_8BIT_CLASS3                 0xF7

/**************************************************************************************
* FunctionName   : m26_shex2int8()
* Description    : 将2个字节的十六进制字符串转换成数字
* EntryParameter : data,指向需要转换的十六进制字符串
* ReturnValue    : 返回转换后的结果
**************************************************************************************/
static inline uint8_t m26_shex2int8(uint8_t *data)
{
    uint8_t i, dec = 0;

    for (i = 0; i < 2; i++) {
        if('0' <= *(data+i) && *(data+i) <= '9') {
            if(i) dec += (*(data+i) - '0');
            else dec += (*(data+i) - '0')*16;
        }
        if('A' <= *(data+i) && *(data+i) <= 'F') {
            if(i) dec += *(data+i) - 'A' + 10;
            else dec += (*(data+i) - 'A'+10)*16;
        }
        if('a' <= *(data+i) && *(data+i) <= 'f') {
            if(i) dec += *(data+i) - 'a' + 10;
            else dec += (*(data+i) - 'a'+10)*16;
        }
    }
    return dec;
}

/**************************************************************************************
* FunctionName   : m26_shex2sbits()
* Description    : 将数据从16进制字符串变成bits字符串
* EntryParameter : data,指向需要翻译的字符串， len指向需要翻译字符串的最大长度
* ReturnValue    : 返回翻译后的字符串长度
**************************************************************************************/
static inline uint8_t m26_shex2sbits(uint8_t *data, int len)
{
    int i, j;

    for (i = 0,j = 0; i < len; i = i+2) {
        data[j++] = m26_shex2int8(data + i);
    }
    data[j] = '\0';
    return j;
}

/**************************************************************************************
* FunctionName   : m26_sms_sca_offset()
* Description    : 获取变长数据SCA的长度
* EntryParameter : sca,指向带有sca信息的PDU
* ReturnValue    : 返回SCA长度
**************************************************************************************/
static inline uint8_t m26_sms_sca_offset(uint8_t *sca, int len)
{
    uint8_t sca_len;

    sca_len = m26_shex2int8(sca);
    // SCA长度为len(2)+TOA+SCA
    return sca_len*2+2;
}

/**************************************************************************************
* FunctionName   : m26_sms_oa_offset()
* Description    : 获取变长数据OA的长度
* EntryParameter : oa,指向带有oa信息的PDU
* ReturnValue    : 返回oa长度
**************************************************************************************/
static inline uint8_t m26_sms_oa_offset(uint8_t *oa, int len)
{
    uint8_t oa_len;

    oa_len = m26_shex2int8(oa);
    // oa_len为基数，需要填充F, SCA长度为len+TOA+OA
    return oa_len+2+2+oa_len%2;
}

#define m26_sms_da_offset(da, len) m26_sms_oa_offset(da, len)

/**************************************************************************************
* FunctionName   : m26_sms_input()
* Description    : 模块短信数据返回处理
* EntryParameter : smsid，新短信存储位置
* ReturnValue    : 返回None
**************************************************************************************/
static void m26_sms_input(uint8_t *pdu, int len)
{
    int i;
    uint8_t call_len = 0;
    uint8_t udl,dcs;
    char telphone[20] = {0};

    //跳过SCA
    pdu += m26_sms_sca_offset(pdu, len);
    //跳过FO
    pdu += SMS_FO_OFFSET;
    //跳过OA
    call_len = m26_sms_oa_offset(pdu, len);
    memcpy(telphone, pdu + 4, call_len - 4);
    for(i = 0;i < call_len - 4; i+=2) {
    	telphone[i] ^= telphone[i+1];
    	telphone[i+1] ^= telphone[i];
    	telphone[i] ^= telphone[i+1];
    }
    telphone[i - m26_shex2int8(pdu)%2] = '\0';
    pdu += call_len;
    //跳过PID
    pdu += SMS_PID_OFFSET;
    dcs = m26_shex2int8(pdu);
    //跳过DCS+SCTS=1+7
    pdu +=SMS_DCS_OFFSET+SMS_SCTS_OFFSET;
    //跳过UDL
    udl = m26_shex2int8(pdu);
    pdu += SMS_UDL_OFFSET;
    len = m26_shex2sbits(pdu, strlen((char *)pdu));

    switch (dcs) {
        case SMS_DCS_UCS2_NOCLASS:
        case SMS_DCS_UCS2_CLASS0:
        case SMS_DCS_UCS2_CLASS1:
        len = ucs2_to_gb2312(pdu, pdu, len);
            break;
        default:break;
    }
    (void)udl;
    pdu[len] = '\0';
    printf("tel:+%s class:%02X send SMS(GB2312):%s\n",telphone, dcs, pdu);
}

int main(int argc, char *argv[])
{
    if(argc != 2) exit(0);
    
    // argv[1] 是从MODEM读取的PDU格式的短信数据
    m26_sms_input((uint8_t *)argv[1], strlen(argv[1]));

    return 0;
}
