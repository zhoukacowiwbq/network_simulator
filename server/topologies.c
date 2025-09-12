/*
 * =====================================================================================
 *
 *       Filename:  topologies.c
 *
 *    Description:  This file contains all topologies that we need to build
 *
 *        Version:  1.0
 *        Created:  Wednesday 18 September 2019 04:29:37  IST
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
/* Visit my Website for more wonderful assignments and projects :
 * https://csepracticals.wixsite.com/csepracticals
 * if above URL dont work, then try visit : https://csepracticals.com*/

#include "graph.h"
#include "comm.h"
#include "Layer2/layer2.h"
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void
network_start_pkt_receiver_thread(graph_t *topo);

graph_t *
build_first_topo(){

#if 0

                          +----------+
                      0/4 |          |0/0
         +----------------+   R0_re  +---------------------------+
         |     40.1.1.1/24| 122.1.1.0|20.1.1.1/24                |
         |                +----------+                           |
         |                                                       |
         |                                                       |
         |                                                       |
         |40.1.1.2/24                                            |20.1.1.2/24
         |0/5                                                    |0/1
     +---+---+                                              +----+-----+
     |       |0/3                                        0/2|          |
     | R2_re +----------------------------------------------+    R1_re |
     |       |30.1.1.2/24                        30.1.1.1/24|          |
     +-------+                                              +----------+

#endif


    graph_t *topo = create_new_graph("Hello World Generic Graph");
    node_t *R0_re = create_graph_node(topo, "R0_re");
    node_t *R1_re = create_graph_node(topo, "R1_re");
    node_t *R2_re = create_graph_node(topo, "R2_re");

    insert_link_between_two_nodes(R0_re, R1_re, "eth0", "eth1", 5);
    insert_link_between_two_nodes(R1_re, R2_re, "eth2", "eth3", 4);
    insert_link_between_two_nodes(R0_re, R2_re, "eth4", "eth5", 9);

    node_set_loopback_address(R0_re, "122.1.1.0");
    node_set_intf_ip_address(R0_re, "eth4", "40.1.1.1", 24);
    node_set_intf_ip_address(R0_re, "eth0", "20.1.1.1", 24);
    
    node_set_loopback_address(R1_re, "122.1.1.1");
    node_set_intf_ip_address(R1_re, "eth1", "20.1.1.2", 24);
    node_set_intf_ip_address(R1_re, "eth2", "30.1.1.1", 24);

    node_set_loopback_address(R2_re, "122.1.1.2");
    node_set_intf_ip_address(R2_re, "eth3", "30.1.1.2", 24);
    node_set_intf_ip_address(R2_re, "eth5", "40.1.1.2", 24);

    network_start_pkt_receiver_thread(topo);

    return topo;
}


graph_t *
build_simple_l2_switch_topo(){

#if 0             
                                       +-----------+
                                       |  H4       |
                                       | 122.1.1.4 |
                                       +----+------+
                                            |eth0/7 - 10.1.1.3/24       
                                            |       
                                            |eth0/1
                                       +----+----+                        +--------+
       +---------+                     |         |                        |        |
       |         |10.1.1.2/24          |   L2Sw  |eth0/2       10.1.1.1/24|  H3    |
       |  H1     +---------------------+         +------------------------+122.1.1.3|
       |122.1.1.1|eth0/5         eth0/4|         |                 eth0/6 |        |
       + --------+                     |         |                        |        |
                                       +----+----+                        +--------+
                                            |eth0/3     
                                            |
                                            |
                                            |
                                            |10.1.1.4/24
                                            |eth0/8
                                      +----++------+
                                      |            |
                                      |   H2       |
                                      |122.1.1.2   |
                                      |            |
                                      +------------+

#endif


    graph_t *topo = create_new_graph("Simple L2 Switch Demo graph");
    node_t *H1 = create_graph_node(topo, "H1");
    node_t *H2 = create_graph_node(topo, "H2");
    node_t *H3 = create_graph_node(topo, "H3");
    node_t *H4 = create_graph_node(topo, "H4");
    node_t *L2SW = create_graph_node(topo, "L2SW");

    insert_link_between_two_nodes(H1, L2SW, "eth5", "eth4", 1);
    insert_link_between_two_nodes(H2, L2SW, "eth8", "eth3", 1);
    insert_link_between_two_nodes(H3, L2SW, "eth6", "eth2", 1);
    insert_link_between_two_nodes(H4, L2SW, "eth7", "eth1", 1);

    node_set_loopback_address(H1, "122.1.1.1");
    node_set_intf_ip_address(H1, "eth5", "10.1.1.2", 24);
    
    node_set_loopback_address(H2, "122.1.1.2");
    node_set_intf_ip_address(H2, "eth8", "10.1.1.4", 24);

    node_set_loopback_address(H3, "122.1.1.3");
    node_set_intf_ip_address(H3, "eth6", "10.1.1.1", 24);
    
    node_set_loopback_address(H4, "122.1.1.4");
    node_set_intf_ip_address(H4, "eth7", "10.1.1.3", 24);
    
    node_set_intf_l2_mode(L2SW, "eth1", ACCESS);
    node_set_intf_l2_mode(L2SW, "eth2", ACCESS);
    node_set_intf_l2_mode(L2SW, "eth3", ACCESS);
    node_set_intf_l2_mode(L2SW, "eth4", ACCESS);

    network_start_pkt_receiver_thread(topo);

    return topo;
}




