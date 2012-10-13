#ifndef PAGE_H_INCLUDED
#define PAGE_H_INCLUDED

#pragma once
#include "Func.h"
#include <string.h>
#include <iostream>
#include <stdio.h>
#define CAPACITY 8192

using namespace std;

class Page{

private:
    char content[ CAPACITY + 1 ];//content of the page

	//use it to get record, it is only a pointer
	char tempStr[ 200 ];
	int tempStrCounter;

    int readInt ( int start, int end );//set the int between start and end
    void setInt ( int start, int end, int number );//read the int between start and end

    bool canAddRecord( char *rec );//if can add rec into this page,return 1
    void slotNumberAdd ( );//the slot number ++


    int getRecordOffset( int index );//return offset in index
	void setRecordOffset( int index, int number );//set the offset in index
	

public:

	/*construct a page with localDepth*/
    Page( int localDepth );

	/*add rec into content, if success, return 1, else 0*/
    bool addRecord (char *rec );
	
	/*set free offset into number*/
    void setFreeOffset( int number );

	/*return the free offset*/
	int freeOffset();
	
	/*set the number of slot*/
	void setSlotNumber( int numebr );

	/*the number of slot*/
	int getSlotNumber();

	/*return the record in index*/
    char* getRecord ( int index );

	/*return the content in the page*/
	char* getContent();

	/*adjust the page after it has been spilted to save space*/
	void adjustPage();

	/*return record i length in index*/
	int getRecordLength ( int index );

	/*set record i length in index to number*/
	void setRecordLength ( int index, int number );

	/*clear this page
	*set slot number to 0 and set localDepth
	*/
	void clearPage( int localDepth );


	/*get the page localDepth*/
	int getLocalDepth();

	/*set the page localDepth*/
	void setLocalDepth( int number );
};



#endif // PAGE_H_INCLUDED
