#pragma once

/****************************************************************
 * file   indexio.h
 * author Tian Xia (tian.xia.ug@dartmouth.edu)
 * date   October 31, 2021
 * 
 * Module that saved and loads indexes : indexsave() and 
 * indexload() saves and load an index to a named file indexnm. 
 * The index file contains one line for each word in the index. 
 * Each line has the format: 
 *
 * <word> <docID1> <count1> <docID2> <count2>..<docIDN> <countN> 
 *
 * where <word> is a string of lowercase letters, <docIDi> is a 
 * positive integer designating a document, <counti> is a 
 * positive integer designating the number of occurrences of 
 * <word> in <docIDi>; each entry should be placed on the line 
 * separated by a space. 
 * 
****************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include "webpage.h"
#include "hash.h"
#include "queue.h"

// Loads index from file
hashtable_t* indexload(char* dirname, char* indexnm);

// Saves index to file
int32_t indexsave(hashtable_t* htp, char* dirname, char* indexnm);
