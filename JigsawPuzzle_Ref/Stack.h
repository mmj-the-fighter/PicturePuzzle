//Copyright 2005 : Manoj M J
#ifndef __STACK_H__
#define __STACK_H__

#include <stdio.h>
#define GRAPH_STACKSIZE 128

class StackX
{
public:
	int Stack[GRAPH_STACKSIZE];
	int Top;
	StackX()
	{
		Top=-1;
	}
	void Push(int Index)
	{
		Stack[++Top] = Index;
	}
	int Pop()
	{
		return Stack[Top--];
	}
	int Peek()
	{
		return Stack[Top];
	}
	int IsEmpty()
	{
		return(Top == -1);
	}
	void CleanStack()
	{
		Top =-1;
	}
};
	
	

#endif