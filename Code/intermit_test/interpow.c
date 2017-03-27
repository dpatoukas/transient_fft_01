/*
 * interpow.c
 *
 *  Created on: Mar 12, 2017
 *      Author: Carlo
 */


#include <string.h>
#include "interpow.h"



void read_field_8(void *f, int8_t *dst, uint8_t self, __program_state *ps)
{
    if (!self) {
        __field *t = (__field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(dst + offset) = *(((int8_t*) t->base_addr) + offset);
            offset++;
        }
    }
    else {
        __self_field *t = (__self_field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;
        int8_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_1) : (base = t->base_addr_0);

        while (i--) {
            *(dst + offset) = *(((int8_t*) base) + offset);
            offset++;
        }
    }
}


void read_field_u8(void *f, uint8_t *dst, uint8_t self, __program_state *ps)
{
    if (!self) {
        __field *t = (__field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(dst + offset) = *(((uint8_t*) t->base_addr) + offset);
            offset++;
        }
    }
    else {
        __self_field *t = (__self_field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;
        uint8_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_1) : (base = t->base_addr_0);

        while (i--) {
            *(dst + offset) = *(((uint8_t*) base) + offset);
            offset++;
        }
    }
}


void read_field_16(void *f, int16_t *dst, uint8_t self, __program_state *ps)
{
    if (!self) {
        __field *t = (__field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(dst + offset) = *(((int16_t*) t->base_addr) + offset);
            offset++;
        }
    }
    else {
        __self_field *t = (__self_field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;
        int16_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_1) : (base = t->base_addr_0);

        while (i--) {
            *(dst + offset) = *(((int16_t*) base) + offset);
            offset++;
        }
    }
}


