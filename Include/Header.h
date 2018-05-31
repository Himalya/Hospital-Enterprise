/******************************************************************************
 *
 * File Name:  	Header.h
 *
 * Description:	This is a file that contains all the declaration of variables,
 *		function prototype as well as structure declaration.
 *
 ******************************************************************************/
#ifndef _HEADER_H
#define _HEADER_H


/****************
 * Include Files
 ****************/
#include <stdio.h>
#include <string.h>
#include "xmlreader.h"
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/types.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdint.h>


/***********************************
 *   Defines, Macros and Typedefs
 ***********************************/

#define  DBG_LEVEL_1    1 		// Lowest debug level
#define  DBG_LEVEL_2    2 		// Highest debug level

#define NODE_VALUE_SIZE 255		// maximum length of node value
#define INPUT_SIZE 50			// maximum value of input
#define INITIAL_VALUE 0			// initial value
#define NODE_VALUE_DISABLED 0		// node value not present
#define ENABLED 1			// value is present
#define DISABLED 0			// value is not present
#define SIZE 5				// maximum size of queue
#define EMPTY -1			// queue empty

#define EYE 0				// value for eye doctor
#define NEURON 1			// value for neuron doctor
#define HEART 2				// value for heart doctor
#define BONES 3				// value for bones doctor
#define SKIN 4				// value for skin doctor

#define AC 1				// value for ac room
#define NON_AC 2			// value for non-ac room
#define DELUXE 3			// value for deluxe room

#define NORMAL_CHECKUP 1		// value for normal checkup
#define OPERATION 2			// value for operation

#define SUCCESS 0			// defines success of a function
#define COMP_SUCCESS 0			// successful comparison
 

/***********************
 *   Enumarations
 ***********************/

enum Config_Errorcode
{
	CONFIGFILE_UNABLE_TO_OPEN=-255,				// configuration file is not opened
	TYPE_VALUE_NOTFOUND,					// value of attribute 'type' is not specified
	TYPE_VALUE_MISMATCH,					// value of attribute 'type' is not supported
	FORMAT_VALUE_NOTFOUND,					// value of format is not specified
	F0RMAT_VALUE_MISMATCH,					// value of format mismatch
	EYE_ENABLE_VALUE_NOTSUPPORTED,				// eye enable value is not supported
	EYE_ENABLE_VALUE_MISMATCH,				// eye enable value mismatch
	NEURON_ENABLE_VALUE_NOTSUPPORTED,			// neuron enable value is not supported
	NEURON_ENABLE_VALUE_MISMATCH,				// neuron enable value mismatch
	HEART_ENABLE_VALUE_NOTSUPPORTED,			// heart enable value is not supported
	HEART_ENABLE_VALUE_MISMATCH,				// heart enable value mismatch
	BONES_ENABLE_VALUE_NOTSUPPORTED,			// bones enable value is not supported
	BONES_ENABLE_VALUE_MISMATCH,				// bones enable value mismatch
	SKIN_ENABLE_VALUE_NOTSUPPORTED,				// skin enable value is not supported
	SKIN_ENABLE_VALUE_MISMATCH,				// skin enable value mismatch
	NODE_MISSING_IN_CONFIG_FILE,				// node not present
	ATTRIBUTE_MISSING_IN_CONFIG_FILE,			// attribute not present
	FAILED_TO_PARSE_FILE,					// file can't not be parsed
};

enum ReturnCode
{
	FAILURE=-155,						// execution failure
	ERROR_IN_OPENING_FILE,					// error in opening file
	ERROR_IN_READING_FILEDATA,				// error in reading file
	ERROR_IN_WRITING_FILEDATA,				// error in writing file
	EXIT_WITH_ERROR=-1,					// abnormal exit
};



/***********************
 *    Globals
 ***********************/
pthread_mutex_t lockEye, lockNeuron, lockHeart, lockBones, lockSkin; 	// mutex variables for different wards



/***********************
 *    Structures
 ***********************/

//stores message buffer info
struct buff{
	long type;				// message buffer type variable
	int enabled[5];				// status of enabled doctors
	char service[10];			// types of services available
	int severity;				// severity level
}msg;

