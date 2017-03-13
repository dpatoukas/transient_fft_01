/*
 * interpow.c
 *
 *  Created on: Mar 12, 2017
 *      Author: Carlo
 */


#include "interpow.h"



void read_field_u16(field *f, uint16_t *dst)
{
    uint16_t i = f->length;
    uint16_t offset = 0;

    while (i--) {
        *(dst + offset) = *(((uint16_t*) f->base_addr) + offset);
        offset++;
    }
}


void write_field_u16(field *f, uint16_t *src)
{
    uint16_t i = f->length;
    uint16_t offset = 0;

    while (i--) {
        *(((uint16_t*) f->base_addr) + offset) = *(src + offset);
        offset++;
    }
}


void write_field_element_u16(field *f, uint16_t *src, uint16_t pos)
{
    ((uint16_t*) f->base_addr)[pos] = *src;
}


void start_task(const task *t, program_state *ps)
{
    // perform all necessary operations like state update, self-field swap etc.

    ps->curr_task = t; // this has to be the last instruction before task_function()
    t->task_function();
}
