//  project2
//  Created by Vatsal Thakar on 1/19/17.
//  Copyright © 2017 Vatsal Thakar. All rights reserved.


#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include <libplayerc++/playerc++.h>
#include "stdlib.h"
#include "args.h"
using namespace std;
using namespace PlayerCc;

//Variables
PlayerClient robot(gHostname, gPort);
Position2dProxy pp(&robot, gIndex);


//methods
bool navigator(char **);
bool pilot(double, double);


int main(int argc, char * argv[]) {
    try
    {
        //setUp Robot
        robot.Read();
        pp.SetMotorEnable(true);
        navigator(argv)?cout << "Robot reached the goal successfully" << endl:cout << "Robot failed to reach the goal" << endl;
        return 1;
    }
     catch (PlayerCc::PlayerError & e) {
        std::cerr << e << std::endl;
        return -1;
    }
    
}

bool navigator(char *file[]) 
{
    bool finished;
    
    							        //Count the number of lines 
    ifstream wayPoints;
    wayPoints.open(file[1]);
    int linecnt = std::count(std::istreambuf_iterator<char>	(wayPoints),std::istreambuf_iterator<char>(), '\n');
    wayPoints.close();
     
    							       //allocate array of double the line count
    float points[linecnt * 2];
    int index = 0;
    wayPoints.open(file[1]);
	if (wayPoints.is_open())
    {
        std::string word;
        while (wayPoints >> word)
        {
            points[index] = atof(word.c_str());
            index++;
        }
        wayPoints.close();
        
        finished = true;
    } 
    else 
    {
       		finished = false;
        	cout << "Error opening file.Check filename or path supplied";
			return false;
    }
    
							       //Upon successful completion of waypoints file move robot using pilot function
    if(finished)
	 {
        for (int i = 0; i < (linecnt * 2) - 1; i++)
	   {
         
            if(pilot(points[i], points[i+1])) 
	    	{
                cout << "\n Robot Moved successfully to :"<<points[i]<<" "<<points[i+1] << endl;
            } 
	    	else
	    	{
                finished = false;
                cout << "Error reaching points:"<<points[i]<<" "<<points[i+1] << endl;
                break;
		
            }
            
           i++;
        }
    }
    return finished;
	
}

bool pilot( double X, double Y)
{
   
double turn,xpos,ypos,zpos,tan,distance, maxSpeed = .3 ;
    while(true)
    {    
        robot.Read();
        xpos = pp.GetXPos();
        ypos = pp.GetYPos();
        zpos = pp.GetYaw();
        
        cout<<"\n--------------------------------------------------------------------------------"<<endl;
        cout<<"X:"<<xpos<<" || Y:"<<ypos<<endl;
        cout << "yaw= "<< zpos << std::endl;
       
        tan=atan2 ((Y-ypos),(X-xpos));
        turn = (-1 * zpos) + tan; // Obtain angle to turn
        cout<<"\nAngle to turn in radians:"<<turn;
        cout<<"\nAngle to turn in degree:"<<rtod(turn);	
        
        cout<<"\n--------------------------------------------------------------------------------"<<endl;
        fabs(rtod(turn)) > 20?pp.SetSpeed (0.05, (turn/2.0)):pp.SetSpeed (maxSpeed,turn);//If angle to turn is large then go slower and turn half way otherwise go full speed
        distance = sqrt(pow((xpos - X), 2) + pow((ypos - Y), 2));


        if (distance < .15 )
        {
            pp.SetSpeed (0,0);
            return true;
        }
        
    }
return false;
}

