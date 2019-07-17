#include<stdio.h>
#include<malloc.h>
#include<string.h>

typedef int BOOL;

#define TRUE 1
#define FALSE 0

typedef struct node
{
	void*start;
	void*end;
	struct node*next;
}NODE,*PNODE,**PPNODE;

//Actual Start Address and End Address
void*start = NULL;
void*end = NULL;

//Heap start and end address
void*startHeapAddr = NULL;
void*endHeapAddr = NULL;

//Unalloc variables
void*unallocStartAddr = NULL;
void*unallocEndAddr = NULL;
PNODE UnallocHead = NULL;

//Alloc variables
void*allocStartAddr = NULL;
void*allocEndAddr = NULL;
PNODE AllocHead = NULL;

//Function Declaration
void yalloc(int, void**, void**);
void ymalloc(int);
BOOL Unallocate(PPNODE, void**, int);
void Display(PNODE);
void yfree(int,PPNODE,PPNODE);
void mergeUnalloc(PPNODE, PPNODE);
void Allocate(PPNODE,PPNODE,int);

int main()
{
	int yallocBytes = 0, ymallocBytes = 0, count = 0, yfreeBytes = 0, totalAllocBytes = 0, i = 0;
	void*tempStart = start;
	void**iArr = NULL;
	PNODE newn = NULL;

	printf("How many bytes for yalloc :\n");
	scanf("%d",&yallocBytes);

	yalloc(yallocBytes,&startHeapAddr,&endHeapAddr);
	printf("\nHeap allocated Successfully\nActual Start Address : %p\nActual End Address : %p\nStarting Address : %p\nEnding Address : %p\n",start,end,startHeapAddr,endHeapAddr);

	//Create an array which contains Base addres of every node of a list
	iArr = (void**)malloc(yallocBytes*sizeof(void*));
	if(iArr == NULL)
	{
		printf("Unable to allocate\n");
		return -1;
	}
	tempStart = start;
	iArr[0] = tempStart;
	for(i=1;i<yallocBytes;i++)
	{
		tempStart = tempStart + (sizeof(NODE)*sizeof(void));
		iArr[i] = tempStart;
	}

	printf("\n-------------------------------------------------------------------------------------------------\n");

	printf("\nNow it is unallocated\n");
	Unallocate(&UnallocHead,iArr,yallocBytes);
	Display(UnallocHead);
	printf("\nUnalloc Start Address : %p\nUnalloc End Address : %p\n",unallocStartAddr,unallocEndAddr);

	printf("\n-------------------------------------------------------------------------------------------------\n");

	printf("\nHow many times ymalloc call :\n");
	scanf("%d",&count);
	if(count > yallocBytes)
	{
		printf("Count of ymalloc should be less than %d\nUnable to proceed\n",yallocBytes);
		return -1;
	}
	
	while(count != 0)
	{
		printf("\nHow many Bytes for ymalloc :\n");
		scanf("%d",&ymallocBytes);
		
		totalAllocBytes = totalAllocBytes + ymallocBytes;
		if(totalAllocBytes > ymallocBytes)
		{
			printf("Insufficient Space\nUnable to allocate ymallocBytes\n");
			return -1;
		}
	
		ymalloc(ymallocBytes);
		printf("\nAllocated list\n");
		Display(AllocHead);
		printf("\nAlloc Start Address : %p\nAlloc End Address : %p\n",allocStartAddr,allocEndAddr);
		
		printf("\n-------------------------------------------------------------------------------------------------\n");

		printf("\nUnallocated List:\n");
		Display(UnallocHead);
		printf("\nUnalloc Start Address : %p\nUnalloc End Address : %p\n",unallocStartAddr,unallocEndAddr);

		count--;
		printf("\n-------------------------------------------------------------------------------------------------\n");
	}
	
	printf("How many bytes you want to free\n");
	scanf("%d",&yfreeBytes);
		
	if(yfreeBytes > totalAllocBytes)
	{
		printf("Unable to free\nBytes to free should be less than %d\n",totalAllocBytes);
	}

	yfree(yfreeBytes,&AllocHead,&UnallocHead);

	printf("After freeing the memory\n");

	printf("\nAllocated List\n");
	Display(AllocHead);
	printf("\nAlloc Start Address : %p\nAlloc End Address : %p\n",allocStartAddr,allocEndAddr);
	
	printf("\n-------------------------------------------------------------------------------------------------\n");

	printf("\nUnallocated List:\n");
	Display(UnallocHead);
	printf("\nUnalloc Start Address : %p\nUnalloc End Address : %p\n",unallocStartAddr,unallocEndAddr);

	return 0;
}

//yalloc Function
/* Allocates a section of N bytes, using malloc */
void yalloc(int yallocBytes,void**startHeapAddr,void**endHeapAddr)
{
	start = (void*)malloc(((sizeof(NODE)*yallocBytes)+yallocBytes)*sizeof(void));
	end = start + (((sizeof(NODE)*yallocBytes)+yallocBytes)*sizeof(void));
	
	*startHeapAddr = start + (sizeof(NODE)*yallocBytes);
	*endHeapAddr = *startHeapAddr + yallocBytes*sizeof(void);
}

