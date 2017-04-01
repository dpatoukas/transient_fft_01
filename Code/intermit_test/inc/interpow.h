/*
 *
 * Transiently-powered devices library
 *
 *
 * TODO:
 *
 * - [L] Take care of write_field_element
 *
 * - [M] Free the user from the explicit declaration of #pragma (try with 
 *       _Pragma inside macros like NewTask, NewField...)
 *
 * - [L] Return error codes for Read/Write operations?
 *
 * - [L] Implement support for Memory Protection Unit
 *
 * - [L] Consider Low-Power Advice
 *
 */

#ifndef INC_INTERPOW_H_
#define INC_INTERPOW_H_

#include <stdint.h>


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


/*
 *******************************************************************************
 *******************************************************************************
 *
 *  Basic structures definition, only for internal use!
 *
 *******************************************************************************
 *******************************************************************************
 */

typedef struct __field {
    void            *base_addr;
    uint16_t        length;
} __field;

typedef struct __self_field {
    uint8_t         code;
    void            *base_addr_0;
    void            *base_addr_1;
    uint16_t        length;
} __self_field;

typedef struct __task {
    void            (*task_function) (void);
    uint8_t         has_self_channel;
    uint16_t        dirty_in;
} __task;

// probably channel is not needed
/*
typedef struct channel {
    __task            *src;
    __task            *dst;
    __field           *flds;
    uint8_t         num_fields;
} channel;
*/

// probably self-channel is not needed
/*
typedef struct self_channel {
    __task            *src;
    __field           *flds;
    uint8_t         num_fields;
    uint16_t        dirty_in;
} self_channel;
*/

typedef struct __program_state {
    __task          *curr_task;
} __program_state;


/*
 *******************************************************************************
 *******************************************************************************
 *
 *  Function prototypes, only for internal use!
 *
 *******************************************************************************
 *******************************************************************************
 */

void read_field_8(void*, int8_t*, uint8_t, __program_state*);

void read_field_u8(void*, uint8_t*, uint8_t, __program_state*);

void read_field_16(void*, int16_t*, uint8_t, __program_state*);

void read_field_u16(void*, uint16_t*, uint8_t, __program_state*);

void read_field_32(void*, int32_t*, uint8_t, __program_state*);

void read_field_u32(void*, uint32_t*, uint8_t, __program_state*);

void write_field_8(void*, int8_t*, uint8_t, __program_state*);

void write_field_u8(void*, uint8_t*, uint8_t, __program_state*);

void write_field_16(void*, int16_t*, uint8_t, __program_state*);

void write_field_u16(void*, uint16_t*, uint8_t, __program_state*);

void write_field_32(void*, int32_t*, uint8_t, __program_state*);

void write_field_u32(void*, uint32_t*, uint8_t, __program_state*);

void write_field_element_8(__field*, int8_t*, uint16_t);

void write_field_element_u8(__field*, uint8_t*, uint16_t);

void write_field_element_16(__field*, int16_t*, uint16_t);

void write_field_element_u16(__field*, uint16_t*, uint16_t);

void write_field_element_32(__field*, int32_t*, uint16_t);

void write_field_element_u32(__field*, uint32_t*, uint16_t);

void start_task(__task*, __program_state*);

void resume_program(__program_state*);


/*
 *******************************************************************************
 *******************************************************************************
 *
 *  Core macro "functions", use them!
 *
 *******************************************************************************
 *******************************************************************************
 */

#define PersState                   __prog_state

#define PersField(S, D, F)          __##S##D##F

#define PersSField0(T, F)           __##T##T##F##_0
#define PersSField1(T, F)           __##T##T##F##_1

#define GetField(S, D, F)           __##S##D##F##__


/**
 * Create a new task.
 *
 * @param NAME              task's name
 * @param FN                pointer to the function to execute
 * @param HAS_SELF_CHANNEL  1->yes, 0->no
 */
