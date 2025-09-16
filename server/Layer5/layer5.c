/*
 * =====================================================================================
 *
 *       Filename:  Layer5.c
 *
 *    Description:  This file represents the application making use of our virtual TCP/IP stack
 *
 *        Version:  1.0
 *        Created:  Thursday 26 September 2019 07:48:10  IST
 *       Revision:  1.0
 *       Compiler:  gcc
 *
 *         Author:  Er. Abhishek Sagar, Networking Developer (AS), sachinites@gmail.com
 *        Company:  Brocade Communications(Jul 2012- Mar 2016), Current : Juniper Networks(Apr 2017 - Present)
 *        
 *        This file is part of the TCP/IP Stack distribution (https://github.com/sachinites).
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

#include <stdio.h>
#include <stdint.h>
#include "graph.h"
#include "../tcpconst.h"
#include "../gluethread/glthread.h"
#include "layer5.h"
#include "../Layer3/netfilter.h"

void
promote_pkt_from_layer3_to_layer5(node_t *node,
					 interface_t *recv_intf, 
                     char *pkt,
                     uint32_t pkt_size,
					 hdr_type_t hdr_code) {

	nf_invoke_netfilter_hook(NF_IP_LOCAL_IN,
			pkt, pkt_size, node, recv_intf, hdr_code);
}

void
promote_pkt_from_layer2_to_layer5(node_t *node,
					 interface_t *recv_intf, 
                     char *pkt,
                     uint32_t pkt_size,
					 hdr_type_t hdr_code) { 

	pkt_notif_data_t pkt_notif_data;

	pkt_notif_data.recv_node = node;
	pkt_notif_data.recv_interface = recv_intf;
	pkt_notif_data.pkt = pkt;
	pkt_notif_data.pkt_size = pkt_size;
	pkt_notif_data.hdr_code = hdr_code;

	nfc_invoke_notif_chain(&node->layer2_proto_reg_db2,
			(void *)&pkt_notif_data,
			sizeof(pkt_notif_data_t),
			pkt, pkt_size);	
}

void
tcp_stack_register_l2_pkt_trap_rule(
		node_t *node,
		nfc_pkt_trap pkt_trap_cb,
		nfc_app_cb app_cb) {

	notif_chain_elem_t nfce_template;

	memset(&nfce_template, 0, sizeof(notif_chain_elem_t));
	nfce_template.is_key_set = false;
	nfce_template.app_cb = app_cb;
	nfce_template.pkt_trap_cb = pkt_trap_cb;	
	init_glthread(&nfce_template.glue);

	nfc_register_notif_chain(&node->layer2_proto_reg_db2,
		&nfce_template);	
}


void
tcp_stack_de_register_l2_pkt_trap_rule(
		node_t *node,
		nfc_pkt_trap pkt_trap_cb,
		nfc_app_cb app_cb) {

	notif_chain_elem_t nfce_template;

	memset(&nfce_template, 0, sizeof(notif_chain_elem_t));
	nfce_template.is_key_set = false;
	nfce_template.app_cb = app_cb;
	nfce_template.pkt_trap_cb = pkt_trap_cb;	
	init_glthread(&nfce_template.glue);

	nfc_de_register_notif_chain(&node->layer2_proto_reg_db2,
		&nfce_template);	
}

