#include "fsl_device_registers.h"
#include "MK64F12.h"
#include <stdio.h>
#include <stdlib.h>


void software_delay(unsigned long delay) //Setting a delay function so that we can change the speed as desired
{
	while (delay > 0) delay--;
}


//Define the clockwise function so that the pointer can rotate clockwise
void clockwise(void){
	unsigned int Delay = 20000;
	for(int i = 0; i<=5; i++){
		GPIOC_PDOR = 0x3A;
		software_delay(Delay);
		GPIOC_PDOR = 0x39;
		software_delay(Delay);
		GPIOC_PDOR = 0x35;
		software_delay(Delay);
		GPIOC_PDOR = 0x36;
		software_delay(Delay);
	}
}

//Define the counterclockwise function so that the pointer can rotate counterclockwise
void counterclockwise(void){
	unsigned int Delay = 20000;
	for(int i = 0; i<=5; i++){
		GPIOC_PDOR = 0x36;
		software_delay(Delay);
		GPIOC_PDOR = 0x35;
		software_delay(Delay);
		GPIOC_PDOR = 0x39;
		software_delay(Delay);
		GPIOC_PDOR = 0x3A;
		software_delay(Delay);
	}
}

int main(void)
{
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;  //Enable Port D Clock Gate Control
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;  //Enable Port C Clock Gate Control

	PORTC_GPCLR = 0x07BF0100; //Setting the pins 0-5, 7-10 of the Port C as GPIO
	PORTD_GPCLR = 0X000F0100; //Setting the pins 0-3 of the Port D as GPIO
	GPIOC_PDDR = 0x0000003F;  //Setting the pins 0-5 of the Port C as Output and pins 7-10 of the Port C as Input
	GPIOD_PDDR = 0x00000000;  //Setting the pins 0-3 of the Port D as Input

	unsigned long Delay; //Declare the variable Delay
	unsigned long DIR_1=0, DIR_2=0, DIR_3=0, DIR_4=0, DIR_5=0, DIR_6=0, DIR_7=0, DIR_8=0; //Declare the variable DIR_1 - DIR_8 for sensor detections
	uint32_t input_val_c = 0, input_val_d = 0; //Declare the variable input_val_c and input_val_d
	int origin = 0, sensor1 = 45, district1= 90, sensor2 = 135, district2 = 180, sensor3 = 225, district3 = 270, sensor4 = 315;  //Declare the variables for different angles in the map
	int current_angle = 0; // Declare the variable that indicates the current angle of the pointer.
	int count = 0; //Declare the variable count so that the pointer rorates to the correct position
	int destination = -1; //Declare and assign -1 to the variable destination as 0 is one of our angles so we have to assign initial value other than 0

	while (1) {
		input_val_c = GPIOC_PDIR & 0x780;  //Read and Store pins 7-10 of the Port C to input_val_c
		input_val_d = GPIOD_PDIR & 0x0F;  //Read and Store pins 0-3 of the Port D to input_val_d
		DIR_1 = (input_val_c & 0x80);  //Store the pin 7 of the Port C
		DIR_2 = (input_val_c & 0x100);  //Store the pin 8 of the Port C
		DIR_3 = (input_val_c & 0x200);  //Store the pin 9 of the Port C
		DIR_4 = (input_val_c & 0x400);  //Store the pin 10 of the Port C
		DIR_5 = (input_val_d & 0x1);  //Store the pin 0 of the Port D
		DIR_6 = (input_val_d & 0x2);  //Store the pin 1 of the Port D
		DIR_7 = (input_val_d & 0x4);  //Store the pin 2 of the Port D
		DIR_8 = (input_val_d & 0x8);  //Store the pin 3 of the Port D

		destination = -1;  //Assign -1 to destination everytime enter the loop so that the pointer can stop without hearing sounds
		count = 0;  //Assign 0 to count so that the pointer can rotate to the correct position every time

        
        //If the sound sensors hears sounds, indicates the angle of destination, we use "else-if" instead of "if" for DIR_2 to DIR_3 so that the destination will be the location where the first sound the sensor received (As the sensors are sensitive and some other sensors located at nearby position may hear the sound when we make sounds to the target position
		if(DIR_1 != 0){
			destination = sensor1;
		}
		else if(DIR_2 != 0){
			destination = sensor2;
		}
		else if(DIR_3 != 0){
			destination = sensor4;
		}
		else if(DIR_4 != 0){
			destination = sensor3;
		}
		else if(DIR_5 != 0){
			destination = origin;
		}
		else if(DIR_6 != 0){
			destination = district1;
		}
		else if(DIR_7 != 0){
			destination = district2;
		}
		else if(DIR_8 != 0){
			destination = district3;
		}

        
        //Make sure the pointer works as desired. The pointer will turn to the target destination based on smallest angel to the destination (e.g. Assume the pointer's current position is 45 degree, when we make sounds at 270 degree, the pointer will turn to 270 degree counterclockwise instead of clockwise
		if((destination != current_angle) && destination != (-1)){
			temp = temp + 1;
			if(current_angle <180){
				if(abs((destination - current_angle) > 180)){
					count = ((abs(destination - current_angle))/45);
					for(int i=0; i<(8-count); i++){
						counterclockwise();
					}
					current_angle = destination;
				}else if((abs(destination - current_angle) <= 180)){
					if((destination - current_angle) > 0){
						count = ((abs(destination - current_angle))/45);
						for(int i=0; i<count; i++){
							clockwise();
						}
						current_angle = destination;
					}
					else if((destination - current_angle) < 0){
						count = ((abs(destination - current_angle))/45);
						for(int i=0; i<count; i++){
							counterclockwise();
						}
						current_angle = destination;
					}
				}
			}else if(current_angle >=180){
				if((abs(destination - current_angle) <= 180)){
					if((destination - current_angle) > 0){
						count = ((abs(destination - current_angle))/45);
						for(int i=0; i<count; i++){
							clockwise();
						}
						current_angle = destination;
					}else if(destination - current_angle < 0){
						count = ((abs(destination - current_angle))/45);
						for(int i=0; i<count; i++){
							counterclockwise();
						}
						current_angle = destination;
					}
				}else if((abs(destination - current_angle) > 180)){
					count = ((abs(destination - current_angle))/45);
					for(int i=0; i<(8-count); i++){
						clockwise();
					}
					current_angle = destination;
				}
			}
		}
	}
	return 0;
}
