#include "DictProducer.hpp"

int testDict()
{
    DictProducer dictionary;
    dictionary.buildEnDict();
    dictionary.buildEnIndex();
    dictionary.storeEnDict();
    dictionary.storeEnIndex();

    dictionary.buildCnDict();
    dictionary.buildCnIndex();
    dictionary.storeCnDict();
    dictionary.storeCnIndex();

    return 0;
}

