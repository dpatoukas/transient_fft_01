/*
 *
 * Transiently-powered devices library
 *
 *
 * TODO:
 *
 * - Implement Read/Write operations for 16-bit signed integers, 8-bit
 *   signed/unsigned integers and 32-bit signed/unsigned integers
 *
 * - Structure identifiers should be preceded by a double underscore to avoid
 *   name collisions with user-defined variables
 *
 * - Implement support for Memory Protection Unit
 *
 * - Consider Low-Power Advice
 *
 */

#ifndef INC_INTERPOW_H_
#define INC_INTERPOW_H_

#include <stdint.h>

#ifndef MAX_SELF_FIELDS_SWAP

#define MAX_SELF_FIELDS_SWAP        4

#endif /* MAX_SELF_FIELDS_SWAP */


/*
 *******************************************************************************
 *******************************************************************************
 *
 *  #define groups, use them!
 *
 *******************************************************************************
 *******************************************************************************
 */

// Self-field codes for self-field declarations
#define SELF_FIELD_CODE_1           0x01
#define SELF_FIELD_CODE_2           0x02
#define SELF_FIELD_CODE_3           0x04
#define SELF_FIELD_CODE_4           0x08
#define SELF_FIELD_CODE_5           0x10
#define SELF_FIELD_CODE_6           0x20
#define SELF_FIELD_CODE_7           0x40
#define SELF_FIELD_CODE_8           0x80

// Types for field/self-field declarations
#define INT8                        int16_t
#define UINT8                       uint16_t
#define INT16                       int16_t
#define UINT16                      uint16_t
#define INT32                       int16_t
#define UINT32                      uint16_t

// Field-type codes for Read/Write operations
#define INT8_CODE                   1
#define UINT8_CODE                  2
#define INT16_CODE                  3
#define UINT16_CODE                 4
#define INT32_CODE                  5
#define UINT32_CODE                 6


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
    void            *base_addr;
    uint16_t        length;
} field;

typedef struct self_field {
    uint8_t         code;
    void            *base_addr_0;
    void            *base_addr_1;
    uint16_t        length;
} self_field;

typedef struct task {
    void            (*task_function) (void);
    uint8_t         has_self_channel;
    uint16_t        dirty_in;
} task;

// probably channel is not needed
/*
typedef struct channel {
    task            *src;
    task            *dst;
    field           *flds;
    uint8_t         num_fields;
} channel;
*/

// probably self-channel is not needed
/*
typedef struct self_channel {
    task            *src;
    field           *flds;
    uint8_t         num_fields;
    uint16_t        dirty_in;
} self_channel;
*/

typedef struct program_state {
    task            *curr_task;
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

void read_field_8(void*, int8_t*, uint8_t, program_state*); // not implemented yet

void read_field_u8(void*, uint8_t*, uint8_t, program_state*); // not implemented yet

void read_field_16(void*, int16_t*, uint8_t, program_state*); // not implemented yet

void read_field_u16(void*, uint16_t*, uint8_t, program_state*);

void read_field_32(void*, int32_t*, uint8_t, program_state*); // not implemented yet

void read_field_u32(void*, uint32_t*, uint8_t, program_state*); // not implemented yet

void write_field_u16(void*, uint16_t*, uint8_t, program_state*);

void write_field_element_u16(void*, uint16_t*, uint16_t, uint8_t, program_state*);

void start_task(task*, program_state*);

void resume_program(program_state*);


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
 * @param NAME              task's name
 * @param FN                pointer to the function to execute
 * @param HAS_SELF_CHANNEL  1->yes, 0->no
 */
#define NewTask(NAME, FN, HAS_SELF_CHANNEL)                                     \
        task NAME = {                                                           \
            .task_function = FN,                                                \
            .has_self_channel = HAS_SELF_CHANNEL,                               \
            .dirty_in = 0                                                       \
        };


/**
 * Define which task has to execute first on the first start of the system.
 *
 * @param TASK  name of the task
 */
#define InitialTask(TASK)                                                       \
        static program_state prog_state = {                                     \
            .curr_task = &TASK,                                                 \
        };


// probably not needed, just use its concept
/*
#define NewChannel(SRC, DST, NAME, TYPE, LEN)                                   \
        TYPE __##SRC##DST##NAME[LEN] = {0};                                     \
        field SRC##DST##NAME = {                                                \
                                 .length = LEN,                                 \
                                 .base_addr = &__##SRC##DST##NAME               \
        };                                                                      \
        channel SRC##DST = {                                                    \
                             .src = &SRC,                                       \
                             .dst = &DST,                                       \
                             .flds = &SRC##DST##NAME,                           \
                             .num_fields = 1                                    \
        };
*/

// probably not needed, just use its concept
/*
#define NewSelfChannel(TASK)                                                    \
        self_channel TASK##TASK = {                                             \
            .src = &TASK,                                                       \
            .num_fields = 0,                                                    \
            .dirty_in = 0                                                       \
        };
*/


/**
 * Define a new field, conceptually belonging to the channel SRC->DST.
 *
 * @param SRC   channel's source task
 * @param DST   channel's destination task
 * @param NAME  field's name
 * @param TYPE  field's TYPE, can be one of the field types defined above
 * @param LEN   field's length (if LEN>1 the field is an array)
 */
#define NewField(SRC, DST, NAME, TYPE, LEN)                                     \
        TYPE __##SRC##DST##NAME[LEN] = {0};                                     \
        field SRC##DST##NAME = {                                                \
            .length = LEN,                                                      \
            .base_addr = &__##SRC##DST##NAME                                    \
        };


