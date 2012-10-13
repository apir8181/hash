#include "Catalog.h"
#include <iostream>

using namespace std;


Catalog::Catalog( int totalSize, char *indexChar ){

	//create a catalog with size of totalSize
	size = totalSize;
	start = 0;
	dirty = 1;

	//initial the pageId 
	for ( int i = 0; i < size; i ++ )
		pageId[ i ] = i;

	//open the index file
	if ( (  indexStream = fopen( indexChar, "wb+") ) == NULL ){
		printf( "error, cann't open index file.\n" );
	}
	
	//calculate the global Depth
	globalDepth = 0;
	while ( totalSize != 1 ) { totalSize = totalSize >> 1; globalDepth ++; }

}

void Catalog::doubleCatalogLow(){
	
	/*if the size is less than maxsize, then it can be modified in memory
	*else must load a block of data from disk and save a block of data into disk
	*/
	if ( size < MAXSIZE ){

		for ( int i = 0; i < size; i ++ )
				pageId[ i + size ] = pageId[ i ];

		size = size << 1;
		dirty = 1;
		start = 0;
			
		
	}
	else{
			
		//if it has been modified, save it
		saveCatalog();

		//double catalog size, write the new part into disk
		int location = size / MAXSIZE;
		for ( int i = 0; i < location; i ++ ){
			fseek( indexStream, i * MAXSIZE * sizeof( int ), 0 );
			fread( pageId,  sizeof( int ), MAXSIZE, indexStream );
			fseek( indexStream, (  i * MAXSIZE + size ) * sizeof( int ), 0 );
			fwrite( pageId, MAXSIZE * sizeof( int ), 1, indexStream );
		}
		start = size * 2 - MAXSIZE;
		size = size << 1;
			
	}

	globalDepth ++;
}

void Catalog::saveCatalog(){

	/*if it is been modified, then do noting
	*else save it into realting page
	*/
	if (!dirty) return;

	fseek( indexStream, start * sizeof ( int ), SEEK_SET );
	fwrite( pageId, MAXSIZE * sizeof( int ), 1, indexStream );
	dirty = 0;

}

int Catalog::getPageId ( int hashKey ){
	
	/*if it is in memory page, return 
	*else, then if the catalog has been modified, save
	*then load a block of realting data into memory
	*/

	if ( start <= hashKey && hashKey < start + MAXSIZE ) return pageId[ hashKey - start ];
	else{
			
		saveCatalog();
		int location = hashKey / MAXSIZE;

		fseek( indexStream, location * MAXSIZE * sizeof( int ), SEEK_SET );
		fread(pageId, sizeof(int), MAXSIZE, indexStream );

		start = location * MAXSIZE;

		return pageId[ hashKey - start ];
	}
}


void Catalog::setPageId ( int hashKey, int number ){

	/*if it is in memory page, modified it
	*else, then if catalog in memory has been modified, save
	*then load a block of realting data into memory and modified it
	*/


	if ( start <= hashKey && hashKey < start + MAXSIZE ) pageId[ hashKey - start ] = number;
	else{
			
		saveCatalog();
		int location = hashKey / MAXSIZE;

		fseek( indexStream, location * MAXSIZE * sizeof( int ), SEEK_SET );
		fread(pageId, sizeof(int), MAXSIZE, indexStream );

		start = location * MAXSIZE;

		pageId[ hashKey - start ] = number;
	}

	dirty = 1;
}

int Catalog::getGlobalDepth (){
	return globalDepth;
}


void Catalog::doubleCatalogUpOne(){
	
	//double the catalog, hash function and spilt rule supported by hashFromUpOne

	if ( size < MAXSIZE ){

		for ( int i = size * 2 - 1; i >= size; i -- )
			pageId[ i ] = pageId[ i >> 1 ];
			
		size = size << 1;
		dirty = 1;
		start = 0;		
		
	}
	else{
			
			
		saveCatalog();
		int location = size / MAXSIZE;
		int temp;

		for ( int i =  size * 2  - 1 ; i >= size; i -- ){
			fseek( indexStream, ( i >> 1 ) * sizeof( int ), SEEK_SET );
			fread( &temp, sizeof( int ), 1, indexStream );
			fseek( indexStream, i * sizeof( int ), SEEK_SET );
			fwrite( &temp, sizeof(int), 1, indexStream ); 
		}

		start = size * 2 - MAXSIZE;
		fseek( indexStream, start * sizeof( int ), SEEK_SET );
		fread( pageId, MAXSIZE * sizeof( int ), 1 , indexStream );

		size = size << 1;
		
		}
		globalDepth ++;
}


void Catalog::doubleCatalogUpTwo(){

	//double the catalog, hash function and spilt rule supported by hashFromUpTwo

	if ( size < MAXSIZE ){

		for ( int i = size * 2 - 1; i >= 0; i -- )
			pageId[ i ] = pageId[ i >> 1 ];
	
		size = size << 1;
		dirty = 1;
		start = 0;
					
	}
	else{
			
		saveCatalog();
		int location = size / MAXSIZE;
		int temp;

		for ( int i =  size * 2  - 1 ; i >= 0; i -- ){
			fseek( indexStream, ( i >> 1 ) * sizeof( int ), SEEK_SET );
			fread( &temp, sizeof( int ), 1, indexStream );
			fseek( indexStream, i * sizeof( int ), SEEK_SET );
			fwrite( &temp, sizeof(int), 1, indexStream ); 
		}

		start = 0;
		fseek( indexStream, 0, SEEK_SET );
		fread( pageId, MAXSIZE * sizeof( int ), 1 , indexStream );
		size = size << 1;
			
	}		
	globalDepth ++;
}
