/****************************************************************************
 ************                                                    ************
 ************	                   m43_ex1.c                     ************
 ************                                                    ************
 ****************************************************************************
 *
 *       Author: ds
 *        $Date: 2013/07/11 14:32:36 $
 *    $Revision: 1.7 $
 *
 *  Description: Simple example program for the M43 MDIS driver
 *
 *     Required: -
 *     Switches: NO_MAIN_FUNC
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 1998-2019, MEN Mikro Elektronik GmbH
 ****************************************************************************/
/*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

static char *RCSid="$Id: m43_ex1.c,v 1.7 2013/07/11 14:32:36 gv Exp $";

#include <stdio.h>
#include <string.h>
#include <MEN/men_typs.h>
#include <MEN/usr_oss.h>
#include <MEN/mdis_api.h>
#include <MEN/mdis_err.h>

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
static int M43_Ex1( char *devName );
static void ShowError( void );


/******************************** main **************************************
 *
 *  Description:  main() - function
 *
 *---------------------------------------------------------------------------
 *  Input......:  argc		number of arguments
 *				  *argv		pointer to arguments
 *				  argv[1]	device name
 *
 *  Output.....:  return	0	if no error
 *							1	if error
 *
 *  Globals....:  -
 ****************************************************************************/
int main( int argc, char *argv[ ] )
{
	if( argc < 2){
		printf("usage: m43_ex1 <device name>\n");
		return 1;
	}
	M43_Ex1(argv[1]);
	return 0;
}


/******************************* M43_Ex1 ************************************
 *
 *  Description:  - open the device
 *				  - configure the device
 *				  - read/write operations
 *                - produce an error
 *				  - close the device
 *
 *---------------------------------------------------------------------------
 *  Input......:  DeviceName
 *
 *  Output.....:  return	0	if no error
 *							1	if error
 *
 *  Globals....:  -
 ****************************************************************************/
int M43_Ex1( char *devName )
{
	MDIS_PATH devHdl;		/* device handle */
	int32     value;
	int32     n;
	int32     byteCount;
	int32     data;
	u_int8    buf[8] = {1,0,1,0,1,0,1,0};


    printf("\n");
	printf("m43_simp - simple example program for the M43 module\n");
    printf("====================================================\n\n");

    printf("%s\n\n", RCSid);

    /*----------------------+
    | open the device       |
    +-----------------------*/
	if( (devHdl = M_open(devName)) < 0 ) goto CLEANUP;
	printf("Device %s opened\n\n", devName);

    /*----------------------+
    | get device properties |
    +-----------------------*/
	printf("Device properties:\n");
	printf("------------------\n");
	/* number of channels */
	if ( M_getstat(devHdl,M_LL_CH_NUMBER, &data) < 0 ) goto CLEANUP;
	printf(" number of channels:      %d\n", (int) data);
	/* channel length */
	if ( M_getstat(devHdl,M_LL_CH_LEN, &data) < 0 ) goto CLEANUP;
	printf(" channel length:          %dBit\n", (int) data);
	/* channel direction */
	if ( M_getstat(devHdl,M_LL_CH_DIR, &data) < 0 ) goto CLEANUP;
	printf(" channel direction type:  %d\n", (int) data);
	/* channel type */
	if ( M_getstat(devHdl,M_LL_CH_TYP, &data) < 0 ) goto CLEANUP;
	printf(" channel type:            %d\n\n", (int) data);

    /*----------------------+
    | device configuration  |
    +-----------------------*/
	/* set current channel to 0 */
	if ( M_setstat(devHdl,M_MK_CH_CURRENT, 0) < 0 ) goto CLEANUP;
	/* set auto-increment mode */
	if ( M_setstat(devHdl,M_MK_IO_MODE,M_IO_EXEC_INC) < 0 ) goto CLEANUP;

	/*----------------------+
    | write operation       |
    +-----------------------*/
	/* set channel (and increment) */
	printf("Set channels 0..7:     ");
	value = 1;
	for (n=0; n<=7; n++)
	{
		if ( M_write(devHdl,value) < 0 ) goto CLEANUP;
		printf("%d ",(int) value);
		fflush(stdout);
		UOS_Delay( 200 );	/* delay */
	}

    /*----------------------+
    | device configuration  |
    +-----------------------*/
	/* set non auto-increment mode */
	if ( M_setstat(devHdl,M_MK_IO_MODE,M_IO_EXEC) < 0 ) goto CLEANUP;

	/*----------------------+
    | write operation       |
    +-----------------------*/
	/* clear/delay/set one channel */
	printf("\n\nToggle (clear/delay/set) channel 0..7\n");
	for(n=0; n<=7; n++)
	{
		if( M_setstat(devHdl,M_MK_CH_CURRENT,n) < 0 ) goto CLEANUP;
		if( M_write(devHdl,0) < 0 ) goto CLEANUP;
		UOS_Delay( 200 );	/* delay */
		if( M_write(devHdl,1) < 0 ) goto CLEANUP;
	}

	/*----------------------+
    | setblock operation    |
    +-----------------------*/
	/* set all channels */
	printf("\nSet channels 0..7:     1 0 1 0 1 0 1 0\n");
	byteCount = M_setblock(devHdl,buf,8);
	if( byteCount < 0 ) goto CLEANUP;
	printf("=> M_setblock: %d bytes set\n", (int) byteCount);
	UOS_Delay( 200 );	/* delay */

	/*----------------------+
    | getblock operation    |
    +-----------------------*/
	/* get all channels */
	byteCount = M_getblock(devHdl,buf,8);
	if( byteCount < 0 ) goto CLEANUP;

	printf("\nGet channels 0..7:     ");
	for (n=0; n<=7; n++)
		printf("%d ",buf[n]);

	printf("\n=> M_getblock: %d bytes got\n", (int) byteCount);

    /*----------------------+
    | device configuration  |
    +-----------------------*/
	/* set auto-increment mode */
	if( M_setstat(devHdl,M_MK_IO_MODE,M_IO_EXEC_INC) < 0 ) goto CLEANUP;

	/*----------------------+
    | write operation       |
    +-----------------------*/
	/* clear channel (and increment) */
	printf("\nClear channels 0..7:   ");
	value = 0;
	for (n=0; n<=7; n++)
	{
		if (M_write(devHdl,value) < 0 ) goto CLEANUP;
		printf("%d ",(int) value);
		fflush(stdout);
		UOS_Delay( 200 );	/* delay */
	}
	printf("\n");

	/*----------------------+
    | produce an error      |
    +-----------------------*/
    printf("\n");
	printf("Produce an error:\n");
	printf("-----------------\n");
	if ( M_setstat(devHdl,M_LL_NOTEXIST, 0) < 0 ) ShowError();

    /*----------------------+
    | close the device      |
    +-----------------------*/
    if( M_close(devHdl) ) goto CLEANUP;
	printf("\nDevice %s closed\n", devName);

    printf("=> OK\n");
    return 0;


CLEANUP:
    ShowError();
    printf("=> Error\n");
    return 1;
}

/******************************** ShowError *********************************
 *
 *  Description:  Show MDIS or OS error message.
 *
 *---------------------------------------------------------------------------
 *  Input......:  -
 *
 *  Output.....:  -
 *
 *  Globals....:  -
 ****************************************************************************/
static void ShowError( void )
{
   u_int32 error;

   error = UOS_ErrnoGet();

   printf("*** %s ***\n",M_errstring( error ) );
}


