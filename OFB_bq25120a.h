#ifndef BQ25120A_H_
#define BQ25120A_H_

#include "stm32l0xx_hal.h"
#include "stdbool.h"

#include "main.h"

/*
////// APPLICATION SPECIFIC INFORMATION - CAN BE CHANGED - It is example for Init_bq25120a struct!!!
#define APP_FAST_CHARGE_CURR            10                //fast charge current = 100 mA
#define APP_TERMINATION_CURR            2                 //termination current = 20 mA
#define APP_SW_VOLTAGE_MV               1450              //1.45V SW output
#define ENABLE_LDO                      LDO_DISABLE        //LDO Enabled selected
#define APP_LDO_VOLTAGE                 LDO_3000_mV       //LDO output 3V//ldo 3V
#define APP_MRRESET_VIN_CFG             MRRESET_VIN_0
#define APP_PUSH_BUTTON_CONFIG          MRWAKE1_600_MS|MRWAKE2_1500_MS|MRREC_SHIPMODE|MRRESET_9s|PGB_MR_MR
#define APP_PUSH_BUTTON_CONFIG_2        MRWAKE1_600_MS|MRWAKE2_1500_MS|MRREC_SHIPMODE|MRRESET_15s|PGB_MR_PG
#define APP_INLIM                       INLIM_300_mA      //Input limit current 300mA
#define APP_BUVLO                       BUVLO_3V          //Buvlo 3V
#define APP_WDT_RESET_ENABLE            WDT_RESET_ENABLE// Wdt reset timer enabled
#define APP_WDT_INTERVAL		40 //	40 second

*/

//////INITILIAZER STRUCTURE
struct Init_bq25120a{

 uint8_t fast_charge_curr;
 uint8_t termination_curr;
 
 uint8_t enable_buck;
 uint16_t buck_voltage_range;
 uint16_t buck_voltage;	
 bool enable_ldo;
 uint16_t ldo_voltage;

 bool mrreset_vin_vfg;
 
 uint8_t push_button_cfg;    
 uint16_t input_curr;      
 uint8_t buvlo_cfg;

 bool wdt_enable; //watchdog enable selectivity
 uint8_t wdt_interval;

 struct Comm_functions functions;
 
};

struct Comm_functions{
	
