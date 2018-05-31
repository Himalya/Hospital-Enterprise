
/******************************************************************************
 *
 * File Name:  functions.c
 *
 * Description:	This is a file that contains all the functions of application.
 *
 ******************************************************************************/
#include "./Include/Debug_Header.h"
#include "./Include/Header.h"
QUEUE queue;								/* Instantiate QUEUE structure */
int eyeArr[3], neuronArr[3], heartArr[3], bonesArr[3], skinArr[3];	/* Room type array for wards */						
int enabledInfo[5];							/* To store enable field info */
int tempIndex = INITIAL_VALUE;						/* temporary index variable */
char tempHosName[20], tempHosAddr[50];					/* Array to store hospital name and address */
int ctrEye = INITIAL_VALUE, ctrNeuron = INITIAL_VALUE,			/* To store counter variable for ward patients*/
ctrHeart = INITIAL_VALUE, ctrBones = INITIAL_VALUE, ctrSkin = INITIAL_VALUE;
/*******************************************************************************
 *
 * Function:	configurationParsing
 *
 * Description:	call different function for parsing the configuration file.
 *
 * Parameters:	char *arg : Name of the configuration file.
 *
 * 		FILESPTR *files : structure pointer to the error log file.
 *
 * 		CONFIG *config : pointer to the configuration buffer.
 *
 * Return Value: It will return array pointer containing information regarding
 *			enabled field in configuration file.
 *
 ******************************************************************************/
int *configurationParsing(char *arg, FILESPTR *files, CONFIG *config)
{
	int returnType; 	/* To store the return type of the function*/
	
	DEBUG_LEVEL_1("ENTERED: configurationParsing\n");

	/* Call initConfigurationParsing function for opening error log file*/
	returnType = initConfigurationParsing(files);
	
	if(returnType != SUCCESS)
	{
		fprintf(stderr, "Error calling initConfigurationParsing...\n");
      		exit(FAILURE);
	}

	/* Call validateParseConfigfile for parsing the xml file*/
	returnType = validateParseConfigfile(arg, files, config);
	
	if(returnType != SUCCESS)
	{
		fprintf(stderr, "Error calling validateParseConfigfile...\n");
      		exit(FAILURE);
	}

	DEBUG_LEVEL_1("COMPLETED: configurationParsing\n");

	return enabledInfo;
}


/*******************************************************************************
 *
 * Function:	initConfigurationParsing
 *
 * Description:	open error log file.
 *
 * Parameters:	structure pointer to the error log file.
 *
 * Return Value: It will return 1 if code evaluates to true else will return
 *				 specific error code.
 *
 ******************************************************************************/
int initConfigurationParsing(FILESPTR *files)
{
	DEBUG_LEVEL_1("ENTERED: initConfigurationParsing\n");

	/* opening error log file*/
	files->err_Ptr = fopen("./Report/Error_Log.txt","w");
	if(files->err_Ptr == NULL)
	{
		fprintf(stderr, "ERROR MSG: Error log file failed to open\n");
		return FAILURE;
	}
	
	DEBUG_LEVEL_1("COMPLETED: initConfigurationParsing\n");

	return SUCCESS;
}	

/*******************************************************************************
 *
 * Function:	validateParseConfigfile
 *
 * Description:	Parse the input XML file and store it into a structure.
 *
 * Parameters:	char *arg : Name of the configuration file.
 *
 * 		FILESPTR *files : structure pointer to the error log file.
 *
 * 		CONFIG *config : pointer to the configuration buffer.
 *
 *
 * Return Value: It will return 1 if code evaluates to true else will return
 *				 specific error code.
 *
 ******************************************************************************/
int validateParseConfigfile(char *arg, FILESPTR *files, CONFIG *config)
{
	xmlTextReaderPtr reader;		/*It is a pointer to the structure xmlTextReader*/
	short int returnReaderValue;		/*To store the return value of the xmlTextReaderRead*/
	int returnType;				/*To store the return type of the function*/

	DEBUG_LEVEL_1("ENTERED: validateParseConfigfile\n");

	/*  xmlReaderForFile is used to parse the input file from the file system
	 *  and if its successfully execute returns new reader*/

	reader = xmlReaderForFile(arg, NULL, 0);
	
	if(reader != NULL)
	{
		/* Points to the first node in the xml file*/
		returnReaderValue = xmlTextReaderRead(reader);
	
		if(returnReaderValue == 1)
		{
			/* call to validateInputfileParamTag function for parsing 
			 * the configuration file for input_file_param tag */
			returnType = validateInputfileParamTag(arg, files, config, reader);
			if(returnType != SUCCESS)
			{
				return returnType;
			}

			/* free xml reader */
			xmlFreeTextReader(reader);
		}
	
		/* if xml file cannot be parsed */
		else
		{
			dumpError(files->err_Ptr, arg, FAILED_TO_PARSE_FILE);
			return FAILED_TO_PARSE_FILE;
		}
	}
	
	/* if xml file cannot be opened */
	else
	{
		dumpError(files->err_Ptr, arg, CONFIGFILE_UNABLE_TO_OPEN);
		return CONFIGFILE_UNABLE_TO_OPEN;
	}
	
	DEBUG_LEVEL_1("COMPLETED: validateParseConfigfile\n");
	
	return SUCCESS;
}


/*******************************************************************************
 *
 * Function:	validateInputfileParamTag
 *
 * Description:	Parse the input XML file for input_file_param tag and store its
 * 				content into a structure.
 *
 * Parameters:	char *arg : Name of the configuration file.
 *
 * 		FILESPTR *files : structure pointer to the error log file.
 *
 *		CONFIG *config : pointer to the configuration buffer.
 *
 *		xmlTextReaderPtr reader : It is a structure pointer to the
 *					structure xmlTextReader.
 *
 * Return Value: It will return 1 if code evaluates to true else will return
 *				 specific error code.
 *
 ******************************************************************************/
int validateInputfileParamTag(char *arg, FILESPTR *files, CONFIG *config, xmlTextReaderPtr reader)
{
	char result[NODE_VALUE_SIZE];		/*To store the value of the xml node*/
	int returnType;				/* To store the return type of the function*/
	
	DEBUG_LEVEL_1("ENTERED: validateInputfileParamTag\n");	
	
	/* call to the getAttributeValue function to get the specified
	 * attribute value of the node specified*/
	returnType = getAttributeValue(reader, "input_file_param", "type", result, files);
	if(returnType != SUCCESS)
	{
		if(returnType == FAILED_TO_PARSE_FILE)
		{
			dumpError(files->err_Ptr, "input_file_param", NODE_MISSING_IN_CONFIG_FILE);
			return returnType;
		}
		
		dumpError(files->err_Ptr, "type", ATTRIBUTE_MISSING_IN_CONFIG_FILE);
		return returnType;
	}
	
	/* If the type value is doctor*/
	if(strcasecmp(result, "doctor") == COMP_SUCCESS)
	{
		strcpy(config->inputParameter, "doctor");
		strcpy(result, "\0");
		
		returnType = validateInputfileParamActiveTag(arg, files, config, reader);
		if(returnType != SUCCESS)
		{
			return returnType;
		}
	}
	
	/* If the type value is missing*/	
	else if(strcmp(result, "VALUE_MISSING") == COMP_SUCCESS)
	{
		dumpError(files->err_Ptr, arg, TYPE_VALUE_NOTFOUND);
		return TYPE_VALUE_NOTFOUND;
	}
	
	/* If the type value is mismatch*/
	else
	{
		dumpError(files->err_Ptr, arg, TYPE_VALUE_MISMATCH);
		return TYPE_VALUE_MISMATCH;
	}
	
	strcpy(result,"\0");
	
	DEBUG_LEVEL_1("COMPLETED: validateInputfileParamTag\n");
	
	return SUCCESS;
}


/*******************************************************************************
 *
 * Function:	validateInputfileParamActiveTag
 *
 * Description:	Parse the input XML file for active_format tag and store its
 * 		content into a structure.
 *
 * Parameters:	char *arg : Name of the configuration file.
 *
 * 		FILESPTR *files : structure pointer to the error log file.
 *
 * 		CONFIG *config : pointer to the configuration buffer.
 *
 *		xmlTextReaderPtr reader : It is a structure pointer to the
 *					structure xmlTextReader.
 *
 * Return Value: It will return 1 if code evaluates to true else will return
 *				 specific error code.
 *
 ******************************************************************************/
