#include "clk_conf.h"
#include "tim1.h"
#include "led.h"
#include "iwdg.h"

/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 ***************************
 * 文件名  ：tim1.c
 * 描述    ：定时器TM1配置函数库   
 * 实验平台：iCreate STM8开发板
 * 寄存器版本  ：V1.0.0
 * 作者    ：ling_guansheng  QQ：779814207
 * 博客    ：
 * 修改时间 ：2011-12-20



****************************************************************************************/

#include "tim1.h"

static  u32 TimingDelay; 
/**************************************************************************
 * 函数名：Tim1_Time_Upmode_conf
 * 描述  ：TIM1定时器功能初始化
 * 输入  ：uint8_t TIM2_Prescaler 
           uint16_t TIM2_Period   1～65535
 *
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用 
 *************************************************************************/
void Tim1_Time_Upmode_conf(uint16_t TIM1_Prescaler,
                           uint16_t TIM1_Period,
                           uint8_t TIM1_RepetitionCounter)
{
      /* Set the Autoreload value */
    TIM1_ARRH = (uint8_t)(TIM1_Period >> 8);
    TIM1_ARRL = (uint8_t)(TIM1_Period);

    /* Set the Prescaler value */
    TIM1_PSCRH = (uint8_t)(TIM1_Prescaler >> 8);
    TIM1_PSCRL = (uint8_t)(TIM1_Prescaler);

    /* Select the Counter Mode */
    TIM1_CR1 = (uint8_t)((uint8_t)(TIM1_CR1 & (uint8_t)(~(MASK_TIM1_CR1_CMS  | MASK_TIM1_CR1_DIR)))
                           | (uint8_t)(TIM1_COUNTERMODE_UP));

    /* Set the Repetition Counter value */
    TIM1_RCR = TIM1_RepetitionCounter;
    /* Set the ARPE Bit */
    TIM1_CR1 |= MASK_TIM1_CR1_ARPE;
    /* Enable the Interrupt Upmode sources */
    TIM1_IER |= (uint8_t)TIM1_IT_UPDATE;
    /* set or Reset the CEN Bit */
    TIM1_CR1 |= MASK_TIM1_CR1_CEN;
}
/**************************************************************************
 * 函数名：TimingDelay_Decrement
 * 描述  ：
 * 输入  ：无        
 *
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：中断服务函数调用 
 *************************************************************************/
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
     { 
	TimingDelay--;
      }
}
/**************************************************************************
 * 函数名：delay_ms
 * 描述  ：
 * 输入  ：nTime       
 *
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用 
 *************************************************************************/
void delay_ms( u32 nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/



#pragma vector=0xD
__interrupt void TIM1_UPD_OVF_TRG_BRK_IRQHandler(void)
{
    TimingDelay_Decrement();
    /* Clear the IT pending Bit */
    ReloadDogTime++;
    if(ReloadDogTime==900)//935
    {    
        ReloadDogTime=0;
        IWDG_KR = IWDG_KEY_REFRESH ;
    }
    
    TIM1_SR1 = (uint8_t)(~(uint8_t)TIM1_IT_UPDATE);
}


/******************** (C) COPYRIGHT  风驰iCreate嵌入式开发工作室 ***************************
 * 文件名  ：iwdg.c
 * 描述    ：看门狗函数库   
 * 实验平台：iCreate STM8开发板
 * 寄存器版本  ：V1.0.0
 * 作者    ：ling_guansheng  QQ：779814207
 * 博客    ：
 * 淘宝    ：http://shop71177993.taobao.com/
 *修改时间 ：2011-12-20

****************************************************************************************/
#include "iwdg.h"
#include "led.h"
/**************************************************************************
 * 函数名：IWDG_Configuration
 * 描述  ：看门狗配置函数
 * 输入  ：无
 *
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用 
 *************************************************************************/
void IWDG_Configuration(void)
{
  /* Enable the IWDG*/
  IWDG_KR = IWDG_KEY_ENABLE;
  /* Enable the access to the IWDG registers*/
  IWDG_KR = (uint8_t)IWDG_WriteAccess_Enable; /* Write Access */
  /* Fixe IWDG Reset period */
  IWDG_PR = (uint8_t)IWDG_Prescaler_256;
  IWDG_RLR = (uint8_t)IWDG_Reload;
  /* Refresh IWDG */
  IWDG_KR = IWDG_KEY_REFRESH ;
}
/**************************************************************************
 * 函数名：Test_IWDGReset
 * 描述  ：看门狗测试函数
 * 输入  ：无
 *
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用 
 *************************************************************************/
void Test_IWDGReset(void)
{
  FlagStatus IwdgFlag;

  /*Get IWDG Reset Status */ 
  IwdgFlag = RST_GetFlagStatus(RST_FLAG_IWDGF);
  /* Test if a IWDG Reset has occured */
  if (IwdgFlag)
  {
    LED_ON_OFF();
    /* Clear IWDGF Flag */
    RST_ClearFlag(RST_FLAG_IWDGF);
    
   }
}
/**************************************************************************
 * 函数名：RST_GetFlagStatus
 * 描述  ：复位标志
 * 输入  ：无
 *
 * 输出  ：无
 * 返回  ：复位标志 
 * 调用  ：外部调用 
 *************************************************************************/
FlagStatus RST_GetFlagStatus(uint8_t RST_Flag)
{
  
    /* Get flag status */

    return ((FlagStatus)((uint8_t)RST_SR & (uint8_t)RST_Flag));
}
/**************************************************************************
 * 函数名：RST_ClearFlag
 * 描述  ：清除复位标志
 * 输入  ：无
 *
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用 
 *************************************************************************/
void RST_ClearFlag(uint8_t RST_Flag)
{
    RST_SR = (uint8_t)RST_Flag;
}

/******************* (C) COPYRIGHT 风驰iCreate嵌入式开发工作室 *****END OF FILE****/



int main(void)
{
  /* Infinite loop */

  /*设置内部高速时钟16M为主时钟*/ 
  Clk_conf();
  LED_conf();
  Set_Led_Off();
  Test_IWDGReset();
  IWDG_Configuration();
  Tim1_Time_Upmode_conf(16,1000,0);
  
  
  EnableInterrupt;
  while(1)
  {
    /************标准的51编程风格*************************/
    PD0_out = 0;    /* PD0引脚输出0 */
    delay_ms(1000); 
    PD0_out = 1;    /* PD0引脚输出1 */
    delay_ms(1000); 
  /**************标准的51编程风格*************************/
  }
}