graph_t *
build_square_topo(){

#if 0     

  +-----------+                      +--------+                            +--------+
  |           |eth0/0     10.1.1.2/24|        | eth0/2               eth0/3|        |
  | R1        +----------------------|  R2    +----------------------------+   R3   |
  |122.1.1.1  |10.1.1.1/24     eth0/1|122.1.1.2|  20.1.1.1/24   20.1.1.2/24| 122.1.1.3|
  +---+--+----+                      |        |                            +-       +
         |eth0/7                     +--------+                            +----+---+
         | 40.1.1.2/24                                                          | eth0/4   
         |                                                                      |30.1.1.1/24
         |                                                                      |
         |                                                                      |
         |                                                                      |
         |                                                                      |
         |                                                                      |
         |                          +-----------+                               |
         |                          |           |                               |
         |                  eth0/6  |  R4       |                               |
         +--------------------------+ 122.1.1.4 |                               |
                         40.1.1.1/24|           +-------------------------------+
                                    |           |eth0/5
                                    +-----------+30.1.1.2/24

config node R1 route /
122.1.1.2 32 10.1.1.2 eth0/0
122.1.1.3 32 10.1.1.2 eth0/0
122.1.1.4 32 40.1.1.1 eth0/7
cd
conf node R2 route 122.1.1.3 32 20.1.1.2 eth0/2
run node R1 ping 122.1.1.3


#endif

    graph_t *topo = create_new_graph("square Topo");
    node_t *R1 = create_graph_node(topo, "R1");
    node_t *R2 = create_graph_node(topo, "R2");
    node_t *R3 = create_graph_node(topo, "R3");
    node_t *R4 = create_graph_node(topo, "R4");

    insert_link_between_two_nodes(R1, R2, "eth0", "eth1", 1);
    insert_link_between_two_nodes(R2, R3, "eth2", "eth3", 1);
    insert_link_between_two_nodes(R3, R4, "eth4", "eth5", 1);
    insert_link_between_two_nodes(R4, R1, "eth6", "eth7", 1);

    node_set_loopback_address(R1, "122.1.1.1");
    node_set_intf_ip_address(R1, "eth0", "10.1.1.1", 24);
    node_set_intf_ip_address(R1, "eth7", "40.1.1.2", 24);
    
    node_set_loopback_address(R2, "122.1.1.2");
    node_set_intf_ip_address(R2, "eth1", "10.1.1.2", 24);
    node_set_intf_ip_address(R2, "eth2", "20.1.1.1", 24);

    node_set_loopback_address(R3, "122.1.1.3");
    node_set_intf_ip_address(R3, "eth3", "20.1.1.2", 24);
    node_set_intf_ip_address(R3, "eth4", "30.1.1.1", 24);
    
    node_set_loopback_address(R4, "122.1.1.4");
    node_set_intf_ip_address(R4, "eth5", "30.1.1.2", 24);
    node_set_intf_ip_address(R4, "eth6", "40.1.1.1", 24);
    
    network_start_pkt_receiver_thread(topo);

    return topo;
}


