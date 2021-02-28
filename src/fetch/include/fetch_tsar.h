/*
fetch_tsar.h --- Supporting header file for TSAR's serial commands
Author: Seth Rohrbach
Modified: Feb 27 2021
---------------------
Description:



*/

#ifndef FETCH_TSAR_H_
#define FETCH_TSAR_H_

#include <stdbool.h>

//Alias functions here. Should be all this file needs.

//All following functions are defined in fetch_tsar.c
//Basic help command. Sends serial message to be printed on local stdout.
bool fetch_tsar_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);

















#endif
