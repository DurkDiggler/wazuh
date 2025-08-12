/* Copyright (C) 2015, Wazuh Inc.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "logging_helper.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Opaque handle to the AgentSyncProtocol C++ object.
///
/// Used to interact with the AgentSyncProtocol instance from C code.
typedef struct AgentSyncProtocol AgentSyncProtocolHandle;

/// @brief Function pointer type for starting a message queue.
///
/// @param key The identifier key for the message queue.
/// @param type The type of queue or message.
/// @param attempts The number of connection attempts.
/// @return Integer status code (0 on success, non-zero on failure).
typedef int (*mq_start_fn)(const char* key, short type, short attempts);

/// @brief Function pointer type for sending a message to the queue.
///
/// @param queue The queue identifier.
/// @param message The message payload to send.
/// @param message_len The length of the message payload in bytes.
/// @param locmsg Additional location/context message (optional).
/// @param loc A character representing the message location or type.
/// @return Integer status code (0 on success, non-zero on failure).
typedef int (*mq_send_binary_fn)(int queue, const void* message, size_t message_len, const char* locmsg, char loc);


/// @brief Callback type for logging messages from the AgentSyncProtocol.
/// @param level Logging level of the message (e.g., LOG_ERROR, LOG_INFO, LOG_DEBUG).
/// @param log   Null-terminated string containing the log message.
typedef void (*asp_logger_t)(modules_log_level_t level, const char* log);

/// @brief Struct containing function pointers for MQ operations.
///
/// This structure provides the implementation of MQ start and send operations.
typedef struct MQ_Functions
{
    /// Callback to start a message queue.
    mq_start_fn start;

    /// Callback to send a message.
    mq_send_binary_fn send_binary;
} MQ_Functions;

/// @brief Creates an instance of AgentSyncProtocol.
///
/// @param module Name of the module associated with this instance.
/// @param db_path The full path to the SQLite database file to be used.
/// @param mq_funcs Pointer to a MQ_Functions struct containing the MQ callbacks.
/// @param logger Callback function used for logging messages.
/// @return A pointer to an opaque AgentSyncProtocol handle, or NULL on failure.
AgentSyncProtocolHandle* asp_create(const char* module, const char* db_path, const MQ_Functions* mq_funcs, asp_logger_t logger);

/// @brief Destroys an AgentSyncProtocol instance.
///
/// @param handle Pointer to the AgentSyncProtocol handle to destroy.
void asp_destroy(AgentSyncProtocolHandle* handle);

/// @brief Persists a difference (diff) for synchronization.
///
/// @param handle Pointer to the AgentSyncProtocol handle.
/// @param id Unique identifier for the diff (usually a hash).
/// @param operation Type of operation (create, modify, delete).
/// @param index Target index or destination for the diff.
/// @param data JSON string representing the data to persist.
void asp_persist_diff(AgentSyncProtocolHandle* handle,
                      const char* id,
                      int operation,
                      const char* index,
                      const char* data);

// @brief Triggers synchronization of a module.
///
/// @param handle Pointer to the AgentSyncProtocol handle.
/// @param mode Synchronization mode (e.g., full, delta).
/// @param sync_timeout The timeout for each attempt to receive a response, in seconds.
/// @param sync_retries The maximum number of attempts for re-sending Start and End messages.
/// @param max_eps The maximum event reporting throughput. 0 means disabled.
/// @return true if the sync was successfully processed; false otherwise.
bool asp_sync_module(AgentSyncProtocolHandle* handle,
                     int mode,
                     unsigned int sync_timeout,
                     unsigned int sync_retries,
                     size_t max_eps);

/// @brief Parses a response buffer encoded in FlatBuffer format.
/// @param handle Protocol handle.
/// @param data Pointer to the FlatBuffer-encoded message.
/// @return 0 if parsed successfully, -1 on error.
int asp_parse_response_buffer(AgentSyncProtocolHandle* handle, const uint8_t* data);

#ifdef __cplusplus
}
#endif
