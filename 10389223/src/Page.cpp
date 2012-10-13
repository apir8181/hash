#pragma once
#include "Page.h"


/*read int in the content from start to end*/
int Page::readInt( int start, int end ){

	//if the number is -1
    if (  content[ end - 1 ] == '-' && content[ end ] == '1' ) return -1;

	
    int result = 0;
    for ( int i = start; i <= end;i ++ )
    {
		result *= 10;
		result += content[ i ] - '0';
	}
    return result;
}


/*set int in the content from start to end*/
void Page::setInt( int start, int end, int number ){

	//reset the content in start and end
	memset( content + start, '0', sizeof(char) * ( end - start + 1 ) );

	//set the content in start and end
    if ( number == -1 ) { content[ end ] = '1'; content[ end - 1 ] = '-'; return; }

    int temp;
    int location = end;

    while ( number != 0 ){
        temp = number % 10;
        number /= 10;
        content[ location -- ] = temp + '0';
    }

}

Page::Page( int localDepth ){
	//clean the page
    memset( content, '0',  sizeof( char ) * CAPACITY );

	//initial value
	content[ CAPACITY ] = '\0';
	setLocalDepth( localDepth );
    setFreeOffset( 4 );
    setSlotNumber( 0 );

}


//if can add record return 1,else 0
bool Page::canAddRecord( char *rec ){
    if ( freeOffset() + strlen(rec) + 8 + 8 * getSlotNumber() + 8 < CAPACITY ) return 1;
	else return 0;
}


//add a record, if success return 1, else 0
bool Page::addRecord(char* rec){

    if ( canAddRecord(rec) ){

        int temp = getSlotNumber();
        int free_off =  freeOffset();
        int rec_length = strlen( rec );

		//copy the record into the page and update related data
		memcpy( content + free_off, rec, sizeof( char ) * rec_length );
        setRecordLength( temp, rec_length );
        setRecordOffset( temp, free_off );

		//update the free offset and number of the slot in the page
        setFreeOffset( free_off + rec_length );
        slotNumberAdd();

        return 1;
    }
    else return 0;

}


//compress a page, used for split page
void Page::adjustPage( ){

	int nextFree = 4;
	int slotSize = getSlotNumber();
	int length, offset;
	int counter = getSlotNumber();
	int location = 0;

	for ( int i = 0; i < slotSize; i ++ )
	{
		length = getRecordLength( i );

		//if the length is -1,it means that this data not in this page

		if ( length == -1 ) counter --;
		else{

			bool flag = 0;

			//find a slot with record length of -1,if it succeed, then move the data into this slot
			for (int j = location; j < i; j ++  )
				if ( getRecordLength( j ) == -1 ) { location = j; flag = 1; break; }

			if ( flag == 1 ){
					offset = getRecordOffset( i );

					memcpy( content + nextFree, content + offset, sizeof( char ) * length );
					setRecordOffset( location, nextFree );
					setRecordLength( location, length );
					setRecordLength( i, -1 );
			}
			
			 nextFree += length;
		}
	}

	//update free offset and slot number
	setSlotNumber( counter );
	setFreeOffset( nextFree );
}


int Page::freeOffset(){
    return readInt( CAPACITY - 4 , CAPACITY - 1);
}



int Page::getSlotNumber(){
    return readInt( CAPACITY - 8, CAPACITY - 5 );
}

void Page::setSlotNumber( int number ){
	setInt( CAPACITY - 8, CAPACITY - 5, number );
}


void Page::setFreeOffset ( int number ){
    setInt( CAPACITY - 4, CAPACITY - 1, number );
}



void Page::slotNumberAdd ( ){
    setSlotNumber(  getSlotNumber() + 1 );
}


char* Page::getRecord ( int index ){
    int offset = getRecordOffset( index );
    int length = getRecordLength( index );

	tempStrCounter = length;
	memcpy( tempStr, content + offset, sizeof( char ) * length );

    tempStr[ length ] = '\0';

    return tempStr;
}



int Page::getRecordOffset( int index ){
    return readInt( CAPACITY - ( index + 1 ) * 8 - 4, CAPACITY - ( index + 1 ) * 8 - 1 );
}

int Page::getRecordLength ( int index ){
	 return readInt( CAPACITY - ( index + 1 ) * 8 - 8, CAPACITY - ( index + 1 ) * 8 - 5 );
}

void Page::setRecordOffset( int index, int number ){
	setInt( CAPACITY - ( index + 1 ) * 8 - 4, CAPACITY - ( index + 1 ) * 8 - 1, number );
}

void Page::setRecordLength ( int index, int number ){
	setInt( CAPACITY - ( index + 1 ) * 8 - 8, CAPACITY - ( index + 1 ) * 8 - 5 , number );
}

char *Page::getContent(){
	return content;
}


void Page::clearPage( int localDepth ){
	//clean the content in the page, and set the localDepth
	memset( content, '0', sizeof(char) * CAPACITY );
	setLocalDepth( localDepth );
	setFreeOffset( 4 );
}

int Page::getLocalDepth(){
	return readInt( 0,3 );
}
void Page::setLocalDepth( int number ){
	setInt( 0, 3, number );
}