int validateInputfileParamActiveTag(char *arg, FILESPTR *files, CONFIG *config, xmlTextReaderPtr reader)
{
	char result[NODE_VALUE_SIZE];			/*To store the value of the xml node*/
	short int doctorCount = INITIAL_VALUE;		/*To store doctors count*/ 
	int returnType;					/*To store the return type of the function*/

	DEBUG_LEVEL_1("ENTERED: validateInputfileParamActiveTag\n");

	/* call to the getAttributeValue function to get the specified
	 * attribute value of the node specified*/
	returnType = getAttributeValue(reader, "doctor_eye", "enable", result, files);
	if(returnType != SUCCESS)
	{
		if(returnType == FAILED_TO_PARSE_FILE)
		{
			dumpError(files->err_Ptr, "doctor_eye", NODE_MISSING_IN_CONFIG_FILE);
			return returnType;
		}
	
		dumpError(files->err_Ptr, "doctor_eye - enable", ATTRIBUTE_MISSING_IN_CONFIG_FILE);
		strcpy(result, "VALUE_MISSING");
	}

	/* call to validateFieldEnabledOrNot to check the enabled fields 
	 * of configuration file */
	returnType = validateFieldEnabledOrNot(arg, files, &(config->enabledEye),EYE_ENABLE_VALUE_NOTSUPPORTED, EYE_ENABLE_VALUE_MISMATCH, result);
	
	if(returnType == ENABLED)
	{
		enabledInfo[tempIndex] = 1;
		tempIndex++;
	}

	else 
	{
		enabledInfo[tempIndex] = 0;
		tempIndex++;
	}
	
	/* call to the getAttributeValue function to get the specified
	 * attribute value of the node specified*/
	returnType = getAttributeValue(reader, "doctor_neuron", "enable", result, files);
	if(returnType != SUCCESS)
	{
		if(returnType == FAILED_TO_PARSE_FILE)
		{
			dumpError(files->err_Ptr, "doctor_neuron", NODE_MISSING_IN_CONFIG_FILE);
			return returnType;
		}
	
		dumpError(files->err_Ptr, "doctor_neuron - enable", ATTRIBUTE_MISSING_IN_CONFIG_FILE);
		strcpy(result, "VALUE_MISSING");
	}

	/* call to validateFieldEnabledOrNot to check the enabled fields 
	 * of configuration file */
	returnType = validateFieldEnabledOrNot(arg, files, &(config->enabledNeuron),NEURON_ENABLE_VALUE_NOTSUPPORTED, NEURON_ENABLE_VALUE_MISMATCH, result);
	
	if(returnType == ENABLED)
	{
		enabledInfo[tempIndex] = 1;
		tempIndex++;
	}

	else 
	{
		enabledInfo[tempIndex] = 0;
		tempIndex++;
	}

	/* call to the getAttributeValue function to get the specified
	 * attribute value of the node specified*/
	returnType = getAttributeValue(reader, "doctor_heart", "enable", result, files);
	
	if(returnType!=SUCCESS)
	{
		if(returnType == FAILED_TO_PARSE_FILE)
		{
			dumpError(files->err_Ptr, "doctor_heart", NODE_MISSING_IN_CONFIG_FILE);
			return returnType;
		}
	
		dumpError(files->err_Ptr, "doctor_heart - enable", ATTRIBUTE_MISSING_IN_CONFIG_FILE);
		strcpy(result, "VALUE_MISSING");
	}

	/* call to validateFieldEnabledOrNot to check the enabled fields 
	 * of configuration file */
	returnType = validateFieldEnabledOrNot(arg, files, &(config->enabledHeart),HEART_ENABLE_VALUE_NOTSUPPORTED, HEART_ENABLE_VALUE_MISMATCH, result);

	if(returnType == ENABLED)
	{
		enabledInfo[tempIndex] = 1;
		tempIndex++;
	}

	else 
	{
		enabledInfo[tempIndex] = 0;
		tempIndex++;
	}

	/* call to the getAttributeValue function to get the specified
	 * attribute value of the node specified*/
	returnType = getAttributeValue(reader, "doctor_bones", "enable", result, files);
	if(returnType != SUCCESS)
	{
		if(returnType == FAILED_TO_PARSE_FILE)
		{
			dumpError(files->err_Ptr, "doctor_bones", NODE_MISSING_IN_CONFIG_FILE);
			return returnType;
		}
	
		dumpError(files->err_Ptr, "doctor_bones - enable", ATTRIBUTE_MISSING_IN_CONFIG_FILE);
		strcpy(result, "VALUE_MISSING");
	}
	
	/* call to validateFieldEnabledOrNot to check the enabled fields 
	 * of configuration file */
	returnType = validateFieldEnabledOrNot(arg, files, &(config->enabledBones), BONES_ENABLE_VALUE_NOTSUPPORTED, BONES_ENABLE_VALUE_MISMATCH, result);
	
	if(returnType == ENABLED)
	{
		enabledInfo[tempIndex] = 1;
		tempIndex++;
	}

	else 
	{
		enabledInfo[tempIndex] = 0;
		tempIndex++;
	}
	
	/* call to the getAttributeValue function to get the specified
	 * attribute value of the node specified*/
	returnType = getAttributeValue(reader, "doctor_skin", "enable", result, files);
	if(returnType != SUCCESS)
	{
		if(returnType == FAILED_TO_PARSE_FILE)
		{
			dumpError(files->err_Ptr, "doctor_skin", NODE_MISSING_IN_CONFIG_FILE);
			return returnType;
		}
	
		dumpError(files->err_Ptr, "doctor_skin - enable", ATTRIBUTE_MISSING_IN_CONFIG_FILE);
		strcpy(result, "VALUE_MISSING");
	}

	/* call to validateFieldEnabledOrNot to check the enabled fields 
	 * of configuration file */
	returnType = validateFieldEnabledOrNot(arg, files, &(config->enabledSkin),SKIN_ENABLE_VALUE_NOTSUPPORTED, SKIN_ENABLE_VALUE_MISMATCH, result);

	if(returnType == ENABLED)
	{
		enabledInfo[tempIndex] = 1;
		tempIndex++;
	}


	else 
	{
		enabledInfo[tempIndex] = 0;
		tempIndex++;
	}

	/* If no field is enabled */
	if((config->enabledEye==0) &&
			(config->enabledNeuron==0) &&
				(config->enabledHeart==0) &&
					(config->enabledBones==0) &&
						(config->enabledSkin==0))
	{
		fprintf(stderr, "ERROR MSG: No doctors available\n");
		return FAILURE;
	}

	config->qty.no_of_doctor = doctorCount;
	
	DEBUG_LEVEL_1("COMPLETED: inputFileParamActiveTag\n");
	return SUCCESS;
}	
	
/*******************************************************************************
 *
 * Function:	dumpError
 *
 * Description:	Dump error into error log file according to the error number
 * 		received.
 *
 * Parameters:	FILE *err_Ptr : File Pointer to the error log file.
 *
 * 		char *error_string : string that contain error message.
 *
 * 		int error_no : contains the enum specified by different functions.
 *
 * Return Value: It will not return anything.
 *
 ******************************************************************************/

void dumpError(FILE *err_Ptr,char *error_string,int error_no)
{
	static int count=0;
	if(count==0)
	{
		fprintf(stderr,"\n*** CHECK ERROR LOG FILE ***\n");
		count++;
	}
	/* switch to appropriate case according to the enum specified by the differnt functions */
	switch(error_no)
	{
		case -255:	fprintf(err_Ptr, "%s : CONFIGFILE_UNABLE_TO_OPEN \n",error_string);
					break;

		case -254:	fprintf(err_Ptr, "%s : TYPE_VALUE_NOTFOUND \n",error_string);
					break;

		case -253:	fprintf(err_Ptr, "%s : TYPE_VALUE_MISMATCH \n",error_string);
					break;

		case -252:	fprintf(err_Ptr, "%s : FORMAT_VALUE_NOTFOUND \n",error_string);
					break;

		case -251:	fprintf(err_Ptr, "%s : FORMAT_VALUE_MISMATCH \n",error_string);
					break;

		case -250:	fprintf(err_Ptr, "%s : EYE_ENABLE_VALUE_NOTSUPPORTED \n",error_string);
					break;

		case -249:	fprintf(err_Ptr, "%s : EYE_ENABLE_VALUE_MISMATCH \n",error_string);
					break;

		case -248:	fprintf(err_Ptr, "%s : NEURON_ENABLE_VALUE_NOTSUPPORTED \n",error_string);
					break;

		case -247:	fprintf(err_Ptr, "%s : NEURON_ENABLE_VALUE_MISMATCH \n",error_string);
					break;

		case -246:	fprintf(err_Ptr, "%s : HEART_ENABLE_VALUE_NOTSUPPORTED \n",error_string);
					break;

		case -245:	fprintf(err_Ptr, "%s : HEART_ENABLE_VALUE_MISMATCH \n",error_string);
					break;

		case -244:	fprintf(err_Ptr, "%s : BONES_ENABLE_VALUE_NOTSUPPORTED \n",error_string);
					break;

		case -243:	fprintf(err_Ptr, "%s : BONES_ENABLE_VALUE_MISMATCH \n",error_string);
					break;

		case -242:	fprintf(err_Ptr, "%s : SKIN_ENABLE_VALUE_NOTSUPPORTED \n",error_string);
					break;

		case -241:	fprintf(err_Ptr, "%s : SKIN_ENABLE_VALUE_MISMATCH \n",error_string);
					break;

		case -240:	fprintf(err_Ptr, "%s : NODE_MISSING_IN_CONFIGURATION_FILE (DEFAULT VALUE TAKEN)\n",error_string);
					break;

		case -239:	fprintf(err_Ptr, "%s : ATTRIBUTE_MISSING_IN_CONFIGURATION_FILE (DEFAULT VALUE TAKEN)\n",error_string);
					break;

		case -238:	fprintf(err_Ptr, "%s : FAILED_TO_PARSE_FILE \n",error_string);
					break;

		case -155:	fprintf(err_Ptr, "%s : FAILURE \n",error_string);
					break;

		case -154:	fprintf(err_Ptr, "%s : ERROR_IN_OPENING_FILE \n",error_string);
					break;

		case -153:	fprintf(err_Ptr, "%s : ERROR_IN_READING_FILEDATA \n",error_string);
					break;

		case -152:	fprintf(err_Ptr, "%s : ERROR_IN_WRITING_FILEDATA \n",error_string);
					break;

		default :   	printf("UNDECLARED ENUM\n");
					break;
	}
}


