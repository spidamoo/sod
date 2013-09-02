#include <stdio.h>
#include <string.h>

#ifndef EXCEPTION_H
#define EXCEPTION_H


class Exception
{
    public:
        Exception(char *);
        void print();
        virtual ~Exception();
    protected:
    char * msg;
    private:
};

#endif // EXCEPTION_H
