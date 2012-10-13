#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Manager.h"
#include "Page.h"

using namespace std;

char inStreamChar[ 100 ] = "lineitem.tbl";
char outStreamChar[ 100 ] = "result.fil";
char testInStreamChar[ 100 ] = "testinput.in";
char testOutStreamChar[ 100 ] = "testoutput.out";
char indexStreamChar[ 100 ] = "hashindex.out";

int main(int argc, char *argv[])
{
	if ( argc != 2 ){
		printf( "argument error.Stop.\n");
		return 0;
	}

	char temp[ 100 ];
	strcpy( temp, argv[ 1 ] );
	strcat( temp,inStreamChar );
	strcpy( inStreamChar, temp );

	strcpy( temp, argv[ 1 ] );
	strcat( temp,outStreamChar );
	strcpy( outStreamChar, temp );

	strcpy( temp, argv[ 1 ] );
	strcat( temp,testInStreamChar );
	strcpy( testInStreamChar, temp );

	strcpy( temp, argv[ 1 ] );
	strcat( temp,testOutStreamChar );
	strcpy( testOutStreamChar, temp );

	strcpy( temp, argv[ 1 ] );
	strcat( temp,indexStreamChar );
	strcpy( indexStreamChar, temp );


	clock_t start, finish;
	printf( "start process:\n" );

	//start time
	start = clock();

    Manager m( inStreamChar, outStreamChar, testInStreamChar, testOutStreamChar, indexStreamChar );
    m.insertData();	//process data
	m.saveBuffer();	//finish process, save the content of memory into disk
	m.saveIndex();	//save the catalog in memory
	m.query();		//query the result

	
	//end time
	finish = clock();
	printf( "%f seconds in total\n", ( double )( finish - start ) / CLOCKS_PER_SEC );


    return 0;
}
