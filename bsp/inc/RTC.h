#ifndef _RTC_H
#define _RTC_H

#include  "stm32l1xx.h"

#define  RTC_CLOCK_SOURCE_LSI /* �����õ�32K ʱ�Ӿ���Դ */
//#define  RTC_CLOCK_SOURCE_LSE /* �����õ�32.768K ʱ�Ӿ���Դ */

extern volatile uint32_t Alarmk_TIME ;  //���Ӷ�ʱʱ��

typedef struct
{
  unsigned int Year;
  unsigned char Month;
  unsigned char Day;
  unsigned char Hour;
  unsigned char Min;
  unsigned char Sec;
  unsigned char Week;
} Time;

extern volatile uint32_t Clock_count;
extern Time Set_Clock;


static void RTC_Configuration(void);
static void RTC_NVIC_Configuration(void);
static void RTC_Alarm_Exit(void);

extern void Set_Time(uint8_t * Dat_Time);
extern Time Get_Time(void);
extern void clock_ini(void);
extern void Set_Alarm_Time(uint8_t * Dat_Time,uint8_t Min);
extern void Set_Alarm(uint32_t tm);	
			   
#endif 