//stores file pointers
typedef struct file_ptrs
{
	FILE *ref_Ptr;				// pointer to reference file
	FILE *proc_Ptr;				// pointer to processed file
	FILE *err_Ptr;				// pointer to error log file
	FILE **report_Ptr;			// pointer to report files
}FILESPTR;

//stores config file's doctor info
typedef struct config_doctor_info_buf
{
	short int no_of_doctor;			// no of doctors enabled
}QM_INFO;

//stores configuration file info
typedef struct configuration_info_buf
{
	short int enabledEye;			// stores availability of eye doctor
	short int enabledNeuron;		// stores availability of neuron doctor
	short int enabledHeart;			// stores availability of heart doctor
	short int enabledBones;			// stores availability of bones doctor
	short int enabledSkin;			// stores availability of skin doctor
	char inputParameter[INPUT_SIZE];	// stores input parameter
	QM_INFO qty;				// variable of type config doctor info buff
}CONFIG;

//stores ward structure of the hospital
typedef struct ward{
	int wardId;				// stores ward id
	char wardName[20];			// stores ward name
	int roomTypes;				// stores room types respective to ward
	struct ward *nextWard;			// pointer to next ward
	struct room *wardRoom;			// pointer to ward room
}WARD;

//stores room structure of the hospital
typedef struct room{
	int numRooms;				// stores number of rooms
	int roomTypes;				// stores room types
	struct room *nextRoom;			// pointer to next room
}ROOM;

//stores patient information
typedef struct patient{
	char patientName[20];			// stores patient name
	char disease[20];			// stores patient's disease
	int roomType;				// stores room type respective to patient
	char *entryTime, *exitTime;		// stores entry and exit time of patient
}PATIENT;

//stores hospital information
typedef struct hospital{
	char hospitalName[25];			// stores hospital's name
	char address[50];			// stores hospital's address
	int numWards;				// stores number of wards
	struct ward *wardQueue;			// pointer to the ward queue
}HOSPITAL;

//stores queue information
typedef struct queue{
	int eyeQueue[SIZE];			// stores eye patient
	int neuronQueue[SIZE];			// stores neuron patient
	int heartQueue[SIZE];			// stores heart patient
	int bonesQueue[SIZE];			// stores bones patient
	int skinQueue[SIZE];			// stores skin patient
	int front[SIZE];			// stores front end of queue
	int rear[SIZE];				// stores rear end of queue
}QUEUE;


/************************
 * Function Prototypes
 ************************/
//Dump error into error log file
void dumpError(FILE *,char *,int);
//close error log file and cleanup function for the XML library
void deinitConfigurationParsing(FILESPTR *);
//Gives the attribute value of the specified tag of the xml file
int getAttributeValue(xmlTextReaderPtr,char *,char *,char *,FILESPTR *);
//Validates whether the tag value is enabled or not
int validateFieldEnabledOrNot(char *,FILESPTR *,short int *,int,int,char *);
//Parse the input XML file for active tag
int validateInputfileParamActiveTag(char *,FILESPTR *,CONFIG *,xmlTextReaderPtr);
//Parse the input XML file for input_file_param tag
int validateInputfileParamTag(char *,FILESPTR *,CONFIG *,xmlTextReaderPtr);
//call different function for parsing the configuration file
int* configurationParsing(char *,FILESPTR *,CONFIG *);
//open error log file
int initConfigurationParsing(FILESPTR *);
//Parse the input XML file and store it into a structure
int validateParseConfigfile(char *,FILESPTR *,CONFIG *);

//creates wards in the hospital
WARD *createWard(int, WARD *, ROOM * );
//creates rooms respective to wards in the hospital
ROOM *createRoom(int, ROOM * , int arr[]);
//creates hospital blueprint
int createHospital(HOSPITAL *, WARD *, ROOM *);
//displays hospital blueprint
int displayDetails(HOSPITAL *, WARD *, ROOM *);
//checks patient information
void *checkPatient(void *);
//checks whether queue is full or not
int isFull(QUEUE *, int);
//checks whether queue is empty or not
int isEmpty(QUEUE *, int);
//inserts patient in queue
int enqueue(QUEUE *, int);
//removes patient from queue
int dequeue(QUEUE *, int);
//displays queue information
void displayQueue(QUEUE *, int);
//checks room status
int roomStatus(int, int *);
//creates patient report
int createReport(PATIENT *);
#endif
