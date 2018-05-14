/* fat12ls.c 
* 
*  Displays the files in the root sector of an MSDOS floppy disk
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#define SIZE 32      /* size of the read buffer */
#define ROOTSIZE 256 /* max size of the root directory */
//define PRINT_HEX   // un-comment this to print the values in hex for debugging

struct BootSector
{
    unsigned char  sName[9];            // The name of the volume
    unsigned short iBytesSector;        // Bytes per Sector
    
    unsigned char  iSectorsCluster;     // Sectors per Cluster
    unsigned short iReservedSectors;    // Reserved sectors
    unsigned char  iNumberFATs;         // Number of FATs
    
    unsigned short iRootEntries;        // Number of Root Directory entries
    unsigned short iLogicalSectors;     // Number of logical sectors
    unsigned char  xMediumDescriptor;   // Medium descriptor
    
    unsigned short iSectorsFAT;         // Sectors per FAT
    unsigned short iSectorsTrack;       // Sectors per Track
    unsigned short iHeads;              // Number of heads
    
    unsigned short iHiddenSectors;      // Number of hidden sectors
};

void parseDirectory(int iDirOff, int iEntries, unsigned char buffer[]);
//  Pre: Calculated directory offset and number of directory entries
// Post: Prints the filename, time, date, attributes and size of each entry

unsigned short endianSwap(unsigned char one, unsigned char two);
//  Pre: Two initialized characters
// Post: The characters are swapped (two, one) and returned as a short

void decodeBootSector(struct BootSector * pBootS, unsigned char buffer[]);
//  Pre: An initialized BootSector struct and a pointer to an array
//       of characters read from a BootSector
// Post: The BootSector struct is filled out from the buffer data

char * toDOSName(char string[], unsigned char buffer[], int offset);
//  Pre: String is initialized with at least 12 characters, buffer contains
//       the directory array, offset points to the location of the filename
// Post: fills and returns a string containing the filename in 8.3 format

char * parseAttributes(char string[], unsigned char key);
//  Pre: String is initialized with at least five characters, key contains
//       the byte containing the attribue from the directory buffer
// Post: fills the string with the attributes

char * parseTime(char string[], unsigned short usTime);
//  Pre: string is initialzied for at least 9 characters, usTime contains
//       the 16 bits used to store time
// Post: string contains the formatted time

char * parseDate(char string[], unsigned short usDate);
//  Pre: string is initialized for at least 13 characters, usDate contains
//       the 16 bits used to store the date
// Post: string contains the formatted date

int roundup512(int number);
// Pre: initialized integer
// Post: number rounded up to next increment of 512


// reads the boot sector and root directory
int main(int argc, char * argv[])
{
    int pBootSector = 0;
    unsigned char buffer[SIZE];
    unsigned char rootBuffer[ROOTSIZE * 32];
    struct BootSector sector;
    int iRDOffset = 0;
    
    // Check for argument
    if (argc < 2) {
    	printf("Specify boot sector\n");
    	exit(1);
    }
    
    // Open the file and read the boot sector
    pBootSector = open(argv[1], O_RDONLY);
    read(pBootSector, buffer, SIZE);
    
    // Decode the boot Sector
    decodeBootSector(&sector, buffer);
    
    // Calculate the location of the root directory
    iRDOffset = (1 + (sector.iSectorsFAT * sector.iNumberFATs) ) * sector.iBytesSector;
    
    // Read the root directory into buffer
    lseek(pBootSector, iRDOffset, SEEK_SET);
    read(pBootSector, rootBuffer, ROOTSIZE);
    close(pBootSector);
    
    // Parse the root directory
    parseDirectory(iRDOffset, sector.iRootEntries, rootBuffer);
    
} // end main


// Converts two characters to an unsigned short with two, one
unsigned short endianSwap(unsigned char one, unsigned char two)
{
     unsigned short shift = (two << 8) | one;
     return shift;
}


// Fills out the BootSector Struct from the buffer
void decodeBootSector(struct BootSector * pBootS, unsigned char buffer[])
{
     int i = 3;  // Skip the first 3 bytes
     int sNameLength = 8;
     int shortLength = 2;

     // Pull the name and put it in the struct (remember to null-terminate)
     memcpy(pBootS->sName, &buffer[i], sNameLength);
     pBootS->sName[sNameLength] = '\0';
     i += sNameLength;
     
     // Read bytes/sector and convert to big endian
     pBootS->iBytesSector = endianSwap(buffer[i], buffer[i+1]);
     i += shortLength;

     // Read sectors/cluster, Reserved sectors and Number of Fats
     pBootS->iSectorsCluster = buffer[i];
     i++;
     pBootS->iReservedSectors = endianSwap(buffer[i], buffer[i+1]);
     i += shortLength;
     pBootS->iNumberFATs = buffer[i];
     i++;

     // Read root entries, logicical sectors and medium descriptor
     pBootS->iRootEntries = endianSwap(buffer[i], buffer[i+1]);
     i += shortLength;
     pBootS->iLogicalSectors = endianSwap(buffer[i], buffer[i+1]);
     i += shortLength;
     pBootS->xMediumDescriptor = buffer[i];
     i++;
     
     // Read and covert sectors/fat, sectors/track, and number of heads
     pBootS->iSectorsFAT = endianSwap(buffer[i], buffer[i+1]);
     i += shortLength;
     pBootS->iSectorsTrack = endianSwap(buffer[i], buffer[i+1]);
     i += shortLength;
     pBootS->iHeads = endianSwap(buffer[i], buffer[i+1]);
     i += shortLength;
     
     // Read hidden sectors
     pBootS->iHiddenSectors = endianSwap(buffer[i], buffer[i+1]);
     
     return;
}


