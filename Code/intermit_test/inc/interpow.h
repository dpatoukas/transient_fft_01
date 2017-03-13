/*
 *
 * Transiently-powered devices library
 *
 *
 * TODO:
 *
 * - Implement support for Memory Protection Unit
 *
 * - Consider Low-Power advices
 *
 */

#ifndef INC_INTERPOW_H_
#define INC_INTERPOW_H_

#include <stdint.h>


/*
 *******************************************************************************
 *******************************************************************************
 *
 *  TYPE definitions
 *
 *******************************************************************************
 *******************************************************************************
 */

#define INT                 int16_t
#define UINT                uint16_t


/*
 *******************************************************************************
 *******************************************************************************
 *
 *  Basic structures definition
 *
 *******************************************************************************
 *******************************************************************************
 */

typedef struct field {
    void        *base_addr;
    uint16_t    length;
} field;

typedef struct self_field {
    void        *base_addr_0;
    void        *base_addr_1;
    uint16_t    length;
    uint8_t     in;
    uint8_t     swap;
} self_field;

typedef struct task {
    void (*task_function) (void);
} task;

// probably channel is not needed
typedef struct channel {
    task        *src;
    task        *dst;
    field       *flds;
    uint8_t     num_fields;
} channel;

typedef struct program_state {
    const task  *curr_task;
} program_state;


/*
 *******************************************************************************
 *******************************************************************************
 *
 *  Functions' prototype, only for internal use
 *
 *******************************************************************************
 *******************************************************************************
 */

void read_field_u16(field*, uint16_t*);

void write_field_u16(field*, uint16_t*);

void write_field_element_u16(field*, uint16_t*, uint16_t);

void start_task(const task*, program_state*);


/*
 *******************************************************************************
 *******************************************************************************
 *
 *  Core macro "functions", use them!
 *
 *******************************************************************************
 *******************************************************************************
 */

#define GetName(S, D, F)            __##S##D##F

#define GetField(S, D, F)           S##D##F

#define GetCurrentTask              T2

#define NewTask(NAME, FN)                                                   \
        static const task NAME = { .task_function = FN };

#define InitialTask(TASK)                                                   \
        static program_state prog_state = {                                 \
                      .curr_task = &TASK                                    \
        };

// probably not needed, just use its concept
#define NewChannel(SRC, DST, NAME, TYPE, LEN)                               \
        TYPE __##SRC##DST##NAME[LEN] = {0};                                 \
        field SRC##DST##NAME = {                                            \
                                 .length = LEN,                             \
                                 .base_addr = &__##SRC##DST##NAME           \
        };                                                                  \
        channel SRC##DST = {                                                \
                             .src = &SRC,                                   \
                             .dst = &DST,                                   \
                             .flds = &SRC##DST##NAME,                       \
                             .num_fields = 1                                \
        };

#define NewField(SRC, DST, NAME, TYPE, LEN)                                 \
        TYPE __##SRC##DST##NAME[LEN] = {0};                                 \
        field SRC##DST##NAME = {                                            \
                                 .length = LEN,                             \
                                 .base_addr = &__##SRC##DST##NAME           \
        };

#define NewSelfField(TASK, NAME, TYPE, LEN)                                 \
        TYPE __##SRC##DST##NAME##0[LEN] = {0};                              \
        TYPE __##SRC##DST##NAME##1[LEN] = {0};                              \
        self_field TASK##TASK##NAME = {                                     \
                                  .length = LEN,                            \
                                  .base_addr_0 = &__##SRC##DST##NAME##0,    \
                                  .base_addr_1 = &__##SRC##DST##NAME##1,    \
                                  .in = 0,                                  \
                                  .swap = 0                                 \
        };

// TODO: replace T2 with CURRENT_TASK
#define ReadField_U16(SRC_TASK, FLD, DST)                                   \
        read_field_u16(&GetField(SRC_TASK, T2, FLD), DST);

// TODO: replace T1 with CURRENT_TASK
#define WriteField_U16(DST_TASK, FLD, SRC)                                  \
        write_field_u16(&GetField(T1, DST_TASK, FLD), SRC);

// TODO: replace T1 with CURRENT_TASK
#define WriteFieldElement_U16(DST_TASK, FLD, SRC, POS)                      \
        write_field_element_u16(&GetField(T1, DST_TASK, FLD), SRC, POS);

// TODO: replace T1 with CURRENT_TASK
#define WriteSelfField_U16(FLD, SRC)                                        \
        write_field_u16(&GetField(T1, T1, FLD), SRC);                       \
        (&GetField(T1, T1, FLD))->swap = 1;

// TODO: replace T1 with CURRENT_TASK
// NOTE: cannot swap when updating only one element of the array,
//       find a solution!
#define WriteSelfFieldElement_U16(FLD, SRC, POS)                            \
        write_field_element_u16(&GetField(T1, T1, FLD), SRC, POS);          \
        (&GetField(T1, T1, FLD))->swap = 1;

#define StartTask(TASK)                                                     \
        start_task(&TASK, &prog_state);

#define ResumeProgram()                                                     \
        (prog_state.curr_task)->task_function();


#endif /* INC_INTERPOW_H_ */
