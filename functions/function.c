#include <stdio.h>
#include <string.h>
#include <ctype.h>

int capitalizeWords(char *input, char *output)
{
    int inputIndex = 0, outputIndex = 0;
    while (input[inputIndex] != '\0')
    {
        if (isupper(input[inputIndex]))
        {
            output[outputIndex++] = input[inputIndex++];
            while (isalpha(input[inputIndex]))
            {
                output[outputIndex++] = input[inputIndex++];
            }
            output[outputIndex++] = ' ';
        }
        else
        {
            inputIndex++;
        }
    }
    if (outputIndex > 0 && output[outputIndex - 1] == ' ')
    {
        outputIndex--;
    }
    output[outputIndex] = '\0';
    return outputIndex;
}