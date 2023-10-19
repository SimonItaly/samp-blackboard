
/*
 *    Writable SA:MP Blackboards
 *	  (c) Simon, December 2014
 *
 *	  The contents of this file are subject to the Mozilla Public License
 *	  Version 1.1 (the "License"); you may not use this file except in
 *	  compliance with the License. You may obtain a copy of the License at
 *	  http://www.mozilla.org/MPL/
 *
 *	  Software distributed under the License is distributed on an "AS IS"
 *	  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 *	  License for the specific language governing rights and limitations
 *	  under the License.
 *
 *    The Original Code is the Writable SA:MP Blackboards include.
 *
 */

//------------------------------------------------------------------------------

#if defined _blackboard_included
	#endinput
#endif
#define _blackboard_included

#if !defined STREAMER_TYPE_OBJECT
	#error blackboard.inc require Incognito's Streamer plugin
#endif

//------------------------------------------------------------------------------

#if !defined SetInvisibleTexture
	#define SetInvisibleTexture(%0,%1) \
			SetDynamicObjectMaterial(%0, %1, 19341, "none", "none", 0x00FFFFFF)
#endif

#if !defined SetObjectColour
	#define SetObjectColour(%0,%1,%2) \
			SetDynamicObjectMaterial(%0, %1, 18646, "matcolours", "white", %2)
#endif

#if !defined strcpy
	#define strcpy(%0,%1,%2) \
			strcat((%0[0] = '\0', %0), %1, %2)
#endif

//------------------------------------------------------------------------------

#define MAX_BOARDS                           	10
#define MAX_BOARD_LINES                         17
#define MAX_BOARD_LINE_LEN                      64      //Real: 29 + newline

enum E_BLACKBOARD
{
	boardArea,
	boardObject,
	boardText
}
static stock
	Writable_BlackBoard[MAX_BOARDS][E_BLACKBOARD],
	Iterator:BlackBoard<MAX_BOARDS>;

/*
#define Writable_BlackBoard[%0][boardLine] \
        _boardLine[%0]
static stock
	_boardLine[MAX_BOARDS][MAX_BOARD_LINES][MAX_BOARD_LINE_LEN];
*/

static
	nearBoard[MAX_PLAYERS];

forward OnPlayerEnterBlackboard(playerid, boardid);
forward OnPlayerExitBlackboard(playerid, boardid);

static _GetXYInDirection(& Float: X, & Float: Y, Float: Angle, Float: distance = 1.0)
{
    X -= (floatsin(Angle, degrees) * distance);
    Y += (floatcos(Angle, degrees) * distance);
}

//******************************************************************************

/*
native CreateBlackboard(Float:x, Float:y, Float:z, Float:rx = 0.0, Float:ry = 0.0, Float:rz = 0.0, worldid = -1, interiorid = -1);
*/
stock CreateBlackboard(Float:x, Float:y, Float:z, Float:rx = 0.0, Float:ry = 0.0, Float:rz = 0.0, worldid = -1, interiorid = -1)
{
	new idx = Iter_Free(BlackBoard);
	if(idx != -1)
	{
	    Iter_Add(BlackBoard, idx);

        Writable_BlackBoard[idx][boardObject] = CreateDynamicObject(2616, x, y, z, rx, ry, rz, worldid, interiorid); //object(police_nb04) (1)
		SetObjectColour(Writable_BlackBoard[idx][boardObject], 1, 0xFF333333);
		SetInvisibleTexture(Writable_BlackBoard[idx][boardObject], 2);

        _GetXYInDirection(x, y, rz, 0.02625);
		//x + 0.02625, y - 0.12687
		
		Writable_BlackBoard[idx][boardText] = CreateDynamicObject(19327, x, y, z + 0.07144, rx, ry, rz, worldid, interiorid);
		SetInvisibleTexture(Writable_BlackBoard[idx][boardText], 0);

        Writable_BlackBoard[idx][boardArea] = CreateDynamicSphere(x, y, z, 2.0, worldid, interiorid);
	}
	return idx;
}

