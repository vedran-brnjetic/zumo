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
    x = (int)( (float)speed / (1.5 - (1 - (( (float)max / min) / 8))) ) - 
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
    
    //motor_start();
    
    /*//drive up to the first line
    do{
        motor_forward(100, 5);
        reflectance_read(&ref);
    }while( ref.l3 < 15000 && ref.r3 < 15000);
    
    motor_stop();
    //*/
    
    /*//Wait for the IR signal
    while(!(get_IR())){}
    motor_start();
    motor_forward(255, 150);
    //*/
    
    ///GO!
    int stop = 0;
    int flag = 0;
    int speed = 0, brake_factor = 0;
    float error=0, lastError=0;
    
    while( 1){    
//      printf("%d %d", ref.l3, ref.r3);
//      printf("%d\n", stop);
        
        speed = 255;
        brake_factor = 175;
        if(stop>0){
            speed = 100;
            brake_factor = 60;
        }
        
        reflectance_read(&ref);
        error = (float)(ref.l1 - ref.r1) / 2;
        
        //should_I_stop(ref.l3, ref.r3, ref.l1, ref.r1, &flag, &stop);
        
        
        if(19000 < ref.l1 && ref.l1 < 21000 && 19000 < ref.r1 && ref.r1 < 21000){                        
            //motor_forward(speed, 1);
            reflectance_read(&ref);
            //should_I_stop(ref.l3, ref.r3, ref.l1, ref.r1, &flag, &stop);
        }
        else if(ref.l1 != ref.r1){
            //max and min are to calculate the factor of the turn
            //r and l to pick the correct motor to slow down
            int max, min, r = 0, l = 0;
            
            do{
                float error = (float)((ref.l1 - ref.r1) / 2);
                if(ref.l1 > ref.r1 && ref.l1 < 18000){
                    error = 9000;
                }
                if(ref.r1 > ref.l1 && ref.r1 < 18000){
                    error = -9000;
                }

                
                float kp= 0.0069;
                float kd= 0.0013;
                float PV;
                
                PV = kp * error + kd * (error - lastError);
                lastError = error;
                printf("%f\n", error);
                CyDelay(250);
                if (PV > 55){
                    PV = 55;
                }
                  
                if (PV < -55){
                    PV = -55;
                }
                
                //reflectance_read(&ref);
                /*//
                motor_turn(
                    100 + PV, 
                    100 - PV,
                     1);///*/
                reflectance_read(&ref);
                //should_I_stop(ref.l3, ref.r3, ref.l1, ref.r1, &flag, &stop);
                //reset direction
                r = 0; l = 0;    
                //*/
                //printf("%f\n", PV);
            }while(!(19000 < ref.l1 && ref.l1 < 21000 && 19000 < ref.r1 && ref.r1 < 21000) && stop <= 1);
            
            
        }
        
        
    }
    
    motor_stop();
    return 0;
}
//*/

/*//drive
int main(){
    //int i    
    CyGlobalIntEnable; 
    UART_1_Start();
    
    while(!(get_IR())){}
    motor_start();
    
    //delay of execution
    //speed from 0-255, time in miliseconds
    motor_forward(0, 3000);
    
    //first line
    motor_forward(100, 5460); //at speed 50 approx 10 cm per second
    //speed of left motor, speed of right motor, time in milisecnods
    motor_turn(205, 0, 360);
    
    //second line
    motor_forward(100, 4000); //at speed 50 approx 10 cm per second
    motor_turn(205, 0, 360);
    
    //third line
    motor_forward(100, 5480); //at speed 50 approx 10 cm per second
    
    
    //sharp turn
    motor_turn(205, 0, 660);
    //smooth curve
    motor_turn(40, 60, 12200);
    
    motor_stop();
    return 0;
    
    //After some testing, batteries gave up at 4.8 V - 
    //First the timings got shortened, later the motors no longer ran
}//*/

