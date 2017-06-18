//  project3
//  Created by Vatsal Thakar on 2/12/17.
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
LaserProxy lp(&robot, gIndex);
double turn,xpos,ypos,zpos,temptan,dist,Lright,Lleft, maxSpeed = 5,minSpeed=0.05,minGoalDistance=0.30,maxAngle=10 ;
//methods
bool navigator(char **);
bool pilot(double, double);
void avoidObstacle();
void GoToGoal(double,double);

int main(int argc, char * argv[]) {
    try
    {
        //setUp Robot
        robot.Read();
        pp.SetMotorEnable(true);
        argc<3?navigator(argv)?cout << "Robot reached the goal successfully" << endl:cout << "Robot failed to reach the goal" << endl:pilot(atof(argv[1]),atof(argv[2]));
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
   

    while(true)
    {    
        robot.Read();
        xpos = pp.GetXPos();
        ypos = pp.GetYPos();
        zpos = pp.GetYaw();

        Lright = lp.GetMinRight();
        Lleft = lp.GetMinLeft();
    
        if(Lright<0.6 || Lleft<0.6)
        {
            avoidObstacle();
        }
        else 
        {
            GoToGoal(X,Y);
            if(dist < minGoalDistance) 
            {
                pp.SetSpeed(0,0);
                return true;
            }
        
        }
        
            
        
    }
return false;
}

void avoidObstacle()
{
        cout<<"\n---------------------------------AvoidObstacle-----------------------------------------------"<<endl;
        cout<<"X:"<<xpos<<" || Y:"<<ypos<<endl;
        cout << "yaw= "<< zpos << std::endl;
       
        temptan=atan2 ((0-ypos),(0-xpos));
        turn = (-1 * zpos) + temptan; // Obtain angle to turn
        cout<<"\nAngle to turn in radians:"<<turn;
        cout<<"\nAngle to turn in degree:"<<rtod(turn)<< endl;  
        cout << "Right : " << Lright << " || " << "Left: " << Lleft << endl;
        cout<<"\n--------------------------------------------------------------------------------"<<endl;

            if(Lright < 0.6) 
            {
                (lp[0] < 0.6 && lp[90] > 5)? pp.SetSpeed(1, 0):pp.SetSpeed(0.1, 1); 
            } 
            else if( Lleft < 0.6) 
            {
                ((lp[180] < 0.6 && lp[90] > 5) || Lright < 1) ?  pp.SetSpeed(1, 0):pp.SetSpeed(0.1, -1);
            
            }
}
void GoToGoal(double X,double Y)
{
        cout<<"\n-----------------------------------GoToGOAL---------------------------------------------"<<endl;
        cout<<"X:"<<xpos<<" || Y:"<<ypos<<endl;
        cout << "yaw= "<< zpos << std::endl;
       
        temptan=atan2 ((Y-ypos),(X-xpos));
        turn = (-1 * zpos) + temptan; // Obtain angle to turn
        cout<<"\nAngle to turn in radians:"<<turn;
        cout<<"\nAngle to turn in degree:"<<rtod(turn)<< endl;  
        cout << "Right : " << Lright << " || " << "Left: " << Lleft << endl;
        cout<<"\n--------------------------------------------------------------------------------"<<endl;
                                   
            fabs(rtod(turn)) > maxAngle?pp.SetSpeed (minSpeed, (turn)):pp.SetSpeed (maxSpeed,turn);//If angle to turn is large then go slower and turn half way otherwise go full speed
             dist = sqrt(pow((xpos - X), 2) + pow((ypos - Y), 2));
            
            
            
            
            cout << "X-Position: " << pp.GetXPos() << " || Y-Position: " << pp.GetYPos() << endl;
            cout << "Angle in Radian: " << turn << " || Angle in Degrees: " << rtod(turn) << endl;
            cout << "--------------------------------------------------------------" << endl;
}