graph_t *
build_linear_topo(){

    graph_t *topo = create_new_graph("Linear Topo");
    node_t *H1 = create_graph_node(topo, "H1");
    node_t *H2 = create_graph_node(topo, "H2");
    node_t *H3 = create_graph_node(topo, "H3");
    
    insert_link_between_two_nodes(H1, H2, "eth1", "eth2", 1);
    insert_link_between_two_nodes(H2, H3, "eth3", "eth4", 1);

    node_set_loopback_address(H1, "122.1.1.1");
    node_set_loopback_address(H2, "122.1.1.2");
    node_set_loopback_address(H3, "122.1.1.3");

    node_set_intf_ip_address(H1, "eth1", "10.1.1.1", 24);
    node_set_intf_ip_address(H2, "eth2", "10.1.1.2", 24);
    node_set_intf_ip_address(H2, "eth3", "20.1.1.2", 24);
    node_set_intf_ip_address(H3, "eth4", "20.1.1.1", 24);

    network_start_pkt_receiver_thread(topo);

    return topo;
}

graph_t *
build_dualswitch_topo(){

#if 0
                                    +---------+                               +----------+
                                    |         |                               |          |
                                    |  H2     |                               |  H5      |
                                    |122.1.1.2|                               |122.1.1.5 |                                           
                                    +---+-----+                               +-----+----+                                           
                                        |10.1.1.2/24                                +10.1.1.5/24                                                
                                        |eth0/3                                     |eth0/8                                                
                                        |                                           |                                                
                                        |eth0/7,AC,V10                              |eth0/9,AC,V10                                                
                                  +-----+----+                                +-----+---+                                            
                                  |          |                                |         |                                            
   +------+---+                   |          |                                |         |                         +--------+         
   |  H1      |10.1.1.1/24        |   L2SW1  |eth0/5                    eth0/7| L2SW2   |eth0/10           eth0/11|  H6    |         
   |122.1.1.1 +-------------------|          |+-------------------------------|         +-------------+----------+122.1.1.6|         
   +------+---+ eth0/1      eth0/2|          |TR,V10,V11            TR,V10,V11|         |AC,V10        10.1.1.6/24|        |         
                            AC,V10|          |                                |         |                         +-+------+         
                                  +-----+----+                                +----+----+                                            
                                        |eth0/6                                    |eth0/12     
                                        |AC,V11                                    |AC,V11 
                                        |                                          |  
                                        |                                          |  
                                        |                                          |  
                                        |                                          |eth0/11
                                        |eth0/4                                    |10.1.1.4/24  
                                        |10.1.1.3/24                             +--+-----+
                                   +----+---+|                                   | H4     |
                                   |  H3     |                                   |        |
                                   |122.1.1.3|                                   |122.1.1.4|
                                   +--------+|                                   +--------+
#endif

    graph_t *topo = create_new_graph("Dual Switch Topo");
    node_t *H1 = create_graph_node(topo, "H1");
    node_set_loopback_address(H1, "122.1.1.1");
    node_t *H2 = create_graph_node(topo, "H2");
    node_set_loopback_address(H2, "122.1.1.2");
    node_t *H3 = create_graph_node(topo, "H3");
    node_set_loopback_address(H3, "122.1.1.3");
    node_t *H4 = create_graph_node(topo, "H4");
    node_set_loopback_address(H4, "122.1.1.4");
    node_t *H5 = create_graph_node(topo, "H5");

    node_set_loopback_address(H5, "122.1.1.5");
    node_t *H6 = create_graph_node(topo, "H6");
    node_set_loopback_address(H6, "122.1.1.6");
    node_t *L2SW1 = create_graph_node(topo, "L2SW1");
    node_t *L2SW2 = create_graph_node(topo, "L2SW2");
    
    insert_link_between_two_nodes(H1, L2SW1, "eth1", "eth2", 1);
    insert_link_between_two_nodes(H2, L2SW1, "eth3", "eth7", 1);
    insert_link_between_two_nodes(H3, L2SW1, "eth4", "eth6", 1);
    insert_link_between_two_nodes(L2SW1, L2SW2, "eth5", "eth7", 1);
    insert_link_between_two_nodes(H5, L2SW2, "eth8", "eth9", 1);
    insert_link_between_two_nodes(H4, L2SW2, "eth11", "eth12", 1);
    insert_link_between_two_nodes(H6, L2SW2, "eth11", "eth10", 1);

    node_set_intf_ip_address(H1, "eth1",  "10.1.1.1", 24);
    node_set_intf_ip_address(H2, "eth3",  "10.1.1.2", 24);
    node_set_intf_ip_address(H3, "eth4",  "10.1.1.3", 24);
    node_set_intf_ip_address(H4, "eth11", "10.1.1.4", 24);
    node_set_intf_ip_address(H5, "eth8",  "10.1.1.5", 24);
    node_set_intf_ip_address(H6, "eth11", "10.1.1.6", 24);

    node_set_intf_l2_mode(L2SW1, "eth2", ACCESS);
    node_set_intf_vlan_membsership(L2SW1, "eth2", 10);
    node_set_intf_l2_mode(L2SW1, "eth7", ACCESS);
    node_set_intf_vlan_membsership(L2SW1, "eth7", 10);
    node_set_intf_l2_mode(L2SW1, "eth5", TRUNK);
    node_set_intf_vlan_membsership(L2SW1, "eth5", 10);
    node_set_intf_vlan_membsership(L2SW1, "eth5", 11);
    node_set_intf_l2_mode(L2SW1, "eth6", ACCESS);
    node_set_intf_vlan_membsership(L2SW1, "eth6", 11);

    node_set_intf_l2_mode(L2SW2, "eth7", TRUNK);
    node_set_intf_vlan_membsership(L2SW2, "eth7", 10);
    node_set_intf_vlan_membsership(L2SW2, "eth7", 11);
    node_set_intf_l2_mode(L2SW2, "eth9", ACCESS);
    node_set_intf_vlan_membsership(L2SW2, "eth9", 10);
    node_set_intf_l2_mode(L2SW2, "eth10", ACCESS);
    node_set_intf_vlan_membsership(L2SW2, "eth10", 10);
    node_set_intf_l2_mode(L2SW2, "eth12", ACCESS);
    node_set_intf_vlan_membsership(L2SW2, "eth12", 11);

    network_start_pkt_receiver_thread(topo);

    return topo;
}