/*// Follow a line
int main(void)
{
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
    
    while(ref.l1 > 18000 || ref.r1 > 18000){
        reflectance_read(&ref);
    
        if(ref.l1 > 18000 && ref.r1 > 18000){
            motor_forward(50,50);  
        }
        else if(ref.l1 < 18000){
            while(!(ref.l1 > 18000 && ref.r1 > 18000)){
                motor_turn(50, 0, 5);
                reflectance_read(&ref);
            }
        }
        else if(ref.r1 < 18000){
            while(!(ref.l1 > 18000 && ref.r1 > 18000)){
                motor_turn(0, 50, 5);
                reflectance_read(&ref);
            }
        }
            
     
        CyDelay(49);
    }
    
    motor_stop();


}


//*/
/*//  Sensor testing
int main(void)
{
    struct sensors_ ref;
    struct sensors_ dig;
    CyGlobalIntEnable; 
    UART_1_Start();
  
    sensor_isr_StartEx(sensor_isr_handler);
    
    reflectance_start();

    IR_led_Write(1);
    
  motor_start();
    
    //motor_backward(20,10000);
    unsigned i=10;
    reflectance_read(&ref);
    
    while(i>0)
    {
        reflectance_read(&ref);
        printf("%d %d %d %d \r\n", ref.l3, ref.l1, ref.r1, ref.r3);       //print out each period of reflectance sensors
        motor_forward(255, 50);
        
        //CyDelay(10);
        i--;
    }
    
    
    motor_stop();
    
    
return 0;
}//*/

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


/*//ultra sonic sensor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Ultra_Start();                          // Ultra Sonic Start function
    while(1) {
        //If you want to print out the value  
        printf("distance = %5.0f\r\n", Ultra_GetDistance());
        CyDelay(1000);
    }
}   
//*/


/*//nunchuk//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    nunchuk_start();
    nunchuk_init();
    
    for(;;)
    {    
        nunchuk_read();
    }
}   
//*/


/*//IR receiver//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    
    unsigned int IR_val; 
    
    for(;;)
    {
       IR_val = get_IR();
       printf("%x\r\n\n",IR_val);
    }    
 }   
//*/


/*//Ambient light sensor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    
    I2C_Start();
    
    I2C_write(0x29,0x80,0x00);          // set to power down
    I2C_write(0x29,0x80,0x03);          // set to power on
    
    for(;;)
    {    
        uint8 Data0Low,Data0High,Data1Low,Data1High;
        Data0Low = I2C_read(0x29,CH0_L);
        Data0High = I2C_read(0x29,CH0_H);
        Data1Low = I2C_read(0x29,CH1_L);
        Data1High = I2C_read(0x29,CH1_H);
        
        uint8 CH0, CH1;
        CH0 = convert_raw(Data0Low,Data0High);      // combine Data0
        CH1 = convert_raw(Data1Low,Data1High);      // combine Data1

        double Ch0 = CH0;
        double Ch1 = CH1;
        
        double data = 0;
        data = getLux(Ch0,Ch1);
        
        // If you want to print out data
        //printf("%lf\r\n",data);    
    }    
 }   
//*/


/*//accelerometer//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    I2C_Start();
  
    uint8 X_L_A, X_H_A, Y_L_A, Y_H_A, Z_L_A, Z_H_A;
    int16 X_AXIS_A, Y_AXIS_A, Z_AXIS_A;
    
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL1_REG, 0x37);           // set accelerometer & magnetometer into active mode
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL7_REG, 0x22);
    
    
    for(;;)
    {
        //print out accelerometer output
        X_L_A = I2C_read(ACCEL_MAG_ADDR, OUT_X_L_A);
        X_H_A = I2C_read(ACCEL_MAG_ADDR, OUT_X_H_A);
        X_AXIS_A = convert_raw(X_L_A, X_H_A);
        
        Y_L_A = I2C_read(ACCEL_MAG_ADDR, OUT_Y_L_A);
        Y_H_A = I2C_read(ACCEL_MAG_ADDR, OUT_Y_H_A);
        Y_AXIS_A = convert_raw(Y_L_A, Y_H_A);
        
        Z_L_A = I2C_read(ACCEL_MAG_ADDR, OUT_Z_L_A);
        Z_H_A = I2C_read(ACCEL_MAG_ADDR, OUT_Z_H_A);
        Z_AXIS_A = convert_raw(Z_L_A, Z_H_A);
        
        printf("ACCEL: %d %d %d %d %d %d \r\n", X_L_A, X_H_A, Y_L_A, Y_H_A, Z_L_A, Z_H_A);
        value_convert_accel(X_AXIS_A, Y_AXIS_A, Z_AXIS_A);
        printf("\n");
        
        CyDelay(50);
    }
}   
//*/


/*//reflectance//
int main()
{
    struct sensors_ ref;
    struct sensors_ dig;
    CyGlobalIntEnable; 
    UART_1_Start();
  
    sensor_isr_StartEx(sensor_isr_handler);
    
    reflectance_start();

    IR_led_Write(1);
    for(;;)
    {
        reflectance_read(&ref);
        printf("%d %d %d %d \r\n", ref.l3, ref.l1, ref.r1, ref.r3);       //print out each period of reflectance sensors
        reflectance_digital(&dig);      //print out 0 or 1 according to results of reflectance period
        printf("%d %d %d %d \r\n", dig.l3, dig.l1, dig.r1, dig.r3);        //print out 0 or 1 according to results of reflectance period
        
        CyDelay(500);
    }
}   
//*/