/**
 * Define a new field, conceptually belonging to the self-channel TASK->TASK.
 *
 * @param TASK  channel's source and destination task
 * @param NAME  field's name
 * @param TYPE  field's TYPE, can be one of the field types defined above
 * @param LEN   field's length (if LEN>1 the field is an array)
 * @param CODE  field's code, this parameter must be SELF_FIELD_CODE_x
 *                  where x = 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8
 *                  representing the x-th self-field declared for the
 *                  self-channel TASK->TASK
 */
#define NewSelfField(TASK, NAME, TYPE, LEN, CODE)                               \
        TYPE __##TASK##TASK##NAME##_0[LEN] = {0};                               \
        TYPE __##TASK##TASK##NAME##_1[LEN] = {0};                               \
        self_field TASK##TASK##NAME = {                                         \
            .length = LEN,                                                      \
            .base_addr_0 = &__##TASK##TASK##NAME##_0,                           \
            .base_addr_1 = &__##TASK##TASK##NAME##_1,                           \
            .code = CODE                                                        \
        };


/**
 * Read 16-bit unsigned integer field from channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to read
 * @param DST       address to store the result at
 */
#define ReadField_U16(SRC_TASK, DST_TASK, FLD, DST)                             \
        read_field_u16(&GetField(SRC_TASK, DST_TASK, FLD), DST, 0, &prog_state);


/**
 * Read self-field from channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to read
 * @param DST   address to store the result at
 */
#define ReadSelfField_U16(TASK, FLD, DST)                                       \
        read_field_u16(&GetField(TASK, TASK, FLD), DST, 1, &prog_state);


/**
 * Write field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_U16(SRC_TASK, DST_TASK, FLD, SRC)                            \
        write_field_u16(&GetField(SRC_TASK, DST_TASK, FLD), SRC, 0, &prog_state);


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
#define WriteFieldElement_U16(SRC_TASK, DST_TASK, FLD, SRC, POS)                \
        write_field_element_u16(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS, 0, &prog_state);


/**
 * Write self-field in channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_U16(TASK, FLD, SRC)                                      \
        write_field_u16(&GetField(TASK, TASK, FLD), SRC, 1, &prog_state);


/**
 * ********** CURRENTLY NOT USABLE! **********
 * Write a single element of a self-field in channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 * @param POS   offset of the element to write
 */
#define WriteSelfFieldElement_U16(TASK, FLD, SRC, POS)                          \
        write_field_element_u16(&GetField(TASK, TASK, FLD), SRC, POS, 1, &prog_state);


/**
 * Switch to another task.
 *
 * @param TASK  task to switch to
 */
#define StartTask(TASK)                                                         \
        start_task(&TASK, &prog_state);


/**
 * Resume program from last executing task, call at the beginning of the main.
 */
#define Resume()                                                                \
        resume_program(&prog_state);


#endif /* INC_INTERPOW_H_ */
