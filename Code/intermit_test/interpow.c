/*
 * interpow.c
 *
 *  Created on: Mar 12, 2017
 *      Author: Carlo
 */


#include <string.h>
#include "interpow.h"



// TODO: add field type as a parameter and assign the void pointer a value
//       with a switch-case
void read_field_u8(void *f, uint8_t *dst, uint8_t self, program_state *ps)
{
    if (!self) {
        field *t = (field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(dst + offset) = *(((uint16_t*) t->base_addr) + offset);
            offset++;
        }
    }
    else {
        self_field *t = (self_field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;
        uint16_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_1) : (base = t->base_addr_0);

        while (i--) {
            *(dst + offset) = *(((uint16_t*) base) + offset);
            offset++;
        }
    }
}


void read_field_u16(void *f, uint16_t *dst, uint8_t self, program_state *ps)
{
    if (!self) {
        field *t = (field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(dst + offset) = *(((uint16_t*) t->base_addr) + offset);
            offset++;
        }
    }
    else {
        self_field *t = (self_field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;
        uint16_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_1) : (base = t->base_addr_0);

        while (i--) {
            *(dst + offset) = *(((uint16_t*) base) + offset);
            offset++;
        }
    }
}


void write_field_u16(void *f, uint16_t *src, uint8_t self, program_state *ps)
{
    if (!self) {
        field *t = (field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(((uint16_t*) t->base_addr) + offset) = *(src + offset);
            offset++;
        }
    }
    else {
        self_field *t = (self_field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;
        uint16_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_0) : (base = t->base_addr_1);

        while (i--) {
            *(((uint16_t*) base) + offset) = *(src + offset);
            offset++;
        }

        uint16_t d = (t->code << 8) + t->code;
        ps->curr_task->dirty_in ^= d;
    }
}


void write_field_element_u16(void *f, uint16_t *src, uint16_t pos, uint8_t self, program_state *ps)
{
    // ((uint16_t*) f->base_addr)[pos] = *src;

    if (!self) {
        field *t = (field*) f;
        ((uint16_t*) t->base_addr)[pos] = *src;
    }
    else {
        self_field *t = (self_field*) f;

        uint16_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_0) : (base = t->base_addr_1);

        ((uint16_t*) base)[pos] = *src;

        // TODO: cannot swap when updating only one element of the array, find a solution!
        uint16_t d = (t->code << 8) + t->code;
        ps->curr_task->dirty_in ^= d;
    }
}


void start_task(task *t, program_state *ps)
{
    // TODO: (CRITICAL POINT) make the following two instructions atomic,
    //       maybe by double-buffering tasks that have self-channels, so that
    //       the current buffer is not spoiled during this operation and the
    //       other buffer is updated adequately, then the address of the updated
    //       buffer is assigned to curr_task
    ps->curr_task->dirty_in &= 0xFF; // clear "dirty" of current task
    ps->curr_task = t;

    t->task_function(); // call the task function
}


void resume_program(program_state *ps)
{
    // swap-and-clear, atomically executed in the last instruction of the if-body
    if (ps->curr_task->has_self_channel) {
        uint16_t d_h = (ps->curr_task->dirty_in >> 8);
        uint16_t d = (d_h << 8) + d_h;
        ps->curr_task->dirty_in ^= d;
    }

    ps->curr_task->task_function();
}
