/******************************************************************************
 *
 * File Name:  	client.c
 *
 * Description:	This is the file where the patient will constantly enter type
 *		of service and level of severity on the basis of doctors
 *		available.
 *
 ******************************************************************************/
#include "./Include/Header.h"
#include "./Include/Debug_Header.h"

int main(int argc, char **argv)
{
	key_t key = 0;					/*stores key value common to message queue*/
   	int msgid;					/*stores message id of message queue*/
   	char invalidInput;				/*stores invalid input*/
	int tempFlag = INITIAL_VALUE;			/*temporary flag variable*/
   	struct msqid_ds bufferInfo;			/*stores msqid_ds buffer variable*/

	key = ftok("prog", 70);
   	msgid = msgget(key, 0666 | IPC_CREAT);
 	msgctl(msgid, IPC_STAT, &bufferInfo);

 	if(bufferInfo.msg_qnum == SUCCESS)
 	{
 		printf("\nServer is currently out of service.\n");
 		exit(ENABLED);
 	}  
	
	/*receives enabled doctors' information from message queue*/
	msgrcv(msgid, &msg, sizeof(msg), 1, 0);

	while(1)
	{
		printf("\nCurrently following types of services are available :\n");
		if(msg.enabled[0] == 1)
			printf("\n->Ophthalmologist\n");
		
		if(msg.enabled[NEURON] == ENABLED)
			printf("\n->Neurologist\n");
	
		if(msg.enabled[HEART] == ENABLED)
			printf("\n->Cardiologist\n");
		
		if(msg.enabled[BONES] == ENABLED)
			printf("\n->Orthopedist\n");
	
		if(msg.enabled[SKIN] == ENABLED)
			printf("\n->Dermatologist\n");

		if(msg.enabled[EYE] == DISABLED && msg.enabled[NEURON] == DISABLED && msg.enabled[HEART] == DISABLED && msg.enabled[BONES] == DISABLED && msg.enabled[SKIN] == DISABLED)
			printf("\n Sorry no service available..\n");
		
		printf("\nFollowing are the types of services available:\n 1. Normal Checkup\n 2. Operation\n");	
    		msg.type = 1;
		
		printf("Please enter the type of service you want:");
		while(1) 
		{
			scanf("%s",msg.service);
					
			if(strcasecmp(msg.service, "none") == COMP_SUCCESS)
			{
				printf("\nThank you for choosing our hospital...\n");
				/*sends exit condition through message queue*/
				msgsnd(msgid, &msg, sizeof(msg), 0);
				msgctl(msgid, IPC_RMID, NULL);	
				system("clear");			
				exit(1);
			}		
			
			
			if(!((strcasecmp(msg.service, "eye") == COMP_SUCCESS) || (strcasecmp(msg.service, "neuron") == COMP_SUCCESS) || (strcasecmp(msg.service, "heart") == COMP_SUCCESS) || (strcasecmp(msg.service, "bones") == COMP_SUCCESS) || (strcasecmp(msg.service, "skin") == COMP_SUCCESS))) 
			{
				printf("\nInvalid service entered\n");	
				printf("Re-enter the type of service required:");
				continue;	
			}		
			break;
			
		}

		printf("Please enter the type of severity:");
		while(scanf("%d", &msg.severity) != 1 || (msg.severity < 1 || msg.severity > 2))	
		{	
			tempFlag == DISABLED;
			while ((invalidInput = getchar()) != '\n')
			{
				printf("\nChoose the severity between 1 or 2.\n");
				tempFlag = ENABLED;
			}
			if((msg.severity > 2 || msg.severity < 1) && tempFlag == DISABLED)
				printf("\nChoose the severity between 1 or 2.\n");
		
			printf("\nRe-enter the type of severity :\n");
			
		}	
		/*sends severity and service over message queue*/
		msgsnd(msgid, &msg, sizeof(msg), 0);
		system("clear");
	}
	return SUCCESS;
}