/*******************************************************************************
 *
 * Function:	getAttributeValue
 *
 * Description:	Gives the attribute value of the specified tag of the xml file.
 *
 * Parameters:	xmlTextReaderPtr reader : structure pointer to the xmlTextReader.
 *
 * 		char node : To contain tag name.
 *
 *		char attribute : To contain the tag attribute name.
 *
 * 		char result : To store the value of the specified tag.
 *
 *		FILESPTR *files : pointer to the files_ptr structure.
 *
 * Return Value: It will return 1 if code evaluates to true else will return
 *				 specific error code.
 *
 ******************************************************************************/
int getAttributeValue(xmlTextReaderPtr reader, char *node, char *attribute, char *result, FILESPTR *files)
{
	int returnValue;			/* To store the return value of the xmlTextReaderRead */
	const xmlChar *name, *value;		/* To store the name and value of the xml node*/
	
	DEBUG_LEVEL_1("ENTERED: getAttributeValue\n");
	
	/* Store the name of the node pointed by the reader pointer*/
	name = xmlTextReaderConstName(reader);

	/* Compare the node name with the one specified by the validateParseConfigfile function */	
	while((strcmp(name, node) != COMP_SUCCESS))
	{
		if(name == NULL)
		{
			name = BAD_CAST "--";
		}
		
		/* Moves the reader pointer to the next node */
		returnValue = xmlTextReaderRead(reader);
		if(returnValue != 1)
		{
			return FAILED_TO_PARSE_FILE;
		}
		
		name = xmlTextReaderConstName(reader);
	}

	/* Store the attribute value of the node pointed by the reader pointer*/
	value = xmlTextReaderGetAttribute(reader, attribute);	
	if(value == NULL)
	{
		return ATTRIBUTE_MISSING_IN_CONFIG_FILE;
	}
	
	else
	{
		strcpy(result, value);
	}

	if(strcmp(result, "\0") == COMP_SUCCESS)
	{
		strcpy(result, "VALUE_MISSING");
	}

	DEBUG_LEVEL_1("COMPLETED: getAttributeValue\n");
	return SUCCESS;
}	
	

/*******************************************************************************
 *
 * Function:	validateFieldEnabledOrNot
 *
 * Description:	Validates whether the tag value is enabled or not.
 *
 * Parameters:	char *arg : Name of the configuration file.
 *
 * 		FILESPTR *files : structure pointer to the error log file.
 *
 * 		short int *value : To store the configuration info.
 *
 * 		int errorNo1 : To report for the specified error if it occurs.
 *
 *		int errorNo2 : To report for the specified error if it occurs.
 *
 *		char *result : contains the value of the tag stored in
 *				configuration file.
 *
 * Return Value: It will return 1 if code evaluates to true else will return
 *				 specific error code.
 *
 ******************************************************************************/
int validateFieldEnabledOrNot(char *arg, FILESPTR *files, short int *value, int errorNo1, int errorNo2, char *result)
{
	DEBUG_LEVEL_1("ENTERED: validateFieldEnabledOrNot\n");
	
	if(atoi(result) < NODE_VALUE_DISABLED || atoi(result) > ENABLED)
	{
		dumpError(files->err_Ptr, arg, errorNo1);
		return errorNo1;
	}

	if((short int)atoi(result) == ENABLED)
	{
		*value = (short int) atoi (result);
		return ENABLED;
	}
	
	else if((short int)atoi(result) == DISABLED)
	{
		*value = (short int) atoi (result);
		return DISABLED;
	}

	else
	{
		*value = (short int) NODE_VALUE_DISABLED;
		return EMPTY;
	}

	
	DEBUG_LEVEL_1("COMPLETED: validateFieldEnabledOrNot\n");
}

/*******************************************************************************
 *
 * Function:	deinitConfigurationParsing
 *
 * Description:	close error log file and cleanup function for the XML library.
 *
 * Parameters:	Pointer to the file_ptr structure.
 *
 * Return Value: It will not return anything.
 *
 ******************************************************************************/
void deinitConfigurationParsing(FILESPTR *files)
{
	DEBUG_LEVEL_1("ENTERED : deinitConfigurationParsing\n");
	fclose(files->err_Ptr);		/* Close error log file */
	xmlCleanupParser();     	/* Cleanup function for the XML library */
	DEBUG_LEVEL_1("COMPLETED : deinitConfigurationParsing\n");
}

/*******************************************************************************
 *
 * Function:	createRoom
 *
 * Description:	creates linked list for the ward rooms.
 *
 * Parameters:	int numTypeRooms: number of types of rooms
 *		ROOM *headPtr1: head pointer to the room structure's first node
 *		int *availableRooms: queue respective to the type of patient
 *
 * Return Value: It will return pointer to room structure.
 *
 ******************************************************************************/
ROOM *createRoom(int numTypeRooms, ROOM *headPtr1, int *availableRooms)
{
	char invalidInput = ' ';			/*To store invalid input type*/	
	ROOM *tempRoom = NULL, *retAddr = NULL;		/*Pointer to store room structure*/
	int counterRoom = INITIAL_VALUE;		/*Counter variable to store count of rooms*/
	int flagRoom = INITIAL_VALUE;			/*Flag variable to check for head pointer for room structure*/

	DEBUG_LEVEL_1("ENTERED : createRoom\n");
	for(counterRoom = 0; counterRoom < numTypeRooms; counterRoom++)
	{
		/* To create room for the head node in room structure */
		if(flagRoom == DISABLED)
		{
			/* allocating storage for room head node */
			tempRoom = (struct room *)malloc(sizeof(struct room));
			retAddr = tempRoom;
			printf("Following are the types of room available :\n1-Ac\n2-Non-Ac\n3-Delux\n");
			printf("\n Choose the room type from menu..\n");
			while(scanf("%d", &tempRoom->roomTypes) != 1 || (tempRoom->roomTypes < 1 || tempRoom->roomTypes > 3))	
			{	
				int tempFlag = DISABLED;		/* Flag variable to check for invalid input type*/	
				while ((invalidInput = getchar()) != '\n')
				{
					printf("\nPlease enter correct option from the menu.\n");
					tempFlag = ENABLED;
				}
				if((tempRoom->roomTypes > 3 || tempRoom->roomTypes < 1) && tempFlag == DISABLED)
					printf("\nPlease enter correct option from the menu.\n");
		
				printf("Re-enter correct room type from menu:");
			}
			printf("How many rooms do you want?\n");
			while(scanf("%d", &tempRoom->numRooms) != 1)	
			{
		
				while ((invalidInput = getchar()) != '\n')
					putchar(invalidInput);  		/* dispose of bad input*/		
			
				printf(" is not an integer.Please enter correct number of rooms.\n");
				printf("Re-enter number of rooms:");
			}

			/* To store selected rooms in respective array */
			if(tempRoom->roomTypes == AC)
				availableRooms[0] = tempRoom->numRooms;
			else if(tempRoom->roomTypes == NON_AC)
				availableRooms[1] = tempRoom->numRooms;
			else if(tempRoom->roomTypes == DELUXE)
				availableRooms[2] = tempRoom->numRooms;
			else
				printf("\nInvalid room type selected!\n");

			headPtr1 = tempRoom;
			headPtr1->nextRoom = NULL;
			tempRoom = NULL;
			flagRoom = ENABLED;
		}

		/* To create room for the other nodes in room structure */
		else 
		{
			/* allocating storage for room's each node */
			tempRoom = (struct room *)malloc(sizeof(struct room));
			printf("Following are the types of room available :\n1-Ac\n2-Non-Ac\n3-Delux\n");
			printf("\n Choose the room type from menu..\n");
			while(scanf("%d", &tempRoom->roomTypes) != 1 || (tempRoom->roomTypes < 1 || tempRoom->roomTypes > 3))	
			{	
				int tempFlag = DISABLED;		/* Flag variable to check for invalid input type*/
				while ((invalidInput = getchar()) != '\n')
				{
					printf("\n Please enter correct option from the menu.\n");
					tempFlag = ENABLED;
				}
				if((tempRoom->roomTypes > 3 || tempRoom->roomTypes < 1) && tempFlag == DISABLED)
					printf("\n Please enter correct option from the menu.\n");
		
				printf("Re-enter correct room type from menu:");
			}
			printf("How many rooms do you want?\n");
			while(scanf("%d", &tempRoom->numRooms) != 1)	
			{
		
				while ((invalidInput = getchar()) != '\n')
					putchar(invalidInput);  /* dispose of bad input	*/	
			
				printf(" is not an integer.Please enter correct number of rooms.\n");
				printf("Re-enter number of rooms:");
			}

			/* To store selected rooms in respective array */
			if(tempRoom->roomTypes == AC)
				availableRooms[0] = tempRoom->numRooms;
			else if(tempRoom->roomTypes == NON_AC)
				availableRooms[1] = tempRoom->numRooms;
			else if(tempRoom->roomTypes == DELUXE)
				availableRooms[2] = tempRoom->numRooms;
			else
				printf("\nInvalid room type selected!\n");

			headPtr1->nextRoom = tempRoom;
			headPtr1 = tempRoom;
			headPtr1->nextRoom = NULL;
			tempRoom = NULL;
		}
	}
	DEBUG_LEVEL_1("COMPLETED : createRoom\n");
	return retAddr;
}

