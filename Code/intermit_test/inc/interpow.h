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
 *  Basic structures definition, only for internal use!
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
 *  Function prototypes, only for internal use!
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

#define PersState                   prog_state

#define PersField(S, D, F)          __##S##D##F

#define GetField(S, D, F)           S##D##F

/**
 * Create a new task.
 *
 * @param NAME  task's name
 * @param FN    pointer to the function to execute
 */
#define NewTask(NAME, FN)                                                   \
        static const task NAME = { .task_function = FN };

/**
 * Define which task has to execute first on the first start of the system.
 *
 * @param TASK  name of the task
 */
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

/**
 * Define a new field, conceptually belonging to the channel SRC->DST.
 *
 * @param SRC   channel's source task
 * @param DST   channel's destination task
 * @param NAME  field's name
 * @param TYPE  field's TYPE, can be one of the field types defined above
 * @param LEN   field's length (if LEN>1 the field is an array)
 */
#define NewField(SRC, DST, NAME, TYPE, LEN)                                 \
        TYPE __##SRC##DST##NAME[LEN] = {0};                                 \
        field SRC##DST##NAME = {                                            \
                                 .length = LEN,                             \
                                 .base_addr = &__##SRC##DST##NAME           \
        };

/**
 * Define a new field, conceptually belonging to the self-channel TASK->TASK.
 *
 * @param TASK  channel's source and destination task
 * @param NAME  field's name
 * @param TYPE  field's TYPE, can be one of the field types defined above
 * @param LEN   field's length (if LEN>1 the field is an array)
 */
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

/**
 * Read field from channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to read
 * @param DST       address to store the result at
 */
#define ReadField_U16(SRC_TASK, DST_TASK, FLD, DST)                         \
        read_field_u16(&GetField(SRC_TASK, DST_TASK, FLD), DST);

/**
 * Write field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_U16(SRC_TASK, DST_TASK, FLD, SRC)                        \
        write_field_u16(&GetField(SRC_TASK, DST_TASK, FLD), SRC);

/**
 * Write a single element of a field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 * @param POS       offset of the element to write
 */
#define WriteFieldElement_U16(SRC_TASK, DST_TASK, FLD, SRC, POS)            \
        write_field_element_u16(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS);

/**
 * Write self-field in channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_U16(TASK, FLD, SRC)                                  \
        write_field_u16(&GetField(TASK, TASK, FLD), SRC);                   \
        (&GetField(TASK, TASK, FLD))->swap = 1;

/**
 * Write a single element of a self-field in channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 * @param POS   offset of the element to write
 */
// TODO: cannot swap when updating only one element of the array, find a solution!
#define WriteSelfFieldElement_U16(TASK, FLD, SRC, POS)                      \
        write_field_element_u16(&GetField(TASK, TASK, FLD), SRC, POS);      \
        (&GetField(TASK, TASK, FLD))->swap = 1;

/**
 * Switch to another task.
 *
 * @param TASK  task to switch to
 */
#define StartTask(TASK)                                                     \
        start_task(&TASK, &prog_state);

/**
 * Resume program from last executing task, call at the beginning of the main.
 */
#define ResumeProgram()                                                     \
        (prog_state.curr_task)->task_function();


#endif /* INC_INTERPOW_H_ */
