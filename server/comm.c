/*
 * =====================================================================================
 *
 *       Filename:  comm.c
 *
 *    Description:  This file contains the routines to implement the communication between nodes
 *
 *        Version:  1.0
 *        Created:  Thursday 19 September 2019 10:31:35  IST
 *       Revision:  1.0
 *       Compiler:  gcc
 *
 *         Author:  Er. Abhishek Sagar, Networking Developer (AS), sachinites@gmail.com
 *        Company:  Brocade Communications(Jul 2012- Mar 2016), Current : Juniper Networks(Apr 2017 - Present)
 *        
 *        This file is part of the NetworkGraph distribution (https://github.com/sachinites).
 *        Copyright (c) 2017 Abhishek Sagar.
 *        This program is free software: you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by  
 *        the Free Software Foundation, version 3.
 *
 *        This program is distributed in the hope that it will be useful, but 
 *        WITHOUT ANY WARRANTY; without even the implied warranty of 
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 *        General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License 
 *        along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * =====================================================================================
 */

#include <pthread.h>
#include <netinet/in.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> // for close
#include <netdb.h>  /*for struct hostent*/
#include <sys/socket.h>
#include "comm.h"
#include "graph.h"
#include "net.h"
#include "EventDispatcher/event_dispatcher.h"

extern graph_t *topo;

pkt_q_t recvr_pkt_q;

static void
pkt_recvr_job_cbk(void *pkt, uint32_t pkt_size){

	node_t *receving_node;
	interface_t *recv_intf;

	ev_dis_pkt_data_t *ev_dis_pkt_data  = 
			(ev_dis_pkt_data_t *)task_get_next_pkt(&pkt_size);

	if(!ev_dis_pkt_data) {
		return;
	}

	for ( ; ev_dis_pkt_data; 
			ev_dis_pkt_data = (ev_dis_pkt_data_t *) task_get_next_pkt(&pkt_size)) {

		receving_node = ev_dis_pkt_data->recv_node;
		recv_intf = ev_dis_pkt_data->recv_intf;
		pkt = ev_dis_pkt_data->pkt;		
		recv_intf->intf_nw_props.pkt_recv++;

		pkt_receive(receving_node, recv_intf, 
					pkt,
					ev_dis_pkt_data->pkt_size);

		free(ev_dis_pkt_data);
		ev_dis_pkt_data = NULL;
		free(pkt);
	}
}

/* called from init_tcp_ip_stack() at the
 * time of initialization
 * */
void
init_pkt_recv_queue() {

	init_pkt_q(&recvr_pkt_q, pkt_recvr_job_cbk);
}

int
send_pkt_to_self(char *pkt, uint32_t pkt_size,
                interface_t *interface){

    int rc = 0;    
    node_t *sending_node = interface->att_node;
    node_t *nbr_node = sending_node;
  
	ev_dis_pkt_data_t *ev_dis_pkt_data;
 
    if(!IF_IS_UP(interface)){
        return 0;
    }

    interface_t *other_interface =  interface;

	ev_dis_pkt_data = calloc(1, sizeof(ev_dis_pkt_data_t));

	ev_dis_pkt_data->recv_node = nbr_node;
	ev_dis_pkt_data->recv_intf = other_interface;
	ev_dis_pkt_data->pkt = calloc(1, MAX_PACKET_BUFFER_SIZE);
	memcpy(ev_dis_pkt_data->pkt, pkt, pkt_size);
	ev_dis_pkt_data->pkt_size = pkt_size;

	pkt_q_enqueue(&recvr_pkt_q, (char *)ev_dis_pkt_data, sizeof(ev_dis_pkt_data_t));
	
	tcp_dump_send_logger(sending_node, interface, 
			pkt, pkt_size, ETH_HDR);

    return pkt_size; 
       
}

/*Public APIs to be used by the other modules*/
int
send_pkt_out(char *pkt, uint32_t pkt_size, 
             interface_t *interface){

	ev_dis_pkt_data_t *ev_dis_pkt_data;
    node_t *sending_node = interface->att_node;
    node_t *nbr_node = get_nbr_node(interface);
    
    
    if(!IF_IS_UP(interface)){
		interface->intf_nw_props.xmit_pkt_dropped++;
        return 0;
    }

    if(!nbr_node)
        return -1;

    if(pkt_size > MAX_PACKET_BUFFER_SIZE){
        printf("Error : Node :%s, Pkt Size exceeded\n", sending_node->node_name);
        return -1;
    }

    interface_t *other_interface = &interface->link->intf1 == interface ? \
                                    &interface->link->intf2 : &interface->link->intf1;

	ev_dis_pkt_data = calloc(1, sizeof(ev_dis_pkt_data_t));

	ev_dis_pkt_data->recv_node = nbr_node;
	ev_dis_pkt_data->recv_intf = other_interface;
	ev_dis_pkt_data->pkt = calloc(1, MAX_PACKET_BUFFER_SIZE);
	memcpy(ev_dis_pkt_data->pkt, pkt, pkt_size);
	ev_dis_pkt_data->pkt_size = pkt_size;

	pkt_q_enqueue(&recvr_pkt_q, (char *)ev_dis_pkt_data, sizeof(ev_dis_pkt_data_t));
	
	interface->intf_nw_props.pkt_sent++;
	tcp_dump_send_logger(sending_node, interface, 
			pkt, pkt_size, ETH_HDR);

    return pkt_size; 
}