/*******************************************************************************
 *
 * Function:	createWard
 *
 * Description:	creates linked list for the hospital wards.
 *
 * Parameters:	int numWards: number of wards
 *		WARD *headPtr: head pointer to the ward structure's first node
 *		ROOM *rooms: room structure pointer
 *
 * Return Value: It will return pointer to ward structure.
 *
 ******************************************************************************/
WARD *createWard(int numWards, WARD *headPtr, ROOM *rooms)
{
	WARD *tempWard = NULL, *retAddr = NULL;		/*Pointer to store ward structure*/
	int counterWard = INITIAL_VALUE;		/*Counter variable to store count of wards*/
	int flagWard = INITIAL_VALUE;			/*Flag variable to check for head pointer for ward structure*/
	char invalidInput = ' ';			/*To store invalid input type*/	
	
	DEBUG_LEVEL_1("ENTERED : createWard\n");
	for(counterWard = 0; counterWard < numWards; counterWard ++)
	{
		/* To create ward for the head node in ward structure */		
		if(flagWard == DISABLED)
		{
			/* allocating storage for ward head node */
			tempWard = (struct ward *)malloc(sizeof(struct ward));
			retAddr = tempWard;
			printf("\nEnter the wardId :\n");
			
			while(scanf("%d", &tempWard->wardId) != 1)	
			{
		
				while ((invalidInput = getchar()) != '\n')
					putchar(invalidInput);  /* dispose of bad input */		
			
				printf(" is not an integer.Please enter correct Ward Id.\n");
				printf("Re-enter Ward Id:");
			}
			while(1)
			{
				printf("\nEnter the ward name :\n");
				scanf("%s",tempWard->wardName);
				if(strcasecmp(tempWard->wardName, "eye") == COMP_SUCCESS || strcasecmp(tempWard->wardName, "neuron") == COMP_SUCCESS || strcasecmp(tempWard->wardName, "heart") == COMP_SUCCESS || strcasecmp(tempWard->wardName, "bones") == COMP_SUCCESS || strcasecmp(tempWard->wardName, "skin") == COMP_SUCCESS)
					break;
				else
				{
					printf("\nEnter a valid ward name.\n");
					continue;
				}
					
			}
		
			printf("\n Enter number of types of rooms required..\n");
			while(scanf("%d", &tempWard->roomTypes) != 1 || (tempWard->roomTypes < 1 || tempWard->roomTypes > 3))	
			{	
				int tempFlag = DISABLED;		/* Flag variable to check for invalid input type*/
				while ((invalidInput = getchar()) != '\n')
				{
					printf("\nEnter room type between 1 to 3.\n");
					tempFlag = ENABLED;
				}
				if((tempWard->roomTypes > 3 || tempWard->roomTypes < 1) && tempFlag == DISABLED)
					printf("\nEnter room type between 1 to 3.\n");
		
				printf("Re-enter number of type of rooms required:");
			}	
			
			/* Creating rooms for each wards */
			while(1)
			{ 
				if(strcasecmp(tempWard->wardName, "eye") == COMP_SUCCESS)
				{
					tempWard->wardRoom = createRoom(tempWard->roomTypes, rooms, eyeArr);
					break;
				}
				else if(strcasecmp(tempWard->wardName, "neuron") == COMP_SUCCESS)
				{
					tempWard->wardRoom = createRoom(tempWard->roomTypes, rooms, neuronArr);
					break;
				}
				else if(strcasecmp(tempWard->wardName, "heart") == COMP_SUCCESS)
				{
					tempWard->wardRoom = createRoom(tempWard->roomTypes, rooms, heartArr);
					break;
				}
				else if(strcasecmp(tempWard->wardName, "bones") == COMP_SUCCESS)
				{
					tempWard->wardRoom = createRoom(tempWard->roomTypes, rooms, bonesArr);
					break;
				}
				else if(strcasecmp(tempWard->wardName, "skin") == COMP_SUCCESS)
				{
					tempWard->wardRoom = createRoom(tempWard->roomTypes, rooms, skinArr);
					break;
				}
				else
				{
					printf("Invalid input...\nPlease try again!\n");
					continue;
				}
			}
			tempWard->nextWard = NULL;
			headPtr = tempWard;
			tempWard = NULL;
			flagWard = ENABLED;
		}

		/* To create ward for the other nodes in ward structure */
		else
		{
			tempWard = (struct ward *)malloc(sizeof(struct ward));
			printf("\nEnter the wardId :\n");
			while(scanf("%d", &tempWard->wardId) != 1)	
			{
		
				while ((invalidInput = getchar()) != '\n')
					putchar(invalidInput);  /* dispose of bad input	*/
			
				printf(" is not an integer.Please enter correct Ward Id.\n");
				printf("Re-enter Ward Id:");
			}
			while(1)
			{
				printf("\nEnter the ward name :\n");
				scanf("%s",tempWard->wardName);
				if(strcasecmp(tempWard->wardName, "eye") == COMP_SUCCESS || strcasecmp(tempWard->wardName, "neuron") == COMP_SUCCESS ||  strcasecmp(tempWard->wardName, "heart") == COMP_SUCCESS || strcasecmp(tempWard->wardName, "bones") == COMP_SUCCESS || strcasecmp(tempWard->wardName, "skin") == COMP_SUCCESS)
					break;
				else
				{
					printf("\nEnter a valid ward name.\n");
					continue;
				}
					
			}
			
			printf("\n Enter number of types of rooms required..\n");
			while(scanf("%d", &tempWard->roomTypes) != 1 || (tempWard->roomTypes < 1 || tempWard->roomTypes > 3))	
			{	
				int tempFlag = DISABLED;		/* Flag variable to check for invalid input type*/
				while ((invalidInput = getchar()) != '\n')
				{
					printf("\nEnter room type between 1 to 3.\n");
					tempFlag = ENABLED;
				}
				if((tempWard->roomTypes > 3 || tempWard->roomTypes < 1) && tempFlag == DISABLED)
					printf("\nEnter room type between 1 to 3.\n");
		
				printf("Re-enter number of type of rooms required:");
			}	
			
			/* Creating rooms for each wards */
			while(1)
			{ 
				if(strcasecmp(tempWard->wardName, "eye") == COMP_SUCCESS)
				{
					tempWard->wardRoom = createRoom(tempWard->roomTypes, rooms, eyeArr);
					break;
				}
				else if(strcasecmp(tempWard->wardName, "neuron") == COMP_SUCCESS)
				{
					tempWard->wardRoom = createRoom(tempWard->roomTypes, rooms, neuronArr);
					break;
				}
				else if(strcasecmp(tempWard->wardName, "heart") == COMP_SUCCESS)
				{
					tempWard->wardRoom = createRoom(tempWard->roomTypes, rooms, heartArr);
					break;
				}
				else if(strcasecmp(tempWard->wardName, "bones") == COMP_SUCCESS)
				{
					tempWard->wardRoom = createRoom(tempWard->roomTypes, rooms, bonesArr);
					break;
				}
				else if(strcasecmp(tempWard->wardName, "Skin") == COMP_SUCCESS)
				{
					tempWard->wardRoom = createRoom(tempWard->roomTypes, rooms, skinArr);
					break;
				}
				else
				{
					printf("Invalid input...\nPlease try again!\n");
					continue;
				}
			}
			headPtr->nextWard = tempWard;
			headPtr = tempWard;
			headPtr->nextWard = NULL;
			tempWard = NULL;
		}
	}
	DEBUG_LEVEL_1("COMPLETED : createWard\n");
	return retAddr;
}

