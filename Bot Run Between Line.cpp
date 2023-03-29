
#include <stdio.h>
#include <iostream>

#include "RobotConnector.h"

#include "cv.h"
#include "highgui.h"

using namespace std;

#define Create_Comport "COM3"

bool isRecord = false;

int main()
{
	CreateData	robotData;
	RobotConnector	robot;

	ofstream	record;
	record.open("../data/robot.txt");

	if( !robot.Connect(Create_Comport) )
	{
		cout << "Error : Can't connect to robot @" << Create_Comport << endl;
		return -1;
	}

	robot.DriveDirect(0, 0);
	cvNamedWindow("Robot");


	while(true)
	{
		char c = cvWaitKey(30);
		if( c == 27 ) break;
	
		double vx, vz;
		vx = vz = 0.0;

		/*switch(c)
		{
		case 'w': vx = +1; break;
		case 's': vx = -1; break;
		case 'a': vz = +1; break;
		case 'd': vz = -1; break;
		case ' ': vx = vz = 0; break;
		case 'c': robot.Connect(Create_Comport); break;
		}*/

		/*int color = (abs(velL)+abs(velR))/4;
		color = (color < 0) ? 0 : (color > 255) ? 255 : color;
		*/

		int inten = (robotData.cliffSignal[1] + robotData.cliffSignal[2])/8 - 63;
		int inten2 = (robotData.cliffSignal[0] + robotData.cliffSignal[3])/8 - 63;
		inten = (inten < 0) ? 0 : (inten > 255) ? 255 : inten;
		inten2 = (inten2 < 0) ? 0 : (inten2 > 255) ? 255 : inten2;

		bool middle = inten > 100 && inten < 200;

		bool middle2 = inten2 > 70 && inten2 < 130;
		bool tooleft1 = robotData.cliffSignal[1] < 460 && robotData.cliffSignal[2] < 920 && robotData.cliffSignal[3] > 800 && robotData.cliffSignal[0] < 460;
		bool tooleft2 = robotData.cliffSignal[1] > 620 && robotData.cliffSignal[2] > 1300 && robotData.cliffSignal[3] < 680 && robotData.cliffSignal[0] > 650;
		bool tooright1 = robotData.cliffSignal[1] < 460 && robotData.cliffSignal[2] < 920 && robotData.cliffSignal[3] < 680 && robotData.cliffSignal[0] > 650;
		bool tooright2 = robotData.cliffSignal[1] > 620 && robotData.cliffSignal[2] > 1300 && robotData.cliffSignal[3] > 800 && robotData.cliffSignal[0] < 460;

		if (middle && middle2) {
			vx = +0.3;
		} 
		else if (tooleft1 || tooleft2) {
			vx = +0.1;
			vz = -0.8;
		}
		else if (tooright1 || tooright2) {
			vx = +0.1;
			vz = 0.8;
		}
		else {
			vz = -0.05;
		}

		double vl = vx - vz;
		double vr = vx + vz;

		int velL = (int)(vl * Create_MaxVel);
		int velR = (int)(vr * Create_MaxVel);


		cout << inten << " " << inten2 << " " << robotData.cliffSignal[1] << " " << robotData.cliffSignal[2]/*2*/<< " " << robotData.cliffSignal[3]<<" " << robotData.cliffSignal[0] << endl;

		robot.LEDs(velL > 0, velR > 0, 0, inten);
		
		if( !robot.DriveDirect(velL, velR) )
			cout << "SetControl Fail" << endl;

		if( !robot.ReadData(robotData) )
			cout << "ReadData Fail" << endl;

		if( isRecord )
			record << robotData.cliffSignal[0] << "\t" << robotData.cliffSignal[1] << "\t" << robotData.cliffSignal[2] << "\t" << robotData.cliffSignal[3] << endl;
		//cout << "Robot " << robotData.infrared << endl;
	}

	robot.Disconnect();

	return 0;
}