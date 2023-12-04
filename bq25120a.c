#include <bq25120a.h>
#include "stdbool.h"

uint8_t VBATMON_value = 0;
volatile uint8_t VBATMON_flag = VBATMON_NOT_MEASURED_FLAG;
volatile uint8_t BQ25120A_RESET_flag = BQ25120A_NOT_RESET_STATE;



extern I2C_HandleTypeDef hi2c1;

static HAL_StatusTypeDef (*I2C_Write)(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout) = HAL_I2C_Mem_Write;
static HAL_StatusTypeDef (*I2C_Read)(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress, uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout)= HAL_I2C_Mem_Read;


 STATE_BQ25120A Init_BQ25120A(uint8_t wdt_reset_state){


    BQ25120A_I2C_WRITE(ADDR_BQ25120A, TS_CTRL_FAULTS_MASK_REG, 0x08); //Close TS functionality

    BQ25120A_ENTER_SHIP_MODE(EXIT_SHIP_MODE);

    BQ25120A_Battery_Voltage_Control(5);

    BQ25120A_Charge_Control(APP_FAST_CHARGE_CURR,APP_TERMINATION_CURR); // fast charge current = 50 mA, termination current = 5 mA

    BQ25120A_Set_SW_Voltage(SW_ENABLE,SW_1100_mV,APP_SW_VOLTAGE_MV); //1.8V for RSL10, MAX9880A, LEVEL SHIFTER

    BQ25120A_Set_LDO(LDO_ENABLE,APP_LDO_VOLTAGE,APP_MRRESET_VIN_CFG); // 3.3V for Oled and level shifter

    BQ25120A_MR_Ctrl(APP_PUSH_BUTTON_CONFIG);

    BQ25120A_Ilim_Uvlo_Ctrl(NO_EFFECT_RESET,APP_INLIM,APP_BUVLO);

    if(wdt_reset_state == WDT_RESET_ENABLE) BQ25120A_BatmonINT_Init(APP_WDT_RESET_PERIOD);

    BQ25120A_Read_Batt_Voltage();

    //BQ25120A_I2C_WRITE(ADDR_BQ25120A, TS_CTRL_FAULTS_MASK_REG, 0x08); //Close TS functionality

    BQ25120A_I2C_WRITE(ADDR_BQ25120A, VINDPM_TIMERS_REG, 0x46);//Safety timer closed


    return BQ25120A_OK;
}

void BQ25120A_ENTER_SHIP_MODE(uint8_t ship_mode_sel){

    if(ship_mode_sel == ENTER_SHIP_MODE){


       HAL_GPIO_WritePin(BQ_CD_GPIO_Port,BQ_CD_Pin, GPIO_PIN_SET); //to enable active battery mode for ı2c and disable charge //To enter Ship Mode drive CD pin high
       HAL_Delay(5);
       BQ25120A_I2C_WRITE(ADDR_BQ25120A,STATUS_SHIP_REG,0x20);//BQ25120A_I2C_WRITE(ADDR_BQ25120A, STATUS_SHIP_REG, 0x20); //Enabled ship mode


    }else{

        BQ25120A_I2C_WRITE(ADDR_BQ25120A, STATUS_SHIP_REG, 0x00); //Disabled ship mode

    }

}