graph_t *
parallel_links_topology(){

/*
    +--------------+0/0 10.1.1.1        1                            10.1.1.2 0/5+----------------+
    |              +------------------------------------------------------------++                |
    |              |                                                             |                |
    |              |0/1 20.1.1.1        1                            20.1.1.2 0/6|                |
    |    R0        +-------------------------------------------------------------+    R1          |
    |  122.1.1.1   |                                                             |   122.1.1.2    |
    |              |0/2 30.1.1.1        1                            30.1.1.2 0/7|                |
    +              +-------------------------------------------------------------+                |
    |              |                                                             |                |
    +              +0/3 40.1.1.1        1                            40.1.1.2 0/8+                +
    |              |+-------------++---------------------------------------------+                |
    |              |                                                             |                |
    |              |0/4 50.1.1.1        1                            50.1.1.2 0/9|                |
    |              |+-------------+----------------------------------------------+                |
    |              |                                                             |                |
    +--------------+                                                             +----------------+
                                                                                 
                                                                                 
*/
    graph_t *topo = create_new_graph("Parallel Links Topology"); 

    node_t *R0 = create_graph_node(topo, "R0");
    node_t *R1 = create_graph_node(topo, "R1");

    insert_link_between_two_nodes(R0, R1, "eth0", "eth5", INTF_METRIC_DEFAULT);
    insert_link_between_two_nodes(R0, R1, "eth1", "eth6", INTF_METRIC_DEFAULT);
    insert_link_between_two_nodes(R0, R1, "eth2", "eth7", INTF_METRIC_DEFAULT);
    insert_link_between_two_nodes(R0, R1, "eth3", "eth8", INTF_METRIC_DEFAULT);
    insert_link_between_two_nodes(R0, R1, "eth4", "eth9", INTF_METRIC_DEFAULT);

    node_set_loopback_address(R0, "122.1.1.1");
    node_set_loopback_address(R1, "122.1.1.2");

    node_set_intf_ip_address(R0, "eth0", "10.1.1.1", 24);
    node_set_intf_ip_address(R0, "eth1", "20.1.1.1", 24);
    node_set_intf_ip_address(R0, "eth2", "30.1.1.1", 24);
    node_set_intf_ip_address(R0, "eth3", "40.1.1.1", 24);
    node_set_intf_ip_address(R0, "eth4", "50.1.1.1", 24);

    node_set_intf_ip_address(R1, "eth5", "10.1.1.2", 24);
    node_set_intf_ip_address(R1, "eth6", "20.1.1.2", 24);
    node_set_intf_ip_address(R1, "eth7", "30.1.1.2", 24);
    node_set_intf_ip_address(R1, "eth8", "40.1.1.2", 24);
    node_set_intf_ip_address(R1, "eth9", "50.1.1.2", 24);

    network_start_pkt_receiver_thread(topo);

    return topo;
}


