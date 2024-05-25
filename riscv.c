#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

//*****************COMMENTS******************
//add,sub,addi,subi,ori,andi,or,and: IF->ID->Ex->WB
//lw:								 IF->ID->Ex->Mem->Wb
//sw:								 IF->ID->Ex->Mem
//j: TBD
//bneq,beq: TBD

//**********DEFINE**************
#define TRUE 0u
#define FALSE 1u

//**********TYPEDEF*************
typedef unsigned char bool;

//**********ENUM**************************
//Enum for hold places of main input parameters
typedef enum
{
	CMD_FILE = 1,
	FORWARDING,
	BRANCH_RES,
}InputParams;

//Enum for indicate instruction index in global array
typedef enum
{
	IF = 0,
	ID,
	EX,
	MEM,
	WB,
}InsInx;

//**********STRUCTS************************
//Struct taht hold BASIC command defenition
typedef struct
{
	char strCmdName[20];
	char strFullCmd[200];
	unsigned char u8OutReg;
	unsigned char u8InputReg_1;
	unsigned char u8InputReg_2;
	char strLabel[20];
	bool bUseWb;
	bool bUseMem;
	bool bIsStall;
	bool bIsBranch;
	int lenghtCmd;
}Cmd;

//Stuct that hold BASIC Instruction defenition
typedef struct
{
	char strUnitName[100];
	Cmd* psCurrentCmd;
}Instruction;

//*********GLOBAL*****************
//Use FORWARDING
bool bForwarding = FALSE;

//BRANCH RESOLUTION: FASLE - MEM, TRUE - ID
bool bBranchResolution = FALSE;

//Cycle counter
unsigned int u32CycleCnt = 0;

//Stall struct
Cmd sStall =
{
	"",
	"Stall",
	0xFF,
	0xFF,
	0xFF,
	"",
	FALSE,
	FALSE,
	TRUE
};


//Stall struct
Cmd sNOP =
{
	"",
	"NOP",
	0xFF,
	0xFF,
	0xFF,
	"",
	FALSE,
	FALSE,
	TRUE
};

//Static array that hold all RISCV units
Instruction gasInstructions[] =
{
	{
		"Fetch",
		NULL
	},
	{
		"Decode",
		NULL
	},
	{
		"Execute",
		NULL
	},
	{
		"Memory",
		NULL
	},
	{
		"Writeback",
		NULL
	}
};

//*****************LOCAL_DECLARATION**************************
//Function for simulate all operations when clock rise in RISCV
bool clock_rise(char* pstrNewCmd);

//Service function for convert CMD from file to STRUCT cmd for internal use
Cmd* con_str_to_cmd(char* pstrCmd);

//Function that implement pipeline logic
void pipeline_mover(Cmd* pNewCmd);

//Service function for print curent status of system
void print_status();

//Function for check if we can move pipe(forwarding/non-forwarding)
bool check_move_cond();

//*******************************MAIN**************************
int main(int argc, char* argv[])
{
	FILE* fd;
	bool bLoop = TRUE;
	char strLine[100] = { 0 };
	int i32Temp = 0;

	/** Get all input parameters **/
	if (argc != 4)
	{
		printf("Illigal input\n");
		return;
	}

	fd = fopen(argv[CMD_FILE], "r");
	if (!fd)
	{
		printf("Cant open CMD file\n");
		return;
	}

	i32Temp = atoi(argv[FORWARDING]);
	if (i32Temp == 0)
	{
		bForwarding = FALSE;
	}
	else
	{
		bForwarding = TRUE;
	}

	i32Temp = atoi(argv[BRANCH_RES]);
	if (i32Temp == 0)
	{
		bBranchResolution = FALSE;
	}
	else
	{
		bBranchResolution = TRUE;
	}

	while (bLoop == TRUE)
	{
		if (!feof(fd))
		{
			if (bBranchResolution == TRUE)
			{
				int lenght_brentch = 0;
				if (gasInstructions[EX].psCurrentCmd != NULL && gasInstructions[EX].psCurrentCmd->bIsBranch == TRUE)
				{
					lenght_brentch = gasInstructions[IF].psCurrentCmd->lenghtCmd + gasInstructions[ID].psCurrentCmd->lenghtCmd;
					fseek(fd, -lenght_brentch, SEEK_CUR);
					bLoop = clock_rise(strLine);
				}
			}
			if (fgets(strLine, 100, fd))
			{
				//printf("%s\n", strLine);
				bLoop = clock_rise(strLine);
			}
		}
		else
		{
			bLoop = clock_rise(NULL);
		}
		print_status();
	}
	fclose(fd);
}

