/*
 * Copyright (C) 2013 University of Chicago.
 * See COPYRIGHT notice in top-level directory.
 *
 */

/* SUMMARY:
 *
 * This is a test harness for the codes workload API.  It sets up two LP
 * types: clients (which consume operations from the workload generator) and
 * servers (which service operations submitted by clients).
 *
 */

#include <string.h>
#include <assert.h>
#include <ross.h>

#include "codes/lp-io.h"
#include "codes/codes.h"
#include "codes/codes-workload.h"

#define NUM_SERVERS 16  /* number of servers */
#define NUM_CLIENTS 48  /* number of clients */

typedef struct svr_msg svr_msg;
typedef struct svr_state svr_state;
typedef struct client_msg client_msg;
typedef struct client_state client_state;

enum client_event_type
{
    CLIENT_KICKOFF,    /* initial event */
};

enum svr_event_type
{
    SVR_OP,
};

struct client_state
{
    int my_rank;
    int wkld_id;
};

struct svr_state
{
};

struct client_msg
{
    enum client_event_type event_type;
};

struct svr_msg
{
    enum svr_event_type event_type;
    struct codes_workload_op op;
    tw_lpid src;          /* source of this request or ack */
};

const tw_optdef app_opt[] = {
    TWOPT_GROUP("CODES Workload Test Model"),
    TWOPT_END()
};

static void svr_init(
    svr_state * ns,
    tw_lp * lp);
static void svr_event(
    svr_state * ns,
    tw_bf * b,
    svr_msg * m,
    tw_lp * lp);
static void svr_rev_event(
    svr_state * ns,
    tw_bf * b,
    svr_msg * m,
    tw_lp * lp);
static void svr_finalize(
    svr_state * ns,
    tw_lp * lp);
static tw_peid node_mapping(
    tw_lpid gid);

tw_lptype svr_lp = {
     (init_f) svr_init,
     (event_f) svr_event,
     (revent_f) svr_rev_event,
     (final_f) svr_finalize, 
     (map_f) node_mapping,
     sizeof(svr_state),
};

static void handle_client_op_loop_rev_event(
    client_state * ns,
    tw_bf * b,
    client_msg * m,
    tw_lp * lp);
static void handle_client_op_loop_event(
    client_state * ns,
    tw_bf * b,
    client_msg * m,
    tw_lp * lp);

static void client_init(
    client_state * ns,
    tw_lp * lp);
static void client_event(
    client_state * ns,
    tw_bf * b,
    client_msg * m,
    tw_lp * lp);
static void client_rev_event(
    client_state * ns,
    tw_bf * b,
    client_msg * m,
    tw_lp * lp);
static void client_finalize(
    client_state * ns,
    tw_lp * lp);
static tw_peid node_mapping(
    tw_lpid gid);

tw_lptype client_lp = {
     (init_f) client_init,
     (event_f) client_event,
     (revent_f) client_rev_event,
     (final_f) client_finalize, 
     (map_f) node_mapping,
     sizeof(client_state),
};

int main(
    int argc,
    char **argv)
{
    int nprocs;
    int rank;
    int lps_per_proc;
    int i;
    int ret;
    lp_io_handle handle;

    g_tw_ts_end = 60*60*24*365;

    tw_opt_add(app_opt);
    tw_init(&argc, &argv);
 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  
    if((NUM_SERVERS + NUM_CLIENTS) % nprocs)
    {
        fprintf(stderr, "Error: number of server LPs (%d total) is not evenly divisible by the number of MPI processes (%d)\n", NUM_SERVERS+NUM_CLIENTS, nprocs);
        exit(-1);
    }

    lps_per_proc = (NUM_SERVERS+NUM_CLIENTS) / nprocs;

    tw_define_lps(lps_per_proc, 512, 0);

    for(i=0; i<lps_per_proc; i++)
    {
        if((rank*lps_per_proc + i) < NUM_CLIENTS)
            tw_lp_settype(i, &client_lp);
        else
            tw_lp_settype(i, &svr_lp);
    }

    g_tw_lookahead = 100;

    ret = lp_io_prepare("codes-workload-test-results", LP_IO_UNIQ_SUFFIX, &handle, MPI_COMM_WORLD);
    if(ret < 0)
    {
       return(-1); 
    }

    tw_run();

    ret = lp_io_flush(handle, MPI_COMM_WORLD);
    assert(ret == 0);

    tw_end();

    return 0;
}