/*******************************************************************************
 *
 * Function:	displayDetails
 *
 * Description:	displays hospital skeleton.
 *
 * Parameters:	HOSPITAL *hospitals: pointer to hospital structure
 *		WARD *wards: pointer to ward structure
 *		ROOM *rooms: pointer to room structure
 *
 * Return Value: It will return 1 if code evaluates to true else will return
 *		specific error code.
 ******************************************************************************/
int displayDetails(HOSPITAL *hospitals, WARD *wards, ROOM *rooms )
{
	
	wards = hospitals->wardQueue;		/*Pointer to ward queue variable*/
	DEBUG_LEVEL_1("ENTERED : displayDetails\n");

	printf("*********************************************\n");
	printf("***Details of the hospital are as follows****\n");
	printf("*********************************************\n");
	printf("**********Welcome to %s************\n", hospitals->hospitalName);
	printf("********** Address: %s ************\n",hospitals->address);
	printf("****There are %d wards in this hospital******\n", hospitals->numWards);
	
	while(wards != NULL)
	{
		printf("********Name of the ward is %s********\n", wards->wardName);  
		printf("******Id of %s ward is %d***********\n", wards->wardName, wards->wardId);
		printf("****There are %d type of room/s in %s ward****\n", wards->roomTypes, wards->wardName);
		
		while(rooms != NULL)
		{		
			rooms = rooms->nextRoom;
		}
		wards = wards->nextWard;
	}
	DEBUG_LEVEL_1("COMPLETED : displayDetails\n");
	return SUCCESS;
}

/*******************************************************************************
 *
 * Function:	createHospital
 *
 * Description:	creates hospital skeleton.
 *
 * Parameters:	HOSPITAL *hospitals: pointer to hospital structure
 *		WARD *wards: pointer to ward structure
 *		ROOM *rooms: pointer to room structure
 *
 * Return Value: It will return 1 if code evaluates to true else will return
 *		specific error code.
 ******************************************************************************/
int createHospital(HOSPITAL *hospitals, WARD *wards, ROOM *rooms)
{
	char invalidInput = ' ';			/*To store invalid input type*/	
	int returnType = INITIAL_VALUE;			/*To store return type of function*/

	DEBUG_LEVEL_1("ENTERED : createHospital\n");

	printf("\nPlease enter the name of your hospital :\n");
	scanf("%s", hospitals->hospitalName);
	strcpy(tempHosName, hospitals->hospitalName);
	printf("\nEnter the hospital address :\n");
	scanf("%s", hospitals->address);	
	strcpy(tempHosAddr, hospitals->address);
	
	printf("\nEnter the number of wards required :\n");
	while(scanf("%d", &hospitals->numWards) != 1 || (hospitals->numWards < 1 || hospitals->numWards > 5))	
	{	
		int tempFlag = DISABLED;		/*Flag variable to check for invalid input type*/		
		while ((invalidInput = getchar()) != '\n')
		{
			printf("\nEnter ward id between 1 to 5.\n");
			tempFlag = ENABLED;
		}
		if((hospitals->numWards > 5 || hospitals->numWards < 1) && tempFlag == DISABLED)
			printf("\nEnter ward id between 1 to 5.\n");
		printf("Re-enter number of wards:");
	}
	
	/*Creates wards in hospital*/
	hospitals->wardQueue = createWard(hospitals->numWards, wards, rooms);
	
	/*Calling display function*/
	displayDetails(hospitals, wards, rooms);
	if(returnType != SUCCESS)
	{
		fprintf(stderr, "Error calling displayDetails...\n");
      		return FAILURE;
	}
	
	DEBUG_LEVEL_1("COMPLETED : createHospital\n");
	return SUCCESS;
}


/*******************************************************************************
 *
 * Function:	roomStatus
 *
 * Description:	checks the availability status of room types.
 *
 * Parameters:	int roomType: room type value
 *		int *roomAvailability: array of room status
 *
 * Return Value: It will return 1 if code evaluates to true else will return
 *		that room is disabled.
 ******************************************************************************/
int roomStatus(int roomType, int *roomAvailability)
{
	DEBUG_LEVEL_1("ENTERED : roomStatus\n");
	if(roomType == AC)
	{
		if(roomAvailability[0] > INITIAL_VALUE)
			return ENABLED;
		else
			return DISABLED;
	}
	else if(roomType == NON_AC)
	{
		if(roomAvailability[1] > INITIAL_VALUE)
			return ENABLED;
		else
			return DISABLED;
	}
	else if(roomType == DELUXE)
	{
		if(roomAvailability[2] > INITIAL_VALUE)
			return ENABLED;
		else
			return DISABLED;
	}
	else
	{
		printf("\nInvalid input\n");
		return DISABLED;
	}	
	DEBUG_LEVEL_1("COMPLETED : roomStatus\n");
}

/*******************************************************************************
 *
 * Function:	checkPatient
 *
 * Description:	checks the Patient information.
 *
 * Parameters:	void *rooms: room pointer
 *
 * Return Value: It will return nothing
 ******************************************************************************/