//******************SERVICE_FUNCTIONS********************
bool clock_rise(char* pstrNewCmd)
{
	bool bMoveIns = TRUE;
	Cmd* pNewCmd = con_str_to_cmd(pstrNewCmd);
	if (pstrNewCmd != NULL)
	{
		pNewCmd->lenghtCmd = strlen(pstrNewCmd) + 1;
	}
	u32CycleCnt++;
	//Initial state
	if ((gasInstructions[IF].psCurrentCmd == NULL) && (pstrNewCmd != NULL))
	{
		gasInstructions[IF].psCurrentCmd = pNewCmd;
	}
	else
	{
		pipeline_mover(pNewCmd);
	}

	//This branch logic will help to end programm(see main)ll
	if ((gasInstructions[MEM].psCurrentCmd == NULL) && (gasInstructions[EX].psCurrentCmd == NULL) &&
		(gasInstructions[ID].psCurrentCmd == NULL) && (gasInstructions[IF].psCurrentCmd == NULL) &&
		(gasInstructions[WB].psCurrentCmd == NULL))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}

}

void pipeline_mover(Cmd* pNewCmd)
{
	bool bRerun = FALSE;
	bool bSkipInsert = FALSE;
	Cmd* pTmp1 = NULL, * pTmp2 = NULL, * pTmp3 = NULL;

RERUN_ST_1:
	if (check_move_cond() == TRUE)
	{
		//UPD FETCH register
		pTmp1 = gasInstructions[IF].psCurrentCmd;
		gasInstructions[IF].psCurrentCmd = pNewCmd;

		//UPD DECODE register
		//pTmp1 = was in FETCH
		pTmp2 = gasInstructions[ID].psCurrentCmd;
		gasInstructions[ID].psCurrentCmd = pTmp1;
	}
	else if ((bBranchResolution == FALSE) && (gasInstructions[ID].psCurrentCmd != NULL) && (gasInstructions[ID].psCurrentCmd->bIsBranch == TRUE))
	{
		pTmp2 = gasInstructions[ID].psCurrentCmd;
		bRerun = TRUE;
		gasInstructions[ID].psCurrentCmd = NULL;
	}
	else
	{
		pTmp2 = NULL;
		bRerun = TRUE;
	}

	//UPD EXECUTION register
	//pTmp2 = was in DECODE
RERUN_ST_2:
	if ((gasInstructions[EX].psCurrentCmd != NULL) && (gasInstructions[EX].psCurrentCmd->bUseMem == FALSE))
	{
		pTmp1 = NULL;
		bSkipInsert = TRUE;
		if ((gasInstructions[MEM].psCurrentCmd != NULL) && (gasInstructions[MEM].psCurrentCmd->bUseWb == TRUE))
		{

		}
		else if (gasInstructions[EX].psCurrentCmd->bIsBranch == TRUE && bBranchResolution == TRUE)
		{
			gasInstructions[IF].psCurrentCmd = NULL;
			gasInstructions[ID].psCurrentCmd = NULL;
			gasInstructions[EX].psCurrentCmd = NULL;
		}
		else
		{

			pTmp3 = gasInstructions[EX].psCurrentCmd;
			if (gasInstructions[EX].psCurrentCmd->bIsBranch == TRUE && bBranchResolution == FALSE)
			{
				pTmp3 = NULL;
			}
			gasInstructions[EX].psCurrentCmd = pTmp2;
		}
	}
	else
	{
		pTmp1 = gasInstructions[EX].psCurrentCmd;
		gasInstructions[EX].psCurrentCmd = pTmp2;
	}

	//UPD MEMORY register
	//pTmp1 = was in EXECUTION
	if ((gasInstructions[MEM].psCurrentCmd != NULL) && (gasInstructions[MEM].psCurrentCmd->bUseWb == FALSE))
	{
		pTmp2 = NULL;
		gasInstructions[MEM].psCurrentCmd = pTmp1;
	}
	else
	{
		pTmp2 = gasInstructions[MEM].psCurrentCmd;
		gasInstructions[MEM].psCurrentCmd = pTmp1;
	}

	//UPD WB register
	//pTmp2 = was in MEMORY
	if (pTmp3 != NULL)
	{
		pTmp1 = gasInstructions[WB].psCurrentCmd;
		gasInstructions[WB].psCurrentCmd = pTmp3;
		pTmp3 = NULL;
	}
	else
	{
		pTmp1 = gasInstructions[WB].psCurrentCmd;
		gasInstructions[WB].psCurrentCmd = pTmp2;
	}

	if (bSkipInsert = TRUE)
	{
		bSkipInsert = FALSE;
		print_status();
		u32CycleCnt++;
		goto RERUN_ST_2;
	}

	if (bRerun == TRUE)
	{
		bRerun = FALSE;
		print_status();
		u32CycleCnt++;
		goto RERUN_ST_1;
	}
}

