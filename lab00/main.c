// Lab 00
// Will Culpepper
// CSCI444 - Advanced Computer Graphics

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void standard(int num) {
    fprintf(stdout, "Num is %d\n", num);
}

void external(int num) {
    FILE *file = fopen("output2.txt", "a");
    fprintf(file, "%d\n", num);
    fclose(file);
}

void writeToStream(void (*f)(int), int n) {
    f(n);
}

void doubleIt(int num) {
    fprintf(stdout, "%d doubled is id %d", num, 2*num);
}

int main(){
    fprintf(stdout, "Hello World!\n");

    FILE *file = fopen("output.txt", "w");
    if(!file) {
        fprintf(stdout, "Could not open file for writine!\n");
    } else {
        fprintf(file, "Hello C file!\n");
    }
    fclose(file);

    int year = 2023;
    float piApprox = 22.0f/7.0f;
    float radius = 4.5f;
    float area = piApprox*radius*radius;

    fprintf(stdout, "The current year is %d.\n", year);
    fprintf(stdout, "Our value of pi is %d.\n", piApprox);
    fprintf(stdout, "Our circle with radius %.1f\" has an area of %.2f sq. in.\n", radius, area);

    int age;
    fprintf(stdout, "What is your age? ");
    scanf("%d", &age);

    const int MAX_INT = 65535;
    FILE *values = fopen("input.csv", "r");
    if(!values) {
        fprintf(stdout, "Could not open file for writine!\n");
    } else {
        fprintf(stdout, "getting values from file...");
        char row[65535];
        fgets(row, MAX_INT, values);
        printf("Row: %s", row);
    }

    int* yourAge = NULL;
    yourAge = (int*)malloc(sizeof(int));
    fprintf(stdout, "What's your age again? ");
    fscanf(stdin, "%d", yourAge);
    free(yourAge);

    int* arr = (int*)malloc(5*sizeof(int));
    // baseAddress + offset*stepSize

    // AoS approach
    struct Record {
        int num;
        float f;
        char id;
    };

    struct RecordSoA {
        int* num;
        float* f;
        char* id;
    } recordSoA;

    struct Record* aos = NULL;

    FILE* data = fopen("data1.txt", "r");

    if(!data) {
        fprintf(stdout, "Unable to locate file!");
    } else {
        char firstLine[100];
        fgets(firstLine, 100, data);
        fprintf(stdout, "%s", firstLine);

        int n = atoi(firstLine);
        fprintf(stdout, "%d\n", n);

        aos = (struct Record*)malloc(n*sizeof(struct Record));
        recordSoA.num = (int*)malloc(n*sizeof(int));
        recordSoA.f = (float*)malloc(n*sizeof(float));
        recordSoA.id = (char*)malloc(n*sizeof(char));

        for(int i = 0; i < n; i++) {
            char line[100];
            fgets(line, 100, data);

            char* token = strtok(line, " ");
            while(token) {
                int number = atoi(token);
                recordSoA.num[i] = number;
                aos[i].num = number;
                token = strtok(NULL, " ");

                float decimal = atof(token);
                recordSoA.f[i] = decimal;
                aos[i].f = decimal;
                token = strtok(NULL, " ");

                char c = *token;
                recordSoA.id[i] = c;
                aos[i].id = c;
                token = strtok(NULL, " ");
            }
        }
    }

    standard(5);
    fprintf(stdout, "The standard function is stored at 0x%p\n", standard);

    writeToStream(standard, 5);
    writeToStream(external, 5);
    writeToStream(doubleIt, 5);

    return 0;
}