//ymalloc Function
/* Allocates the memory from the section allocated by yalloc */
void ymalloc(int iBytes)
{
	Allocate(&AllocHead,&UnallocHead,iBytes);
}

//yfree Function
/* Delete the node from allocated list and pass it to merge into deallocated list */
void yfree(int iBytes, PPNODE allocFirst, PPNODE unallocFirst)
{
	PNODE temp1Alloc = *allocFirst, temp2Alloc = *allocFirst;
	int tempBytes = iBytes;
	
	while(iBytes != 0)
	{
		*allocFirst = (*allocFirst)->next;
		iBytes--;
	}
	iBytes = tempBytes;
	
	while(iBytes >= 2)
	{
		temp2Alloc = temp2Alloc->next;
		iBytes--;
	}
	iBytes = tempBytes;
	temp2Alloc->next = NULL;
	
	mergeUnalloc(&temp1Alloc,&UnallocHead);
}

// Merge Function
/* Merge the node in Unallocated list */
void mergeUnalloc(PPNODE actalUnallocStart, PPNODE partialUnallocStart)
{
	PNODE temp = *actalUnallocStart;

	while(temp->next != NULL)
	{
		temp = temp->next;
	}
	temp->next = *partialUnallocStart;
	*partialUnallocStart = *actalUnallocStart;
	
	unallocStartAddr = (*partialUnallocStart)->start;
}

//Unalloc Function
/* Linked list which contains addresses which are unallocated */
BOOL Unallocate(PPNODE first, void** iArr, int iBytes)
{
	//Logic of Inserting a node at last position
	int i = 0;
	PNODE newn = NULL, Temp = *first;
	void*TempStartAddr = NULL;
	unallocStartAddr = startHeapAddr;
	
	TempStartAddr = unallocStartAddr;
	
	while(iBytes != 0)
	{
		newn = (PNODE)malloc(sizeof(NODE));
		
		if(newn == NULL)
		{
			return FALSE;
		}
	
		newn->start = TempStartAddr;
		newn->end = newn->start + 1*sizeof(void);
		newn->next = NULL;

		memmove((PNODE)iArr[i],newn,sizeof(NODE));
		newn = (PNODE)iArr[i];
		
		if(*first == NULL)
		{
			*first = newn;
		}
		else
		{
			Temp = *first;
			while(Temp->next != NULL)
			{
				Temp = Temp->next;
			}
			Temp->next = newn;
		}
		
		iBytes--;
		i++;
		TempStartAddr = newn->end;
		printf("newn %p\n",newn);
		printf("iArr[i]->start : %p\n",newn->start);
		printf("iArr[i]->end : %p\n",newn->end);
		printf("iArr[i]->next : %p\n",newn->next);
	}

	unallocEndAddr = TempStartAddr;
}

//Alloc Function
/* Linked list which contains addresses which are allocated */
void Allocate(PPNODE allocFirst, PPNODE unallocFirst, int iBytes)
{
	int tempBytes = iBytes;
	PNODE forAllocStart = *unallocFirst, forAllocEnd = *unallocFirst, tempAllocFirst = *allocFirst;
	
	if(*allocFirst == NULL)
	{
		while(iBytes != 0)
		{
			*unallocFirst = (*unallocFirst)->next;
			iBytes--;
		}
			
		iBytes = tempBytes;
		while(iBytes >= 2)
		{
			forAllocEnd = forAllocEnd->next;
			iBytes--;
		}
		iBytes = tempBytes;
		forAllocEnd->next = NULL;
		AllocHead = forAllocStart;
		allocStartAddr = AllocHead->start;
		allocEndAddr = forAllocEnd->end;
		unallocStartAddr = (*unallocFirst)->start;
	}
	else
	{
		while(tempAllocFirst->next != NULL)
		{
			tempAllocFirst = tempAllocFirst->next;
		}
		while(iBytes != 0)
		{
			*unallocFirst = (*unallocFirst)->next;
			iBytes--;
		}

		iBytes = tempBytes;
		while(iBytes >= 2)
		{
			forAllocEnd = forAllocEnd->next;
			iBytes--;
		}
		forAllocEnd->next = NULL;
		tempAllocFirst->next = forAllocStart;
		allocStartAddr = AllocHead->start;
		allocEndAddr = forAllocEnd->end;
		unallocStartAddr = (*unallocFirst)->start;
	}
}

//Display Function
void Display(PNODE first)
{
	while(first != NULL)
	{
		printf("\nAt address %p\n",first);
		printf("Start Address : %p\n",first->start);
		printf("End Address : %p\n",first->end);
		printf("Next Address : %p\n",first->next);
		printf("\n\n");

		first = first->next;
	}
}
