/**
* @mainpage ZumoBot Project
* @brief    You can make your own ZumoBot with various sensors.
* @details  <br><br>
    <p>
    <B>General</B><br>
    You will use Pololu Zumo Shields for your robot project with CY8CKIT-059(PSoC 5LP) from Cypress semiconductor.This 
    library has basic methods of various sensors and communications so that you can make what you want with them. <br> 
    <br><br>
    </p>
    
    <p>
    <B>Sensors</B><br>
    &nbsp;Included: <br>
        &nbsp;&nbsp;&nbsp;&nbsp;LSM303D: Accelerometer & Magnetometer<br>
        &nbsp;&nbsp;&nbsp;&nbsp;L3GD20H: Gyroscope<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Reflectance sensor<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Motors
    &nbsp;Wii nunchuck<br>
    &nbsp;TSOP-2236: IR Receiver<br>
    &nbsp;HC-SR04: Ultrasonic sensor<br>
    &nbsp;APDS-9301: Ambient light sensor<br>
    &nbsp;IR LED <br><br><br>
    </p>
    
    <p>
    <B>Communication</B><br>
    I2C, UART, Serial<br>
    </p>
*/

#include <project.h>
#include <stdio.h>
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "I2C_made.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "IR.h"
#include "Ambient.h"
#include "Beep.h"
#include "music.h"
#include <math.h>

int rread(void);

/**
 * @file    main.c
 * @brief   
 * @details  ** You should enable global interrupt for operating properly. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

int turn_rate(int speed, int a, int break_factor, int min, int max){
    int x;
    x = (int)( (float)speed / (1.48 - (1 - (( (float)max / min) / 8.1))) ) - 
    (int)(a * break_factor * ((float)max / min));
    
    if(x > 255) x = 255;
    if(x < 0) x = 0;
    
    return x;
}

void should_I_stop(int l, int r, int lm, int rm, int * flag, int * stop){
    if(l > 16000 && r > 16000 && * flag == 0) {
        * flag = * flag + 1; 
        * stop = * stop + 1;
    }
    else if(l < 16000 && r < 16000 && * flag == 1){
        * flag = * flag - 1;
    }
    else if(rm < 5000 && lm < 5000){
            motor_backward(255, 10);
            motor_forward(255, 1);
     
    }
}
///Serious attempt
int main(void){

    struct sensors_ ref;
    struct sensors_ dig;
    CyGlobalIntEnable; 
    UART_1_Start();
  
    sensor_isr_StartEx(sensor_isr_handler);
    
    reflectance_start();

    IR_led_Write(1);
   
    
    //clear the initial sensor state (discharge the capacitor for the first time)
    while(ref.l1<1 && ref.l1<1 && ref.l1<1 && ref.l1<1 && ref.l1<1){
        reflectance_read(&ref);
    }
    
    motor_start();
    
    ///drive up to the first line
    do{
        motor_forward(100, 5);
        reflectance_read(&ref);
    }while( ref.l3 < 15000 && ref.r3 < 15000);
    //*/
    motor_stop();
    
    ///Wait for the IR signal
    while(!(get_IR())){}
    motor_start();
    //motor_forward(255, 150);
    //*/
    
    ///GO!
    int stop = 0;
    int flag = 1;
    int speed, brake_factor;
    
    while(stop <= 1){
      
        speed = 255;
        brake_factor = 190;
       
        
        reflectance_read(&ref);
        
        
        if(19000 < ref.l1 && ref.l1 < 21000 && 19000 < ref.r1 && ref.r1 < 21000){
            
            
            motor_forward(speed, 1);
            reflectance_read(&ref);
            should_I_stop(ref.l3, ref.r3, ref.l1, ref.r1, &flag, &stop);
        }
        else if(ref.l1 != ref.r1){
            //max and min are to calculate the factor of the turn
            //r and l to pick the correct motor to slow down
            int max, min, r = 0, l = 0;
            
            do{
                if(ref.l1 > ref.r1){max = ref.l1; min = ref.r1; l = 1;}
                else{max = ref.r1; min = ref.l1; r = 1;}
                
                
                motor_turn(
                    //scale down speed by factor between 1 and 2
                                                           //further reduce speed to turn
                    turn_rate(speed, l, brake_factor, min, max), 
                    turn_rate(speed, r, brake_factor, min, max), 
                     1);
                reflectance_read(&ref);
                should_I_stop(ref.l3, ref.r3, ref.l1, ref.r1, &flag, &stop);
                //reset direction
                r = 0; l = 0;    
            }while(!(19000 < ref.l1 && ref.l1 < 21000 && 19000 < ref.r1 && ref.r1 < 21000) && stop <= 1);
            
            
        }
        
        
    }
    
    motor_stop();
    return 0;
}
//*/


/*//battery level//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    ADC_Battery_Start();        
    int16 adcresult =0;
    float volts = 0.0;

    printf("\nBoot\n");

    //BatteryLed_Write(1); // Switch led on 
    BatteryLed_Write(0); // Switch led off 
    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board

    for(;;)
    {
        
        ADC_Battery_StartConvert();
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for get ADC converted value
            adcresult = ADC_Battery_GetResult16();
            volts = ADC_Battery_CountsTo_Volts(adcresult);                  // convert value to Volts
        
            // If you want to print value
            printf("%d %f\r\n",adcresult, volts * 3 / 2);
        }
        CyDelay(500);
        
    }
 }   
//*/



/* Don't remove the functions below */
int _write(int file, char *ptr, int len)
{
    (void)file; /* Parameter is not used, suppress unused argument warning */
	int n;
	for(n = 0; n < len; n++) {
        if(*ptr == '\n') UART_1_PutChar('\r');
		UART_1_PutChar(*ptr++);
	}
	return len;
}

int _read (int file, char *ptr, int count)
{
    int chs = 0;
    char ch;
 
    (void)file; /* Parameter is not used, suppress unused argument warning */
    while(count > 0) {
        ch = UART_1_GetChar();
        if(ch != 0) {
            UART_1_PutChar(ch);
            chs++;
            if(ch == '\r') {
                ch = '\n';
                UART_1_PutChar(ch);
            }
            *ptr++ = ch;
            count--;
            if(ch == '\n') break;
        }
    }
    return chs;
}
/* [] END OF FILE */
