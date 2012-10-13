#pragma once
#include "Manager.h"
#include "Buffer.h"
#include "Catalog.h"
#include "Page.h"
#include "Func.h"
#include <stdio.h>
#include <time.h>
#include <fstream>

//constructor
Manager::Manager(char *inChar, char *outChar, char *testInChar, char *testOutChar, char* indexChar){
	
    //open input file
    if ( ( inStream = fopen(inChar, "r") ) == NULL ){
		printf( "error,cann't open input file.\n" );
	}
	if ( ( testInStream = fopen( testInChar, "r") ) == NULL ){
		printf("error,cann't open testinput.in\n" );
	}
	if ( (testOutStream = fopen(testOutChar, "w+" ) ) == NULL ){
		printf("error,cann't open testoutput.out\n" );
	}
	

	//cout << "Please type in the number of pages in the memory:" << endl;
	int temp = 8;
	//cin >> temp;


	//create buffer and catalog
	int value = temp;
	buffer = new Buffer( temp - 1,  outChar );
	catalog = new Catalog( temp , indexChar );
	
	for ( int i = 0; i < temp; i ++ )	
		if ( i != temp - 1 ) buffer->saveOutPage( i );
	
	if ( temp == 8 ){
		buffer->setPageId( 0, 7 );
		buffer->saveOutPage( 0 );
		buffer->setPageId( 0, 0 );
	}
	else if ( temp == 128 ){
		buffer->setPageId( 0, 127 );
		buffer->saveOutPage( 0 );
		buffer->setPageId( 0, 0 );
	}

	pidCount = temp;
}


void Manager::insertLow( char *str ){

			int hashKey, nowPageId, location, total, nextPageId, key;
			int globalDepth = catalog->getGlobalDepth();

			key = getKey( str );
            hashKey = hashFromLow( key, globalDepth );
			nowPageId = catalog->getPageId( hashKey );
            location = buffer->ifPageExist( nowPageId );


            /*if the bucket is in buffer, then do noting
			*else chosse a page to be swapped
			*/
            if ( location == -1 )   {
				 location = buffer->choseByClock();
				if ( buffer->ifChange( location ) ) buffer->saveOutPage( location );
				buffer->readOutPage( location, nowPageId );
				
             }
			 
			//if insert succeed, the do noting
			if ( buffer->insertData( str,location ) ) ;
            else{

				//get two frames to spilt
				buffer->setPinCount(location);
				int localDepth = buffer->getLocalDepth( location );
				int anotherKey = hashFromLow( key, localDepth );
                nextPageId = pidCount++;
				total = totalSize(globalDepth);

				int spiltLocation = location;
				while ( spiltLocation == location ) spiltLocation = buffer->choseByClock();
				if ( buffer->ifChange( spiltLocation ) ) buffer->saveOutPage( spiltLocation );
				buffer->setPageId( spiltLocation, nextPageId );

				
                if ( localDepth == globalDepth ) 
                {
					//if the directory need to be double, spilt bucket and update catalog

					buffer->spiltPageLow( location, spiltLocation, hashKey, localDepth );
					
					catalog->doubleCatalogLow();
					
					catalog->setPageId( hashKey + total, nextPageId );   
									
				}
                else{  
					//if localDepth is less than globalDepth

					buffer->spiltPageLow( location, spiltLocation, anotherKey, localDepth );
				
					//update catalog
					int another = totalSize( localDepth );
					catalog->setPageId( anotherKey + another, nextPageId );  

					int value = 1;
					int offset;

					while ( 1 ){
						offset = shiftLeft( value, localDepth + 1 );

						if ( anotherKey + offset < total ) {
							catalog->setPageId( anotherKey + offset, nowPageId );  
						}
						else break;

						if ( anotherKey + another + offset < total ){
							catalog->setPageId( anotherKey + another + offset, nextPageId ); 
						}
						else break;

						value ++;
					}

				}

			   buffer->setDirty( location );
			   buffer->setDirty( spiltLocation );
               insertLow(str);			//insert again
			}
}


