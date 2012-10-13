#ifndef FUNC_H_INCLUDED
#define FUNC_H_INCLUDED

#include <string.h>

static int stack[ 25 ];	//use it as a stack to calculate result
static int stackCounter = 0;

int hashFromLow ( int key, int localDepth );			//get hashKey of hashFromLow of localDepth

int hashFromUpOne ( int key, int localDepth );			//get hashKey of hashFromOne of localDepth

int hashFromUpTwo ( int key , int localDepth );			//get hashKey of hashFromTwo of localDepth

int getKey ( char*str );								//get the key in str

int totalSize ( int globalDepth );						//get the total size of globalDepth

int shiftLeft ( int number, int i );					//shift a number left i time

int getPartKey ( char* str );							//get L_PartKey in str

#endif // FUNC_H_INCLUDED
