/*
* Project 1: EXIF file viewer
* Pitt cs449 fall 2016
*
* Program takes in a JPG filename as a paramater and
* prints the files information to the user.
*
* @author Craig Mazzotta
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Structs
struct head {
  unsigned short startOfFile;
  unsigned short app1Marker;
  unsigned short app1BlockLength;
  char exifString[4];
  unsigned short nullZero;
  unsigned char endianness[2];
  unsigned short version42;
  unsigned int exifBlockOffset;
};
struct tiffTag {
  unsigned short tagId;
  unsigned short dataType;
  unsigned int num;
  unsigned int offset; //(or value)
};
//Function declarations
int validate(struct head);
void parseTiff(FILE*);
void parseTiffsSub(FILE*);
void printData(int, int, FILE*);
void getTypeFive(unsigned int*, unsigned int*, int, FILE*);

/*
* Main function of the program. Opens a file and begins
* parsing information in the file.
*
* @param argc Number of arguments passed to run program
* @param argv Pointer to array containing arguments
*/
int main(int argc, char *argv[]){
  if(argc !=2){ //validate correct number of arguments passed
    printf("Usage: ./exifview <FILENAME>.jpg\n");
    return 1;
  }

  FILE *infile;
  struct head exifHead;

  infile = fopen(argv[1],"rb");
  if(infile==NULL){
    printf("Cannot open %s, does not exist", argv[1]);
    return 1;
  }

  if(fread(&exifHead,sizeof(struct head),1,infile)==1){
    if(!validate(exifHead)){
      printf("exiting\n");
      return 1;
    }
    parseTiff(infile); //header was valid, parse tiffs
  } else {
    printf("Failed to read the header from the file");
    return 1;
  }

  fclose(infile);
  return 0;
}

/*
* Verifies if the EXIF header is valid
* @param head The struct containing header info
*/
int validate(struct head head){
  char a[5];
  if(!(head.startOfFile == 0xD8FF)){ //verify file is a jpg
    printf("Error: could not verify file as a JPG\n");
    return 0;
  }
  if(!(head.app1Marker == 0xE1FF)){ //verify APP1 block
    printf("Error: could not verify APP1");
    return 0;
  }
  //strcpy(a, head.exifString);
  //a[4]= '\0'; //add null terminator
  if(!strcmp(head.exifString,"EXIF")){
    printf("Error: EXIF tag not found\n");
    return 0;
  }
  if(!(head.endianness[0]=='I' && head.endianness[1]=='I')){
    printf("Error: endianness is not supported\n");
    return 0;
  }

  return 1; //infile is a valid file
}

/*
* Parses the file count times to read in each TIFF tag
* @param infile The file containing the TIFF tags
*/
void parseTiff(FILE* infile){
  unsigned short count;
  int i;
  struct tiffTag tiff;

  fread(&count, sizeof(short),1,infile); //get count from byte 20-21
  for(i=0; i<count; i++){
    fread(&tiff, sizeof(struct tiffTag),1,infile);
    if(tiff.tagId == 0x010F){ //Manufacturer
      printf("%-16s", "Manufacturer: ");
      printData(tiff.num, tiff.offset, infile);
      printf("\n");
    }else if(tiff.tagId == 0x0110){ //Camera Model
      printf("%-16s", "Model: ");
      printData(tiff.num, tiff.offset, infile);
      printf("\n");
    }else if(tiff.tagId == 0x8769){ //EXIF sub block address
      fseek(infile, (tiff.offset + 12), SEEK_SET);
      parseTiffsSub(infile);
      break;
    }
  }
}

/*
* Parse additional Exif sub block
* @param infile The file containing the TIFF tags
*/
void parseTiffsSub(FILE* infile){
  unsigned short count;
  unsigned int i, a, b;
  struct tiffTag tiff;

  fread(&count, sizeof(short), 1,infile);

  for(i=0; i<count; i++){
    fread(&tiff, sizeof(struct tiffTag), 1, infile);

    if(tiff.tagId == 0xA002){
      printf("%-16s", "Width: ");
      printf("%d", tiff.offset); //offset is data value
      printf(" pixels\n");
    }
    else if(tiff.tagId == 0xA003){
      printf("%-16s", "Height: ");
      printf("%d", tiff.offset); //offset is data value
      printf(" pixels\n");
    }
    else if(tiff.tagId == 0x8827){
      printf("%-16s", "ISO: ");
      printf("ISO %d", tiff.offset); //offset is data value
      printf("\n");
    }
    else if(tiff.tagId == 0x829a){
      printf("%-16s", "Exposure Time: ");
      getTypeFive(&a,&b, tiff.offset, infile); //assign values to a & b to print ratio
      printf("%d/%d second\n", a, b);
    }
    else if(tiff.tagId == 0x829d){
      printf("%-16s", "F-stop: ");
      getTypeFive(&a,&b, tiff.offset, infile); //assign values to a & b to print ratio
      printf("f/%1.1f\n", (double)a/b);
    }
    else if(tiff.tagId == 0x920a){
      printf("%-16s", "F-stop: ");
      getTypeFive(&a,&b, tiff.offset, infile); //assign values to a & b to print ratio
      printf("%2.0f mm\n", (double)a/b);
    }
    else if(tiff.tagId == 0x9003){
      printf("%-16s", "Date Taken: ");
      printData(tiff.num, tiff.offset, infile); //prints date taken
      printf("\n");
    }
  }
}

/*
* Prints data found at offset from tag
*
* @param n Number of data items from TIFF tag
* @param o Offset of data in the file
* @param infile The file being read from
*/
void printData(int n, int o, FILE* infile){
  long long int fpointer = ftell(infile); //save file pointer
  char* a = malloc(n*sizeof(char));

  if(a == NULL){
    printf("\n Error allocating memory \n");
  }
  o += 12;
  fseek(infile, o, SEEK_SET); //go to file at o+12
  fread(a, sizeof(char), n, infile); //get the data
  printf("%s", a);

  fseek(infile, fpointer, SEEK_SET); //return to previous location in the file
  free(a);
}

/*
* Retrieves data items from specified location in memory
*
* @param a The first int
* @param b The second int
* @param o Location - 12 where data is in file
* @param infile The file being read from
*/
void getTypeFive(unsigned int* a, unsigned int* b, int o, FILE* infile){
  long long int pointer = ftell(infile); //save file pointer
  o += 12;
  fseek(infile, o, SEEK_SET); //go to data location in file
  fread(a, sizeof(int), 1, infile); //read first int and assign to a
  fread(b, sizeof(int), 1, infile); //read second int and assign to b
  fseek(infile, pointer, SEEK_SET); //return to original location in file
}
