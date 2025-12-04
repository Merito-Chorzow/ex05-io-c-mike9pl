#include "ringbuf.h"

void rb_init(rb_t* r){ r->head = r->tail = r->dropped = 0; r->overflow_flag = 0; }

size_t rb_count(const rb_t* r){
    return (r->head - r->tail) % RB_SIZE;
}

size_t rb_free(const rb_t* r){
    return RB_SIZE - 1 - rb_count(r); // jeden slot pusty dla rozróżnienia pełny/pusty
}

int rb_put(rb_t* r, uint8_t b){
    if (rb_free(r) == 0){
        r->dropped++;                 // polityka: odrzucamy NOWE bajty
        r->overflow_flag = 1;         // ustawiamy flagę przepełnienia
        return 0;
    }
    r->q[r->head % RB_SIZE] = b;
    r->head = (r->head + 1) % RB_SIZE;
    return 1;
}

int rb_get(rb_t* r, uint8_t* out){
    if (rb_count(r) == 0) return 0;
    *out = r->q[r->tail % RB_SIZE];
    r->tail = (r->tail + 1) % RB_SIZE;
    return 1;
}
// Zwraca 1, jeżeli był overflow od ostatniego sprawdzenia, 0 w przeciwnym razie.
int rb_had_overflow(rb_t* r){
    if (r->overflow_flag){
        r->overflow_flag = 0;
        return 1;
    }
    return 0;
}
