#include "Func.h"
#include <math.h>


int hashFromLow ( int number, int localDepth )
{
	//get low localDepth bit
	int value = 1;
	for ( int i = 0; i < localDepth; i ++ )
		value = value << 1;

    return number % value;
}



int getPartKey ( char* str ){
	
	int counter = 0;
	while ( str[ counter++ ] != '|');
	int result = 0;

	while ( str[ counter ] != '|' ){
		result *= 10;
		result += str[ counter ++ ] - '0';
	}

	return result;
}



int hashFromUpOne ( int number, int localDepth ){
	//get fisrt localDepth bit
	stackCounter = 0;
	int result = 0;

	while ( number != 0 ) { stack[ stackCounter ++ ] = number % 2; number = number >> 1; }

	while ( localDepth != 0 ){
		if ( stackCounter == 0 ) break;
		result = result << 1;
		result += stack[ stackCounter - 1 ];
		stackCounter --;
		localDepth --;
	}

	return result;
}



int hashFromUpTwo ( int number , int localDepth )
{
	//get first localDepth bit
	int result = 0;
	stackCounter = 0;
	
	while ( number != 0 ){
		stack[ stackCounter++ ] = number % 2;
		number = number >> 1;
	}
	
	for ( int i = 22; i >=0; i -- ){
		if ( localDepth == 0 ) break;
		result = result << 1;
		result += stack[ i ];
		localDepth --;
	}

	for ( int i = 0; i < 23; i ++ )
		stack[ i ] = 0;

	return result;
}



int getKey ( char*str ){

	int result = 0;
	int counter = 0;

	while ( str[ counter ] != '|') {
		result *= 10;
		if ( str[ counter ] <= '9' && str[ counter ] >= '0' ) result += str[ counter ] - '0';
		counter ++;
	}

	return result;
}


int totalSize ( int globalDepth ){
    return pow( ( double )2, globalDepth );
}

int shiftLeft ( int number, int i ){
	while ( i != 0 ) { number = number << 1; i -- ; }
	return number;
}