// iterates through the directory to display filename, time, date,
// attributes and size of each directory entry to the console
void parseDirectory(int iDirOff, int iEntries, unsigned char buffer[])
{
    int i = 0;
    char string[13];
    
    // Display table header with labels
    printf("Filename\tAttrib\t\tTime\t\tDate\t\tSize\n");
    
    // loop through directory entries to print information for each
    for(i = 0; i < (iEntries); i = i + /* entry width */ 32)   {

        // buffer begins with 0x00 or 0xE5
    	if (buffer[i] != 0 && buffer[i] != 229) {
    		// Display filename
    		printf("%-16s", toDOSName(string, buffer, /* name offset */ i)  );
    		// Display Attributes
    		printf("%-8s", parseAttributes(string, /* attr offset */ (buffer + i)[10])  );
    		// Display Time
    		printf("%16s", parseTime(string, /*time offsets */  ((buffer + i)[22] << 8) | (buffer + i)[23])  );
    		// Display Date
    		printf("%13s", parseDate(string, /* date offsets */ ((buffer + i)[24] << 8) | (buffer + i)[25])  );
    		// Display Size
    		int size = (((buffer + i)[28] << 3) | ((buffer + i)[29] << 2) | ((buffer + i)[30] << 1) | ((buffer + i)[31]));
    		printf("%15d\n", /* size offsets */ roundup512(size * 32));
    	}
    }
    
    // Display key
    printf("(R)ead Only (H)idden (S)ystem (A)rchive\n");
} // end parseDirectory()

int roundup512(int number)
{
    return ((number / 512) + 1) * 512;
}


// Parses the attributes bits of a file
char * parseAttributes(char string[], unsigned char key)
{

   int i = 0;
   if((key & 1) == 1) 
   {
       string[i] = 'R';
       i++;
   }
   if((key & (1 << 1)) == (1 << 1))
   {
       string[i] = 'H';
       i++;
   }
   if((key & (1 << 2)) == (1 << 2))
   {
       string[i] = 'S';
       i++;
   }
   if((key & (1 << 5)) == (1 << 5))
   {
       string[i] = 'A';
       i++;
   }
   string[i] = '\0';
   return string;
} // end parseAttributes()


// Decodes the bits assigned to the time of each file
char * parseTime(char string[], unsigned short usTime)
{
    unsigned char hour = 0x00, min = 0x00, sec = 0x00;
    
    unsigned short t = endianSwap(usTime >> 8, usTime & 255);

    sec  |= (31  &  t);        // Retrieve first 5 bits for hour
    min  |= (63  & (t >> 5));  // Retrieve middle 6 bits for minute
    hour |= (31  & (t >> 10)); // Retrieve last 5 bits for seconds
    sec  *= 2;                      // Then multiply for compensation
    
    sprintf(string, "%02i:%02i:%02i", hour, min, sec);
    
    return string;
} // end parseTime()


// Decodes the bits assigned to the date of each file
char * parseDate(char string[], unsigned short usDate)
{
    unsigned char month = 0x00, day = 0x00;
    unsigned short year = 0x0000;

    unsigned short t = endianSwap(usDate >> 8, usDate & 255);
    
    day   |= (31  &  t);
    month |= (15  & (t >> 5));
    year  =  (127 & (t >> 9)) + 1980;
    sprintf(string, "%d/%d/%d", year, month, day);
    
    return string;
    
} // end parseDate()


// Formats a filename string as DOS (adds the dot to 8-dot-3)
char * toDOSName(char string[], unsigned char buffer[], int offset)
{
    // Loop to length of string
    int j = 0;
    for(int i = 0; i < 12; i++)
    {
        // Add chars to our fileName 
        if((buffer+offset)[i] != ' ' && i < 8)
        {
            string[j] = (buffer+offset)[i];
            j++;
        }
        // Dot placement
        if(i == 8)
        {
            string[j] = '.';
            j++;
        }
        // Add extension
        if((buffer + offset)[i] != ' ' && i >= 8)
        {
            string[j] = (buffer + offset)[i];
            j++;
        }
    }
    string[j] = '\0';

    return string;
} // end toDosNameRead-Only Bit