void read_field_u16(void *f, uint16_t *dst, uint8_t self, __program_state *ps)
{
    if (!self) {
        __field *t = (__field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(dst + offset) = *(((uint16_t*) t->base_addr) + offset);
            offset++;
        }
    }
    else {
        __self_field *t = (__self_field*) f;

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


void read_field_32(void *f, int32_t *dst, uint8_t self, __program_state *ps)
{
    if (!self) {
        __field *t = (__field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(dst + offset) = *(((int32_t*) t->base_addr) + offset);
            offset++;
        }
    }
    else {
        __self_field *t = (__self_field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;
        int32_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_1) : (base = t->base_addr_0);

        while (i--) {
            *(dst + offset) = *(((int32_t*) base) + offset);
            offset++;
        }
    }
}


void read_field_u32(void *f, uint32_t *dst, uint8_t self, __program_state *ps)
{
    if (!self) {
        __field *t = (__field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(dst + offset) = *(((uint32_t*) t->base_addr) + offset);
            offset++;
        }
    }
    else {
        __self_field *t = (__self_field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;
        uint32_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_1) : (base = t->base_addr_0);

        while (i--) {
            *(dst + offset) = *(((uint32_t*) base) + offset);
            offset++;
        }
    }
}


void write_field_8(void *f, int8_t *src, uint8_t self, __program_state *ps)
{
    if (!self) {
        __field *t = (__field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(((int8_t*) t->base_addr) + offset) = *(src + offset);
            offset++;
        }
    }
    else {
        __self_field *t = (__self_field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;
        int8_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_0) : (base = t->base_addr_1);

        while (i--) {
            *(((int8_t*) base) + offset) = *(src + offset);
            offset++;
        }

        uint16_t d = (t->code << 8) + t->code;
        ps->curr_task->dirty_in ^= d;
    }
}


void write_field_u8(void *f, uint8_t *src, uint8_t self, __program_state *ps)
{
    if (!self) {
        __field *t = (__field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(((uint8_t*) t->base_addr) + offset) = *(src + offset);
            offset++;
        }
    }
    else {
        __self_field *t = (__self_field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;
        uint8_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_0) : (base = t->base_addr_1);

        while (i--) {
            *(((uint8_t*) base) + offset) = *(src + offset);
            offset++;
        }

        uint16_t d = (t->code << 8) + t->code;
        ps->curr_task->dirty_in ^= d;
    }
}


void write_field_16(void *f, int16_t *src, uint8_t self, __program_state *ps)
{
    if (!self) {
        __field *t = (__field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(((int16_t*) t->base_addr) + offset) = *(src + offset);
            offset++;
        }
    }
    else {
        __self_field *t = (__self_field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;
        int16_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_0) : (base = t->base_addr_1);

        while (i--) {
            *(((int16_t*) base) + offset) = *(src + offset);
            offset++;
        }

        uint16_t d = (t->code << 8) + t->code;
        ps->curr_task->dirty_in ^= d;
    }
}


void write_field_u16(void *f, uint16_t *src, uint8_t self, __program_state *ps)
{
    if (!self) {
        __field *t = (__field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(((uint16_t*) t->base_addr) + offset) = *(src + offset);
            offset++;
        }
    }
    else {
        __self_field *t = (__self_field*) f;

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


void write_field_32(void *f, int32_t *src, uint8_t self, __program_state *ps)
{
    if (!self) {
        __field *t = (__field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(((int32_t*) t->base_addr) + offset) = *(src + offset);
            offset++;
        }
    }
    else {
        __self_field *t = (__self_field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;
        int32_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_0) : (base = t->base_addr_1);

        while (i--) {
            *(((int32_t*) base) + offset) = *(src + offset);
            offset++;
        }

        uint16_t d = (t->code << 8) + t->code;
        ps->curr_task->dirty_in ^= d;
    }
}


void write_field_u32(void *f, uint32_t *src, uint8_t self, __program_state *ps)
{
    if (!self) {
        __field *t = (__field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;

        while (i--) {
            *(((uint32_t*) t->base_addr) + offset) = *(src + offset);
            offset++;
        }
    }
    else {
        __self_field *t = (__self_field*) f;

        uint16_t i = t->length;
        uint16_t offset = 0;
        uint32_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_0) : (base = t->base_addr_1);

        while (i--) {
            *(((uint32_t*) base) + offset) = *(src + offset);
            offset++;
        }

        uint16_t d = (t->code << 8) + t->code;
        ps->curr_task->dirty_in ^= d;
    }
}


void write_field_element_u16(void *f, uint16_t *src, uint16_t pos, uint8_t self, __program_state *ps)
{
    // ((uint16_t*) f->base_addr)[pos] = *src;

    if (!self) {
        __field *t = (__field*) f;
        ((uint16_t*) t->base_addr)[pos] = *src;
    }
    else {
        __self_field *t = (__self_field*) f;

        uint16_t *base;
        (t->code & ps->curr_task->dirty_in) ? (base = t->base_addr_0) : (base = t->base_addr_1);

        ((uint16_t*) base)[pos] = *src;

        // TODO: [L] cannot swap when updating only one element of the array, find a solution!
        // (or do not write self-field element)
        uint16_t d = (t->code << 8) + t->code;
        ps->curr_task->dirty_in ^= d;
    }
}


void start_task(__task *t, __program_state *ps)
{
    // TODO: [APPARENTLY FIXED] make the following two instructions atomic,
    //       maybe by double-buffering tasks that have self-channels, so that
    //       the current buffer is not spoiled during this operation and the
    //       other buffer is updated adequately, then the address of the updated
    //       buffer is assigned to curr_task

    // ps->curr_task->dirty_in &= 0xFF; // clear "dirty" of current task
    // ps->curr_task = t;

    t->dirty_in &= 0xFF;
    ps->curr_task = t;

    t->task_function();
}


void resume_program(__program_state *ps)
{
    if (ps->curr_task->has_self_channel) {
        uint16_t d_h = (ps->curr_task->dirty_in >> 8);
        uint16_t d = (d_h << 8) + d_h;
        ps->curr_task->dirty_in ^= d; // swap-and-clear, atomically executed
    }

    ps->curr_task->task_function();
}
