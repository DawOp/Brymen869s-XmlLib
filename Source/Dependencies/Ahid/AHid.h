/*
	AHid.h

	Interface of AHid.dll v3.0. 
	This driver can be used to read and write to custom USB HID devices.

	Copyright 12.04.2019 by Ralf Handrich
*/
#ifndef __AHID_H
#define __AHID_H

#include <Windows.h>

#ifndef AHIDLIBAPI
#define AHIDLIBAPI extern "C" __declspec(dllimport)
#endif //AHIDLIBAPI

/*	Function return values. */
#define AHID_OK							(0)
#define AHID_ERROR						(-1)

/*	Report types. */
#define AHID_REPORT_TYPE_INPUT			(0)
#define AHID_REPORT_TYPE_OUTPUT			(1)

/* Function prototypes */
/*	
	int AHid_init(unsigned char * parm)

	Initialization of the DLL. This function must be called before any other.
	
	Parameters:		parm - License key (can be NULL).
	
	Return value:	AHID_ERROR	- License key is not valid.
					AHID_OK		- Otherwise.
*/
AHIDLIBAPI int AHid_init(unsigned char * parm);

/*
	int AHid_register(int * handle, int vid, int pid, int interfaceID, int reportID, int reportSize, int reportType)

	Create a pipe handle that is used for writing or reading data to/from your HID device. The parameters used are all device
	specific. Therefore, you need access to the USB descriptors of your device.

	Parameters:		*handle		- A pointer to a pipe handle that is used in other functions.
					vid			- Vendor ID.
					pid			- Product ID.
					interfaceID	- Interface ID (set to -1 in case no interfaces are defined).
					reportID	- Report ID (set to 0 in case no report ID is defined).
					reportSize	- Size of the HID report.
					reportType	- Input or Output.

	Return value:	AHID_OK		- You have allready called AHid_init() and the parameters are valid.
					AHID_ERROR	- Otherwise.
*/
AHIDLIBAPI int AHid_register(int * handle, int vid, int pid, int interfaceID, int reportID, int reportSize, int reportType);

/*
	int AHid_deregister(int * handle)

	Remove a pipe handle. 

	Parameters:		*handle		- A pointer to the pipe handle to remove. This handle can no longer be used to call other functions.

	Return value:	AHID_OK		- You have allready called AHid_init() and the pipe handle is valid.
					AHID_ERROR	- Otherwise.
*/
AHIDLIBAPI int AHid_deregister(int * handle);

/*
	int AHid_write(int handle, unsigned char * buffer, int bytesToWrite, int * bytesWritten)

	Write to your HID device.

	Parameters:		handle		- Valid pipe handle of an Output report.
					*buffer		- A pointer to a buffer with the data to write.
					bytesToWrite	- Size of the transfer buffer.
					*bytesWritten	- The number of bytes that are going to be transferred.

	Return value:	AHID_OK		- You have allready called AHid_init() and the pipe handle identifies an Output report.
					AHID_ERROR	- Otherwise.
*/
AHIDLIBAPI int AHid_write(int handle, unsigned char * buffer, int bytesToWrite, int * bytesWritten);

/*
	int AHid_read(int handle, unsigned char * buffer, int bytesToRead, int * bytesRead)

	Read from your HID device.

	Parameters:		handle		- Valid pipe handle of an Input report.
					*buffer		- A pointer to a buffer with the data read.
					bytesToRead	- Size of the transfer buffer.
					*bytesRead	- The number of valid bytes in the buffer.

	Return value:	AHID_OK		- You have allready called AHid_init(), the pipe handle identifies an Input report and data was read.
					AHID_ERROR	- Otherwise
*/
AHIDLIBAPI int AHid_read(int handle, unsigned char * buffer, int bytesToRead, int * bytesRead);

/*
	int AHid_find(int handle)

	Find your HID device on USB.

	Parameters:		handle		- Valid pipe handle.

	Return value:	AHID_OK		- The HID device was found on USB.
					AHID_ERROR	- Otherwise
*/
AHIDLIBAPI int AHid_find(int handle);

/*
	int AHid_identify(int handle, unsigned char * buffer, int bufferSize, int * bytesProcessed)

	The buffer holds a string with the GUID of your HID device. Pipe handles of the same HID device have the same GUID.

	Parameters:		handle		- Valid pipe handle of an Input report.
					*buffer		- A pointer to a buffer with the data read.
					bufferSize	- Size of the buffer.
					*bytesProcessed	- The number of valid bytes in the buffer.

	Return value:	AHID_OK		- You have allready called AHid_init(), the pipe handle is valid and the buffer has valid data.
					AHID_ERROR	- Otherwise
*/
AHIDLIBAPI int AHid_identify(int handle, unsigned char * buffer, int bufferSize, int * bytesProcessed);

/*
	int AHid_info(unsigned char * buffer1, int bufferSize1, unsigned char * buffer2, int bufferSize2)

	You can call this function for additional information of the driver. 

	Parameters:		*buffer1	- A pointer to a buffer with the data read.
					bufferSize1	- Buffer size.
					*buffer2	- A pointer to a buffer with the data read.
					bufferSize2	- Buffer size.

	Return value:	AHID_OK		- You have allready called AHid_init() and the buffers have valid data.
					AHID_ERROR	- Otherwise
*/
AHIDLIBAPI int AHid_info(unsigned char * buffer1, int bufferSize1, unsigned char * buffer2, int bufferSize2);

#endif //__AHID_H


