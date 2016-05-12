#include <stdio.h>
#include <time.h>
extern "C" char get_pixel(int row,int col,int colour);
extern "C" int init(int d_lev);
extern "C" int set_motor(int motor , int speed );
extern "C" int Sleep( int sec , int usec );
extern "C" int take_picture();
extern "C" int connect_to_server( char server_addr[15],int port);
extern "C" int send_to_server(char message[24]);
extern "C" int receive_from_server(char message[24]);
extern "C" int display_picture(int delay_sec, int delay_usec);

const double CONST_PROPORTIONAL = 1;
const int CONST_DIFFERENTIAL = 0;
const int CONST_INTEGRAL = 0;

int SUM_PREV; //FOR WHEN WE USE DIFFERENTIATE
int SUM_TIME; //FOR WHEN WE DIFFERENTIATE

//LEFT wheel is motor ONE
//RIGHT wheel is motor TWO

void network() {
    connect_to_server("130.195.6.196", 1024); //TODO: fix Server IP
   //sends a message to the connected server
   send_to_server("Please");

   //receives message from the connected server
   char message[24];
   receive_from_server(message); //this may be buggy!
   //printf("%s", message);
   send_to_server(message);
}

void follow_the_line(){
    int testClock = 0;
    while(testClock<1){
        take_picture();
        double sum_of_pixels = 0;  
        char specific_pixel= 0; 
 
        for (int iteration=-160; iteration < 160; iteration++){
            specific_pixel = get_pixel(iteration+160, 120, 3);
            if(specific_pixel>127){specific_pixel=1;}
            else{specific_pixel=0;}
            
            sum_of_pixels = sum_of_pixels + specific_pixel;
            printf("Specific Pixel:  %f\n", specific_pixel);
        }
    
        double proportional_signal = sum_of_pixels*CONST_PROPORTIONAL;
        
        printf("The Proportional Signal is:  %f\n", proportional_signal);
    
        //Ideally sum_of_pixels = 0.
        set_motor(1, (proportional_signal/(160*-1*CONST_PROPORTIONAL))*255);
        set_motor(2, (proportional_signal/(160*1*CONST_PROPORTIONAL))*255);
        Sleep(0, 200000);
        testClock++;
    }
}

//LEAVE AT BOTTOM OF CODE
int main(){
    init(0);
    //network();
    follow_the_line();


    set_motor(1,0);
    set_motor(2,0);
    
    return 0;
}
