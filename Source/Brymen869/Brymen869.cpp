/* Brymen869.cpp : Defines the exported functions for the DLL application. */

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <ctime>    
#include <map>
#include "stdafx.h"
#include "Brymen869.h"
#include <Ahid\AHid.h>
#include <tinyxml2\tinyxml2.h>


#pragma warning(disable : 4996)


#define TIMER_INTERVALL_MS				(100)
#define VID								(0x0820)
#define PID								(0x0001)   

unsigned char data_table[64];  /* buffer */
int inputHandle;
int outputHandle;
bool isConnected;
bool readed;
std::string date; 


void init(){
	isConnected = false;
	outputHandle = 0;
	inputHandle = 0;
	readed = false;
}


int connect() {
	if (!isConnected) {
		unsigned char * parm = (unsigned char *)"\0";
		int result;

		result = AHid_init(parm);
		if (AHID_ERROR == result) {
			return-1;  
		}

		AHid_register(&outputHandle, VID, PID, -1, 0, 64, AHID_REPORT_TYPE_OUTPUT);
		AHid_register(&inputHandle, VID, PID, -1, 0, 64, AHID_REPORT_TYPE_INPUT);

		if (AHid_find(inputHandle) || AHid_find(outputHandle)) {
			return -1;
		}

		isConnected = true;
		return 0;
	}
	else {
		AHid_deregister(&outputHandle);
		AHid_deregister(&inputHandle);
		isConnected = false;
		return -1;
	}
}



void xmlBuild(std::string& measurement_1, std::string& measurement_2, unsigned char* data_table, std::string& date, Brymen_CallbackType cb, void* user_data) {
	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLNode * pRoot = xmlDoc.NewElement("POMIARY_BRYMEN");
	xmlDoc.InsertFirstChild(pRoot);

	tinyxml2::XMLElement* pElement = xmlDoc.NewElement("Pomiar_dolny");
	pElement->SetText(measurement_1.c_str());
	pRoot->InsertEndChild(pElement);

	pElement = xmlDoc.NewElement("Pomiar_gorny");
	pElement->SetText(measurement_2.c_str());
	pRoot->InsertEndChild(pElement);

	if ((data_table[1]) & 1) {
		pElement = xmlDoc.NewElement("AUTO");
		pElement->SetText("ON");
		pRoot->InsertEndChild(pElement);
	}
	if ((data_table[1] >> 1) & 1) {
		pElement = xmlDoc.NewElement("R");
		pElement->SetText("ON");
		pRoot->InsertEndChild(pElement);
	}
	if ((data_table[1] >> 2) & 1) {
		pElement = xmlDoc.NewElement("C");
		pElement->SetText("ON");
		pRoot->InsertEndChild(pElement);
	}
	if ((data_table[1] >> 3) & 1) {
		pElement = xmlDoc.NewElement("H");
		pElement->SetText("ON");
		pRoot->InsertEndChild(pElement);
	}
	if ((data_table[1] >> 5) & 1) {
		pElement = xmlDoc.NewElement("MAX");
		pElement->SetText("ON");
		pRoot->InsertEndChild(pElement);
	}
	if ((data_table[1] >> 6) & 1) {
		pElement = xmlDoc.NewElement("MIN");
		pElement->SetText("ON");
		pRoot->InsertEndChild(pElement);
	}
	if ((data_table[1] >> 7) & 1) {
		pElement = xmlDoc.NewElement("AVG");
		pElement->SetText("ON");
		pRoot->InsertEndChild(pElement);
	}
	if ((data_table[2] >> 6) & 1) {
		pElement = xmlDoc.NewElement("VFD");
		pElement->SetText("ON");
		pRoot->InsertEndChild(pElement);
	}
	if ((data_table[3]) & 1) {
		pElement = xmlDoc.NewElement("Triangle");
		pElement->SetText("ON");
		pRoot->InsertEndChild(pElement);
	}

	if ((data_table[9] >> 7) & 1) {
		pElement = xmlDoc.NewElement("+-");
		pElement->SetText("ON");
		pRoot->InsertEndChild(pElement);
	}

	pElement = xmlDoc.NewElement("Data");
	pElement->SetText(date.c_str());
	pRoot->InsertEndChild(pElement);


	std::string n;
	tinyxml2::XMLElement* plement = pRoot->ToElement();

	for (tinyxml2::XMLElement* child = plement->FirstChildElement(); child != NULL; child = child->NextSiblingElement()) {
		std::stringstream ss;
		tinyxml2::XMLPrinter printer;
		child->Accept(&printer);
		ss << printer.CStr();
		std::string d = ss.str();
		n += d;
	}

	/* Perform callback function */
	cb(n.c_str(),n.size(),user_data);
}



