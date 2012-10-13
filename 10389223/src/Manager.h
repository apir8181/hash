#ifndef MANAGER_H_INCLUDED
#define MANAGER_H_INCLUDED

#include "Buffer.h"
#include "Catalog.h"
#include "Page.h"
#include "Func.h"
#include <stdio.h>
#include <fstream>

class Manager{

private:

	FILE *inStream;  //file input,lineitem.tbl
	
	FILE *testInStream;		//test data input file, "testinput.in"
	FILE *testOutStream;		//test data output file, "testoutput.out"


	Buffer *buffer;		//buffer
    Catalog *catalog;	//directory
	int pidCount;		//next new page Id


public:

	Manager(char *inChar, char *outChar, char *testInChar, char *testOutChar, char* indexChar);	//constructor
    void insertData();			//insert the data of instream
	void saveBuffer();			//save the content in the buffer
	void insertLow( char*str );	//insert through hash from low
	void insertUpOne ( char*str );	//insert through hash from high of type one
	void insertUpTwo ( char*str );		//insert through hash from high of type two
	void saveIndex();			//save the directory into space 
	void query();				//query the result
};



#endif // MANAGER_H_INCLUDED
