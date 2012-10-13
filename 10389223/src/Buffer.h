#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED
#include "Page.h"


/*calss:Buffer 
 *To imitate the limited size of memory
 *Incluing function of reading pages from disk and writting pages into the disk
 */


class Buffer{

private:

	Page** pool;		    //buffer pool page
	int* pageId;			//pageId of the frame   

	bool* ref_bit;			
	bool* pin_count;		//exclusive lock


	FILE *outStream;		//result.fil
	int currentPageId;		//the latest page to be seeked 

	bool* dirty;			//pool pages' dirty
	int size;				//the size of the pool
		
	int current;			//where the chose by clock start from 
	
	
public:

	int iotime;				//to check I/O from the disk to analyse

	/*constructor, construct a buffer with size pages*/
	Buffer(int size, char*outChar );		

	/*return the number of pages in the buffer*/
	int getSize();			


	/*insert str into frame f_offset
	*if succeed, return 1, else 0
	*/
	bool insertData(char* str,int f_offset);	

					
	/*if the page requested to be loaded exists in the buffer
	*return the offset of the frame if exist;else,return -1 */
	int ifPageExist(int pageNum);

					
	 /*if the frame has been changed
	 *return 1 for Yes , 0 for No*/
	bool ifChange(int frameNum);
				
	/*set frame frameNum to be modifief*/
	void setDirty ( int frameNum );		
	
	/*lock the frame frameNum*/
	void setPinCount ( int frameNum );	

	/*release the frame frameNum*/
	void clearPinCount ( int frameNum );	

	/*adjust frame frameNum in the pool*/
	void adjustPage ( int frameNum );		

	/*reset the content in frame frameNum
	 *and set the frame frameNum to have localDepth of localDepth*/
	void clearPage ( int frameNum, int localDepth );	
											
	/*return the offset in the buffer to be swapped*/
	int choseByClock();
	           
	/*set frame frameNum with pageId pId*/
	void setPageId (  int frameNum, int pId );

	/*return frame frameNum pageId*/
    int getPageId ( int frameNum );

	/*save the frame frameNum into disk*/
	void saveOutPage ( int frameNum );


	/*spilt frame oldLocation into newLocation with the least hashKey
	 *and localDepth hash function supported by hashFromLow*/
	void spiltPageLow ( int oldLocation, int newLocation, int hashKey, int localDepth );

	/*spilt frame oldLocation into newLocationOne and 
	 *newLocationTwo with the least hashKey and  localDepth
	 *hash function supported by hashFromUpOne*/
	void spiltPageUpOne ( int oldLocation, int newLocationOne, int newLocationTwo, int hashKey, int localDepth );


	/*spilt frame oldLocation into newLocation with the least hashKey
	 *and localDepth hash function supported by hashFromUpTwo*/
	void spiltPageUpTwo( int oldLocation, int newLocation, int hashKey, int localDepth );


	/*read out page with pageId in the disk into frame location*/
	void readOutPage ( int frameNum, int pageId );


	/*return frame frameNum in the pool*/
	Page* getPage( int frameNum );


	/*return localDepth of frame frameNum*/
	int getLocalDepth ( int frameNum );


	/*set localDepth of frame frameNum to be localDepth*/
	void setLocalDepth ( int location , int localDepth );
};



/*
               request for a page                                                                     release a page
		               |                                                                                    |
	         if_pageExist(int pageNum)                                                            if_pageExist(int pageNum)
	            /               \                                                                    /               \
	        Y  /                 \ N                                                             N  /                 \ Y
(return frame offset)       if_freeFrame()                                                       error         change_pin_count(--)
change_pin_count(++)        /            \                                                                        pin_count==0?
                        N  /              \ Y                                                                      /         \
			    chose_by_clock() ----> (return frame offset)                                                    Y /           \ N
				                       if_change(int frameNum)                                   if_change(int frameNum)      return
									     /                 \                                         /            \
									  N /                   \ Y                                   N /              \ Y
		              readOutPage (int offset) <----- saveOutPage (Page* p, int offset)       release() <----- saveOutPage (Page* p, int offset)
					     pin_count[offset]=1





		 */

#endif // BUFFER_H_INCLUDED
