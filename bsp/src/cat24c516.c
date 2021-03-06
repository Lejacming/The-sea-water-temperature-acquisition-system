#include "cat24c516.h"
#include  "stm32l1xx.h"

static  void IIC_Wait()
{
    uint8_t i = 5                                          ;
    while(i--)                                             {
                                                           }
}
void IIC_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure                   ;   
    RCC_AHBPeriphClockCmd(IIC_Pin_CLOCK, ENABLE)           ;     
    GPIO_InitStructure.GPIO_Pin = IIC_SDA_Pin              ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT           ;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD          ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz       ;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP            ;
    GPIO_Init(IIC_SDA_PType, &GPIO_InitStructure)          ;
    
    GPIO_InitStructure.GPIO_Pin = IIC_SCL_Pin              ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT           ;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD          ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz       ;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP            ;
    GPIO_Init(IIC_SCL_PType, &GPIO_InitStructure)          ;
    
    GPIO_InitStructure.GPIO_Pin = IIC_WP_Pin               ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT           ;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP          ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz       ;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP            ;
    GPIO_Init(IIC_WP_PType, &GPIO_InitStructure)           ;
    
    SCL_H()                                                ;
    SDA_H()                                                ;
    WP_H()                                                 ;
}


/*******************      IIC开始     *****************************/
//_____________                 
//             \____________                SCL
//_________
//         \________________                SDA
//
/******************************************************************/

static  void IIC_Start(void)   //__inline
{
   SDA_Out()                                                      ;
   SDA_H()                                                        ;
   SCL_H()                                                        ;
   delay_us(1)                                                    ;
   SDA_L()                                                        ;
   delay_us(1)                                                    ;
   //钳住I2C总线，准备发送或接收数据
   SCL_L()                                                        ;                       
}


/*******************      II结束     *****************************/
//        _____________
//_______/                             SCL
//          ___________          
//_________/                           SDA
//
/******************************************************************/
static  void IIC_Stop(void)
{
    SDA_Out()                                                      ;
    SDA_L()                                                        ;
    SCL_H()                                                        ;
    delay_us(1)                                                    ;
    SDA_H()                                                        ;
    delay_us(1)                                                    ;
    SCL_L()                                                        ;
}

static  void IIC_Ask(void)
{
    SDA_L()                                                        ;   
    delay_us(1)                                                    ;
    SCL_H()                                                        ;
    delay_us(1)                                                    ;
    SCL_L()                                                        ;
    delay_us(1)                                                    ;
}

static  void IIC_NoAsk(void)
{
    SDA_H()                                                        ;   
    delay_us(1)                                                    ;
    SCL_H()                                                        ;
    delay_us(1)                                                    ;
    SCL_L()                                                        ;
    delay_us(1)                                                    ;
}

static  uint8_t IIC_ISAsk(void)
{
    uint8_t flag = 1                                               ; 
    SDA_H()                                                        ;
    delay_us(1)                                                    ;
    SCL_H()                                                        ;
    delay_us(1)                                                    ;
    SDA_In()                                                       ;
    if(1 == GPIO_ReadInputDataBit(IIC_SDA_PType, IIC_SDA_Pin))     {
        flag = 0                                                   ;
                                                                   }
    SCL_L()                                                        ;
    SDA_Out()                                                      ;   
    IIC_Wait()                                                     ;     
    return  flag                                                   ;
}

static uint8_t IIC_Read_Byte()
{
    uint8_t  data = 0                                              ; 
    uint8_t  i                                                     ;
    SDA_H()                                                        ;
    SDA_In()                                                       ;                                                 
    for(i = 0; i < 8; i++)                                         {
        data = data << 1                                           ;
        SCL_L()                                                    ;
        delay_us(1)                                                ;
        SCL_H()                                                    ;
        IIC_Wait()                                                 ;
        data |= GPIO_ReadInputDataBit(IIC_SDA_PType, IIC_SDA_Pin)  ;
        delay_us(1)                                                ;
                                                                   }
    SCL_L()                                                        ;
    IIC_Wait()                                                     ;
    SDA_Out()                                                      ; 
    return data                                                    ;
}

