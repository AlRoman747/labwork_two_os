#include <stdio.h>
#include <string.h>

void reverse_string(char *s)
{
    int len_s = strlen(s);
    int temp;
    int i;
    int j;
    for (i = 0, j = len_s - 1; i < j; i++, j--)
    {
        temp = s[i];
        s[i] = s[j];
        s[j] = temp;
    }
}

int main(void)
{
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        reverse_string(buffer);
        printf("%s", buffer);
        fflush(stdout);
    }
    return 0;
}