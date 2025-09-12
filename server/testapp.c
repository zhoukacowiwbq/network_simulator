/*
 * =====================================================================================
 *
 *       Filename:  testapp.c
 *
 *    Description:  This file represents the Test application to test graph topology creation
 *
 *        Version:  1.0
 *        Created:  Wednesday 18 September 2019 04:41:41  IST
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
#include <stdio.h>
#include "CommandParser/libcli.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

extern void init_tcp_ip_stack();

extern graph_t *build_first_topo();
extern graph_t *build_simple_l2_switch_topo();
extern graph_t *build_square_topo();
extern graph_t *build_linear_topo();
extern graph_t *build_dualswitch_topo();
extern graph_t *parallel_links_topology();
extern graph_t *cross_link_topology();
extern graph_t *create_topology_from_json(const char *);

extern void nw_init_cli();

graph_t *topo = NULL;
int tcp_sock = 99;

int 
main(int argc, char **argv){

    nw_init_cli();
    topo = build_square_topo();
    // topo = build_dualswitch_topo();
    topo = create_topology_from_json("topo.json");
    //topo = cross_link_topology();
    init_tcp_ip_stack();
    init_tcp_client("192.168.31.28", 55555);
    start_shell(); 
    return 0;
}

// 初始化TCP客户端
int init_tcp_client(const char* server_ip, int port) {
    // 创建套接字
    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock == -1) {
        perror("socket"); // 使用 perror 输出更详细的错误信息
        return -1; // 返回错误码
    }

    // 初始化服务器地址结构
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr)); // 使用 memset 初始化为 0
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // 将点分十进制的 IP 地址转换为网络字节序的地址
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(tcp_sock);
        tcp_sock = -1; // 恢复为标准错误值
        return -1; // 返回错误码
    }

    // 连接到服务器
    if (connect(tcp_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        close(tcp_sock);
        tcp_sock = -1; // 恢复为标准错误值
        return -1; // 返回错误码
    }

    return 0; // 返回 0 表示成功
}