extern void
layer2_frame_recv(node_t *node, interface_t *interface,
                     char *pkt, uint32_t pkt_size);

int
pkt_receive(node_t *node, interface_t *interface,
            char *pkt, uint32_t pkt_size){

    tcp_dump_recv_logger(node, interface, 
            (char *)pkt, pkt_size, ETH_HDR);
    
    /*Make room in the packet buffer by shifting the data towards
      right so that tcp/ip stack can append more hdrs to the packet 
      as required */
    pkt = pkt_buffer_shift_right(pkt, pkt_size, 
            MAX_PACKET_BUFFER_SIZE - IF_NAME_SIZE);
    
    /*Do further processing of the pkt here*/
    layer2_frame_recv(node, interface, pkt, pkt_size );
    return 0;
}

int
send_pkt_flood(node_t *node, interface_t *exempted_intf, 
                char *pkt, uint32_t pkt_size){

    uint32_t i = 0;
    interface_t *intf; 

    for( ; i < MAX_INTF_PER_NODE; i++){

        intf = node->intf[i];
        if(!intf) return 0;

        if(intf == exempted_intf)
            continue;

        send_pkt_out(pkt, pkt_size, intf);
    }
    return 0;
}

/* Enabling pkt Reception via Socket interface. Each node shall be
listening to UDP port no, so that an external process can inject the
traffic into the topology. Once Node recv the pkt, the traffic will be
set on its course towards destination as per the usual pseudo TCPIP stack
implementation */

static uint32_t udp_port_number = 40000;

static uint32_t 
node_get_next_udp_port_number(){
    
    return udp_port_number++;
}

void
node_init_udp_socket(node_t *node){

    if(node->udp_port_number)
        return;
    
    node->udp_port_number = node_get_next_udp_port_number();
     
    int udp_sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    
    if(udp_sock_fd == -1){
        printf("Socket Creation Failed for node %s\n", node->node_name);
        return;   
    }

    struct sockaddr_in node_addr;
    node_addr.sin_family      = AF_INET;
    node_addr.sin_port        = node->udp_port_number;
    node_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(udp_sock_fd, (struct sockaddr *)&node_addr, sizeof(struct sockaddr)) == -1) {
        printf("Error : socket bind failed for Node %s\n", node->node_name);
        return;
    }

    node->udp_sock_fd = udp_sock_fd;
}

static void
_pkt_receive(node_t *receving_node, 
            char *pkt_with_aux_data, 
            uint32_t pkt_size){

    char *recv_intf_name = pkt_with_aux_data;
    interface_t *recv_intf = node_get_intf_by_name(receving_node, recv_intf_name);

    if(!recv_intf){
        printf("Error : Pkt recvd on unknown interface %s on node %s\n", 
                    recv_intf_name, receving_node->node_name);
        return;
    }

    send_pkt_to_self(pkt_with_aux_data + IF_NAME_SIZE, 
                    pkt_size - IF_NAME_SIZE,
                    recv_intf);
}

static char recv_buffer[MAX_PACKET_BUFFER_SIZE];

static void *
_network_start_pkt_receiver_thread(void *arg){

    node_t *node;
    glthread_t *curr;
    
    fd_set active_sock_fd_set,
           backup_sock_fd_set;
    
    int sock_max_fd = 0;
    int bytes_recvd = 0;
    
    graph_t *topo = (void *)arg;

    int addr_len = sizeof(struct sockaddr);

    FD_ZERO(&active_sock_fd_set);
    FD_ZERO(&backup_sock_fd_set);
    
    struct sockaddr_in sender_addr;

    ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr){

        node = graph_glue_to_node(curr);
        
        if(!node->udp_sock_fd) 
            continue;

        if(node->udp_sock_fd > sock_max_fd)
            sock_max_fd = node->udp_sock_fd;

        FD_SET(node->udp_sock_fd, &backup_sock_fd_set);
            
    } ITERATE_GLTHREAD_END(&topo->node_list, curr);

    while(1){

        memcpy(&active_sock_fd_set, &backup_sock_fd_set, sizeof(fd_set));

        select(sock_max_fd + 1, &active_sock_fd_set, NULL, NULL, NULL);

        ITERATE_GLTHREAD_BEGIN(&topo->node_list, curr){

            node = graph_glue_to_node(curr);

            if(FD_ISSET(node->udp_sock_fd, &active_sock_fd_set)){
    
                bytes_recvd = recvfrom(node->udp_sock_fd, (char *)recv_buffer, 
                            MAX_PACKET_BUFFER_SIZE, 0,
                            (struct sockaddr *)&sender_addr,
                            &addr_len);
                
                _pkt_receive(node, recv_buffer, bytes_recvd);
            }
            
        } ITERATE_GLTHREAD_END(&topo->node_list, curr);
    }
}


void
network_start_pkt_receiver_thread(){

    pthread_attr_t attr;
    static pthread_t recv_pkt_thread;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&recv_pkt_thread, &attr, 
                    _network_start_pkt_receiver_thread, 
                    (void *)topo);
}
