#ifndef IR_REMOTE_H
#define IR_REMOTE_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    IR_CMD_NONE = 0,

    IR_CMD_CH_MINUS,
    IR_CMD_CH,
    IR_CMD_CH_PLUS,

    IR_CMD_PLAY,

    IR_CMD_1,
    IR_CMD_2,
    IR_CMD_3

} ir_command_t;

void ir_remote_init(void);

bool ir_remote_get_command(ir_command_t *cmd);

#endif