STATE_BQ25120A BQ25120A_Charge_Control(uint8_t fastcharge_current, float termination_current){

    if( fastcharge_current < 5 || fastcharge_current > 300 ) return CHARGE_CTRL_ERR;
    if( termination_current < 0.5 || termination_current > 37.0 ) return CHARGE_CTRL_ERR;

    uint8_t ICHRG_RANGE_BIT = 0;
    uint8_t ichrg_CODE = 0;

    if(fastcharge_current <= 35){

        ICHRG_RANGE_BIT = 0;
        ichrg_CODE = (fastcharge_current - 5) / 1;

    }else{

        ICHRG_RANGE_BIT = 1;
        ichrg_CODE = (fastcharge_current - 40) / 10;

    }

    uint32_t fast_charge_data =  (ICHRG_RANGE_BIT << ICHRG_RANGE_BIT_POS) | (ichrg_CODE << ichrg_CODE_POS) | 0; // charger enabled,not high impadence mode
    BQ25120A_I2C_WRITE(ADDR_BQ25120A,FAST_CHARGE_REG,fast_charge_data); //50mA fast charge current, charge enable, not high impadance mode

    uint8_t IPRETERM_RANGE_BIT = 0;
    uint8_t IPRETERM_CODE = 0;

    if(termination_current <= 5){

        IPRETERM_RANGE_BIT = 0;
        IPRETERM_CODE = (uint8_t)((termination_current - 0.5) / 0.5);

    }else{

        IPRETERM_RANGE_BIT = 1;
        IPRETERM_CODE = (uint8_t)((termination_current - 6.0) / 1.0);

    }

    uint8_t termination_charge_data = (IPRETERM_RANGE_BIT << IPRETERM_RANGE_BIT_POS) | (IPRETERM_CODE << IPRETERM_CODE_POS) | (1 << TERMINATION_CURR_ENABLE_POS ); //termination current enable
    BQ25120A_I2C_WRITE(ADDR_BQ25120A,TERMINATION_PRECHARGE_REG,termination_charge_data); //5 mA termination current, termination current enabled


    return CHARGE_CTRL_OK;
}


STATE_BQ25120A BQ25120A_Battery_Voltage_Control(uint16_t bat_voltage){

    BQ25120A_I2C_WRITE(ADDR_BQ25120A,BATTERY_VOLTAGE_CTRL_REG,0x78); // Set battery voltage control to 4.2V

    return SW_OUTPUT_OK;
}

STATE_BQ25120A BQ25120A_Set_SW_Voltage(uint8_t enable_sw,uint32_t voltage_range,uint16_t sw_voltage){

    uint32_t voltage_range_bit = 0;

    switch(voltage_range){
        case SW_1100_mV:
            voltage_range_bit = 0;
            break;
        case SW_1300_mV:
            voltage_range_bit = 1;
            break;
        case SW_1500_mV:
            voltage_range_bit = 2;
            break;
        case SW_1800_mV:
            voltage_range_bit = 3;
            break;
        default:
            return SW_OUTPUT_ERR;

    }

    uint32_t sw_voltage_diff = sw_voltage - voltage_range;
    uint32_t sw_data =  enable_sw | (((voltage_range_bit << SYS_SEL_BIT_POS )) | ((sw_voltage_diff / 100) << SYS_VOUT_BIT_POS)); //128 for enable SW output
    BQ25120A_I2C_WRITE(ADDR_BQ25120A,SYS_VOUT_CTRL_REG,sw_data);

    HAL_Delay(1000);//wait for other IC

    return SW_OUTPUT_OK;
}