void *checkPatient(void *rooms)
{	
	ROOM *room = (ROOM *)rooms;					/*to store room structure variable*/
	PATIENT patient;						/*instantiate patient structure*/
	struct tm *timeinfo1, *timeinfo2;				/*struct tm for fetching system time*/
	time_t rawtime1, rawtime2;					/*to store system time in time_t variable*/
	int eyeIndex, neuronIndex, heartIndex, bonesIndex, skinIndex;	/*index variable for different wards*/
	int tempFlag = INITIAL_VALUE;					/*temporary flag variable*/
	int itrName = INITIAL_VALUE;					/*iteration variable for loop*/
	int returnType = INITIAL_VALUE;					/*To store return type of function*/
	
	DEBUG_LEVEL_1("ENTERED : checkPatient\n");

	strcpy(patient.disease, msg.service);
	printf("\nEnter the patient details for %s treatment :\n", patient.disease);
	while(1)
	{
		tempFlag = DISABLED;
		printf("\nEnter patient name :\n");
		scanf("%s", patient.patientName);
		for(itrName = 0; patient.patientName[itrName] != '\0'; itrName++)
		{
			if(isalpha(patient.patientName[itrName]) == COMP_SUCCESS)
			{
				printf("\nPatient name field can only contain the characters!\n");
				tempFlag = ENABLED;
				break;
			}
		}
		if(tempFlag == ENABLED)
			continue;

		printf("\nEnter the room type :\n1-Ac\n2-Non-Ac\n3-Delux\n");
		scanf("%d", &patient.roomType);
		break;
	}
	
	if(strcasecmp(patient.disease, "eye") == COMP_SUCCESS)
	{
		/*If selected room is available*/
		if(roomStatus(patient.roomType, eyeArr) == ENABLED)
		{
			pthread_mutex_lock(&lockEye);
			
			/*To insert patient in eye queue */
			returnType = enqueue(&queue, EYE);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling enqueue from eye ward...\n");
      				exit(FAILURE);
			}

			/*To remove from patient pool*/
			eyeArr[patient.roomType - 1]--;

			printf("\nTreatment begins :\n");
			time ( &rawtime1);
  			timeinfo1 = localtime ( &rawtime1 );
  			patient.entryTime = asctime(timeinfo1);
  			printf("\nPatient entry time is :\n%s\n", patient.entryTime);
  			
			if(msg.severity == NORMAL_CHECKUP)
  				for(eyeIndex = 0; eyeIndex < (0xFFFFFFFF); eyeIndex++);
  			if(msg.severity == OPERATION)
  			{
  				for(eyeIndex = 0; eyeIndex < (0xFFFFFFFF); eyeIndex++);
  				for(eyeIndex = 0; eyeIndex < (0xFFFFFFFF); eyeIndex++);
  			}

  			printf("\nTreatment ends :\n");
  			printf("\nGenerating the medical report for %s.\n", patient.patientName);
 			time ( &rawtime2 ); 			
  			timeinfo2 = localtime ( &rawtime2);
  			patient.exitTime = asctime(timeinfo2);

			/*To delete patient from eye queue*/
  			returnType = dequeue(&queue, EYE);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling dequeue from eye ward...\n");
      				exit(FAILURE);
			}

			/*To insert patient back in patient pool*/
  			eyeArr[patient.roomType - 1]++;

  			printf("\nPatient exit time :\n%s\n", patient.exitTime);
			
			/*to generate patient report*/
  			returnType = createReport(&patient);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling createReport from eye ward...\n");
      				exit(FAILURE);
			}
  			pthread_mutex_unlock(&lockEye);
  		}
		
		/*If selected room is not available*/
  		else
  		{
  			printf("\nPatient %s cannot be treated since the room is not currently available!\n", patient.patientName);
  			pthread_mutex_unlock(&lockEye);
  		}	
	}

	else if(strcasecmp(patient.disease, "neuron") == COMP_SUCCESS)
	{
		/*If selected room is available*/
		if(roomStatus(patient.roomType, neuronArr) == ENABLED)
		{
			pthread_mutex_lock(&lockNeuron);

			/*To insert patient in neuron queue */
			returnType = enqueue(&queue, NEURON);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling enqueue from neuron ward...\n");
      				exit(FAILURE);
			}

			/*To remove from patient pool*/
			neuronArr[patient.roomType - 1]--;

			printf("\nTreatment begins :\n");
			time ( &rawtime1 );
  			timeinfo1 = localtime ( &rawtime1 );
  			patient.entryTime = asctime(timeinfo1);
  			printf("\nPatient entry time is :\n%s\n", patient.entryTime);

  			if(msg.severity == NORMAL_CHECKUP)
  				for(neuronIndex = 0; neuronIndex < (0xFFFFFFFF); neuronIndex++);
  			if(msg.severity == OPERATION)
  			{
  				for(neuronIndex = 0; neuronIndex < (0xFFFFFFFF); neuronIndex++);
  				for(neuronIndex = 0; neuronIndex < (0xFFFFFFFF); neuronIndex++);
  			}

  			printf("\nTreatment ends :\n");
  			printf("\nGenerating the medical report for %s.\n", patient.patientName);
  			time ( &rawtime2 );
  			timeinfo2 = localtime ( &rawtime2 );
  			patient.exitTime = asctime(timeinfo2);

			/*To delete patient from neuron queue*/			
  			returnType = dequeue(&queue, NEURON);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling dequeue from neuron ward...\n");
      				exit(FAILURE);
			}

			/*To insert patient back in patient pool*/
  			neuronArr[patient.roomType - 1]++;
  			printf("\nPatient exit time :\n%s\n", patient.exitTime);

			/*to generate patient report*/
  			returnType = createReport(&patient);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling createReport from neuron ward...\n");
      				exit(FAILURE);
			}
  			pthread_mutex_unlock(&lockNeuron);
  		}

		/*If selected room is not available*/
  		else
  		{
  			printf("\nPatient %s cannot be treated since the room is not currently available!\n", patient.patientName);
  			pthread_mutex_unlock(&lockNeuron);
  		}	
	}

	else if(strcasecmp(patient.disease, "heart") == COMP_SUCCESS)
	{
		/*If selected room is available*/
		if(roomStatus(patient.roomType, heartArr) == ENABLED)
		{
			pthread_mutex_lock(&lockHeart);

			/*To insert patient in heart queue */
			returnType = enqueue(&queue, HEART);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling enqueue from heart ward...\n");
      				exit(FAILURE);
			}

			/*To remove from patient pool*/
			heartArr[patient.roomType - 1]--;

			printf("\nTreatment begins :\n");
			time ( &rawtime1 );
  			timeinfo1 = localtime ( &rawtime1 );
  			patient.entryTime = asctime(timeinfo1);
  			printf("\nPatient entry time is :\n%s\n", patient.entryTime);

  			if(msg.severity == NORMAL_CHECKUP)
  				for(heartIndex = 0; heartIndex < (0xFFFFFFFF); heartIndex++);
  			if(msg.severity == OPERATION)
  			{
  				for(heartIndex = 0; heartIndex < (0xFFFFFFFF); heartIndex++);
  				for(heartIndex = 0; heartIndex < (0xFFFFFFFF); heartIndex++);
  			}

  			printf("\nTreatment ends :\n");
  			printf("\nGenerating the medical report for %s.\n", patient.patientName);
 			time ( &rawtime2);
  			timeinfo2 = localtime ( &rawtime2 );
  			patient.exitTime = asctime(timeinfo2);
		
			/*To delete patient from heart queue*/			
  			returnType = dequeue(&queue, HEART);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling dequeue from heart ward...\n");
      				exit(FAILURE);
			}

			/*To insert patient back in patient pool*/
  			heartArr[patient.roomType - 1]++;
  			printf("\nPatient exit time :\n%s\n", patient.exitTime);

			/*to generate patient report*/
  			returnType = createReport(&patient);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling createReport from heart ward...\n");
      				exit(FAILURE);
			}
  			pthread_mutex_unlock(&lockHeart);
  		}

		/*If selected room is not available*/
  		else
  		{
  			printf("\nPatient %s cannot be treated since the room is not currently available!\n", patient.patientName);
  			pthread_mutex_unlock(&lockHeart);
  		}	
	}
	else if(strcasecmp(patient.disease, "bones") == COMP_SUCCESS)
	{
		/*If selected room is available*/		
		if(roomStatus(patient.roomType, bonesArr) == ENABLED)
		{
			pthread_mutex_lock(&lockBones);

			/*To insert patient in bones queue */
			returnType = enqueue(&queue, BONES);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling enqueue from bones ward...\n");
      				exit(FAILURE);
			}

			/*To remove from patient pool*/
			bonesArr[patient.roomType - 1]--;

			printf("\nTreatment begins :\n");
			time ( &rawtime1 );
  			timeinfo1 = localtime ( &rawtime1 );
  			patient.entryTime = asctime(timeinfo1);
  			printf("\nPatient entry time is :\n%s", patient.entryTime);
	
  			if(msg.severity == NORMAL_CHECKUP)
  				for(bonesIndex = 0; bonesIndex < (0xFFFFFFFF); bonesIndex++);
  			if(msg.severity == OPERATION)
  			{
  				for(bonesIndex = 0; bonesIndex < (0xFFFFFFFF); bonesIndex++);
  				for(bonesIndex = 0; bonesIndex < (0xFFFFFFFF); bonesIndex++);
  			}

  			printf("\nTreatment ends :\n");
			printf("\nGenerating the medical report for %s.\n", patient.patientName);
 			time ( &rawtime2 );  			
  			timeinfo2 = localtime ( &rawtime2 );
  			patient.exitTime = asctime(timeinfo2);
			
			/*To delete patient from bones queue*/			
  			returnType = dequeue(&queue, BONES);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling dequeue from bones ward...\n");
      				exit(FAILURE);
			}
	
			/*To insert patient back in patient pool*/
  			bonesArr[patient.roomType - 1]++;
  			printf("\nPatient exit time :\n%s\n", patient.exitTime);

			/*to generate patient report*/
  			returnType = createReport(&patient);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling createReport from bones ward...\n");
      				exit(FAILURE);
			}
  			pthread_mutex_unlock(&lockBones);
  		}

		/*If selected room is not available*/
  		else
  		{
  			printf("\nPatient %s cannot be treated since the room is not currently available!\n", patient.patientName);
  			pthread_mutex_unlock(&lockBones);
  		}	
	}

	else if(strcasecmp(patient.disease, "skin") == COMP_SUCCESS)
	{
		if(roomStatus(patient.roomType, skinArr) == ENABLED)
		{
			pthread_mutex_lock(&lockSkin);
	
			/*To insert patient in skin queue */
			returnType = enqueue(&queue, SKIN);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling enqueue from skin ward...\n");
      				exit(FAILURE);
			}

			/*To remove from patient pool*/
			skinArr[patient.roomType - 1]--;

			printf("\nTreatment begins : \n");
			time ( &rawtime1 );
  			timeinfo1 = localtime ( &rawtime1 );
  			patient.entryTime = asctime(timeinfo1);
  			printf("\nPatient entry time is :\n%s", patient.entryTime);
	
  			if(msg.severity == NORMAL_CHECKUP)
  				for(skinIndex = 0; skinIndex < (0xFFFFFFFF); skinIndex++);
  			if(msg.severity == OPERATION)
  			{
  				for(skinIndex = 0; skinIndex < (0xFFFFFFFF); skinIndex++);
  				for(skinIndex = 0; skinIndex < (0xFFFFFFFF); skinIndex++);
  			}

  			printf("\nTreatment ends :\n");
  			printf("\nGenerating the medical report for %s.\n", patient.patientName);
 			time ( &rawtime2 );
  			timeinfo2 = localtime ( &rawtime2 );
  			patient.exitTime = asctime(timeinfo2);
	
			/*To delete patient from skin queue*/			
  			returnType = dequeue(&queue, SKIN);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling dequeue from skin ward...\n");
      				exit(FAILURE);
			}
	
			/*To insert patient back in patient pool*/
			skinArr[patient.roomType - 1]++;
  			printf("\nPatient exit time :\n%s\n", patient.exitTime);

			/*to generate patient report*/
  			returnType = createReport(&patient);
			if(returnType != SUCCESS)
			{
				fprintf(stderr, "Error calling createReport from skin ward...\n");
      				exit(FAILURE);
			}
  			pthread_mutex_unlock(&lockSkin);
  		}

		/*If selected room is not available*/
  		else
  		{
  			printf("\nPatient %s cannot be treated since the room is not currently available!\n", patient.patientName);
  			pthread_mutex_unlock(&lockSkin);
  		}	
	}
	else
	{
		printf("\nInvalid request\n");
	}
	DEBUG_LEVEL_1("COMPLETED : checkPatient\n");
}

