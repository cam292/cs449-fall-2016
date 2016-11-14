/*
* Project 2: mystrings
* made for Pitt cs449 fall 2016
* @author Craig Mazzotta
*/
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]){

  char str[100];
  int len = 0;
  char c;
  FILE *file;

  if(argv[1]== NULL){
    printf("Usage: ./mystrings <filename>\n");
    return 0;
  }
  file = fopen(argv[1], "r");
  if (file == NULL){
    printf("The file does not exist\n");
    return 0;
  }

  while(feof(file) == 0) {
    fread(&c, sizeof(c), 1, file);
    if (c > 31 && c < 127){
      str[len] = c;
      len++;
      continue;
    }

    str[len] = '\0';
    if (4 <= len){
      printf("%s\n", str);
    }
    len = 0;
    str[0] = '\0';
  }

  fclose(file);
  return 0;
}
