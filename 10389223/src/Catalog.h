#ifndef CATALOG_H_INCLUDED
#define CATALOG_H_INCLUDED

#pragma once
#include <string>
#include <stdio.h>
#define MAXSIZE 2048

using namespace std;

/*class:Catalog
*it occupied a page in memory, it is been used as directory
*/

class Catalog{

private:
	int size;			//the total size ,including data in the disk, of catalog
	int globalDepth;	//globalDepth of catalog
	FILE *indexStream;	//directory file in disk
	int pageId[ MAXSIZE ];	//directory in the memory
	int start;				//the start of the block directory data in memory
	int dirty;				//to record it is modified or not, 1 for yes
public:

	Catalog( int totalSize, char *indexChar );	//create a catalog with size of totalSize

	void doubleCatalogLow();	//double the catalog under the rule of hash from low

	void doubleCatalogUpTwo();	//double the catalog under the rule of hash from up of type one

	void doubleCatalogUpOne();	//double the catalog under the rule of hash from up of type two

	void saveCatalog();			//save the catalog in memory into disk

	int getPageId ( int hashKey );		//get PageId with index of hashKey

	void setPageId ( int hashKey, int number );	//set hashKey index into number

	int getGlobalDepth ();	//get the globalDepth

};

#endif



