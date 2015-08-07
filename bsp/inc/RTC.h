#ifndef _RTC_H
#define _RTC_H

#include "bsp.h"

// #define  RTC_CLOCK_SOURCE_LSI /* 用内置的32K 时钟晶振源 */
#define  RTC_CLOCK_SOURCE_LSE /* 用外置的32.768K 时钟晶振源 */

extern volatile uint32_t Alarmk_TIME ;  //闹钟定时时间

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
