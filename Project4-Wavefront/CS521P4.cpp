//
//  
//  Project4
//
//  Created by Vatsal thakar on 3/10/17.
//  Copyright © 2017 Vatsal thakar. All rights reserved.
//

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
PlayerClient aRobot(gHostname, gPort);
Position2dProxy pp( & aRobot, gIndex);

#define SCALE_MAP 2


bool XFLAG=false,YFLAG=false,trigger = false;

int width, height, r_ROW, r_COL, g_row, g_COL,brValue, rValue, traversedNode, globalX
, globalY,tempCounter,nothing, object, grownobject, goal, robot, min_node, min_node_location, reset_min,temp_x,temp_y,counter = 0;

double OLDX=-20,OLDY=9,tr_ROW, tr_COL,grid_row, grid_col;

int * * aMap;

int pilot(double x, double y) {
  cout << "[" << x << ", " << y << "]" << endl;
  double angle, distance;
  double closeEnough = .10;
  double baseSpeed = .1;

  while (true) {
    aRobot.Read();

    double xP = pp.GetXPos();
    double yP = pp.GetYPos();
    double zP = pp.GetYaw();

    
    angle = (-1 * zP) + atan2((y - yP), (x - xP));

   
    pp.SetSpeed(baseSpeed, angle);
   
   
    double Tempdistance = sqrt(pow(pp.GetXPos() - x, 2) + pow((pp.GetYPos() - y), 2));
    distance = sqrt(pow(pp.GetXPos() - temp_x, 2) + pow((pp.GetYPos() - temp_y), 2));

    

    if (distance < closeEnough) {
      
      pp.SetSpeed(0, 0);
      return 1;
    }
    else if (Tempdistance<closeEnough)
      {return 1;}

   
  }
  return 0;
}
int conversion(bool grid) {
  
  if (!grid) {
   
    r_ROW = fabs((tr_COL - 8) * 7);
    r_COL = fabs((tr_ROW + 20) * 7);

    
  } else {
    
    grid_row = (r_COL / 7.0) - 20.0;
    grid_col=(((-1)*r_ROW)/7)+8;

  }
}

void initializeGrid()
{
  
  char inputLine1[80], nextChar;
  ifstream inFile("/usr/local/share/stage/worlds/bitmaps/scaled_hospital_section.pnm");

  // get width and height of the image
  inFile.getline(inputLine1, 80);

  
  inFile >> height >> width;
  
  double matrix=(width * height);
  
  nothing = 0;
  goal = 2;
  object = matrix + 1;                                                           //object
  robot = matrix + 2;                                                          //robot
  brValue = matrix+ 3;                                               //intial start
  grownobject = matrix + 4;                                                      //new object
  traversedNode =matrix + 5;                                                    //Visited node
  rValue = robot;
  min_node = matrix;
  min_node_location = matrix;
  reset_min = matrix;

  
  aMap = new int * [width];
  for (int i = 0; i < width; ++i) {
    aMap[i] = new int[height];
  }
   for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      inFile >> nextChar;
      if (!nextChar) {
        aMap[i / SCALE_MAP][j / SCALE_MAP] = (width * height) + 1;
      }
    }
  }

}

void initializeMapGoal()
{
  //robot's starting position in terms of rows and columns
  tr_ROW = pp.GetXPos();
  tr_COL = pp.GetYPos();
  conversion(false);
  //Goal location in terms of rows and colmns
  g_row = fabs((temp_y - 8) * 7);
  g_COL = fabs((temp_x + 20) * 7);
  cout << "Goal : " << g_row << " || " << g_COL << endl;
  trigger = true;
}
void growUp(int rRow,int rCol)
{
   //up
        if ((rRow - 1) >= 0) {
          if (aMap[rRow - 1][rCol] == nothing) {
            aMap[rRow - 1][rCol] = grownobject;

            if ((rRow - 2) >= 0) {
              if (aMap[rRow - 2][rCol] == nothing) {
                aMap[rRow - 2][rCol] = grownobject;
              
              }
            }
          }
        }
}