/*******************************************************************************
 *
 * Function:	createReport
 *
 * Description:	generates patient report.
 *
 * Parameters:	PATIENT *patient: pointer to patient structure
 *
 * Return Value: It will return 1 if code evaluates to true else will return
 *		associated error code.
 ******************************************************************************/

int createReport(PATIENT *patient)
{
	int reportFlag = INITIAL_VALUE;			/*to store flag variable for report*/
	FILE *reportPtr = NULL;				/*file pointer to report*/			
	char reportName[50], *disPtr;			/*string to store report*/

	DEBUG_LEVEL_1("ENTERED : createReport\n");
	if(reportFlag == DISABLED)
	{
		reportFlag = ENABLED;
	
		strcpy(reportName, patient->patientName);
		strcat(reportName, patient->disease);
		strcat(reportName, "MedicReport.txt");
		reportPtr = fopen(reportName, "w");
		disPtr = "**************************\n*******Welcome******\n**************************\n";
		fprintf(reportPtr, "%s", disPtr);
		fprintf(reportPtr,"Hospital name : %s\nHospital address : %s\nPatient name : %s\nPatient disease : %s\nReport Generated at : %s\n", tempHosName, tempHosAddr, patient->patientName, patient->disease, patient->exitTime);
		fclose(reportPtr);
		system("mv *.txt MedicReport");
		reportFlag = DISABLED;
	}
	DEBUG_LEVEL_1("COMPLETED : createReport\n");
	return SUCCESS;
}

/*******************************************************************************
 *
 * Function:	isFull
 *
 * Description:	checks if the queue is full.
 *
 * Parameters:	QUEUE *queue: pointer to queue structure
 *		int doctor: doctor value
 *
 * Return Value: It will return 1 if code evaluates to true else will return
 *		associated error code.
 ******************************************************************************/
int isFull(QUEUE *queue, int doctor)
{
	DEBUG_LEVEL_1("ENTERED : isFull\n");
	if(doctor == EYE)
	{
		if((queue->front[EYE] == queue->rear[EYE] + 1) || (queue->front[EYE] == INITIAL_VALUE && queue->rear[EYE] == SIZE - 1))
		{
			fprintf(stderr, "Error: eye queue is full...\n");			
			return FAILURE;
		}
	
		return SUCCESS;
	}
	else if(doctor == NEURON)
	{
		if((queue->front[NEURON] == queue->rear[NEURON] + 1) || (queue->front[NEURON] == INITIAL_VALUE && queue->rear[NEURON] == SIZE - 1))
		{
			fprintf(stderr, "Error: neuron queue is full...\n");			
			return FAILURE;
		}
	
		return SUCCESS;
	}
	else if(doctor == HEART)
	{
		if((queue->front[HEART] == queue->rear[HEART] + 1) || (queue->front[HEART] == INITIAL_VALUE && queue->rear[HEART] == SIZE - 1))
		{
			fprintf(stderr, "Error: heart queue is full...\n");			
			return FAILURE;
		}
	
		return SUCCESS;
	}
	else if(doctor == BONES)
	{
		if((queue->front[BONES] == queue->rear[BONES] + 1) || (queue->front[BONES] == INITIAL_VALUE && queue->rear[BONES] == SIZE - 1))
		{
			fprintf(stderr, "Error: bones queue is full...\n");			
			return FAILURE;
		}
	
		return SUCCESS;
	}
	else if(doctor == SKIN)
	{
		if((queue->front[SKIN] == queue->rear[SKIN] + 1) || (queue->front[SKIN] == INITIAL_VALUE && queue->rear[SKIN] == SIZE - 1))
		{
			fprintf(stderr, "Error: skin queue is full...\n");			
			return FAILURE;
		}
	
		return SUCCESS;
	}
	else
	{
		fprintf(stderr,"Invalid choice\n");
		return FAILURE;
	}
	DEBUG_LEVEL_1("COMPLETED : isFull\n");
}

/*******************************************************************************
 *
 * Function:	isEmpty
 *
 * Description:	checks if the queue is empty.
 *
 * Parameters:	QUEUE *queue: pointer to queue structure
 *		int doctor: doctor value
 *
 * Return Value: It will return 1 if code evaluates to true else will return
 *		associated error code.
 ******************************************************************************/
int isEmpty(QUEUE *queue, int doctor)
{
	DEBUG_LEVEL_1("ENTERED : isEmpty\n");

	if(doctor == EYE)
	{
		if((queue->front[EYE]) == EMPTY)
		{
			fprintf(stderr, "Error: eye queue is empty...\n");			
			return FAILURE;
		}
		return SUCCESS;
	}
	else if(doctor == NEURON)
	{
		if((queue->front[NEURON]) == EMPTY)
		{
			fprintf(stderr, "Error: neuron queue is empty...\n");			
			return FAILURE;
		}
		return SUCCESS;
	}
	else if(doctor == HEART)
	{
		if((queue->front[HEART]) == EMPTY)
		{
			fprintf(stderr, "Error: heart queue is empty...\n");			
			return FAILURE;
		}
		return SUCCESS;
	}
	else if(doctor == BONES)
	{
		if((queue->front[BONES]) == EMPTY)
		{
			fprintf(stderr, "Error: bones queue is empty...\n");			
			return FAILURE;
		}
		return SUCCESS;
	}
	else if(doctor == SKIN)
	{
		if((queue->front[SKIN]) == EMPTY)
		{
			fprintf(stderr, "Error: skin queue is empty...\n");			
			return FAILURE;
		}
		return SUCCESS;
	}
	else
	{
		fprintf(stderr, "Invalid choice...\n");			
		return FAILURE;
	}
	DEBUG_LEVEL_1("COMPLETED : isEmpty\n");
}

/*******************************************************************************
 *
 * Function:	enqueue
 *
 * Description:	inserts patient in the queue.
 *
 * Parameters:	QUEUE *queue: pointer to queue structure
 *		int doctor: doctor value
 *
 * Return Value: It will return 1 if code evaluates to true else will return
 *		associated error code.
 ******************************************************************************/
