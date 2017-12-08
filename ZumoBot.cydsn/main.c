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

//The function that checks for the stop lines. 
//It also checks if the line is lost and backs the robot for a bit
//l and r are raw sensor values
//lm and rm are left and right middle sensors raw values 
//flag is a pointer to an external flag defined in the main function outside of the loop
//stop is a pointer to an external stop variable defined in the main function, it is used as a stop condition
void should_I_stop(int l, int r, int lm, int rm, int * flag, int * stop){
    //I used 16000 for baseline black, a little bit conservative but doesn't fail
    if(l > 16000 && r > 16000 && * flag == 0) {
        * flag = * flag + 1; 
        * stop = * stop + 1;
    }
    else if(l < 16000 && r < 16000 && * flag == 1){
        * flag = * flag - 1;
    }//why am I using else if here?
    else if(rm < 5000 && lm < 5000){
            motor_backward(155, 10);
            motor_forward(155, 1);
     
    }
}

int main(void){

    struct sensors_ ref;
    CyGlobalIntEnable; 
    UART_1_Start();
  
    sensor_isr_StartEx(sensor_isr_handler);
    
    reflectance_start();

    IR_led_Write(1);
   
    
    //clear the initial sensor state (discharge the capacitor for the first time)
    while(ref.l1<1 && ref.l1<1 && ref.l1<1 && ref.l1<1 && ref.l1<1){
        reflectance_read(&ref);
    }
    
    ///drive up to the first line
    motor_start();
    do{
        motor_forward(100, 5);
        reflectance_read(&ref);
    }while( ref.l3 < 15000 && ref.r3 < 15000); //15000 as a conservative failsafe value
    
    motor_stop();
    //*/
    
    ///Wait for the IR signal
    while(!(get_IR())){}
    motor_start();
    
    //*/
    
    //Initialization - stop and flag - race control
    int stop = 0;
    int flag = 1;
    
    //KP, KD, Sensor value scaling to the motor control range
    float error = 0, lastError = 0;
    
    float x=0.0013; //scaling factor because I work with raw sensor data
    float kp = 39;
    float kd = 680;
    float PV = 0;
    
    //Main tunning loop
    while( stop<=1){    
        
            do{
                //read the sensors
                reflectance_read(&ref);
                //check if we need to fix lost line or stop at the Finnish line
                should_I_stop(ref.l3, ref.r3, ref.l1, ref.r1, &flag, &stop);                
                
                
                error = (float)((ref.l1 - ref.r1) / 2);
                
                //keep the error at maximum if the line is lost to either side
                if(ref.l1 > ref.r1 && ref.l1 < 18000){
                    error = 9000;
                }
                if(ref.r1 > ref.l1 && ref.r1 < 18000){
                    error = -9000;
                }
                 
                PV = x * (kp * error + (kd * (error - lastError)));
                
                float left, right;
             
                
                left = 234 - PV;
                right = 234 + PV;
                
                
                if(left>255) left = 234;
                if(right>255) right = 234;
                if(left<0) left = 0;
                if(right<0) right = 0;
             
                lastError = error;
                
                motor_turn(left, right, 1);
                
            }while(stop<=1);
            
            
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