void growdown(int rRow,int rCol)
{
    //down
        if ((rRow + 1) <= (width - 1)) {
          if (aMap[rRow + 1][rCol] == nothing) {
            aMap[rRow + 1][rCol] = grownobject;

            
            if ((rRow + 2) <= (width - 1)) {
              if (aMap[rRow + 2][rCol] == nothing) {
                aMap[rRow + 2][rCol] = grownobject;
                                
              }
            }
          }
        }
}
void growleft(int rRow,int rCol)
{
     //left
        if ((rCol - 1) >= 0) {
          if (aMap[rRow][rCol - 1] == nothing) {
            aMap[rRow][rCol - 1] = grownobject;

            
            if ((rCol - 2) >= 0) {
              if (aMap[rRow][rCol - 2] == nothing) {
                aMap[rRow][rCol - 2] = grownobject;
                         
              }
            }
          }
        }
}
void growright(int rRow,int rCol)
{
     //right
        if ((rCol + 1) <= (height - 1)) {
          if (aMap[rRow][rCol + 1] == nothing) {
            aMap[rRow][rCol + 1] = grownobject;

            
            if ((rCol + 2) <= (height - 1)) {
              if (aMap[rRow][rCol + 2] == nothing) {
                aMap[rRow][rCol + 2] = grownobject;
                
              }
            }
          }
        }
}
void obstacleGrowth() {
  
  for (int rRow = 0; rRow < width; rRow++) {
    for (int rCol = 0; rCol < height; rCol++) {
      if (aMap[rRow][rCol] == object) {
      
        growUp(rRow,rCol);
        growdown(rRow,rCol);

        growleft(rRow,rCol);
        growright(rRow,rCol);
      }
    }
  }
}
void clearPropagation(int r_ROW, int r_COL) {
  for (int row = 0; row < width; row++) {
    for (int col = 0; col < height; col++) {
      if (aMap[row][col] != object && aMap[row][col] != goal && aMap[row][col] != grownobject) {
        aMap[row][col] = nothing;
      }
    }
  }
  aMap[r_ROW][r_COL] = robot;
 // cout << "Propogation reset" << endl << endl;
}
int min_surrounding_node_value(int row, int col) {
  min_node = reset_min; 

  //down
  if (row < (width - 1)) {
    if (aMap[row + 1][col] < min_node && aMap[row + 1][col] != nothing) {
      min_node = aMap[row + 1][col];
      min_node_location = 3;
    }
  }

  //up
  if (row > 0) {
    if (aMap[row - 1][col] < min_node && aMap[row - 1][col] != nothing) {
      min_node = aMap[row - 1][col];
      min_node_location = 1;
    }
  }

  //right
  if (col < (height - 1)) {
    if (aMap[row][col + 1] < min_node && aMap[row][col + 1] != nothing) {
      min_node = aMap[row][col + 1];
      min_node_location = 2;
    }
  }

  //left
  if (col > 0) {
    if (aMap[row][col - 1] < min_node && aMap[row][col - 1] != nothing) {
      min_node = aMap[row][col - 1];
      min_node_location = 4;
    }
  }
  return min_node;
}
int propagate_wavefront(int r_ROW, int r_COL) {
  counter = 0;
  clearPropagation(r_ROW, r_COL);
   aMap[g_row][g_COL] = goal;
  while (counter < (width * height)) {
    int pRow = 0;
    int pCol = 0;
    while (pRow < width && pCol < height) {
      if (aMap[pRow][pCol] != object && aMap[pRow][pCol] != goal && aMap[pRow][pCol] != grownobject) {
        if (min_surrounding_node_value(pRow, pCol) < reset_min && aMap[pRow][pCol] == robot) {
          return min_node_location;
        } else if (min_node != reset_min) {
          aMap[pRow][pCol] = min_node + 1;
        }
      }
      pCol++;
      if (pCol == height && pRow != width) {
        pRow++;
        pCol = 0;
      }
    }
    counter++;
  }
  return 0;
}



//return the lowest node around the given node


