

## Architecture

[server]⟵ws⟶[c11n]
                ↑↓
             [mailbox]
                │
                ├[task 1]
                ├[task 2]
                └[task n]


## Code style
    ### Variable Names
        Variables are prefixed with their type: 
        - 'c' for char
        - 's' for int16_t (short)
        - 'l' int32_t (long),
        - 'x' for BaseType_t and any other non-standard types (structures, task handles, queue handles, etc.).
        - If a variable is unsigned, it is also prefixed with a 'u'.
        - If a variable is a pointer, it is also prefixed with a 'p'.
        For example, a variable of type uint8_t will be prefixed with 'uc',
        and a variable of type pointer to char will be prefixed with 'pc'.

    ### Function Names
        Functions are prefixed with both the type they return, and the file they are defined within. For example:
        - vTaskPrioritySet() returns a void and is defined within task.c.
        - xQueueReceive() returns a variable of type BaseType_t and is defined within queue.c.
        - pvTimerGetTimerID() returns a pointer to void and is defined within timers.c.
        - File scope (private) functions are prefixed with 'prv'.
    
    ### Macro Names
        Macros should written in upper case, and prefixed with lower case letters that indicate where the macro is defined:

        | Prefix |        Example       |   Location of definition  |
        |:------:|:--------------------:|:-------------------------:|
        | port   | portMAX_DELAY        | portable.h or portmacro.h |
        | task   | taskENTER_CRITICAL() | task.h                    |
        | pd     | pdTRUE               | projdefs.h                |
        | config | configUSE_PREEMPTION | FreeRTOSConfig.h          |
        | err    | errQUEUE_FULL        | projdefs.h                |
