#include "../../types.h"
#include <stdlib.h>

void hsl_random(struct hefesto_modio **modio) {
    if ((*modio) == NULL || (*modio)->args == NULL ||
        (*modio)->args->data == NULL) {
        return;
    }
    (*modio)->rtype = HEFESTO_VAR_TYPE_INT;
    if ((*modio)->args->dtype != HEFESTO_VAR_TYPE_INT) {
        return;
    }
    (*modio)->ret = (int *) malloc(sizeof(int));
    *(int *)(*modio)->ret = (rand() %
                              (*((int *)(*modio)->args->data)));
}
