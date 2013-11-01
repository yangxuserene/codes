/*
 * Copyright (C) 2013 University of Chicago.
 * See COPYRIGHT notice in top-level directory.
 *
 */

/* I/O workload generator API to be used for reading I/O operations into 
 * storage system simulations.  This API just describes the operations to be
 * executed; it does not service the operations.
 */

#ifndef CODES_WORKLOAD_H
#define CODES_WORKLOAD_H

#include "ross.h"

typedef struct bgp_params bgp_params;

struct bgp_params
{
    int num_cns_per_lp;
    char* io_kernel_meta_path;
    char* bgp_config_file;
    char* io_kernel_path;
    char* io_kernel_def_path;
};

/* supported I/O operations */
enum codes_workload_op_type
{
    /* terminator; there are no more operations for this rank */
    CODES_WK_END = 1, 
    /* sleep/delay to simulate computation or other activity */
    CODES_WK_DELAY,
    /* block until specified ranks have reached the same point */
    CODES_WK_BARRIER,
    /* open */
    CODES_WK_OPEN,
    /* close */ 
    CODES_WK_CLOSE,
    /* write */
    CODES_WK_WRITE,
    /* read */
    CODES_WK_READ
};

/* I/O operation paramaters */
struct codes_workload_op
{
    /* TODO: do we need different "classes" of operations to differentiate
     * between different APIs?
     */

    /* what type of operation this is */
    enum codes_workload_op_type op_type;

    /* parameters for each operation type */
    union
    {
        struct {
            double seconds;
        } delay;
        struct {
            int count;  /* num ranks in barrier, -1 means "all" */
            int root;   /* root rank */
        } barrier;
        struct {
            int file_id;      /* integer identifier for the file */
            int create_flag;  /* file must be created, not just opened */
        } open;
        struct {
            int file_id;  /* file to operate on */
            off_t offset; /* offset and size */
            size_t size;
        } write;
        struct {
            int file_id;  /* file to operate on */
            off_t offset; /* offset and size */
            size_t size;
        } read;
        struct {
            int file_id;  /* file to operate on */
        } close;
    }u;
};

/* load and initialize workload of of type "type" with parameters specified by
 * "params".  The rank is the caller's relative rank within the collection
 * of processes that will participate in this workload.   
 *
 * This function is intended to be called by a compute node LP in a model
 * and may be called multiple times over the course of a
 * simulation in order to execute different application workloads.
 * 
 * Returns and identifier that can be used to retrieve operations later.
 * Returns -1 on failure.
 */
int codes_workload_load(const char* type, const char* params, int rank);

/* Retrieves the next I/O operation to execute.  the wkld_id is the
 * identifier returned by the init() function.  The op argument is a pointer
 * to a structure to be filled in with I/O operation information.
 */
void codes_workload_get_next(int wkld_id, int rank, struct codes_workload_op *op);

/* Reverse of the above function. */
void codes_workload_get_next_rc(int wkld_id, int rank, const struct codes_workload_op *op);

/* get related information for the codes workload (specifically for the BG/P model) */
void* codes_workload_get_info(int wkld_id, int rank);

/* NOTE: there is deliberately no finalize function; we don't have any
 * reliable way to tell when a workload is truly done and will not
 * participate in further reverse computation.   The underlying generators
 * will shut down automatically once they have issued their last event.
 */

#endif /* CODES_WORKLOAD_H */

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ft=c ts=8 sts=4 sw=4 expandtab
 */