#define NewTask(NAME, FN, HAS_SELF_CHANNEL)                                     \
        __task NAME = {                                                         \
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
        static __program_state __prog_state = {                                 \
            .curr_task = &TASK,                                                 \
        };


// probably not needed, just use its concept
/*
#define NewChannel(SRC, DST, NAME, TYPE, LEN)                                   \
        TYPE __##SRC##DST##NAME[LEN] = {0};                                     \
        __field SRC##DST##NAME = {                                              \
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
        __field __##SRC##DST##NAME##__ = {                                      \
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
        __self_field __##TASK##TASK##NAME##__ = {                               \
            .length = LEN,                                                      \
            .base_addr_0 = &__##TASK##TASK##NAME##_0,                           \
            .base_addr_1 = &__##TASK##TASK##NAME##_1,                           \
            .code = CODE                                                        \
        };


/**
 * Read 8-bit signed integer field from channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to read
 * @param DST       address to store the result at
 */
#define ReadField_8(SRC_TASK, DST_TASK, FLD, DST)                               \
        read_field_8(&GetField(SRC_TASK, DST_TASK, FLD), DST, 0, &__prog_state);


/**
 * Read 8-bit unsigned integer field from channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to read
 * @param DST       address to store the result at
 */
#define ReadField_U8(SRC_TASK, DST_TASK, FLD, DST)                              \
        read_field_u8(&GetField(SRC_TASK, DST_TASK, FLD), DST, 0, &__prog_state);


/**
 * Read 8-bit signed integer self-field from channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to read
 * @param DST   address to store the result at
 */
#define ReadSelfField_8(TASK, FLD, DST)                                         \
        read_field_8(&GetField(TASK, TASK, FLD), DST, 1, &__prog_state);


/**
 * Read 8-bit unsigned integer self-field from channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to read
 * @param DST   address to store the result at
 */
#define ReadSelfField_U8(TASK, FLD, DST)                                        \
        read_field_u8(&GetField(TASK, TASK, FLD), DST, 1, &__prog_state);


/**
 * Read 16-bit signed integer field from channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to read
 * @param DST       address to store the result at
 */
#define ReadField_16(SRC_TASK, DST_TASK, FLD, DST)                              \
        read_field_16(&GetField(SRC_TASK, DST_TASK, FLD), DST, 0, &__prog_state);


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
        read_field_u16(&GetField(SRC_TASK, DST_TASK, FLD), DST, 0, &__prog_state);


/**
 * Read 16-bit signed integer self-field from channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to read
 * @param DST   address to store the result at
 */
#define ReadSelfField_16(TASK, FLD, DST)                                        \
        read_field_16(&GetField(TASK, TASK, FLD), DST, 1, &__prog_state);


/**
 * Read 16-bit unsigned integer self-field from channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to read
 * @param DST   address to store the result at
 */
#define ReadSelfField_U16(TASK, FLD, DST)                                       \
        read_field_u16(&GetField(TASK, TASK, FLD), DST, 1, &__prog_state);


/**
 * Read 32-bit signed integer field from channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to read
 * @param DST       address to store the result at
 */
#define ReadField_32(SRC_TASK, DST_TASK, FLD, DST)                              \
        read_field_32(&GetField(SRC_TASK, DST_TASK, FLD), DST, 0, &__prog_state);


/**
 * Read 32-bit unsigned integer field from channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to read
 * @param DST       address to store the result at
 */
#define ReadField_U32(SRC_TASK, DST_TASK, FLD, DST)                             \
        read_field_u32(&GetField(SRC_TASK, DST_TASK, FLD), DST, 0, &__prog_state);


/**
 * Read 32-bit signed integer self-field from channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to read
 * @param DST   address to store the result at
 */
#define ReadSelfField_32(TASK, FLD, DST)                                        \
        read_field_32(&GetField(TASK, TASK, FLD), DST, 1, &__prog_state);


/**
 * Read 32-bit unsigned integer self-field from channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to read
 * @param DST   address to store the result at
 */
#define ReadSelfField_U32(TASK, FLD, DST)                                       \
        read_field_u32(&GetField(TASK, TASK, FLD), DST, 1, &__prog_state);


/**
 * Write 8-bit signed integer field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_8(SRC_TASK, DST_TASK, FLD, SRC)                              \
        write_field_8(&GetField(SRC_TASK, DST_TASK, FLD), SRC, 0, &__prog_state);


/**
 * Write 8-bit unsigned integer field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_U8(SRC_TASK, DST_TASK, FLD, SRC)                             \
        write_field_u8(&GetField(SRC_TASK, DST_TASK, FLD), SRC, 0, &__prog_state);


/**
 * Write 8-bit signed integer self-field in channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_8(TASK, FLD, SRC)                                        \
        write_field_8(&GetField(TASK, TASK, FLD), SRC, 1, &__prog_state);


/**
 * Write 8-bit unsigned integer self-field in channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_U8(TASK, FLD, SRC)                                       \
        write_field_u8(&GetField(TASK, TASK, FLD), SRC, 1, &__prog_state);


/**
 * Write 16-bit signed integer field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_16(SRC_TASK, DST_TASK, FLD, SRC)                             \
        write_field_16(&GetField(SRC_TASK, DST_TASK, FLD), SRC, 0, &__prog_state);


/**
 * Write 16-bit unsigned integer field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_U16(SRC_TASK, DST_TASK, FLD, SRC)                            \
        write_field_u16(&GetField(SRC_TASK, DST_TASK, FLD), SRC, 0, &__prog_state);


/**
 * Write 16-bit signed integer self-field in channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_16(TASK, FLD, SRC)                                       \
        write_field_16(&GetField(TASK, TASK, FLD), SRC, 1, &__prog_state);


/**
 * Write 16-bit unsigned integer self-field in channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_U16(TASK, FLD, SRC)                                      \
        write_field_u16(&GetField(TASK, TASK, FLD), SRC, 1, &__prog_state);


/**
 * Write 32-bit signed integer field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_32(SRC_TASK, DST_TASK, FLD, SRC)                             \
        write_field_32(&GetField(SRC_TASK, DST_TASK, FLD), SRC, 0, &__prog_state);


/**
 * Write 32-bit unsigned integer field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 */
#define WriteField_U32(SRC_TASK, DST_TASK, FLD, SRC)                            \
        write_field_u32(&GetField(SRC_TASK, DST_TASK, FLD), SRC, 0, &__prog_state);


/**
 * Write 32-bit signed integer self-field in channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_32(TASK, FLD, SRC)                                       \
        write_field_32(&GetField(TASK, TASK, FLD), SRC, 1, &__prog_state);


/**
 * Write 32-bit unsigned integer self-field in channel TASK->TASK.
 * For the time being, TASK is needed as input param.
 *
 * @param TASK  channel's source and destination task
 * @param FLD   field to write
 * @param SRC   address of the variable to write into the field
 */
#define WriteSelfField_U32(TASK, FLD, SRC)                                      \
        write_field_u32(&GetField(TASK, TASK, FLD), SRC, 1, &__prog_state);


/**
 * Write a single element of a 8-bit signed field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 * @param POS       offset of the element to write
 */
#define WriteFieldElement_8(SRC_TASK, DST_TASK, FLD, SRC, POS)                  \
        write_field_element_8(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS);


/**
 * Write a single element of a 8-bit unsigned field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 * @param POS       offset of the element to write
 */
#define WriteFieldElement_U8(SRC_TASK, DST_TASK, FLD, SRC, POS)                 \
        write_field_element_u8(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS);


/**
 * Write a single element of a 16-bit signed field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 * @param POS       offset of the element to write
 */
#define WriteFieldElement_16(SRC_TASK, DST_TASK, FLD, SRC, POS)                 \
        write_field_element_16(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS);


/**
 * Write a single element of a 16-bit unsigned field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 * @param POS       offset of the element to write
 */
#define WriteFieldElement_U16(SRC_TASK, DST_TASK, FLD, SRC, POS)                \
        write_field_element_u16(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS);


/**
 * Write a single element of a 32-bit signed field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 * @param POS       offset of the element to write
 */
#define WriteFieldElement_32(SRC_TASK, DST_TASK, FLD, SRC, POS)                 \
        write_field_element_32(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS);


/**
 * Write a single element of a 32-bit unsigned field in channel SRC_TASK->DST_TASK.
 * For the time being, both SRC_TASK and DST_TASK are needed as input params.
 *
 * @param SRC_TASK  channel's source task
 * @param DST_TASK  channel's destination task
 * @param FLD       field to write
 * @param SRC       address of the variable to write into the field
 * @param POS       offset of the element to write
 */
#define WriteFieldElement_U32(SRC_TASK, DST_TASK, FLD, SRC, POS)                \
        write_field_element_u32(&GetField(SRC_TASK, DST_TASK, FLD), SRC, POS);


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
        write_field_element_u16(&GetField(TASK, TASK, FLD), SRC, POS, 1, &__prog_state);


/**
 * Switch to another task.
 *
 * @param TASK  task to switch to
 */
#define StartTask(TASK)                                                         \
        start_task(&TASK, &__prog_state);


/**
 * Resume program from last executing task, call at the beginning of the main.
 */
#define Resume()                                                                \
        resume_program(&__prog_state);


#endif /* INC_INTERPOW_H_ */