graph_t *
cross_link_topology(){

/*                                                +--------+-+
                   +---------+                    | R2       |
               eth1| R1      |eth2     20.1.1.2/24|122.1.1.2 |eth8      
       +-----------+122.1.1.1+--------------------+          +------------------+
       |10.1.1.2/24|         |20.1.1.1/24     eth3|          |50.1.1.1/24       |
       |           +---------+                    +-----+--+-+                  +
       +                                         eth4/  |eth7                   |
       |10.1.1.1/24                      30.1.1.1/24/   |40.1.1.2/24            |
       |eth0                                       /    |                  eth9 |50.1.1.2/24
   +---+---+--+                                   /     |                  +----+-----+
   |          |                                  /      |                  |    R3    |
   | R0       |                                 /       |                  | 122.1.1.3|
   |122.1.1.0 |                                /        |                  |          |
   |          |                               /         |                  +----+-----+
   +---+---+--|               ---------------/          |                       |eth10
       |eth14                /                          |                       |60.1.1.1/24
       |80.1.1.1/24         /                           |                       |
       |                   /                            |                       |
       |                  /                        eth6 |40.1.1.1/24            |
       |             eth5/30.1.1.2/24             +-----+----+                  |
       |           +----/----+                    |   R4     |                  |
       |      eth15|   R5    |eth12    70.1.1.2/24|122.1.1.4 |eth11             |
       +-----------+122.1.1.5|+-------------------+          +------------------+
        80.1.1.2/24|         |70.1.1.1/24    eth13|          |60.1.1.2/24
                  -+---------+                    +----------+

*/
    graph_t *topo = create_new_graph("Cross Links Topology"); 

    node_t *R0 = create_graph_node(topo, "R0");
    node_t *R1 = create_graph_node(topo, "R1");
    node_t *R2 = create_graph_node(topo, "R2");
    node_t *R3 = create_graph_node(topo, "R3");
    node_t *R4 = create_graph_node(topo, "R4");
    node_t *R5 = create_graph_node(topo, "R5");

    insert_link_between_two_nodes(R0, R1, "eth0",  "eth1",  INTF_METRIC_DEFAULT);
    insert_link_between_two_nodes(R0, R5, "eth14", "eth15", INTF_METRIC_DEFAULT);
    insert_link_between_two_nodes(R1, R2, "eth2",  "eth3",  INTF_METRIC_DEFAULT);
    insert_link_between_two_nodes(R2, R3, "eth8",  "eth9",  INTF_METRIC_DEFAULT);
    insert_link_between_two_nodes(R2, R4, "eth7",  "eth6",  INTF_METRIC_DEFAULT);
    insert_link_between_two_nodes(R2, R5, "eth4",  "eth5",  INTF_METRIC_DEFAULT);
    insert_link_between_two_nodes(R3, R4, "eth10", "eth11", INTF_METRIC_DEFAULT);
    insert_link_between_two_nodes(R4, R5, "eth13", "eth12", INTF_METRIC_DEFAULT);

    node_set_loopback_address(R0, "122.1.1.0");
    node_set_loopback_address(R1, "122.1.1.1");
    node_set_loopback_address(R2, "122.1.1.2");
    node_set_loopback_address(R3, "122.1.1.3");
    node_set_loopback_address(R4, "122.1.1.4");
    node_set_loopback_address(R5, "122.1.1.5");

    node_set_intf_ip_address(R0, "eth0", "10.1.1.1", 24);
    node_set_intf_ip_address(R0, "eth14","80.1.1.1", 24);

    node_set_intf_ip_address(R1, "eth1", "10.1.1.2", 24);
    node_set_intf_ip_address(R1, "eth2", "20.1.1.1", 24);

    node_set_intf_ip_address(R2, "eth3", "20.1.1.2", 24);
    node_set_intf_ip_address(R2, "eth8", "50.1.1.1", 24);
    node_set_intf_ip_address(R2, "eth4", "30.1.1.1", 24);
    node_set_intf_ip_address(R2, "eth7", "40.1.1.2", 24);

    node_set_intf_ip_address(R3, "eth9", "50.1.1.2", 24);
    node_set_intf_ip_address(R3, "eth10","60.1.1.1", 24);

    node_set_intf_ip_address(R4, "eth6", "40.1.1.1", 24);
    node_set_intf_ip_address(R4, "eth11","60.1.1.2", 24);
    node_set_intf_ip_address(R4, "eth13","70.1.1.2", 24);

    node_set_intf_ip_address(R5, "eth5", "30.1.1.2", 24);
    node_set_intf_ip_address(R5, "eth12","70.1.1.1", 24);
    node_set_intf_ip_address(R5, "eth15","80.1.1.2", 24);

    network_start_pkt_receiver_thread(topo);

    return topo;
}

