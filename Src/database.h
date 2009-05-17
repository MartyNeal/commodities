/*
 * header file database.h
 *
 * Database containing the type, tick size and tick value for each commodity
 *
 * Becky Engley and Martin Neal
 * May 2009
 *
 */

#include "tradeSystem.h"

#define NUMCOMMODS 14

//data is:
//name
//type
//base year
//number of years (only up to 2004)
//tick value
//tick size

commod comDatabase[NUMCOMMODS] = {  {"Jun_CL", LONG, 1983, 21, 10, 0.01},
                                    {"May_HO", LONG, 1979, 26, 4.2, 0.0001},
                                    {"Jun_LH", LONG, 1967, 38, 10, 0.025},
                                    {"Jun_HU", LONG, 1985, 20, 4.2, 0.0001},
                                    {"Aug_PB", SHORT, 1962, 43, 10, 0.025},
                                    {"Sep_OJ", SHORT, 1986, 37, 7.5, 0.05},
                                    {"Dec_C", SHORT, 1949, 56, 12.5, .25},
                                    {"Dec_LH", SHORT, 1966, 39, 10, 0.025},
                                    {"Dec_KW", SHORT, 1966, 39, 12.5, 0.25},
                                    {"Nov_HO", LONG, 1979, 26, 4.2, 0.0001},
                                    {"Nov_CL", LONG, 1983, 22, 10, 0.01},
                                    {"Oct_LH", LONG, 1966, 39, 10, 0.025},
                                    {"Feb_CL", SHORT, 1984, 21, 10, 0.01},
                                    {"Apr_LC", LONG, 1965, 40, 10, 0.025} };