STATE_BQ25120A BQ25120A_Set_LDO(uint8_t enable_ldo,uint32_t ldo_voltage,uint8_t MRRESET_cfg){

    uint32_t ldo_data;

    if(enable_ldo == LDO_ENABLE){

        if(ldo_voltage > LDO_MAX_mV || ldo_voltage < LDO_MIN_mV) return LDO_VOLTAGE_ERR;
        if(MRRESET_cfg != MRRESET_VIN_0 && MRRESET_cfg != MRRESET_VIN_1) return LDO_MRRESET_ERR;

        ldo_data = ((ldo_voltage - LDO_MIN_mV )/ 100) ;
        ldo_data = LDO_ENABLE | (ldo_data << LS_LDO_BIT_POS) | MRRESET_cfg; // 128 to ldo enable

        BQ25120A_I2C_WRITE(ADDR_BQ25120A,LOADSW_LDO_CTRL_REG,0x00); //Disabled LDO output over I2C /////close it
        HAL_GPIO_WritePin(BQ_LSCTRL_GPIO_Port,BQ_LSCTRL_Pin,GPIO_PIN_RESET); //Disabled LDO Output over LSCTRL Pin

        HAL_Delay(100);

        BQ25120A_I2C_WRITE(ADDR_BQ25120A,LOADSW_LDO_CTRL_REG,ldo_data); //Enabled LDO output and set ldo output to ldo_voltage
        HAL_GPIO_WritePin(BQ_LSCTRL_GPIO_Port,BQ_LSCTRL_Pin, GPIO_PIN_SET); //Enabled LDO output
        HAL_Delay(100);//wait for rise ldo output and other IC

    }else{

        BQ25120A_I2C_WRITE(ADDR_BQ25120A,LOADSW_LDO_CTRL_REG,0x00);
        HAL_GPIO_WritePin(BQ_LSCTRL_GPIO_Port,BQ_LSCTRL_Pin,GPIO_PIN_RESET); //Disabled LDO Output over LSCTRL Pin
        HAL_Delay(100);
    }

    return LDO_OK;
}

void BQ25120A_MR_Ctrl(uint8_t MR_cfg){

    BQ25120A_I2C_WRITE(ADDR_BQ25120A,PUSH_BUTTON_CTRL_REG,MR_cfg);

}

STATE_BQ25120A BQ25120A_Ilim_Uvlo_Ctrl(uint8_t reset,uint16_t input_current, uint8_t buvlo){

    if(reset != NO_EFFECT_RESET && reset != RESET_ALL_REG) return ILIM_RESET_ERR;
    if(input_current > INLIM_400_mA || input_current < INLIM_50_mA) return ILIM_CURR_ERR;
    if(buvlo > BUVLO_2_2V || buvlo < BUVLO_3V) return ILIM_BUVLO_ERR;

    uint32_t inlimCODE = (input_current - 50) / 50 ;
    uint32_t ilim_data = reset | inlimCODE << INLIM_BIT_POS |  buvlo ;

    BQ25120A_I2C_WRITE(ADDR_BQ25120A, ILIM_UVLO_CTRL_REG, ilim_data);

    return ILIM_OK;
}

STATE_BQ25120A BQ25120A_BatmonINT_Init(uint8_t timer_period){


    //shot timer interrupt to 20 seconds


    return BATMON_INT_OK;
}

void BQ25120A_Clear_Flags(){

    /*uint32_t status_arr[2]={0,0};
    status_arr[0]=BQ25120A_I2C_READ(STATUS_SHIP_REG);
    status_arr[1]=BQ25120A_I2C_READ(FAULTS_FAULTS_MASK_REG);*/

}

bool BQ25120A_Check_Valid_VinSource(GPIO_TypeDef* PG_PORT,uint32_t PG_pin_dio){

    BQ25120A_MR_Ctrl(APP_PUSH_BUTTON_CONFIG_2); //change PG pin to not reflects MR pin setting
    HAL_Delay(50);

    bool input_state = (bool)HAL_GPIO_ReadPin(PG_PORT, BQ_PG_Pin);

    BQ25120A_MR_Ctrl(APP_PUSH_BUTTON_CONFIG); //change PG  pin to reflects MR pin setting

    HAL_Delay(20); //we wait due to that  I2C function affect to INT signal state!

    if(input_state){

        return false; //there is not valid Vin source

    }else{

        return true; //there is valid Vin source

    }

}

