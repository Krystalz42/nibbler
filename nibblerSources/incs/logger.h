#ifndef LOGGER_H
# define LOGGER_H

# include "logger_utils.h"

/*
** Log levels.
*/

# define D_OFF            0

# define D_FATAL        1
# define D_ERROR        2
# define D_WARN            3
# define D_SUCCESS        4
# define D_INFO            5
# define D_DEBUG        6
# define D_TRACE        7

/*
** Functions to use for instance log messages.
*/

#define log_fatal(...)        \
    logger_fatal(fd(-1), __FILENAME__, __LINE__, __VA_ARGS__)

#define log_error(...)        \
    logger_error(fd(-1), __FILENAME__, __LINE__, __VA_ARGS__)

#define log_warn(...)        \
    logger_warn(fd(-1), __FILENAME__, __LINE__, __VA_ARGS__)

#define log_success(...)    \
    logger_success(fd(-1), __FILENAME__, __LINE__, __VA_ARGS__)

#define log_info(...)        \
    logger_info(fd(-1), __FILENAME__, __LINE__, __VA_ARGS__)

#define log_debug(...)        \
    logger_debug(fd(-1), __FILENAME__, __LINE__, __VA_ARGS__)

#define log_trace(...)        \
    logger_trace(fd(-1), __FILENAME__, __LINE__, __VA_ARGS__)

/*
** Setup the logger and write a sample message 'INFO' if success on the given
** log output (the log_file paramater).
** - The level parameter must be one of the define D_*. If it is not one of
** them, the define D_OFF will be used.
*/

int logger_init(char *log_file);

/*
** Display an 'INFO' message and close the file descriptor of the logger.
*/

int logger_close(void);

#endif