static void client_init(
    client_state * ns,
    tw_lp * lp)
{
    tw_event *e;
    client_msg *m;
    tw_stime kickoff_time;
    
    memset(ns, 0, sizeof(*ns));
    ns->my_rank = lp->gid;

    /* each client sends a dummy event to itself */

    /* skew each kickoff event slightly to help avoid event ties later on */
    kickoff_time = g_tw_lookahead + tw_rand_unif(lp->rng); 

    e = codes_event_new(lp->gid, kickoff_time, lp);
    m = tw_event_data(e);
    m->event_type = CLIENT_KICKOFF;
    tw_event_send(e);

    return;
}

static void svr_init(
    svr_state * ns,
    tw_lp * lp)
{
    memset(ns, 0, sizeof(*ns));

    return;
}

static void client_event(
    client_state * ns,
    tw_bf * b,
    client_msg * m,
    tw_lp * lp)
{

    switch (m->event_type)
    {
        case CLIENT_KICKOFF:
            handle_client_op_loop_event(ns, b, m, lp);
            break;
        default:
            assert(0);
            break;
    }
}

static void svr_event(
    svr_state * ns,
    tw_bf * b,
    svr_msg * m,
    tw_lp * lp)
{

    switch (m->event_type)
    {
        default:
            assert(0);
            break;
    }
}

static void client_rev_event(
    client_state * ns,
    tw_bf * b,
    client_msg * m,
    tw_lp * lp)
{
    switch (m->event_type)
    {
        case CLIENT_KICKOFF:
            handle_client_op_loop_rev_event(ns, b, m, lp);
            break;
        default:
            assert(0);
            break;
    }

    return;
}

static void svr_rev_event(
    svr_state * ns,
    tw_bf * b,
    svr_msg * m,
    tw_lp * lp)
{
    switch (m->event_type)
    {
        default:
            assert(0);
            break;
    }

    return;
}

static void client_finalize(
    client_state * ns,
    tw_lp * lp)
{
    return;
}

static void svr_finalize(
    svr_state * ns,
    tw_lp * lp)
{
#if 0
    char buffer[256];
    int ret;

    sprintf(buffer, "LP %ld finalize data\n", (long)lp->gid);

    /* test having everyone write to same identifier */
    ret = lp_io_write(lp->gid, "node_state_pointers", strlen(buffer)+1, buffer);
    assert(ret == 0);

    /* test having only one lp write to a particular identifier */
    if(lp->gid == 3)
    {
        ret = lp_io_write(lp->gid, "subset_example", strlen(buffer)+1, buffer);
        assert(ret == 0);
    }

    /* test having one lp write two buffers to the same id */
    if(lp->gid == 5)
    {
        sprintf(buffer, "LP %ld finalize data (intentional duplicate)\n", (long)lp->gid);
        ret = lp_io_write(lp->gid, "node_state_pointers", strlen(buffer)+1, buffer);
        assert(ret == 0);
    }
#endif

    return;
}

static tw_peid node_mapping(
    tw_lpid gid)
{
    return (tw_peid) gid / g_tw_nlp;
}

static void handle_client_op_loop_rev_event(
    client_state * ns,
    tw_bf * b,
    client_msg * m,
    tw_lp * lp)
{
    /* TODO: fill this in */
    assert(0);

    return;
}

/* handle initial event */
static void handle_client_op_loop_event(
    client_state * ns,
    tw_bf * b,
    client_msg * m,
    tw_lp * lp)
{
    struct codes_workload_op op;
    tw_event *e;
    svr_msg *m_out;
    tw_lpid dest_svr_id;

    printf("handle_client_op_loop_event(), lp %llu.\n", (unsigned long long)lp->gid);

    if(m->event_type == CLIENT_KICKOFF)
    {
        /* first operation; initialize the desired workload generator */
        ns->wkld_id = codes_workload_load("test", NULL, ns->my_rank);
        assert(ns->wkld_id > -1);
    }

    codes_workload_get_next(ns->wkld_id, ns->my_rank, &op);

    switch(op.op_type)
    {
        case CODES_WK_END:
            printf("Client rank %d completed workload.\n", ns->my_rank);
            return;
            break;
        case CODES_WK_OPEN:
            dest_svr_id = NUM_CLIENTS + op.u.open.file_id % NUM_SERVERS;
            break;
        default:
            assert(0);
            break;
    }

    e = codes_event_new(dest_svr_id, 1, lp);
    m_out = tw_event_data(e);
    m_out->event_type = SVR_OP;
    m_out->op = op;
    tw_event_send(e);

    return;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ft=c ts=8 sts=4 sw=4 expandtab
 */