int enqueue(QUEUE *queue, int doctor)
{
	DEBUG_LEVEL_1("ENTERED : enqueue\n");

	if(doctor == EYE)
	{
		if(isFull(queue, EYE))
		{
			printf("Checking eye queue status...\n");
		}
	
		else
		{
			if(queue->front[EYE] == EMPTY)
			{
				queue->front[EYE] = INITIAL_VALUE;
			}
			queue->rear[EYE] = (queue->rear[EYE] + 1) % SIZE;
			queue->eyeQueue[queue->rear[EYE]] = ctrEye++;
			printf("Inserted patient in Eye ward is:%d\n", ctrEye);
		}
	}

	else if(doctor == NEURON)
	{
		if(isFull(queue, NEURON))
		{
			printf("Checking neuron queue status..\n");
		}
	
		else
		{
			if(queue->front[NEURON] == EMPTY)
			{
				queue->front[NEURON] = INITIAL_VALUE;
			}
			queue->rear[NEURON] = (queue->rear[NEURON] + 1) % SIZE;
			queue->neuronQueue[queue->rear[NEURON]] = ctrNeuron++;
			printf("Inserted patient in Neuron ward is:%d\n", ctrNeuron);
		}
	}

	else if(doctor == HEART)
	{
		if(isFull(queue, HEART))
		{
			printf("Checking heart queue status....\n");
		}
	
		else
		{
			if(queue->front[HEART] == EMPTY)
			{
				queue->front[HEART] = INITIAL_VALUE;
			}
			queue->rear[HEART] = (queue->rear[HEART] + 1) % SIZE;
			queue->heartQueue[queue->rear[HEART]] = ctrHeart++;
			printf("Inserted patient in Heart ward is:%d\n", ctrHeart);
		}
	}

	else if(doctor == BONES)
	{
		if(isFull(queue, BONES))
		{
			printf("Checking bones queue status....\n");
		}
	
		else
		{
			if(queue->front[BONES] == EMPTY)
			{
				queue->front[BONES] = INITIAL_VALUE;
			}
			queue->rear[BONES] = (queue->rear[BONES] + 1) % SIZE;
			queue->bonesQueue[queue->rear[BONES]] = ctrBones++;
			printf("Inserted patient in Bones ward is:%d\n", ctrBones);
		}
	}

	else if(doctor == SKIN)
	{
		if(isFull(queue, SKIN))
		{
			printf("Checking skin queue status....\n");
		}
	
		else
		{
			if(queue->front[SKIN] == EMPTY)
			{
				queue->front[SKIN] = INITIAL_VALUE;
			}
			queue->rear[SKIN] = (queue->rear[SKIN] + 1) % SIZE;
			queue->skinQueue[queue->rear[SKIN]] = ctrSkin++;
			printf("Inserted patient in Skin ward is:%d\n", ctrSkin);
		}
	}
	DEBUG_LEVEL_1("COMPLETED : enqueue\n");
	return SUCCESS;
}

/*******************************************************************************
 *
 * Function:	dequeue
 *
 * Description:	removes patient from the queue.
 *
 * Parameters:	QUEUE *queue: pointer to queue structure
 *		int doctor: doctor value
 *
 * Return Value: It will return 1 if code evaluates to true else will return
 *		associated error code.
 ******************************************************************************/

int dequeue(QUEUE *queue, int doctor)
{
	DEBUG_LEVEL_1("ENTERED : dequeue\n");
	if(doctor == EYE)
	{	
		if(isEmpty(queue, EYE))
		{
			printf("Checking eye queue status....\n");
		}
	
		ctrEye = queue->eyeQueue[queue->front[EYE]];
	
		if(queue->front[EYE] == queue->rear[EYE])
		{
			queue->front[EYE] = EMPTY;
			queue->rear[EYE] = EMPTY;
		}
		else
		{
			queue->front[EYE] = (queue->front[EYE] + 1) % SIZE;
		}	

		printf("Dequeued patient from Eye ward is:%d", ctrEye);
	}

	else if(doctor == NEURON)
	{	
		if(isEmpty(queue, NEURON))
		{
			printf("Checking neuron queue status....\n");
		}
	
		ctrNeuron = queue->neuronQueue[queue->front[NEURON]];
	
		if(queue->front[NEURON] == queue->rear[NEURON])
		{
			queue->front[NEURON] = EMPTY;
			queue->rear[NEURON] = EMPTY;
		}
		else
		{
			queue->front[NEURON] = (queue->front[NEURON] + 1) % SIZE;
		}	

		printf("Dequeued patient from Neuron ward is:%d", ctrNeuron);
	}

	else if(doctor == HEART)
	{	
		if(isEmpty(queue, HEART))
		{
			printf("Checking heart queue status....\n");
		}
	
		ctrHeart = queue->heartQueue[queue->front[HEART]];
	
		if(queue->front[HEART] == queue->rear[HEART])
		{
			queue->front[HEART] = EMPTY;
			queue->rear[HEART] = EMPTY;
		}
		else
		{
			queue->front[HEART] = (queue->front[HEART] + 1) % SIZE;
		}	

		printf("Dequeued patient from Heart ward is:%d", ctrHeart);
	}

	else if(doctor == BONES)
	{	
		if(isEmpty(queue, BONES))
		{
			printf("Checking bones queue status...\n");
		}
	
		ctrBones = queue->bonesQueue[queue->front[BONES]];
	
		if(queue->front[BONES] == queue->rear[BONES])
		{
			queue->front[BONES] = EMPTY;
			queue->rear[BONES] = EMPTY;
		}
		else
		{
			queue->front[BONES] = (queue->front[BONES] + 1) % SIZE;
		}	

		printf("Dequeued patient from Bones ward is:%d", ctrBones);
	}

	else if(doctor == SKIN)
	{	
		if(isEmpty(queue, SKIN))
		{
			printf("Checking skin queue status..\n");
		}
	
		ctrSkin = queue->skinQueue[queue->front[SKIN]];
	
		if(queue->front[SKIN] == queue->rear[SKIN])
		{
			queue->front[SKIN] = EMPTY;
			queue->rear[SKIN] = EMPTY;
		}
		else
		{
			queue->front[SKIN] = (queue->front[SKIN] + 1) % SIZE;
		}	

		printf("Dequeued patient from Skin ward is:%d", ctrSkin);
	}
	DEBUG_LEVEL_1("COMPLETED : dequeue\n");
	return SUCCESS;
}

/*******************************************************************************
 *
 * Function:	displayQueue
 *
 * Description:	displays queue members.
 *
 * Parameters:	QUEUE *queue: pointer to queue structure
 *		int doctor: doctor value
 *
 * Return Value: It will return nothing.
 ******************************************************************************/
void displayQueue(QUEUE *queue, int doctor)
{
	int itrLoop = 0;		/* Iteration variable for loop*/
	DEBUG_LEVEL_1("ENTERED : displayQueue\n");
	if(doctor == EYE)
	{
		if(isEmpty(queue, EYE))
		{
			printf("Checking eye queue status....\n");	
		}

		else
		{	
			printf("Front is : %d\n", queue->front[EYE]);
			printf("Patients are: ");
			for(itrLoop = queue->front[EYE]; itrLoop != queue->rear[EYE]; itrLoop = (itrLoop+1)%SIZE)
			{
				printf("%d ", queue->eyeQueue[itrLoop]);
			}
			printf("%d ", queue->eyeQueue[itrLoop]);
			printf("\n Rear is : %d\n", queue->rear[EYE]);
		}
	}

	if(doctor == NEURON)
	{
		if(isEmpty(queue, NEURON))
		{
			printf("Checking neuron queue status....\n");	
		}

		else
		{
			printf("Front is : %d\n", queue->front[NEURON]);
			printf("Patients are: ");
			for(itrLoop = queue->front[NEURON]; itrLoop != queue->rear[NEURON]; itrLoop = (itrLoop+1)%SIZE)
			{
				printf("%d ", queue->neuronQueue[itrLoop]);
			}
			printf("%d ", queue->neuronQueue[itrLoop]);
			printf("\n Rear is : %d\n", queue->rear[NEURON]);
		}
	}

	if(doctor == HEART)
	{
		if(isEmpty(queue, HEART))
		{
			printf("Checking heart queue status....\n");	
		}

		else
		{
			printf("Front is : %d\n", queue->front[HEART]);
			printf("Patients are: ");
			for( itrLoop = queue->front[HEART]; itrLoop != queue->rear[HEART]; itrLoop = (itrLoop+1)%SIZE)
			{
				printf("%d ", queue->heartQueue[itrLoop]);
			}
			printf("%d ", queue->heartQueue[itrLoop]);
			printf("\n Rear is : %d\n", queue->rear[HEART]);
		}
	}

	if(doctor == BONES)
	{
		if(isEmpty(queue, BONES))
		{
			printf("Checking bones queue status....\n");	
		}

		else
		{
			printf("Front is : %d\n", queue->front[BONES]);
			printf("Patients are: ");
			for( itrLoop = queue->front[BONES]; itrLoop != queue->rear[BONES]; itrLoop = (itrLoop+1)%SIZE)
			{
				printf("%d ", queue->bonesQueue[itrLoop]);
			}
			printf("%d ", queue->bonesQueue[itrLoop]);
			printf("\n Rear is : %d\n", queue->rear[BONES]);
		}
	}

	if(doctor == SKIN)
	{
		if(isEmpty(queue, SKIN))
		{
			printf("Checking skin queue status....\n");	
		}

		else
		{
			printf("Front is : %d\n", queue->front[SKIN]);
			printf("Patients are: ");
			for(itrLoop = queue->front[SKIN]; itrLoop != queue->rear[SKIN]; itrLoop = (itrLoop+1)%SIZE)
			{
				printf("%d ", queue->skinQueue[itrLoop]);
			}
			printf("%d ", queue->skinQueue[itrLoop]);
			printf("\n Rear is : %d\n", queue->rear[SKIN]);
		}
	}
	DEBUG_LEVEL_1("COMPLETED : displayQueue\n");
}	
		

	
