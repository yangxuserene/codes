/*
 * Copyright (C) 2014 University of Chicago.
 * See COPYRIGHT notice in top-level directory.
 *
 */

#ifndef TORUS_H
#define TORUS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum nodes_event_t nodes_event_t;
typedef struct nodes_message nodes_message;

/* event type of each torus message, can be packet generate, flit arrival, flit send or credit */
enum nodes_event_t
{
  GENERATE = 1,
  ARRIVAL, 
  SEND,
  CREDIT,
  T_COLLECTIVE_INIT,
  T_COLLECTIVE_FAN_IN,
  T_COLLECTIVE_FAN_OUT  
};

struct nodes_message
{
  /* category: comes from codes message */
  char category[CATEGORY_NAME_MAX];
  /* time the packet was generated */
  tw_stime travel_start_time;
  /* for reverse event computation*/
  tw_stime saved_available_time;

  /* message saved collective time */
  tw_stime saved_collective_init_time;

  /* for saving recv and total times*/
  tw_stime saved_recv_time;
  tw_stime saved_total_time;
  tw_stime saved_busy_time;

  /* packet ID */
  unsigned long long packet_ID;
  /* event type of the message */
  nodes_event_t	 type;

  /* for reverse computation */
  int saved_channel;

  /* coordinates of the destination torus nodes */
  int* dest;

  /* final destination LP ID, comes from codes, can be a server or any other I/O LP type */
  tw_lpid final_dest_gid;
  /* destination torus node of the message */
  tw_lpid dest_lp;
  /* LP ID of the sender, comes from codes, can be a server or any other I/O LP type. Should not change
     during network operations. */
  tw_lpid sender_svr;

  /* LP ID of the sending node, has to be a network node in the torus */
  tw_lpid sender_node;

  /* number of hops traversed by the packet */
  int my_N_hop;
  /* source dimension of the message */
  int source_dim;
  /* source direction of the message */
  int source_direction;
  /* next torus hop that the packet will traverse */
  int next_stop;
  /* size of the torus packet */
  uint64_t packet_size;

 /* for reverse computation of a node's fan in*/
  int saved_fan_nodes;

  int source_channel;

  int saved_queue;
  /* chunk id of the flit (distinguishes flits) */
  uint64_t chunk_id;

  model_net_event_return event_rc;
  int is_pull;
  uint64_t pull_size;

  /* for codes local and remote events, only carried by the last packet of the message */
  int local_event_size_bytes;
  int remote_event_size_bytes;
};

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: TORUS_H */

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 * End:
 *
 * vim: ft=c ts=8 sts=4 sw=4 expandtab
 */

