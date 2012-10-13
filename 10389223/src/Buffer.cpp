#include	"Buffer.h"


Buffer::Buffer(int size, char* outChar)
{
	/*initialization contents in the buffer
	 *and allocate size pages in memory
	*/
	this->size = size;
    pool = new Page*[size];
   	pageId = new int[size];
	dirty = new bool[ size ];
	pin_count = new bool[ size ];
	ref_bit = new bool[ size ];
	currentPageId = 0;
	current = 0;
	iotime = 0;

	//open bucket file
	if ( ( outStream = fopen(outChar,"wb+") ) == NULL ){
		printf( "error, cann't open output file.\n" );
	}


	//buffer size only can be 7 or 127
	int localDepth = 3;
	if ( size == 127 ) localDepth = 7;


	//take first size bucket into memory
    for ( int i = 0; i < size; i ++ )
    {
		pool[i] = new Page(  localDepth );
		pin_count[ i ] = 0;
		ref_bit[ i ] = 0;
	    dirty[i] = false;
	    pageId[i] = i; 
    }

};

int Buffer::getSize()
{
	return size;
};

bool Buffer::insertData(char* str,int f_offset)
{
	ref_bit[ f_offset ] = true;		//it is been modified
	dirty[ f_offset ] = true;
	return pool[f_offset]->addRecord(str);
};


int Buffer::ifPageExist(int pageNum)
{
	int i;
	for(i=0;i<size;i++)
		if(pageId[i]==pageNum) return i;

	return -1;
};



bool Buffer::ifChange(int frameNum)
{
	return dirty[frameNum];
};


int Buffer::choseByClock()
{	
	//if a page is been locked, it cann't be swapped out
	while ( 1 ){
		if ( ref_bit[ current ] == false && pin_count[ current ] == false) break;	//if a LRU page is found,break
		else if ( pin_count[ current ] == false ) ref_bit[ current ] = false;	
		current = ( current + 1 ) % size;
	}

	int temp = current;//result

	current = ( current + 1 ) % size;//the next start location

	return temp;
};

void Buffer::setPinCount ( int frameNum ){
	pin_count[ frameNum ] = 1;
}

int Buffer::getPageId ( int offset ){
    return pageId[ offset ];
}

void Buffer::setPageId ( int f_offset, int pId ){
    pageId[ f_offset ] = pId;
}

void Buffer::setDirty ( int frameNum ){
	dirty[ frameNum ] = true;
}

void Buffer::clearPinCount ( int frameNum ){
	pin_count[ frameNum ] = 0;
}


void Buffer::saveOutPage ( int location ){

	
	int offset = ( pageId[ location ] - currentPageId ) * CAPACITY;
	fseek( outStream, offset, SEEK_CUR );
	fwrite( pool[ location ]->getContent(), CAPACITY, 1,outStream );

	ref_bit[ location ] = 0;
	currentPageId = pageId[ location ] + 1;
	dirty[ location ] = false;
	
	
	iotime ++;
	/*
	//if the file is bigger than 2G, it must been addressed through _fseeki64
	
	__int64 offset = ( __int64 )( ( pageId[ location ] - currentPageId ) )* CAPACITY;
	
	_fseeki64( outStream, offset, SEEK_CUR );
	fwrite( pool[ location ]->getContent(), CAPACITY, 1,outStream );

	ref_bit[ location ] = 0;
	currentPageId = pageId[ location ] + 1;
	dirty[ location ] = false;
	*/
	
}

void Buffer::readOutPage ( int location, int pageId ){

	
	int offset = ( pageId - currentPageId ) * CAPACITY;

	fseek( outStream, offset, SEEK_CUR );
	fread( pool[ location ]->getContent(), sizeof(char), CAPACITY, outStream );

    this->pageId[ location ] = pageId;
	this->dirty[ location ] = false;
	currentPageId = pageId + 1;
	
	iotime++;
	
	/*
	//if the file is bigger than 2G, it must been addressed through _fseeki64
	
	__int64 offset = ( __int64 )( ( pageId - currentPageId ) )* CAPACITY;

	_fseeki64( outStream, offset, SEEK_CUR );

	fread( pool[ location ]->getContent(), sizeof(char), CAPACITY, outStream );

    this->pageId[ location ] = pageId;
	this->dirty[ location ] = false;
	currentPageId = pageId + 1;
	*/
	
}

