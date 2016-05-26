// Import Libraries
#include <stdio.h>
#include <time.h>

// Import method statements. 
extern "C" char get_pixel(int row,int col,int colour);
extern "C" int init(int d_lev);
extern "C" int set_motor(int motor , int speed );
extern "C" int Sleep( int sec , int usec );
extern "C" int take_picture();
extern "C" int connect_to_server( char server_addr[15],int port);
extern "C" int send_to_server(char message[24]);
extern "C" int receive_from_server(char message[24]);
extern "C" int read_analog(int ch_adc);
extern "C" int read_digital(int chan);
extern "C" int select_IO(int chan, int direct);
extern "C" int display_picture(int delay_sec, int delay_usec);

const double PROPORTIONAL = 500;	// Proportional Constant
const double DIFFERENTIAL = 0;		// Differential Constant
const double INTEGRAL = 0;			// Integral Constant

int num_of_decisions = 0;


// This method determines the average white/black levels.

int determine_average(){
	int max = 0;
	int min = 255;
	int average;

	take_picture();							// Takes initial picture.
	for(int i = 0; i<320; i++){

		if(get_pixel(i, 120, 3)>max){		// Establishes Max.
			max = get_pixel(i, 120, 3);
		}

		if(get_pixel(i, 120, 3)<min){		// Establishes Min.
			min = get_pixel(i, 120, 3);

		}
	}

	average = (max+min)/2;
	return average;
}


// This method finds the error for 5 rows and averages them to create one.

int average_error(int i){
	int error1 = get_pixel(i, 120, 3);
	int error2 = get_pixel(i, 118, 3);
	int error3 = get_pixel(i, 122, 3);
	int error4 = get_pixel(i, 116, 3);
	int error5 = get_pixel(i, 124, 3);

	int average_error = (int) (error5+error4+error3+error2+error1)/5;

	return average_error;
}


void turn_Rpi(int direction){			
	if(direction == 0){
		// 180 Degree Turn
		set_motor(1, 45);
		set_motor(2, -45);
		Sleep(1, 0);
		set_motor(1, 0);
		set_motor(2, 0);
	}

	else if(direction == 1){
		// 90 Degree clockwise
		set_motor(1, 45);
		set_motor(2, 45);
		Sleep(0, 500000);
		
		set_motor(1, 45);
		set_motor(2, -45);
		Sleep(1, 0);
	}

	else if(direction == 2){
		// 90 Degree antclockwise
		set_motor(1, 45);
		set_motor(2, 45);
		Sleep(0, 500000);
		
		set_motor(1, -45);
		set_motor(2, 45);
		Sleep(1, 0);
	}
	else if(direction == 3){
		// Forwards
		set_motor(1, 40);
		set_motor(40);
		Sleep(0, 800000);
	}
return;
}

void short_forward(){
		set_motor(1, 30);
		set_motor(2, 30);
		Sleep(1, 0);
}
/* 		The Following Methods Drive the RPi		*/





// This is the network() method.  Opens the gate.

void network(){
	char message[24];							// Assigns memory to password.

	connect_to_server("130.195.6.196", 1024);	// Connects to Gate.
	send_to_server("Please");					// Requests permission.

	receive_from_server(message);				// Assigns the password to 'message'.
	send_to_server(message);					// Sends password to server

	printf(message);
}



// This is the follow_the_line() method.  Should have the RPi follow the line.

void follow_the_line(){

	//Define local variables
	int testClock = 0;							// For testing the RPi.  Can terminate movement.
	int proportional_signal_previous;
	int num_of_white = 0;
	int threshold = determine_average(); //PUT THIS BACK IN LOOP IF BREAKS QUAD 2
	

	while(true){

		bool seeLine = false;					// Whether or not the line can be seen.
		int current_error = 0;
		num_of_white = 0;
		
		take_picture();

		for(int i=0; i<320; i++){
			int error = average_error(i);

			if(error >= threshold){				// If RPi sees 'white'
				error = 1;						// Converts to binary represenation
				seeLine = true;					// The Line can be seen
				num_of_white++;
			}
			else{								// If RPi sees 'black'
				error = 0;						// Converts to binary representation
			}

			current_error = current_error + error*(i-160);
		}

		int proportional_signal = (int) (current_error/PROPORTIONAL);	//Sets proportional signal

		// Print checks:
		//printf("Current Error: %d\n", current_error);
		//printf("Proportional Signal: %d\n", proportional_signal);
		//printf("Number of White Pixels: %d\n", num_of_white);

		if(num_of_white>=300){
			printf("SWITCHING TO TURNING CODE\n");
			set_motor(1, 40);
			set_motor(2, 40);
			Sleep(1,500000);
			return;
		}
		else if(seeLine){
			set_motor(1, 35+proportional_signal);
			set_motor(2, 35-proportional_signal);
			proportional_signal_previous = proportional_signal;
		}
		else{
			set_motor(1, 50+proportional_signal_previous*7);
			set_motor(2, 50-proportional_signal_previous*7);
		}

	testClock++;
	}
	set_motor(1, 0);
	set_motor(2, 0);
	return;
}