bool check_move_cond()
{
	bool bRet = TRUE;
	if (gasInstructions[ID].psCurrentCmd != NULL && bForwarding == FALSE)
	{
		if (gasInstructions[EX].psCurrentCmd != NULL)
		{
			if (gasInstructions[ID].psCurrentCmd->u8InputReg_1 == gasInstructions[EX].psCurrentCmd->u8OutReg)
			{
				bRet = FALSE;
			}
			else if (gasInstructions[ID].psCurrentCmd->u8InputReg_2 == gasInstructions[EX].psCurrentCmd->u8OutReg)
			{
				bRet = FALSE;
			}
		}
	}
	if (gasInstructions[ID].psCurrentCmd != NULL && bBranchResolution == FALSE && gasInstructions[ID].psCurrentCmd->bIsBranch == TRUE)
	{
		bRet = FALSE;
	}
	return bRet;
}

Cmd* con_str_to_cmd(char* pstrCmd)
{
	int i32Tmp = 0;
	char strTmp[100];
	char strCmd[10];
	char strRegOut[10];
	char strRegIn_1[10];
	char strRegIn_2[10];

	if (pstrCmd == NULL)
	{
		return NULL;
	}

	Cmd* pTmp = (Cmd*)malloc(sizeof(Cmd));
	if (pTmp != NULL)
	{
		strcpy(strTmp, pstrCmd);
		strTmp[strlen(strTmp) - 2] = '\0';
		//Split string to relevant data segments
		sscanf(pstrCmd, "%d %s %s %s %s\n", &i32Tmp, strCmd, strRegOut, strRegIn_1, strRegIn_2);
		strcpy(pTmp->strCmdName, strCmd);

		snprintf(pTmp->strFullCmd, sizeof(pTmp->strFullCmd), "%s %s %s %s", strCmd, strRegOut, strRegIn_1, strRegIn_2);

		//In case if it is branch command tatke label, otherwise take register values
		if (!strcmp(strCmd, "beq") || !strcmp(strCmd, "bneq"))
		{
			strcpy(pTmp->strLabel, strRegIn_2);
			if (strRegOut[0] == 'x')
			{
				strRegOut[0] = '0';
			}
			if (strRegIn_1[0] == 'x')
			{
				strRegIn_1[0] = '0';
			}
			pTmp->u8OutReg = 0xFF;
			pTmp->u8InputReg_1 = atoi(strRegOut);
			pTmp->u8InputReg_2 = atoi(strRegIn_1);
			pTmp->bIsBranch = TRUE;
			pTmp->bUseWb = FALSE;
			pTmp->bUseMem = FALSE;
		}
		else
		{
			//If it register replcae x by 10 for normal conversion
			if (strRegOut[0] == 'x')
			{
				strRegOut[0] = '0';
			}
			if (strRegIn_1[0] == 'x')
			{
				strRegIn_1[0] = '0';
			}
			if (strRegIn_2[0] == 'x')
			{
				strRegIn_2[0] = '0';
			}
			pTmp->u8OutReg = atoi(strRegOut);
			pTmp->u8InputReg_1 = atoi(strRegIn_1);
			pTmp->u8InputReg_2 = atoi(strRegIn_2);
		}
		//Set flags for indicate Ins stages that required(see top block COMMENTS)
		if (!strcmp(pTmp->strCmdName, "add") || !strcmp(pTmp->strCmdName, "sub") ||
			!strcmp(pTmp->strCmdName, "addi") || !strcmp(pTmp->strCmdName, "subi") ||
			!strcmp(pTmp->strCmdName, "or") || !strcmp(pTmp->strCmdName, "ori") ||
			!strcmp(pTmp->strCmdName, "and") || !strcmp(pTmp->strCmdName, "andi"))
		{
			pTmp->bUseWb = TRUE;
			pTmp->bUseMem = FALSE;
		}

		if (!strcmp(pTmp->strCmdName, "lw"))
		{
			pTmp->bUseWb = TRUE;
			pTmp->bUseMem = TRUE;
		}

		if (!strcmp(pTmp->strCmdName, "sw"))
		{
			pTmp->bUseWb = FALSE;
			pTmp->bUseMem = TRUE;
		}
	}
	return pTmp;
}

void print_status()
{
	printf("------------------------------------------------\n");
	printf("Cycle %d\n", u32CycleCnt);
	for (int i = 0; i < 5; i++)
	{
		if (gasInstructions[i].psCurrentCmd == NULL)
		{
			printf("%s instruction: STALL\n", gasInstructions[i].strUnitName);
		}
		else
		{
			printf("%s instruction: %s\n", gasInstructions[i].strUnitName, gasInstructions[i].psCurrentCmd->strFullCmd);
		}
	}
	printf("------------------------------------------------\n");
}
