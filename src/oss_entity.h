#ifndef OSS_ENTITY_H
#define OSS_ENTITY_H

#include <string>

#include "oss_vector.h"

using namespace std;

class OSS_Entity
{
public:
    OSS_Entity(string setname = "");

    void update();

    void render();

    string getName();

private:
    string name;

};

#endif // OSS_ENTITY_H