/*void Complete_until_maze(){
	
	//Define local variables
	int testClock = 0;							// For testing the RPi.  Can terminate movement.
	int proportional_signal_previous;
	int num_of_white = 0;
	int leftpixel = 0;
	int threshold = determine_average();

	while(true){

		bool seeLine = false;					// Whether or not the line can be seen.
		int current_error = 0;
		num_of_white = 0;

		take_picture();

		for(int i=0; i<320; i++){
			int error = average_error(i);

			if(error >= threshold){				// If RPi sees 'white'
				error = 1;						// Converts to binary represenation
				seeLine = true;					// The Line can be seen
				num_of_white++;
			}
			else{								// If RPi sees 'black'
				error = 0;						// Converts to binary representation
			}

			current_error = current_error + error*(i-160);
		}

		int proportional_signal = (int) (current_error/PROPORTIONAL);	//Sets proportional signal

		// Print checks:
		//printf("Current Error: %d\n", current_error);
		//printf("Proportional Signal: %d\n", proportional_signal);
		//printf("Number of White Pixels: %d\n", num_of_white);

		if(num_of_white<50){
			if(leftpixel == 1)
			{
				set_motor(1, -40);
				set_motor(2, 40);
				printf("Bend: L, Turn: Left\n");
			}
			else
			{
				set_motor(1, 40);
				set_motor(2, -40);
				printf("Bend: L, Turn: Right\n");
			}
			Sleep(0,600000);
			
		}
		if(num_of_white>=280){
			printf("Bend: T, Turn: Left\n");
			set_motor(1, 35);
			set_motor(2, 35);
			Sleep(0,800000);
			set_motor(1, -40);
			set_motor(2, 40);
			Sleep(0,900000);
			
			
		}
		else if(seeLine){
			set_motor(1, 35+proportional_signal);
			set_motor(2, 35-proportional_signal);
			proportional_signal_previous = proportional_signal;
		}
		else{
			set_motor(1, 50+proportional_signal_previous*7);
			set_motor(2, 50-proportional_signal_previous*7);
		}
		
		if(average_error(80) >= threshold) 
		{
			leftpixel = 1;
		}
		else{
			leftpixel = 0;
		}
		
		int redAvr = (get_pixel(120, 120, 0) + get_pixel(160, 120, 0) + get_pixel(200, 120, 0))/3;
		printf("Red: %d\n",redAvr);
		if(redAvr > 200)
		{
			printf("SWITCHING TO WALLED CODE, Red: %d\n",redAvr);
			return;
		}
		
	testClock++;
	}
	set_motor(1, 0);
	set_motor(2, 0);
	return;

}*/

void Complete_until_maze(){ //Experimental
	//Define local variables
	int testClock = 0;							// For testing the RPi.  Can terminate movement.
	int proportional_signal_previous;
	int num_of_white = 0;
	int leftpixel = 0;
	int rightpixel = 0;
	int frontpixel = 0;
	int threshold = determine_average();

	while(true){

		bool seeLine = false;					// Whether or not the line can be seen.
		int current_error = 0;
		num_of_white = 0;

		take_picture();

		for(int i=0; i<320; i++){
			int error = average_error(i);

			if(error >= threshold){				// If RPi sees 'white'
				error = 1;						// Converts to binary represenation
				seeLine = true;					// The Line can be seen
				num_of_white++;
			}
			else{								// If RPi sees 'black'
				error = 0;						// Converts to binary representation
			}

			current_error = current_error + error*(i-160);
		}

		int proportional_signal = (int) (current_error/PROPORTIONAL);	//Sets proportional signal

		// Print checks:
		//printf("Current Error: %d\n", current_error);
		//printf("Proportional Signal: %d\n", proportional_signal);
		//printf("Number of White Pixels: %d\n", num_of_white);
		
		//Turn
		if(num_of_white > 160)
		{
		Sleep(0,100000);
		if(average_error(80) >= threshold) 
		{
			leftpixel = 1;
		}
		else{
			leftpixel = 0;
		}
		
		if(average_error(240) >= threshold) 
		{
			rightpixel = 1;
		}
		else{
			rightpixel = 0;
		}
		
		if(get_pixel(160, 10, 3) >= threshold) 
		{
			frontpixel = 1;
		}
		else{
			frontpixel = 0;
		}
			if(leftpixel == 1 && rightpixel == 1 && frontpixel == 0){
				printf("Bend: T, Turn: Left\n");
				turn_Rpi(2);
			}
			else if(leftpixel == 1 && rightpixel == 1 && frontpixel == 1){
				printf("Bend: +, Turn: None\n");
				turn_Rpi(3);
			}
			else if(leftpixel == 1 && rightpixel == 0 && frontpixel == 0){
				printf("Bend: L, Turn: Left\n");
				turn_Rpi(2);
			}
			else if(leftpixel == 1 && rightpixel == 0 && frontpixel == 1){
				printf("Bend: -|, Turn: None\n");
				turn_Rpi(3);
			}
			else if(leftpixel == 0 && rightpixel == 1  && frontpixel == 0){
				printf("Bend: L, Turn: Right\n");
				turn_Rpi(1);
			}
			else if(leftpixel == 0 && rightpixel == 1  && frontpixel == 1){
				printf("Bend: |-, Turn: None\n");
				turn_Rpi(3);
			}
			
		}
		else if(num_of_white < 30)
		{
			turn_Rpi(0);
		}
		else if(seeLine){
			set_motor(1, 35+proportional_signal);
			set_motor(2, 35-proportional_signal);
			proportional_signal_previous = proportional_signal;
		}
		
		if(read_analog(0) > 550 && read_analog(1) > 550)  //If on Red Square (Maze Detector)
		{
			printf("SWITCHING TO WALLED CODE\n");
			return;
		}
		
	testClock++;
	}
	set_motor(1, 0);
	set_motor(2, 0);
	return;

}

// This is the Main() method.  Controls the holistic behaviour or the RPi.

int main(){
	
	init(0);			// Initialise Hardware
			//Assigns a place in memory for average whiteness.	int proportional_signal_previous = 0;

//	network();			// Open Gate
	Sleep(5,0);  //Walking to Track
	follow_the_line();
	Complete_until_maze();    // Begin Following Line


	//Emergency Stop
	set_motor(1, 0);
	set_motor(2, 0);

	return 0;
}