char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("无法打开配置文件");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *data = (char *)malloc(length + 1);
    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);
    return data;
}

intf_l2_mode_t string_to_l2_mode(const char *mode_str) {
    if (strcmp(mode_str, "access") == 0) return ACCESS;
    if (strcmp(mode_str, "trunk") == 0) return TRUNK;
    return L2_MODE_UNKNOWN; // 默认值
}

graph_t *create_topology_from_json(const char *json_file_path) {
    // 1. 读取文件
    char *json_str = read_file(json_file_path);
    if (!json_str) return NULL;

    // 2. 解析 JSON
    cJSON *root = cJSON_Parse(json_str);
    if (!root) {
        printf("JSON 解析错误: %s\n", cJSON_GetErrorPtr());
        free(json_str);
        return NULL;
    }

    // 3. 获取 topology
    cJSON *topology = cJSON_GetObjectItem(root, "topology");
    if (!topology) {
        printf("缺少 topology 对象\n");
        cJSON_Delete(root);
        free(json_str);
        return NULL;
    }

    // 4. 创建拓扑
    cJSON *name = cJSON_GetObjectItem(topology, "name");
    graph_t *topo = create_new_graph(cJSON_IsString(name) ? name->valuestring : "Unnamed Topology");

    // 5. 创建节点
    cJSON *nodes = cJSON_GetObjectItem(topology, "nodes");
    if (!cJSON_IsArray(nodes)) {
        printf("nodes 必须是数组\n");
        cJSON_Delete(root);
        free(json_str);
        return NULL;
    }

    typedef struct {
        char *name;
        node_t *node;
    } node_map_t;
    node_map_t *node_map = (node_map_t *)malloc(cJSON_GetArraySize(nodes) * sizeof(node_map_t));
    int node_count = 0;

    for (int i = 0; i < cJSON_GetArraySize(nodes); i++) {
        cJSON *node_item = cJSON_GetArrayItem(nodes, i);
        cJSON *name = cJSON_GetObjectItem(node_item, "name");
        if (!cJSON_IsString(name)) {
            printf("节点 %d 缺少 name\n", i);
            continue;
        }

        node_t *node = create_graph_node(topo, name->valuestring);
        node_map[node_count].name = strdup(name->valuestring);
        node_map[node_count].node = node;
        node_count++;
    }

    // 6. 解析链路
    cJSON *links = cJSON_GetObjectItem(topology, "links");
    if (cJSON_IsArray(links)) {
        for (int i = 0; i < cJSON_GetArraySize(links); i++) {
            cJSON *link = cJSON_GetArrayItem(links, i);
            cJSON *n1 = cJSON_GetObjectItem(link, "n1");
            cJSON *n2 = cJSON_GetObjectItem(link, "n2");
            cJSON *i1 = cJSON_GetObjectItem(link, "i1");
            cJSON *i2 = cJSON_GetObjectItem(link, "i2");
            cJSON *cost = cJSON_GetObjectItem(link, "cost");

            if (!cJSON_IsString(n1) || !cJSON_IsString(n2) || !cJSON_IsString(i1) || 
                !cJSON_IsString(i2) || !cJSON_IsNumber(cost)) {
                printf("链路 %d 配置错误\n", i);
                continue;
            }

            node_t *node1 = NULL, *node2 = NULL;
            for (int j = 0; j < node_count; j++) {
                if (strcmp(node_map[j].name, n1->valuestring) == 0) node1 = node_map[j].node;
                if (strcmp(node_map[j].name, n2->valuestring) == 0) node2 = node_map[j].node;
            }

            if (node1 && node2) {
                insert_link_between_two_nodes(node1, node2, i1->valuestring, i2->valuestring, cost->valueint);
            } else {
                printf("未找到节点 %s 或 %s\n", n1->valuestring, n2->valuestring);
            }
        }
    }

    // 7. 配置节点回环和接口
    for (int i = 0; i < cJSON_GetArraySize(nodes); i++) {
        cJSON *node_item = cJSON_GetArrayItem(nodes, i);
        cJSON *name = cJSON_GetObjectItem(node_item, "name");
        cJSON *loopback = cJSON_GetObjectItem(node_item, "loopback");

        if (!cJSON_IsString(name) || !cJSON_IsString(loopback)) {
            printf("节点 %d 缺少 name 或 loopback\n", i);
            continue;
        }

        // 查找节点
        node_t *node = NULL;
        for (int j = 0; j < node_count; j++) {
            if (strcmp(node_map[j].name, name->valuestring) == 0) {
                node = node_map[j].node;
                break;
            }
        }
        if (!node) continue;

        // 配置回环
        node_set_loopback_address(node, loopback->valuestring);

        // 配置接口
        cJSON *interfaces = cJSON_GetObjectItem(node_item, "interfaces");
        if (cJSON_IsArray(interfaces)) {
            for (int j = 0; j < cJSON_GetArraySize(interfaces); j++) {
                cJSON *iface = cJSON_GetArrayItem(interfaces, j);
                cJSON *intf_name = cJSON_GetObjectItem(iface, "name");
                cJSON *ip = cJSON_GetObjectItem(iface, "ip");
                cJSON *ip_mask = cJSON_GetObjectItem(iface, "mask");
                cJSON *mode = cJSON_GetObjectItem(iface, "mode");
                cJSON *vlans = cJSON_GetObjectItem(iface, "vlans");

                if (!cJSON_IsString(intf_name)) continue;

                if (cJSON_IsString(ip) && strlen(ip->valuestring) > 0 && cJSON_IsNumber(ip_mask)) {
                    char ip_addr[16];
                    int mask = ip_mask->valueint;
                    sscanf(ip->valuestring, "%15[^/]/%d", ip_addr, &mask);
                    node_set_intf_ip_address(node, intf_name->valuestring, ip_addr, mask);
                }

                if (cJSON_IsString(mode) && strlen(mode->valuestring) > 0) {
                    intf_l2_mode_t l2_mode = string_to_l2_mode(mode->valuestring);
                    node_set_intf_l2_mode(node, intf_name->valuestring, l2_mode);
                }

                if (cJSON_IsArray(vlans)) {
                    for (int k = 0; k < cJSON_GetArraySize(vlans); k++) {
                        cJSON *vlan = cJSON_GetArrayItem(vlans, k);
                        if (cJSON_IsNumber(vlan)) {
                            node_set_intf_vlan_membsership(node, intf_name->valuestring, vlan->valueint);
                        }
                    }
                }
            }
        }
    }

    // 8. 清理内存
    for (int i = 0; i < node_count; i++) free(node_map[i].name);
    free(node_map);
    cJSON_Delete(root);
    free(json_str);

    // 9. 启动线程
    network_start_pkt_receiver_thread(topo);
    return topo;
}