void decode(Brymen_CallbackType cb, void* user_data) {
	std::map<unsigned char, std::string> sevNum;


	sevNum[0b10111110] = "0";
	sevNum[0b10100000] = "1";
	sevNum[0b11011010] = "2";
	sevNum[0b11111000] = "3";
	sevNum[0b11100100] = "4";
	sevNum[0b01111100] = "5";
	sevNum[0b01111110] = "6";
	sevNum[0b10101000] = "7";
	sevNum[0b11111110] = "8";
	sevNum[0b11111100] = "9";
	sevNum[0b00000000] = " ";
	sevNum[0b01000000] = "-";
	sevNum[0b01001110] = "F";
	sevNum[0b00011110] = "C";
	sevNum[0b00010110] = "L";
	sevNum[0b11110010] = "d";
	sevNum[0b00100000] = "i";
	sevNum[0b01110010] = "o";
	sevNum[0b01011110] = "E";
	sevNum[0b01000010] = "r";
	sevNum[0b01100010] = "n";


	std::string measurement_1;
	std::string measurement_2;
	std::string unit_1;
	std::string unit_2;


	for (int i = 3; i < 9; i++) {
		if (i != 3 && i != 8 && (data_table[i] & 1)) {
			measurement_1 += ".";
			measurement_1 += sevNum[data_table[i] - 1];
		}
		else if ((i == 3 || i == 8) && (data_table[i] & 1)) {
			measurement_1 += sevNum[data_table[i] - 1];
		}
		else {
			measurement_1 += sevNum[data_table[i]];
		}
	}



	for (int i = 10; i < 14; i++) {
		if (i != 10 && (data_table[i]) & 1) {
			measurement_2 += ".";
			measurement_2 += sevNum[data_table[i] - 1];
		}
		else if (i == 10 && (data_table[i] & 1)) {
		    measurement_1 += sevNum[data_table[i] - 1];
		}
		else {
		    measurement_2 += sevNum[data_table[i]];
		}
	}




	if ((data_table[2] >> 7) & 1) {
	    measurement_1.insert(0, "-");
	}
	if ((data_table[9] >> 4) & 1) {
	    measurement_2.insert(0, "-");
	}

	if (((data_table[2]) & 1) && ((data_table[1] >> 4) & 1)) {
	    measurement_1.insert(0, "DC+AC  ");
	}
	else if ((data_table[1] >> 4) & 1) {
	    measurement_1.insert(0, "DC  ");
	}
	else if ((data_table[2]) & 1) {
	    measurement_1.insert(0, "AC  ");
	}
	else if (((data_table[2] >> 1) & 1) && ((data_table[2] >> 3) & 1)) {
	    measurement_1.insert(0, "T1-T2  ");
	}
	else if ((data_table[2] >> 1) & 1) {
	    measurement_1.insert(0, "T1  ");
	}
	else if ((data_table[2] >> 3) & 1) {
	    measurement_1.insert(0, "T2  ");
	}

	if ((data_table[9] >> 5) & 1) {
	    measurement_2.insert(0, "AC  ");
	}
	else if ((data_table[9] >> 6) & 1) {
	    measurement_2.insert(0, "T2  ");
	}


	if ((data_table[15] >> 4) & 1) {
	    unit_1 = "Ohm";
	}
	else if (data_table[15] & 1) {
	    unit_1 = "Hz";
	}
	else if ((data_table[15] >> 1) & 1) {
	    unit_1 = "dBm";
	}
	else if (data_table[8] & 1) {
	    unit_1 = "V";
	}
	else if ((data_table[14] >> 7) & 1) {
	    unit_1 = "A";
	}
	else if ((data_table[14] >> 5) & 1) {
	    unit_1 = "F";
	}
	else if ((data_table[14] >> 4) & 1) {
	    unit_1 = "S";
	}
	else if ((data_table[15] >> 7) & 1) {
	    unit_1 = "D%";
	}

	if ((data_table[15] >> 6) & 1) {
	    unit_1.insert(0, "k");
	}
	else if ((data_table[15] >> 5) & 1) {
	    unit_1.insert(0, "M");
	}
	else if ((data_table[15] >> 2) & 1 && !((data_table[15] >> 1) & 1)) {
	    unit_1.insert(0, "m");
	}
	else if ((data_table[15] >> 3) & 1) {
	    unit_1.insert(0, "u");
	}
	else if ((data_table[14] >> 6) & 1) {
	    unit_1.insert(0, "n");
	}



	if ((data_table[14] >> 2) & 1) {
	    unit_2 = "Hz";
	}
	else if ((data_table[9] >> 2) & 1) {
	    unit_2 = "A";
	}
	else if ((data_table[14] >> 3) & 1) {
	    unit_2 = "V";
	}
	else if ((data_table[9] >> 3) & 1) {
	    unit_2 = "%4-20mA";
	}

	if (data_table[14] & 1) {
	    unit_2.insert(0, "M");
	}
	else if ((data_table[14] >> 1) & 1) {
	    unit_2.insert(0, "k");
	}
	else if (data_table[9] & 1) {
	    unit_2.insert(0, "u");
	}
	else if (data_table[9] >> 1 & 1) {
	    unit_2.insert(0, "m");
	}

	/* Passsing strings to build xml */
	xmlBuild(measurement_1, measurement_2, data_table, date, cb, user_data);
}



void read() {
    if (!isConnected) {
	return;
    }

    int bytesRead = 0;
    std::string measure;

    for (int i = 0; i < TIMER_INTERVALL_MS; i++) {
	if (AHID_OK == AHid_read(inputHandle, data_table, 64, &bytesRead)) {
	    if (bytesRead > 20) {
		auto end = std::chrono::system_clock::now();
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);
		std::string date = std::ctime(&end_time);
		readed = true;
	    }
	}
    }
}


void sendOut() { // send queries
    int bytesWritten = 0;
    unsigned char data_commands[] = { '\0', '†' , 'f' };

    AHid_write(outputHandle, data_commands, 3, &bytesWritten);
}

void readCall() { // call read per 100 ms
    while (!readed && isConnected) {
	std::this_thread::sleep_for(std::chrono::milliseconds(TIMER_INTERVALL_MS));
	read();
    }
}


int Brymen_start() {
    init();
    if (connect() == AHID_ERROR) {
	return -1;
    }
    return 0;
}

void Brymen_shutdown() {
    readed = false;
    isConnected = false;
}

void Brymen_registerCallback(Brymen_CallbackType cb, void * user_data) {
    std::thread(readCall).detach();

    while (!readed && isConnected) {
	sendOut();
	std::chrono::seconds dura(1);
	std::this_thread::sleep_for(dura);
    }

    decode(cb, user_data);
    readed = false;
}