void Manager::insertUpOne ( char*str ){

			int hashKey, nowPageId, location,  key;
			int globalDepth = catalog->getGlobalDepth();

			key = getKey( str );
            hashKey = hashFromUpOne( key, globalDepth );
			nowPageId = catalog->getPageId( hashKey );
            location = buffer->ifPageExist( nowPageId );

			/*if the bucket is in buffer, then do noting
			*else chosse a page to be swapped
			*/
            if ( location == -1 )   {
				location = buffer->choseByClock();
				if ( buffer->ifChange( location ) ) buffer->saveOutPage( location );
				buffer->readOutPage( location, nowPageId );
				
             }

			 
			//if insert succeed, the do noting
			if ( buffer->insertData( str,location ) ) ;
			else
            {
				//get three frame to spilt
				buffer->setPinCount(location);
				int localDepth = buffer->getLocalDepth( location );
				int newPageOne = pidCount ++;
				int newPageTwo = pidCount ++;

				int newLocationOne =  buffer->choseByClock();
				if ( buffer->ifChange( newLocationOne ) ) buffer->saveOutPage( newLocationOne );
				buffer->setPinCount( newLocationOne );
				buffer->setPageId( newLocationOne ,newPageOne );

				int newLocationTwo = buffer->choseByClock();
				if ( buffer->ifChange( newLocationTwo ) )  buffer->saveOutPage( newLocationTwo );
				buffer->setPinCount( newLocationTwo );
				buffer->setPageId( newLocationTwo, newPageTwo );

				
				
				if (  localDepth == catalog->getGlobalDepth() ) 
                {
					//if the directory need to be double, spilt bucket and update catalog

					buffer->spiltPageUpOne( location, newLocationOne, newLocationTwo, hashKey, localDepth );
					
					catalog->doubleCatalogUpOne();

					catalog->setPageId( hashKey << 1, newPageOne );
					catalog->setPageId( hashKey << 1 | 1, newPageTwo );
						
				}
                else{   

					//if localDepth is less than globalDepth

					int anotherKey = hashFromUpOne( key, localDepth );

					buffer->spiltPageUpOne( location, newLocationOne, newLocationTwo, anotherKey, localDepth );

					//update the catalog
					int pass = 2, total = totalSize( catalog->getGlobalDepth() );
					int tempKey = anotherKey << 1;

					while ( tempKey  < total ){
		
						for ( int i = 0; i < pass / 2; i ++ )
							catalog->setPageId( tempKey + i, newPageOne );

						for ( int i = pass / 2; i < pass; i ++ )
							catalog->setPageId( tempKey + i, newPageTwo );

							tempKey = tempKey << 1;
							pass = pass << 1;

					}

				}
				
			   buffer->setDirty( location );
			   buffer->setDirty( newLocationOne );
			   buffer->setDirty( newLocationTwo );
			   insertUpOne( str );		//insert again

			}
}




void Manager::insertUpTwo ( char*str ){
		
			
			int hashKey, nowPageId, location, total, nextPageId, key;
			int globalDepth = catalog->getGlobalDepth();

			key = getKey( str );
            hashKey = hashFromUpTwo( key, globalDepth );
			nowPageId = catalog->getPageId( hashKey );
            location = buffer->ifPageExist( nowPageId );

            //if the record is not in the buffer, choose a frame to swap
             if ( location == -1 )   {
                location = buffer->choseByClock();
				if ( buffer->ifChange( location ) ) buffer->saveOutPage( location );	//if the page has been modified, then save it
				buffer->readOutPage( location, nowPageId );
				
              }
			 

			//if insert succeed, the do noting
			if ( buffer->insertData( str,location ) );
			else {

				buffer->setPinCount(location);
				int localDepth = buffer->getLocalDepth( location );
                nextPageId = pidCount++;
				total = totalSize(globalDepth);

				int spiltLocation = location;
				while ( spiltLocation == location ) spiltLocation = buffer->choseByClock();
				if ( buffer->ifChange( spiltLocation ) ) buffer->saveOutPage( spiltLocation );
				buffer->setPageId( spiltLocation, nextPageId );
				
				
                if (  localDepth == globalDepth )
                {
					//if the directory need to be double, spilt bucket and update catalog
					buffer->spiltPageUpTwo( location, spiltLocation, hashKey << 1, localDepth );
					 
					catalog->doubleCatalogUpTwo();
					
					catalog->setPageId( hashKey << 1 | 1, nextPageId );   
					
				}
                else{   

					//if localDepth is less than globalDepth

					int anotherKey = hashFromUpTwo( key, localDepth );	//the minimum key which first pointed to this bucket

					//spilt page
					buffer->spiltPageUpTwo( location, spiltLocation, anotherKey << 1, localDepth );
					
					//update catalog
					int value = totalSize( globalDepth - localDepth );
					int start = anotherKey << ( globalDepth - localDepth );
					int size =  value >> 1;
					for ( int j = 0; j < size; j ++ )
						catalog->setPageId( start + j, nowPageId );

					for ( int j = size; j < 2 * size; j ++ )
						catalog->setPageId( start + j, nextPageId );

				}

			   
			   buffer->setDirty( location );
			   buffer->setDirty( spiltLocation );
               insertUpTwo( str );	//insert the data again
			}

}