  bool (*I2C_Write)(uint16_t DevAddress, uint16_t MemAddress,uint16_t MemAddSize, uint8_t *pData, uint16_t Size); 
  bool (*I2C_Read)(uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

  bool (*GPIO_Write_High)(uint8_t pin);
  bool (*GPIO_Write_Low)(uint8_t pin);

  bool (*GPIO_Read_Pin)(uint8_t pin);

};


//////MACRO FUNCTIONS FOR BQ25120A
//#define CONCAT_TIMER                    TIMER##SELECTED_WDT_RESET_TIMER##_IRQn


////// BQ25120A I2C ADDRESS
#define ADDR_BQ25120A                   (0x6A << 1)


//////BQ25120A CONTROL PINS
#define BQ25120A_LDOCTRL_Pin            0
#define BQ25120A_CD_Pin                 1
#define BQ25120A_MR_PIN                 5

//////STATUS DEFINITIONS
#define ENTER_SHIP_MODE                 1
#define EXIT_SHIP_MODE                  0


//////FAST CHARGE DEFINITIONS
#define ICHRG_RANGE_BIT_POS             0x07
#define ichrg_CODE_POS                  0x02


//////TERMINATION CURRENT DEFINITONS
#define IPRETERM_RANGE_BIT_POS          0x07
#define IPRETERM_CODE_POS               0x02
#define TERMINATION_CURR_ENABLE_POS     0x01


//////SW DEFINITIONS
#define EN_SYS_OUT_BIT_POS              0x07
#define SYS_SEL_BIT_POS                 0x05
#define SYS_VOUT_BIT_POS                0x01


#define SW_ENABLE                       0x01 << EN_SYS_OUT_BIT_POS
#define SW_DISABLE                      0x00 << EN_SYS_OUT_BIT_POS

#define SW_1100_mV                      1100
#define SW_1300_mV                      1300
#define SW_1500_mV                      1500
#define SW_1800_mV                      1800


//////LDO DEFINITIONS
#define EN_LS_LDO_BIT_POS               0x07
#define LS_LDO_BIT_POS                  0x02
#define MR_RESET_VIN_BIT_POS            0x00


#define LDO_ENABLE                      0x01 << EN_LS_LDO_BIT_POS
#define LDO_DISABLE                     0x00 << EN_LS_LDO_BIT_POS

#define LDO_MIN_mV                      800
#define LDO_900_mV                      900
#define LDO_1000_mV                     1000
#define LDO_1100_mV                     1100
#define LDO_1200_mV                     1200
#define LDO_1300_mV                     1300
#define LDO_1400_mV                     1400
#define LDO_1500_mV                     1500
#define LDO_1600_mV                     1600
#define LDO_1700_mV                     1700
#define LDO_1800_mV                     1800
#define LDO_1900_mV                     1900
#define LDO_2000_mV                     2000
#define LDO_2100_mV                     2100
#define LDO_2200_mV                     2200
#define LDO_2300_mV                     2300
#define LDO_2400_mV                     2400
#define LDO_2500_mV                     2500
#define LDO_2600_mV                     2600
#define LDO_2700_mV                     2700
#define LDO_2800_mV                     2800
#define LDO_2900_mV                     2900
#define LDO_3000_mV                     3000
#define LDO_3100_mV                     3100
#define LDO_3200_mV                     3200
#define LDO_MAX_mV                      3300

#define MRRESET_VIN_0                   0x00 << MR_RESET_VIN_BIT_POS
#define MRRESET_VIN_1                   0x01 << MR_RESET_VIN_BIT_POS


//////PUSH-BUTTON CONTROL DEFINITIONS
#define MRWAKE1_BIT_POS                 0x07
#define MRWAKE2_BIT_POS                 0x06
#define MRREC_BIT_POS                   0x05
#define MRRESET_BIT_POS                 0x03
#define PGB_MR_BIT_POS                  0x02


#define MRWAKE1_80_MS                   0x00 << MRWAKE1_BIT_POS
#define MRWAKE1_600_MS                  0x01 << MRWAKE1_BIT_POS

#define MRWAKE2_1000_MS                 0x00 << MRWAKE2_BIT_POS
#define MRWAKE2_1500_MS                 0x01 << MRWAKE2_BIT_POS

#define MRREC_SHIPMODE                  0x00 << MRREC_BIT_POS
#define MRREC_HIZ_MODE                  0x01 << MRREC_BIT_POS

#define MRRESET_5s                      0x00 << MRRESET_BIT_POS
#define MRRESET_9s                      0x01 << MRRESET_BIT_POS
#define MRRESET_11s                     0x02 << MRRESET_BIT_POS
#define MRRESET_15s                     0x03 << MRRESET_BIT_POS

#define PGB_MR_PG                       0x00 << PGB_MR_BIT_POS
#define PGB_MR_MR                       0x01 << PGB_MR_BIT_POS


//////ILIM AND BATTERY UVLO CONTROL DEFINITIONS
#define RESET_BIT_POS                   0x07
#define INLIM_BIT_POS                   0x03
#define BUVLO_BIT_POS                   0x00

#define NO_EFFECT_RESET                 0x00 << RESET_BIT_POS
#define RESET_ALL_REG                   0x01 << RESET_BIT_POS

#define INLIM_50_mA                     50
#define INLIM_100_mA                    100
#define INLIM_150_mA                    150
#define INLIM_200_mA                    200
#define INLIM_250_mA                    250
#define INLIM_300_mA                    300
#define INLIM_350_mA                    350
#define INLIM_400_mA                    400

#define BUVLO_2_2V                      0x07 << BUVLO_BIT_POS
#define BUVLO_2_2V_R                    0x06 << BUVLO_BIT_POS
#define BUVLO_2_4V                      0x05 << BUVLO_BIT_POS
#define BUVLO_2_6V                      0x04 << BUVLO_BIT_POS
#define BUVLO_2_8V                      0x03 << BUVLO_BIT_POS
#define BUVLO_3V                        0x02 << BUVLO_BIT_POS


//////VOLTAGE BASED BATTERY MONITOR DEFINITIONS
#define VBMON_READ_BIT_POS              0x07
#define VBMON_RANGE_BIT_POS             0x05
#define VBMON_TH_BIT_POS                0x02

#define VBMON_RANGE_BIT_MASK            0x03
#define VBMON_TH_BIT_MASK               0x07

#define VBATREG_PERCENT_60              0x00
#define VBATREG_PERCENT_70              0x01
#define VBATREG_PERCENT_80              0x02
#define VBATREG_PERCENT_90              0x03

#define VBMON_RANGE_0                   0x01
#define VBMON_RANGE_2                   0x02
#define VBMON_RANGE_4                   0x03
#define VBMON_RANGE_6                   0x06
#define VBMON_RANGE_8                   0x07

#define WDT_RESET_DISABLE               0x00
#define WDT_RESET_ENABLE                0x01
#define TIMER_100MS_VAL                 0.02
#define TIMER_CFG_                      TIMER_FREE_RUN | TIMER_SLOWCLK_DIV2 | TIMER_PRESCALE_1

#define VBATMON_NOT_MEASURED_FLAG       0
#define VBATMON_MEASURED_FLAG           1

#define BQ25120A_NOT_RESET_STATE        0
#define BQ25120A_RESET_STATE            1
//////Register address of BQ25120a

#define STATUS_SHIP_REG                 0x00
#define FAULTS_FAULTS_MASK_REG          0x01
#define TS_CTRL_FAULTS_MASK_REG         0x02
#define FAST_CHARGE_REG                 0x03
#define TERMINATION_PRECHARGE_REG       0x04
#define BATTERY_VOLTAGE_CTRL_REG        0x05
#define SYS_VOUT_CTRL_REG               0x06
#define LOADSW_LDO_CTRL_REG             0x07
#define PUSH_BUTTON_CTRL_REG            0x08
#define ILIM_UVLO_CTRL_REG              0x09
#define VOLTAGE_BATMON_REG              0x0A
#define VINDPM_TIMERS_REG               0x0B




typedef enum {

    BQ25120A_OK, INIT_ERR,

    CHARGE_CTRL_OK, CHARGE_CTRL_ERR,

    REGULATION_OK, REGULATION_ERR,

    SW_OUTPUT_OK, SW_OUTPUT_ERR,

    LDO_OK, LDO_VOLTAGE_ERR, LDO_MRRESET_ERR,

    ILIM_OK,  ILIM_RESET_ERR, ILIM_CURR_ERR, ILIM_BUVLO_ERR,

    BATMON_INT_OK,  BATMON_INT_ERR,

}STATE_BQ25120A;


STATE_BQ25120A Init_BQ25120A(uint8_t wdt_reset_state);
void BQ25120A_ENTER_SHIP_MODE(uint8_t ship_mode_sel);
STATE_BQ25120A BQ25120A_Charge_Control(uint8_t fastcharge_current, float termination_current);
STATE_BQ25120A BQ25120A_Battery_Voltage_Control(uint16_t bat_voltage);
STATE_BQ25120A BQ25120A_Set_SW_Voltage(uint8_t enable_sw,uint32_t voltage_range,uint16_t sw_voltage);
STATE_BQ25120A BQ25120A_Set_LDO(uint8_t enable_ldo,uint32_t ldo_voltage,uint8_t MRRESET_cfg);
void BQ25120A_MR_Ctrl(uint8_t MR_cfg);
STATE_BQ25120A BQ25120A_Ilim_Uvlo_Ctrl(uint8_t reset,uint16_t input_current, uint8_t buvlo);
STATE_BQ25120A BQ25120A_BatmonINT_Init(uint8_t timer_period);
void BQ25120A_Clear_Flags();
bool BQ25120A_Check_Valid_VinSource(GPIO_TypeDef* PG_PORT,uint32_t PG_pin_dio);
void BQ25120A_Reset_Config();
uint8_t BQ25120A_Read_Batt_Voltage();
void TIMER2_IRQHandler();
void BQ25120A_I2C_WRITE(uint8_t slaveaddr, uint8_t regaddr, uint8_t data);
uint8_t BQ25120A_I2C_READ(uint8_t regaddr);


#endif
