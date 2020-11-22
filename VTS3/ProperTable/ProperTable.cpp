// ProperTable.cpp Format a comma-delimited C struct into columns.
//
// Primarily intended for use on the VTS properfy tables, but perhaps useful elsewhere
//
// Written by John Hartman
//
// Yes, I know YOU could do this in 4 lines of Python or a dazzling line of AWK

#include "stdafx.h"
#include "ctype.h"
#include "string.h"
#include "stdlib.h"

void EatWhite( const char *&pBuf )
{
   while ((*pBuf == ' ') || (*pBuf == '\t'))
   {
      pBuf += 1;
   }
}

// Parse out a token until comma, right bracket, or end of line
// Return true if not at end of line
bool GetToken( char *pToken, const char *&pBuf )
{
   bool retval = false;
   EatWhite(pBuf);
   if (*pBuf != 0)
   {
      // Find the length of the token until comma, }, or end of string
      int len = strcspn( pBuf, ",}" );

      // Remove any trailing space on the token
      int ix = len;
      while ((ix > 0) && ((pBuf[ix-1] == ' ') || (pBuf[ix-1] == '\t')))
      {
         ix -= 1;
      }

      if (ix > 0)
      {
         // Copy the token
         strncpy( pToken, pBuf, ix );
      }
      pToken[ix] = 0;

      pBuf += len;
      retval = (*pBuf == ',');
      if (retval)
      {
         pBuf += 1;
         EatWhite(pBuf);
      }
   }

   return retval;
}

int _tmain(int argc, _TCHAR* argv[])
{
   if (argc < 3)
   {
      printf( "ProperTable 1.0: output a column-formatted version of the VTS property table\n"
              "usage:\n"
              "  ProperTable infile outfile {width1 width2 width3...}\n"
              "where the \"widths\" specify the minimum width of each column.\n"
              "Lines not of the format { item, item, ... item } will be passed unchanged,\n"
              "as will lines with fewer items than the number of \"widths\" specified.\n"
            );
      return 1;
   }

   FILE *pInput = fopen( argv[1], "r" );
   if (pInput == NULL)
   {
      fprintf( stderr, "Cannot open input file %s\n", argv[1] );
      return 2;
   }

   FILE *pOutput = fopen( argv[2], "w" );
   if (pOutput == NULL)
   {
      fprintf( stderr, "Cannot create output file %s\n", argv[2] );
      return 3;
   }

   // Array of column widths.  Default or as specified on command line
   int ix;
   int colpos[20];
   int maxWidth[20];
   for (ix = 0; ix < 20; ix++)
   {
      colpos[ix] = 5 + 20*ix;
      maxWidth[ix] = 0;
   }

   int colCount = 0;
   int col = 5;
   for (ix = 3; ix < argc; ix++)
   {
      colpos[colCount++] = col;
      col += atoi( argv[ix] );
   }

   char buffer[ 1000 ];
   char token[20][ 1000 ];
   while (fgets( buffer, sizeof(buffer), pInput ) != NULL)
   {
      bool formatIt = true;
      int tokenCount = 0;
      const char *pBuf = buffer;
      EatWhite(pBuf);
      formatIt = (*pBuf == '{');
      if (formatIt)
      {
         pBuf += 1;
         while ((tokenCount <= colCount) && GetToken( token[tokenCount++], pBuf ))
         {
         }

         formatIt = (tokenCount >= colCount);
      }

      if (!formatIt)
      {
         // Not a special line.  Just dump it
         fprintf( pOutput, "%s", buffer );
      }
      else
      {
         // Special line.  Dump it nice
         col = fprintf( pOutput, "   { " );
         for (ix = 0; ix < tokenCount; ix++)
         {
            // Track the widest in each column
            int len = strlen( token[ix] );
            if (len > maxWidth[ix])
            {
               maxWidth[ix] = len;
            }

            while (col < colpos[ix])
            {
               col += fprintf( pOutput, " " );
            }

            col += fprintf( pOutput, (ix < tokenCount-1) ? "%s, " : "%s", token[ix] );
         }
         fprintf( pOutput, " %s", pBuf );
      }
   }

   printf( "Maximum column widths\n" );
   for (ix = 0; ix < 20; ix++)
   {
      printf( "%2d) %3d\n", ix, maxWidth[ix] );
   }

   fclose( pInput );
   fclose( pOutput );
   return 0;
}