void BQ25120A_Reset_Config(){


        BQ25120A_I2C_WRITE(ADDR_BQ25120A, TS_CTRL_FAULTS_MASK_REG, 0x08); //Close TS functionality

        BQ25120A_ENTER_SHIP_MODE(EXIT_SHIP_MODE);

        BQ25120A_Battery_Voltage_Control(5);

        BQ25120A_Charge_Control(APP_FAST_CHARGE_CURR,APP_TERMINATION_CURR); // fast charge current = 50 mA, termination current = 5 mA

        BQ25120A_MR_Ctrl(APP_PUSH_BUTTON_CONFIG);

        BQ25120A_Ilim_Uvlo_Ctrl(NO_EFFECT_RESET,APP_INLIM,APP_BUVLO);

        BQ25120A_I2C_WRITE(ADDR_BQ25120A, TS_CTRL_FAULTS_MASK_REG, 0x08); //Close TS functionality

        BQ25120A_I2C_WRITE(ADDR_BQ25120A, VINDPM_TIMERS_REG, 0x46);//Safety timer closed


}

uint8_t BQ25120A_Read_Batt_Voltage(){

    BQ25120A_I2C_WRITE(ADDR_BQ25120A,VOLTAGE_BATMON_REG,0x80); // Initiate new battery voltage measurement process
    HAL_Delay(50);//some delay added for reading VBAT operation
    uint32_t batmon_data = BQ25120A_I2C_READ(VOLTAGE_BATMON_REG);

    switch((batmon_data >> VBMON_RANGE_BIT_POS) & VBMON_RANGE_BIT_MASK){
       case VBATREG_PERCENT_90:
          VBATMON_value = 90;
          break;
       case VBATREG_PERCENT_80:
          VBATMON_value = 80;
          break;
       case VBATREG_PERCENT_70:
          VBATMON_value = 70;
          break;
       case VBATREG_PERCENT_60:
          VBATMON_value = 60;
          break;
       default:
          VBATMON_value = 0;
          break;
    }

    switch((batmon_data >> VBMON_TH_BIT_POS ) & VBMON_TH_BIT_MASK){
       case VBMON_RANGE_8:
          VBATMON_value = VBATMON_value + 8;
          break;
       case VBMON_RANGE_6:
          VBATMON_value = VBATMON_value + 6;
          break;
       case VBMON_RANGE_4:
          VBATMON_value = VBATMON_value + 4;
          break;
       case VBMON_RANGE_2:
          VBATMON_value = VBATMON_value + 2;
          break;
       case VBMON_RANGE_0:
          VBATMON_value = VBATMON_value + 0;
          break;
       default:
           VBATMON_value = 0;
           break;
    }



    return VBATMON_value;
}

void TIMER2_IRQHandler(){

	//shot timer stopped

    VBATMON_flag = VBATMON_MEASURED_FLAG; // we set VBATMON_flag to that value  to reset wdt and read battery voltage every 20 second

    //shot timer enabled again

}

void BQ25120A_I2C_WRITE(uint8_t slaveaddr, uint8_t regaddr, uint8_t data){

	HAL_GPIO_WritePin(BQ_CD_GPIO_Port,BQ_CD_Pin, GPIO_PIN_SET); //to enable active battery mode for ı2c and disable charge
	HAL_Delay(10);
	I2C_Write(&hi2c1,slaveaddr,regaddr,1,(uint8_t *)&data,1,500);
	HAL_Delay(10);
	HAL_GPIO_WritePin(BQ_CD_GPIO_Port,BQ_CD_Pin, GPIO_PIN_RESET); //to disable active battery mode for enable charge

}

uint8_t BQ25120A_I2C_READ(uint8_t regaddr){

    uint8_t rx_data[2] = {0};
    HAL_GPIO_WritePin(BQ_CD_GPIO_Port,BQ_CD_Pin, GPIO_PIN_SET); //to enable active battery mode for ı2c and disable charge
    HAL_Delay(10);
    I2C_Read(&hi2c1,ADDR_BQ25120A,regaddr,1,(uint8_t *)&rx_data,1,500);
    HAL_Delay(10);
    HAL_GPIO_WritePin(BQ_CD_GPIO_Port,BQ_CD_Pin, GPIO_PIN_RESET);  //to disable active battery mode for enable charge

    return rx_data[0];
}



