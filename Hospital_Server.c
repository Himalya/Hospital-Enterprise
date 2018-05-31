/******************************************************************************
 *
 * File Name:  	server.c
 *
 * Description:	This is the file where the hospital blueprint is created 
 * 		and patient's information is inputted based on doctor's
 *     		availability and assigning rooms accordingly with patient 
 *		report.
 *
 ******************************************************************************/
#include "./Include/Header.h"
#include "./Include/Debug_Header.h"

int main(int argc, char **argv)
{
	key_t key = 0;					/*stores key value common to message queue*/
	int msgid;					/*stores message id of message queue*/
	int enabled[5];					/*stores enable value of doctors*/
	int returnType = INITIAL_VALUE;			/*stores function's return type*/
	int *configPtr = NULL;				/*configuration file pointer*/
	int itrLoop = INITIAL_VALUE;			/*iteration variable for loop*/
	int tempFlag = INITIAL_VALUE;			/*temporary flag variable*/
	CONFIG config= {};				/*initializing config structure*/
	FILESPTR files = {0};				/*initializing file pointer structure*/
	WARD wards = {};				/*initializing ward structure*/
	ROOM rooms = {};				/*initializing room structure*/
	HOSPITAL hospitals ={};				/*initializing hospital structure*/
	pthread_t threadEye, threadBones,		
	 threadNeuron, threadSkin, threadHeart;		/*creating threads for different types of threads*/
	
	key = ftok("prog1", 70);
	msgid = msgget(key, 0666 | IPC_CREAT);
	WARD *tempWard;
	
	if (argc != 2)
   	{
		fprintf(stderr,"usage: <Configuration FileName>\n\n");
		exit(EXIT_WITH_ERROR);
   	}

   	configPtr = configurationParsing(argv[1],&files,&config);
   	for(itrLoop = 0; itrLoop < 5; itrLoop++)
   	{
    		msg.enabled[itrLoop] = *configPtr;
    		enabled[itrLoop] = *configPtr;
		configPtr++;
   	}

	msg.type = 1;
	strcpy(msg.service, "No service");

	/*Sends enabled doctors' status*/
	msgsnd(msgid, &msg, sizeof(msg), 0);

	/*Mutex initialization of all the services*/
	if (pthread_mutex_init(&lockEye, NULL) != COMP_SUCCESS)
   	{
      		printf("\n mutex init has failed\n");
      		return FAILURE;
   	}
	
	if (pthread_mutex_init(&lockNeuron, NULL) != COMP_SUCCESS)
   	{
   		printf("\n mutex init has failed\n");
      		return FAILURE;
   	}

   	if (pthread_mutex_init(&lockHeart, NULL) != COMP_SUCCESS)
   	{
   		printf("\n mutex init has failed\n");
      		return FAILURE;
   	}

   	if (pthread_mutex_init(&lockBones, NULL) != COMP_SUCCESS)
   	{
   		printf("\n mutex init has failed\n");
      		return FAILURE;
   	}
   
	if (pthread_mutex_init(&lockSkin, NULL) != COMP_SUCCESS)
   	{
   		printf("\n mutex init has failed\n");
     		return FAILURE;
   	}
  
	printf("Please enter the hospital details along with blueprint :\n");

	/*Call to create hospital function*/
	returnType = createHospital(&hospitals, &wards, &rooms);
	if(returnType != SUCCESS)
	{
		fprintf(stderr, "Error: failed to call createHospital...\n");
		return FAILURE;
	}
	
	while(1)
	{
		/*receiving message service and severity from message queue*/
		msgrcv(msgid, &msg, sizeof(msg), 1, 0);
		
		if(strcasecmp(msg.service, "none") == COMP_SUCCESS)
		{
			system("clear");
			exit(ENABLED);
		}

		tempWard = hospitals.wardQueue;
		while(tempWard != NULL)
		{
			if(strcasecmp(tempWard->wardName, msg.service) == COMP_SUCCESS)
			{
				tempFlag = ENABLED;
				break;
			}
			else
				tempWard = tempWard->nextWard;
		}

		if(tempFlag == DISABLED)
		{
			printf("\n%s Patients are currently not treated here.\n", msg.service);
			continue;
		}

		/*calling checkPatient function in thread*/
		if((strcasecmp(msg.service, "eye") == COMP_SUCCESS) && enabled[EYE] == ENABLED)
		{
			pthread_create(&threadEye, NULL, checkPatient, (void *)&rooms); 
		}

		else if((strcasecmp(msg.service, "neuron") == COMP_SUCCESS) && enabled[NEURON] == ENABLED)
		{
			pthread_create(&threadNeuron, NULL, checkPatient, (void *)&rooms);
		}

		else if((strcasecmp(msg.service, "heart") == COMP_SUCCESS) && enabled[HEART] == ENABLED)
		{
			pthread_create(&threadHeart, NULL, checkPatient, (void *)&rooms);
		}

		else if((strcasecmp(msg.service, "bones") == COMP_SUCCESS) && enabled[BONES] == ENABLED)
		{
			pthread_create(&threadBones, NULL, checkPatient, (void *)&rooms);
 		}

		else if((strcasecmp(msg.service, "skin") == COMP_SUCCESS) && enabled[SKIN] == ENABLED)
		{	
			pthread_create(&threadSkin, NULL, checkPatient, (void *)&rooms);
		}
		else
			printf("\nInvalid request!\n");
	}
	
	/*joining threads of all services*/
	pthread_join(threadEye, NULL);
	pthread_join(threadNeuron, NULL);
	pthread_join(threadHeart, NULL);
	pthread_join(threadBones, NULL);
	pthread_join(threadSkin, NULL);

	/*destroying mutex of all services*/	
	pthread_mutex_destroy(&lockSkin);
   	pthread_mutex_destroy(&lockBones);      
   	pthread_mutex_destroy(&lockHeart);
   	pthread_mutex_destroy(&lockEye);
	pthread_mutex_destroy(&lockNeuron);	
	
	if(returnType != SUCCESS)
  	{
    		deinitConfigurationParsing(&files);
    		fprintf(stderr,"\n*** EXITED WITH ERROR : CHECK ERROR LOG FILE ***\n");
    		exit(EXIT_WITH_ERROR);
   	}
   
	printf("\n*** Configuration file Parsed Successfully ***\n");
	printf("\n\n***** Program Completed Successfully *****\n");
	return SUCCESS;
}
