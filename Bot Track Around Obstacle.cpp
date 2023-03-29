#include <chrono>
#include <stdio.h>
#include <iostream>

#include "RobotConnector.h"

#include "cv.h"
#include "highgui.h"

#include<ctime>

using namespace std;

#define Create_Comport "COM3"

bool isRecord = false;

int main()
{
	CreateData	robotData;
	RobotConnector	robot;

	ofstream	record;
	record.open("../data/robot.txt");

	auto start_time = std::chrono::high_resolution_clock::now();
	auto current_time = std::chrono::high_resolution_clock::now();


	if (!robot.Connect(Create_Comport))
	{
		cout << "Error : Can't connect to robot @" << Create_Comport << endl;
		return -1;
	}

	robot.DriveDirect(0, 0);
	cvNamedWindow("Robot");

	int ang = 0;
	int maxx = 0;
	int count = 0;

	float atime = 0;
	float dtime = 0;
	int state = 0;
	bool stop = false;
	int TOTALT = 0;
	int btime = 0;

	while (true)
	{
		if (stop) {
			cout << ((float)TOTALT / 1000) * 5.3447619;
			break;
		}
	
		char c = cvWaitKey(30);

		double vx, vz;
		vx = vz = 0.0;
		int  t = robotData.angle;
		int  dis = robotData.distance;
		int Wall = robotData.wallSignal;
		
		ang += t;
		maxx = maxx > ang ? maxx : ang;
		if ( ang <= -360 - 1.2*(maxx)) count+=1;

		cout << ang << " " << dis << endl;


		
		
		
		
		/*
		switch(c)
		{
		case 'w': vx = +1; break;
		case 's': vx = -1; break;
		case 'a': vz = +1; break;
		case 'd': vz = -1; break;
		case ' ': vx = vz = 0; break;
		case 'c': robot.Connect(Create_Comport); break;
		}
		*/

		/*int color = (abs(velL)+abs(velR))/4;
		color = (color < 0) ? 0 : (color > 255) ? 255 : color;
		*/

		/*
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

		cout << inten << " " << inten2 << " " << robotData.cliffSignal[1] << " " << robotData.cliffSignal[2]<< " " << robotData.cliffSignal[3]<<" " << robotData.cliffSignal[0] << endl;
		if( isRecord )
			record << robotData.cliffSignal[0] << "\t" << robotData.cliffSignal[1] << "\t" << robotData.cliffSignal[2] << "\t" << robotData.cliffSignal[3] << endl;
		*/

		switch (state)
		{
		case 0:
			start_time = std::chrono::high_resolution_clock::now();
			vx = 0.3;
			if (robotData.bumper[0] != 0 || robotData.bumper[1] != 0) state = 1;
			break;
		case 1:
			current_time = std::chrono::high_resolution_clock::now();
			vz = 0.1;
			if (robotData.bumper[0] == 0 && robotData.bumper[1] != 0) {
				btime = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();
			}
			dtime += 0.01;
			atime += 0.01;
			if (atime >= 0.2) {
				atime = 0;
				state = 2;
			}
			break;
		case 2:
			start_time = std::chrono::high_resolution_clock::now();
			vx = 0.2;
			vz = -0.2;
			if (robotData.bumper[0] != 0 || robotData.bumper[1] != 0) state = 1;
			TOTALT += btime;
			btime = 0;
			break;
		default:
			vx = 0;
			break;
		}
		if (count >= 1) c = 'a';
		if (c == 'a') {
			vx = vz = 0;
			stop = true;
		}

		double vl = vx - vz;
		double vr = vx + vz;

		int velL = (int)(vl * Create_MaxVel);
		int velR = (int)(vr * Create_MaxVel);

		robot.LEDs(velL > 0, velR > 0, 0, 0);


		if (!robot.DriveDirect(velL, velR))
			cout << "SetControl Fail" << endl;

		if (!robot.ReadData(robotData))
			cout << "ReadData Fail" << endl;

	}

	robot.Disconnect();

	return 0;
}