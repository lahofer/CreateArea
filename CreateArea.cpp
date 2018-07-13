//============================================================================
// Name        : CreateArea.cpp
// Author      : 
// Version     :

//============================================================================

#include "Area.h"
#include "Droplets.h"
#include <iostream>
#include <ctime>
#include <string>
#include <fstream>
#include <cmath>
#include <limits>
#include "textread.h"
#include <string.h>
using namespace std;

/*
 * returns the distance from a point(x/y) to the center of a given droplet
 */
double getDistanceToDroplet(double x, double y, Droplets d){
	return (sqrt(pow(d.getX()-x, 2) + pow(d.getY()-y, 2)));
}

/*
 * generates a filename, which depends on the current date and time 
 */
string generateFilenamePrefix(){
    time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	time (&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer,sizeof(buffer),"%d-%m-%Y-%I:%M:%S",timeinfo);
	std::string str(buffer);
    return str;
}

/*
 * parses the given arguments
 */
void parseArgs(int &argc, char **argv, string &inputFile, string &outputFile, bool &createOutputFile){
	string in("-i");
	string out("-o");
    for(int i = 1; i < argc; i++) {
        if(string(argv[i]) == in ){
			if(i+1 == argc || string(argv[i+1]) == out){
				inputFile = "";
				cerr << "Input file not specified. Process will be terminated." << endl;
				exit(0);
			}
			else{
				i++;
				inputFile = string(argv[i]);
				cout << "Input file: " << inputFile << endl;
				
			}
		} else if(string(argv[i]) == out){
			createOutputFile = true;
			if(i+1 == argc || argv[i+1] == in){
				outputFile = generateFilenamePrefix();
				cout << "No output file specified. The data will be stored in "<< outputFile << endl;
			}
			else{
				i++;
				outputFile = string(argv[i]);
				cout << "Output file: " << outputFile << endl;
			}
		}
		else{
			cout << "Option \""<< argv[i] << "\" was not recognized." << endl; 
		}
    }
}

/*
 * writes every Droplet of the area in the given file
 */
void writeBasicOutput(Area a, string outputFile){
	ofstream fos(outputFile+".txt");
	if(!fos || ! fos.is_open()){
		cerr << "There was a problem opening the output file " << outputFile <<".txt" << endl;
		exit(0);
	}
	for(Droplets d : a.getDroplets() ){
		fos << d << "\n";
	}
	fos.close();
}

/*
 * creates a vtk file for paraview used
 */
void writeVTKFiles(Area a, string outputFilePrefix){
	ofstream fosS(outputFilePrefix+"_area_size.vtk");
	ofstream fosA(outputFilePrefix+"_angle_dist.vtk");
	if(!fosS  || !fosA|| ! fosS.is_open() ||  !fosA.is_open()){
		cerr << "There was a problem opening the output files." << endl;
		exit(0);
	}
	cout << "Output files successfully opened." << endl;
	fosS << "# vtk DataFile Version 3.0\n" ;
	fosA << "# vtk DataFile Version 3.0\n" ;
	fosS << "vtk Output in ascii\n";
	fosA << "vtk Output in ascii\n";
	fosS << "ASCII\n";
	fosA << "ASCII\n";
	fosS << "DATASET STRUCTURED_POINTS\n";
	fosA << "DATASET STRUCTURED_POINTS\n";
	fosS <<"DIMENSIONS " << a.getWidth() << " " << a.getHeight() << " 1"<<"\n";
	fosA <<"DIMENSIONS " << a.getWidth() << " " << a.getHeight() << " 1"<<"\n";
	fosS << "ORIGIN 0 0 0\n";
	fosA << "ORIGIN 0 0 0\n";
	fosS << "SPACING 1 1 1\n";
	fosA << "SPACING 1 1 1\n";
	fosS << "POINT_DATA " << (a.getWidth()*a.getWidth()) << "\n\n";
	fosA << "POINT_DATA " << (a.getWidth()*a.getWidth()) << "\n\n";
	fosS << "SCALARS Cells int 1\n";
	fosA << "SCALARS Cells double 1\n";
	fosS << "LOOKUP_TABLE default\n";
	fosA << "LOOKUP_TABLE default\n";
	for(int x = 0; x < (int)(a.getWidth()+0.5); x++){
		for(int y = 0; y < (int)(a.getHeight()+0.5); y++){
			int value = 0;
			unsigned int minID = 0;
			double angle;
            double distanceMin = numeric_limits<double>::max();
			for(Droplets d : a.getDroplets() ){
				double distXY = getDistanceToDroplet(x, y, d);
				if(distXY <= d.getRadian()){
					value = 1;
				}
				if(distXY < distanceMin){
					distanceMin = distXY;
					minID = d.getID();
					angle = d.getAngle();
				}
				
			}
			fosS << value << "\n";
			fosA << angle << "\n";
		}
	}
	fosS.close();
	fosA.close();
	cout << "Output files successfully closed." << endl;
}

void writeOutputFiles(Area a, string outputFilePrefix){
	writeBasicOutput(a, outputFilePrefix);
    writeVTKFiles(a, outputFilePrefix);
}

int main(int argc, char *argv[]){
	string inputFile("");
    string outputFile("");
    bool createOutputFile = false;
	parseArgs(argc, argv, inputFile, outputFile, createOutputFile);
	
	if(inputFile != string("")){
        double paras[11];
		char *cstr = new char[inputFile.length() + 1];
		strcpy(cstr, inputFile.c_str());
		int state = textread(cstr, paras, 11);
		if(state < 0){
			cerr << "There was a problem opening the input file." << endl;
			exit(0);
		}
		delete [] cstr;
		/* read the parameters for the constructor of the area */
		
		cout << "n = " << paras[0] << endl;
		cout << "width = " << paras[1] << "\theight = " << paras[2] << endl;
		cout << "Angle Minimum = " << paras[3] << "\tAngle Maximum = " << paras[4] << 
			"\tAngle Mean = " << paras[5] << "\tAngle Standard Deviation = " << paras[6] << endl;
		cout << "Droplet Size Minimum = " << paras[7] << "\tDroplet size Maximum = " << paras[8] << 
			"\tDroplet Size Mean = " << paras[9] << "\tDroplet Size Standard Deviation = " << paras[10] << endl;

		
		Area a((int)(paras[0]), paras[1], paras[2], paras[3], paras[4], paras[5], paras[6],
			paras[7], paras[8], paras[9], paras[10]);

		if(createOutputFile){
			
			if( outputFile == string("")){
				
				outputFile = generateFilenamePrefix();
				cout << "The data will be stored in "<< outputFile << endl;
			}
			writeOutputFiles(a, outputFile);
		}
		
		else{
			
			for(Droplets d : a.getDroplets() ){
				cout << d << endl;
			}
		}
		
	}
	
	if(inputFile == string("")){
		
		cout << "Default values are used." << endl;
		Area a;
		if(createOutputFile){
			if( outputFile == string("")){
				outputFile = generateFilenamePrefix();
				cout << "The data will be stored in "<< outputFile << endl;
			}
			writeOutputFiles(a, outputFile);
		}
		else{
			for(Droplets d : a.getDroplets() ){
				cout << d << endl;
			}
		}
	}
	cout << "Process successful" << endl;
	return 0;
}
