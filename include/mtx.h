#ifndef __MTX_H__
#define __MTX_H__

#include "types.h"

void MTX44Identity(Mtx44* m);
void MTX44Copy(const Mtx44* src, Mtx44* dst);
void MTX44Concat(const Mtx44* a, const Mtx44* b, Mtx44* ab);

#endif
