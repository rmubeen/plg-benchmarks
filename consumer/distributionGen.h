#ifndef distribution_Gen_H
#define distribution_Gen_H

#include <stdlib.h>

typedef struct s_uniformDistMetaData uniformDistMetaData;
uniformDistMetaData* uniformDistInit(size_t min, size_t max, size_t step);
int next(uniformDistMetaData* uniformDist);

#endif