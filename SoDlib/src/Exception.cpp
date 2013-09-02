#include "Exception.h"

Exception::Exception(char * msg)
{
    this->msg = strdup(msg);
}

void Exception::print()
{
    printf("%s", msg);
}

Exception::~Exception()
{
    //dtor
}