void moveRobotToGoal() {
  
  int rRow = r_ROW;
  int rCol = r_COL;

  if (rValue == robot) {
    aMap[rRow][rCol] = brValue;
  } else {
    aMap[rRow][rCol] = traversedNode;
  }

  
  int up = (width * height);
  int down = (width * height);
  int left = (width * height);
  int right = (width * height);

  //up
  if ((rRow - 1) >= 0) {
    if (aMap[rRow - 1][rCol] != object && aMap[rRow - 1][rCol] != nothing && aMap[rRow - 1][rCol] != grownobject) {
      up = aMap[rRow - 1][rCol];
      //cout << "| Up: " << up << " ";
    }
  }

  //down
  if ((rRow + 1) <= (width - 1)) {
    if (aMap[rRow + 1][rCol] != object && aMap[rRow + 1][rCol] != nothing && aMap[rRow + 1][rCol] != grownobject) {
      down = aMap[rRow + 1][rCol];
      //cout << "| Down: " << down << " ";
    }
  }

  //left
  if ((rCol - 1) >= 0) {
    if (aMap[rRow][rCol - 1] != nothing && aMap[rRow][rCol - 1] != object && aMap[rRow][rCol - 1] != object) {
      left = aMap[rRow][rCol - 1];
      //cout << "| Left: " << left << " ";
    }
  }

  //right
  if ((rCol + 1) <= (height - 1)) {
    if (aMap[rRow][rCol + 1] != nothing && aMap[rRow][rCol + 1] != object && aMap[rRow][rCol + 1] != object) {
      right = aMap[rRow][rCol + 1];
     // cout << "| Right: " << right << " ";
    }
  }

  //get min
  int minValue = std::min(std::min(up, down), std::min(left, right));
  //cout << endl << "Min: " << minValue << endl;

  //Get the Location
  if (up == minValue) {
    r_ROW = rRow - 1;
    r_COL = rCol;
  } else if (down == minValue) {
    r_ROW = rRow + 1;
    r_COL = rCol;
  } else if (left == minValue) {
    r_ROW = rRow;
    r_COL = rCol - 1;
  } else if (right == minValue) {
    r_ROW = rRow;
    r_COL = rCol + 1;
  }

  
  rValue = aMap[r_ROW][r_COL];
  aMap[r_ROW][r_COL] = robot;

  
  conversion(true);

//cout<<"XFLAG Condition"<<" OLDX:"<<OLDX << "||"<<"grid_row:"<<grid_row << "XFLAG: "<<XFLAG;
//cout<<"YFLAG Condition"<<" OLDY:"<<OLDY << "||"<<"grid_col:"<<grid_col << "YFLAG: "<<YFLAG;

//Path relaxation
  XFLAG=OLDX!=-20 && OLDX!=grid_row;
  YFLAG=OLDY!=9 && OLDY!=grid_col;


tempCounter++;  

if(OLDX==-20 && OLDY==9)
{
	pilot(grid_row,grid_col);
}
 else if(XFLAG && YFLAG)
  {
      XFLAG=false;
      YFLAG=false;
      pilot(grid_row,grid_col);
  }
  else if (tempCounter>2)
  {
  	pilot(grid_row,grid_col);
  	tempCounter=0;
  }

  OLDX=grid_row;
  OLDY=grid_col;

 
}





void print_map() {
    ofstream outFile("/usr/local/share/player/examples/libplayerc++/result.txt");

    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        if (aMap[i][j] == object) {
          outFile << "O ";
        } else if (aMap[i][j] == grownobject) {
          outFile << "N ";
        } else if (aMap[i][j] == robot) {
          outFile << "*R* ";
        } else if (aMap[i][j] == goal) {
          outFile << "*G* ";
        } else if (aMap[i][j] == brValue) {
          outFile << "*S* ";
        } else if (aMap[i][j] == traversedNode) {
          outFile << "P ";
        } else {
          outFile << aMap[i][j] << " ";
        }
      }
      outFile << endl;
    }
    cout << "Check result file" << endl;

  
  cout << endl;
}

void deallocate()
{
  for (int i = 0; i < width; i++) {
    delete[] aMap[i];
  }
  delete[] aMap;
}

int main(int argc, const char * argv[]) {

  temp_x = atof(argv[1]);
  temp_y = atof(argv[2]);


  //setUp Robot
  aRobot.Read();
  pp.SetMotorEnable(true);
  //read pnm file and initlize grid
  initializeGrid();
  //coordinates to index
  initializeMapGoal();
  //grow obstacles
  obstacleGrowth();
  propagate_wavefront(r_ROW, r_COL);
  while (rValue != 2) {
     // cout << "Previous robot location: " << r_ROW << ":" << r_COL << endl;
    //Move robot to goal
    moveRobotToGoal();
     // cout << "Current robot location: " << r_ROW << ":" << r_COL << endl;
    }
   print_map();
  deallocate();
  return 0;
}