void Buffer::clearPage( int frameNum, int localDepth ){
	pool[ frameNum ]->setLocalDepth( localDepth );
	pool[ frameNum ]->setFreeOffset( 4 );
	pool[ frameNum ]->setSlotNumber( 0 );
}


void Buffer::spiltPageLow ( int oldLocation, int newLocation, int hashKey, int localDepth )
{
	int slotSize = pool[ oldLocation ]->getSlotNumber();
	char* str;
	int key;

	pool[ newLocation ]->clearPage( localDepth + 1 );
	pool[ oldLocation ]->setLocalDepth( localDepth + 1 );

	for ( int i = 0; i < slotSize; i ++ ){
		str = pool[ oldLocation ]->getRecord( i );
		key = getKey( str );

		if ( hashFromLow( key, localDepth + 1 ) == hashKey );
		else{
			insertData(str,newLocation);
			pool[ oldLocation ]->setRecordLength( i, -1 );
		}

	}
	pool[ oldLocation ]->adjustPage();
	
	pin_count[ newLocation ] = 0;
	pin_count[ oldLocation ] = 0;
}

void Buffer::spiltPageUpOne ( int oldLocation, int newLocationOne, int newLocationTwo, int hashKey, int localDepth ){
	
	int slotSize = pool[ oldLocation ]->getSlotNumber();
	char* str;
	int key;
	int temp;
	int counter = 0;

	pool[ newLocationOne ]->clearPage( localDepth + 1 );
	pool[ newLocationTwo ]->clearPage( localDepth + 1 );
	pool[ oldLocation ]->setLocalDepth( localDepth + 1 );

	

	for ( int i = 0; i < slotSize; i ++ ){
		str = pool[ oldLocation ]->getRecord( i );
		key = getKey( str );
		temp = hashFromUpOne( key, localDepth + 1 );

		if ( temp == hashKey ) ;
		else if ( temp == ( hashKey << 1 ) )
		{
			insertData(str,newLocationOne );
			pool[ oldLocation ]->setRecordLength( i, -1 );
		}
		else{
			insertData( str, newLocationTwo );
			pool[ oldLocation ]->setRecordLength( i, -1 );
		}

	}

	pool[ oldLocation ]->adjustPage();

	
	pin_count[ newLocationOne ] = 0;
	pin_count[ newLocationTwo ] = 0;
	pin_count[ oldLocation ] = 0;

}

void Buffer::spiltPageUpTwo ( int oldLocation, int newLocation, int hashKey, int localDepth )
{
	int slotSize = pool[ oldLocation ]->getSlotNumber();
	char* str;
	int key;

	pool[ newLocation ]->clearPage( localDepth + 1 );
	pool[ oldLocation ]->setLocalDepth( localDepth + 1 );

	for ( int i = 0; i < slotSize; i ++ ){
		str = pool[ oldLocation ]->getRecord( i );
		key = getKey( str );

		if ( hashFromUpTwo( key, localDepth + 1 ) == hashKey );
		else{
			insertData(str,newLocation);
			pool[ oldLocation ]->setRecordLength( i, -1 );
		}

	}
	pool[ oldLocation ]->adjustPage();
	
	pin_count[ newLocation ] = 0;
	pin_count[ oldLocation ] = 0;

}





Page* Buffer::getPage( int f_offset ){
	return pool[ f_offset ];
}

int Buffer::getLocalDepth ( int location ){
	return pool[ location ]->getLocalDepth();
}

void Buffer::setLocalDepth ( int location, int localDepth ){
	pool[ location ]->setLocalDepth( localDepth );
	dirty[ location ] = 1;
}