#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define HEADER_BYTE 0x56
#define MIN_TEMPERATURE -32000
#define MAX_TEMPERATURE 32000

void write_values_hex(const char *filename, uint16_t *values, int num_values) {
  FILE *file = fopen(filename, "wb");
  if (file == NULL) {
    perror("Error opening file for writing");
    exit(EXIT_FAILURE);
  }

  fprintf(file, "%02X ", HEADER_BYTE);
  fprintf(file, "%02X ", num_values);
  for (int i = 0; i < num_values; ++i) {
    if (values[i] < MIN_TEMPERATURE || values[i] > MAX_TEMPERATURE) {
      fprintf(stderr, "Error: Temperature value out of range. Exiting.\n");
      fclose(file);
      exit(EXIT_FAILURE);
    }
    uint16_t network_order_value = htons(values[i]);
    fprintf(file, "%04X ", network_order_value);
  }
  fclose(file);
}

void read_values_hex(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file==NULL){
    perror("Error opening for reading\n");
    exit(EXIT_FAILURE);
  }

  //Read the size of values
  int numberofVal;
  if(fscanf(file, "%X", &numberofVal)!=1){
    fprintf(stderr, "Error reading the sz of values from file\n");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  printf("Read values from %s:\n", filename);

  int val;
  if(fscanf(file, "%X", &val)!=1){
    fclose(file);
    exit(EXIT_FAILURE);
  }

  for(int i=1; i<numberofVal;i++){ //skip printing the first value
    if(fscanf(file, "%X", &val)!=1){
      fclose(file);
      exit(EXIT_FAILURE);
    }
    printf("%d ", ntohs(val));
  }
  printf("\n");
  fclose(file);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <filename> <value1> [<value2> <value3> ...]\n", argv[0]);
    return 1;
  }

  const char *filename = argv[1];
  if (argc > 2) {
    int num_values = argc - 2;
    uint16_t *values = (uint16_t *)malloc(num_values * sizeof(uint16_t));

    for (int i = 2; i < argc; ++i) {
      values[i - 2] = atoi(argv[i]);
    }
    write_values_hex(filename, values, num_values);

    printf("Values written to %s\n", filename);
    free(values);
  } else {
    read_values_hex(filename);
    printf("\n");
  }

  return 0;
}