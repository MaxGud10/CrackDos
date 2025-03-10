#include <stdio.h>
#include <stdlib.h>

int main() {
    char filename[256];
    int count;

    // Prompt for the file name
    printf("Enter the file name: ");
    scanf("%s", filename);

    printf("Enter the number of '0' characters: ");
    scanf("%d", &count);

    FILE *file = fopen(filename, "w");
    if (file == NULL) 
    {
        printf("Error: Unable to open the file.\n");
        return 1;
    }

    for (int i = 0; i < count; i++) 
        fputc('0', file);

    fclose(file);

    printf("File '%s' successfully filled with %d '0' characters.\n", filename, count);

    return 0;
}