void Manager::insertData(){
   
    char content[ CAPACITY + 1 ];
	content[ CAPACITY ] = '\0';
	char str[ 300 ];
	int strCounter = 0;

    while ( !feof( inStream ) ){

		
        size_t current = fread( content, sizeof(char), CAPACITY, inStream );	
		
        for ( int i = 0; i < current; i ++ )
        {
            str[ strCounter ++ ] = content[ i ];	//get a record to insert

            if ( content[ i ] == '\n' )				//insert a record
            {	
				str[ strCounter - 1 ] = '\0';
				insertLow( str );	
                strCounter = 0;
            }

        }
		
		
    }

}



void Manager::saveBuffer(){
		//save the buffer in memory
		int size = buffer->getSize();
		for ( int i = 0; i < size; i ++ )		
			if ( buffer->ifChange( i ) ) buffer->saveOutPage( i );
}


void Manager::query(){
	
	int temp, t;
	int counter = 0;
	char str[ 10 ][ 300 ];
	int partKey[ 10 ], tempPartKey;
	char tempStr[ 300 ];
	clock_t start, finish;

	fscanf(testInStream, "%d", &t );
	start = clock();

	while ( t-- ){

		fscanf( testInStream, "%d", &temp );
		counter = 0;
		int hashKey = hashFromLow( temp, catalog->getGlobalDepth() );
		int pageId = catalog->getPageId( hashKey );

		//allocate a page and the get the result
		buffer->readOutPage( 0, pageId );
		Page* p = buffer->getPage( 0 );
		int size = p->getSlotNumber();

		for ( int i = 0; i < size; i ++ ){
			if ( getKey( p->getRecord( i ) ) == temp )
			{
				strcpy( str[ counter ], p->getRecord( i ) );
				partKey[ counter ] = getPartKey( str[ counter ] );
				counter ++;
			}
		}


		//sort the result in memory
		for ( int i = 0; i < counter; i ++ )
			for ( int j = 0; j < counter - 1; j ++ ){
				if ( partKey[ j ] > partKey[ j + 1 ] ){
					tempPartKey = partKey[ j ];
					partKey[ j ] = partKey[ j + 1 ];
					partKey[ j + 1 ] = tempPartKey;
					strcpy( tempStr, str[ j ] );
					strcpy( str[ j ], str[ j + 1 ] );
					strcpy( str[ j + 1 ], tempStr );
				}
			}

		//output the sorted result
		for ( int i = 0; i < counter; i ++ )
			fprintf( testOutStream, "%s\n", str[ i ] );
		
		fprintf( testOutStream, "-1\n" );
	}

	finish = clock();

	
	printf( "%f query per second.\n", (double) temp / ( ( double )( finish - start ) / CLOCKS_PER_SEC ) );
	cout << "Bucket Number:" << pidCount << endl;
	cout << "Total I/O times:" << buffer->iotime << endl;
}


void Manager::saveIndex(){
	catalog->saveCatalog();
}