#include <stdio.h>
#include <assert.h>

int CalcHash (const char* buffer, int size);

int main(const int argc, const char** argv)
{
    if (argc);
    const char* buffer = argv[1];
    printf("\n\nHash sum of \"%s\": %x\n\n", buffer, CalcHash(buffer, 5));
}

int CalcHash (const char* buffer, int size)
{
    //assert(buffer);

    int hash = 0;

    for (int i = 0; i < size; ++i)
    {
        hash += buffer[i];
    }

    return hash;
}