/*
native DestroyBlackboard(boardid);
*/
stock DestroyBlackboard(boardid)
{
	if(Iter_Contains(BlackBoard, boardid))
	{
	    for(new idx = 0; idx < MAX_BOARD_LINES; idx++)
		{
		    Writable_BlackBoard[boardid][boardLine][idx] = '\0';
		}

	    DestroyDynamicObject(Writable_BlackBoard[boardid][boardText]);
	    DestroyDynamicArea(Writable_BlackBoard[boardid][boardArea]);

	    Iter_Remove(Terminal, boardid);
	    return 1;
	}
	return 0;
}

/*
native ClearBlackboard(boardid);
*/
stock ClearBlackboard(boardid)
{
	for(new idx = 0; idx < MAX_BOARD_LINES; idx++)
	{
	    Writable_BlackBoard[boardid][boardLine][idx] = '\0';
	}
	SetDynamicObjectMaterialText(Writable_BlackBoard[boardid][boardText], 0, text, OBJECT_MATERIAL_SIZE_512x256, "Comic Sans MS",
								.fontsize = 30, .bold = 1, .fontcolor = 0xCCFFFFFF,
								.textalignment = 0);
}

//******************************************************************************

hook OnPlayerEnterDynArea(playerid, areaid)
{
    foreach(new boardid : BlackBoard)
    {
	    if(areaid == Writable_BlackBoard[boardid][boardArea])
	    {
			CallLocalFunction("OnPlayerEnterBlackboard", "ii", playerid, boardid);

	        nearBoard[playerid] = boardid;
	        
	        return 1;
	    }
	}

    return 1;
}

//------------------------------------------------------------------------------

hook OnPlayerLeaveDynArea(playerid, areaid)
{
	if(nearBoard[playerid] != -1)
	{
	    if(areaid == Writable_BlackBoard[ nearBoard[playerid] ][boardArea])
	    {
			CallLocalFunction("OnPlayerExitBlackboard", "ii", playerid, nearBoard[playerid]);

    		nearBoard[playerid] = -1;
			
    		return 1;
		}
    }

    return 1;
}

//------------------------------------------------------------------------------

hook OnPlayerConnect(playerid)
{
    nearBoard[playerid] = -1;

    return 1;
}

//------------------------------------------------------------------------------

hook OnPlayerText(playerid, text[])
{
	new boardid = nearBoard[playerid];
	
	if(boardid != -1)
	{
		new string[256];
		
	    //Pipe
	    if(text[0] == '|')
	    {
	        text[0] = ' ';
	        
	        new
		    materialsize,
		    fontface[1],
		    fontsize,
		    bold,
		    fontcolor,
		    backcolor,
		    textalignment;

		    GetDynamicObjectMaterialText(Writable_BlackBoard[boardid][boardText], 0, string, materialsize, fontface, fontsize, bold, fontcolor, backcolor, textalignment);
		    strcat(string, "  ");
			strcat(string, text);
		}
	    else
		{
		    string[0] = 0;
		    strcat(string, "   ");
	    	strcat(string, text);
	    }
	    
	    for(new i = 0, j = strlen(string), count; i < j; i++)
	    {
	        count++;
	        
	        if(count > 30 || string[i] == '|')
	        {
	            string[i] = '\n';
	            strins(string, "   ", i+1);
	            j = strlen(string);
	            count = 0;
	        }
		}
		
		SetDynamicObjectMaterialText(Writable_BlackBoard[boardid][boardText], 0, string, OBJECT_MATERIAL_SIZE_512x256, "Comic Sans MS",
											.fontsize = 30, .bold = 1, .fontcolor = 0xCCFFFFFF,
											.textalignment = 0);
		return ~0;
	}

	return 1;
}