/*  //motor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();

    motor_start();              // motor start

    motor_forward(100,2000);     // moving forward
    motor_turn(200,50,2000);     // turn
    motor_turn(50,200,2000);     // turn
    motor_backward(100,2000);    // movinb backward
       
    motor_stop();               // motor stop
    
    for(;;)
    {

    }
}
//*/
    

/*//gyroscope//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    I2C_Start();
  
    uint8 X_L_G, X_H_G, Y_L_G, Y_H_G, Z_L_G, Z_H_G;
    int16 X_AXIS_G, Y_AXIS_G, Z_AXIS_G;
    
    I2C_write(GYRO_ADDR, GYRO_CTRL1_REG, 0x0F);             // set gyroscope into active mode
    I2C_write(GYRO_ADDR, GYRO_CTRL4_REG, 0x30);             // set full scale selection to 2000dps    
    
    for(;;)
    {
        //print out gyroscope output
        X_L_G = I2C_read(GYRO_ADDR, OUT_X_AXIS_L);
        X_H_G = I2C_read(GYRO_ADDR, OUT_X_AXIS_H);
        X_AXIS_G = convert_raw(X_H_G, X_L_G);
        
        
        Y_L_G = I2C_read(GYRO_ADDR, OUT_Y_AXIS_L);
        Y_H_G = I2C_read(GYRO_ADDR, OUT_Y_AXIS_H);
        Y_AXIS_G = convert_raw(Y_H_G, Y_L_G);
        
        
        Z_L_G = I2C_read(GYRO_ADDR, OUT_Z_AXIS_L);
        Z_H_G = I2C_read(GYRO_ADDR, OUT_Z_AXIS_H);
        Z_AXIS_G = convert_raw(Z_H_G, Z_L_G);
     
        // If you want to print value
        printf("%d %d %d \r\n", X_AXIS_G, Y_AXIS_G, Z_AXIS_G);
        CyDelay(50);
    }
}   
//*/


/*//magnetometer//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    I2C_Start();
   
    uint8 X_L_M, X_H_M, Y_L_M, Y_H_M, Z_L_M, Z_H_M;
    int16 X_AXIS, Y_AXIS, Z_AXIS;
    
    I2C_write(GYRO_ADDR, GYRO_CTRL1_REG, 0x0F);             // set gyroscope into active mode
    I2C_write(GYRO_ADDR, GYRO_CTRL4_REG, 0x30);             // set full scale selection to 2000dps
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL1_REG, 0x37);           // set accelerometer & magnetometer into active mode
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL7_REG, 0x22);
    
    
    for(;;)
    {
        X_L_M = I2C_read(ACCEL_MAG_ADDR, OUT_X_L_M);
        X_H_M = I2C_read(ACCEL_MAG_ADDR, OUT_X_H_M);
        X_AXIS = convert_raw(X_L_M, X_H_M);
        
        Y_L_M = I2C_read(ACCEL_MAG_ADDR, OUT_Y_L_M);
        Y_H_M = I2C_read(ACCEL_MAG_ADDR, OUT_Y_H_M);
        Y_AXIS = convert_raw(Y_L_M, Y_H_M);
        
        Z_L_M = I2C_read(ACCEL_MAG_ADDR, OUT_Z_L_M);
        Z_H_M = I2C_read(ACCEL_MAG_ADDR, OUT_Z_H_M);
        Z_AXIS = convert_raw(Z_L_M, Z_H_M);
        
        heading(X_AXIS, Y_AXIS);
        printf("MAGNET: %d %d %d %d %d %d \r\n", X_L_M, X_H_M, Y_L_M, Y_H_M, Z_L_M, Z_H_M);
        printf("%d %d %d \r\n", X_AXIS,Y_AXIS, Z_AXIS);
        CyDelay(50);      
    }
}   
//*/


#if 0
int rread(void)
{
    SC0_SetDriveMode(PIN_DM_STRONG);
    SC0_Write(1);
    CyDelayUs(10);
    SC0_SetDriveMode(PIN_DM_DIG_HIZ);
    Timer_1_Start();
    uint16_t start = Timer_1_ReadCounter();
    uint16_t end = 0;
    while(!(Timer_1_ReadStatusRegister() & Timer_1_STATUS_TC)) {
        if(SC0_Read() == 0 && end == 0) {
            end = Timer_1_ReadCounter();
        }
    }
    Timer_1_Stop();
    
    return (start - end);
}
#endif

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