static uint8_t IIC_Send_Byte(uint8_t data)
{
    uint8_t i                                                      ; 
    for(i = 0; i < 8; i++)                                         {
        if(((data<<i) & 0x80) == 0x80)                             {
            SDA_H()                                                ;
                                                                   }
        else                                                       {
            SDA_L()                                                ;
                                                                   }
        IIC_Wait()                                                 ;
        SCL_H()                                                    ;
        delay_us(1)                                                ;
        SCL_L()                                                    ;
        delay_us(1)                                                ;
                                                                   }
    if(IIC_ISAsk() == 0)                                           {
        return 0;
                                                                   }
    return  1                                                      ;                                                                   
}

void CAT24C_Word_W(uint16_t word_adds, uint8_t * words, uint16_t count)
{
    uint8_t  i                                                     ;
    WP_L()                                                         ;
    for(i = 0; i< count; i++)                                      {
        IIC_Start()                                                ;
        IIC_Send_Byte(Address_Write)                               ;
        IIC_Send_Byte(word_adds >> 8)                              ;                                          
        IIC_Send_Byte(word_adds & 0xff)                            ;
        IIC_Send_Byte(words[i])                                    ;
        IIC_Stop()                                                 ;
        word_adds++                                                ;
        //必须延时3ms以上
        delay_ms(3)                                                ;
                                                                   }
    WP_H()                                                         ;
}

void CAT24C_Page_W(uint16_t word_adds, uint8_t * words, uint8_t count)
{
    uint8_t  i                                                     ;
    if(count > 64)    return                                       ;  
    WP_L()                                                         ;    
    IIC_Start()                                                    ;
    IIC_Send_Byte(Address_Write)                                   ;
    IIC_Send_Byte(word_adds >> 8)                                  ;                                          
    IIC_Send_Byte(word_adds & 0xff)                                ;
    for(i = 0; i < count; i++)                                     {       
        IIC_Send_Byte(*words)                                      ;
        words++                                                    ;
                                                                   }
    IIC_Stop()                                                     ;
    WP_H()                                                         ;
}

void CAT24C_Selective_R(uint16_t word_adds, uint8_t * words, uint8_t count)
{
    uint16_t  i                                                    ;
    for(i = 0; i < count; i++)                                     {
        IIC_Start()                                                ;
        IIC_Send_Byte(Address_Write)                               ;
        IIC_Send_Byte(word_adds >> 8)                              ;                                          
        IIC_Send_Byte(word_adds & 0xff)                            ;
        IIC_Start()                                                ;
        IIC_Send_Byte(Address_Read)                                ;        
        words[i] = IIC_Read_Byte()                                 ;
        IIC_NoAsk()                                                ;        
        IIC_Stop()                                                 ;
        delay_us(10)                                               ;
        word_adds++                                                ;
                                                                   }
}


void CAT24C_Sequential_R(uint16_t word_adds, uint8_t * words, uint8_t count)
{
    uint8_t  i                                                     ;    
    IIC_Start()                                                    ;
    IIC_Send_Byte(Address_Write)                                   ;
    IIC_Send_Byte(word_adds >> 8)                                  ;                                          
    IIC_Send_Byte(word_adds & 0xff)                                ;
    IIC_Start()                                                    ;
    IIC_Send_Byte(Address_Read)                                    ;
    for(i = 0; i < (count-1); i++)                                 {       
        *words = IIC_Read_Byte()                                   ;
        IIC_Ask()                                                  ;  
        words++                                                    ;
                                                                   }
    *words = IIC_Read_Byte()                                       ;
    IIC_NoAsk()                                                    ;  
    IIC_Stop()                                                     ;
}

void CAT24C_Mask()
{
    uint8_t  i                                                     ;
    uint16_t word_adds;
    WP_L()                                                         ;    
    IIC_Start()                                                    ;                              ;
    IIC_Stop()                                                     ;
    WP_H()                                                         ;
}

