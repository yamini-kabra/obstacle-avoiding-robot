/*
 * pid1.c
 *
 * Created: 02-02-2019 16:04:35
 * Author : Lenovo
 */ 

#include <avr/io.h>
void pwm_init()
{
	TCCR0 |= (1<<WGM00)|(1<<WGM01); //Set PWM to Fast PWM Mode
	TCCR0 |= (1<<COM00)|(1<<COM01); //Set Fast PWM to inverting mode
	TCCR0 |= (1<<CS01); //Pre scale 8 bit counter to prescaling of 8 to count till 2048 counting (>100)(coz ICR1 is 1000 and hence couter must be greater than 1000)
	TCCR0 &= ~(1<<CS02);
	ICR1 = 1000; //Sets ICP1(Reference to 1000)(Although it is called ICP1
}

int pin_status[8]={0};
int pin_weightage[8]={0,2,4,6,8,10,12,14};
float desired_value=7,kp,kd;
float error=0,error1=0,last_error=0,d_error=0,base_value=150,error2=0;
int c,i;

void move(int left,int right)
{
	PORTA |= (1<<PINA2)|(1<<PINA4);
	PORTA &= ~(1<<PINA3);
	PORTA &= ~(1<<PINA5);
	OCR1A =  left;
	OCR1B =  right;
	
}
int pid_calc(void)
{
	c=0;
	for(i=0;i<8;i++)
	{
		if(bit_is_set(PINA,i)==1)
			c++;
		error+=(bit_is_set(PINA,i)*pin_weightage[i]);
	}
	if(c==0)
	{
		c=1;
		if(last_error>0)
			error=20;
		else
			error=-20;
	}
	
	error=error/c;
	
	error=error-desired_value;
	error1+=kp*error+kd*d_error;
	d_error=error1-last_error;
	last_error=error1;
	return error1;
	
}



int main(void)
{
    DDRD=0b00000000;
    DDRA=0xFF;
	pwm_init();
	
    while (1) 
    {
		//stop code 
		if(c==8)
		move(0,0);
		
		//basic line following
		error2=pid_calc();
		move(base_value+error2,base_value-error2);
		
	}
 
	return 0;
}

