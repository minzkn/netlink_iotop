/*
	Copyright (C) MINZKN.COM
	All rights reserved.
	Author: JAEHYUK CHO <mailto:minzkn@minzkn.com>
*/

#if !defined(_ISOC99_SOURCE)
# define _ISOC99_SOURCE (1L)
#endif

#if !defined(_GNU_SOURCE)
# define _GNU_SOURCE (1L)
#endif

#include <stddef.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>

#include <linux/netlink.h>
#include <linux/genetlink.h>

#include <linux/taskstats.h>

#define hwport_peek_const_vector(m_cast,m_base,m_sign,m_offset) ((m_cast)((const void *)(((const uint8_t *)(m_base)) m_sign ((size_t)(m_offset)))))
#define hwport_peek_const_f(m_cast,m_base,m_offset) hwport_peek_const_vector(m_cast,m_base,+,m_offset)
#define hwport_peek_const(m_base,m_offset) hwport_peek_const_vector(const void *,m_base,+,m_offset)

#define hwport_peek_vector(m_cast,m_base,m_sign,m_offset) ((m_cast)((void *)(((uint8_t *)(m_base)) m_sign ((size_t)(m_offset)))))
#define hwport_peek_f(m_cast,m_base,m_offset) hwport_peek_vector(m_cast,m_base,+,m_offset)
#define hwport_peek(m_base,m_offset) hwport_peek_vector(void *,m_base,+,m_offset)

#define hwport_peek_const_type(m_cast,m_from,m_offset) (*(hwport_peek_const_f(const m_cast *,m_from,m_offset)))
#define hwport_peek_uint8(m_from,m_offset) hwport_peek_const_type(uint8_t,m_from,m_offset)

#define hwport_peek_type(m_cast,m_from,m_offset) (*(hwport_peek_f(m_cast *,m_from,m_offset)))
#define hwport_poke_type(m_cast,m_to,m_offset,m_value) do{hwport_peek_type(m_cast,m_to,m_offset)=(m_cast)(m_value);}while(0)
#define hwport_poke_uint8(m_to,m_offset,m_value) hwport_poke_type(uint8_t,m_to,m_offset,m_value)

#define _ntohll(x) (((uint64_t)(ntohl((uint32_t)((x<<32)>>32)))<<32)|ntohl(((uint32_t)(x>>32))))

static size_t hwport_generate_netlink_message(void *s_buffer, size_t s_buffer_size, unsigned int s_nlmsg_type, unsigned int s_nlmsg_flags, unsigned int s_nlmsg_seq, unsigned int s_nlmsg_pid, const void *s_payload, size_t s_payload_size);
static size_t hwport_append_netlink_attr(void *s_buffer, size_t s_buffer_size, unsigned int s_nla_type, const void *s_attr, size_t s_attr_size);

int main(int s_argc, char **s_argv);

static size_t hwport_generate_netlink_message(void *s_buffer, size_t s_buffer_size, unsigned int s_nlmsg_type, unsigned int s_nlmsg_flags, unsigned int s_nlmsg_seq, unsigned int s_nlmsg_pid, const void *s_payload, size_t s_payload_size)
{
	int s_aligned_payload_size;
	
	struct nlmsghdr *s_nlmsghdr;
	void *s_payload_ptr;

	s_aligned_payload_size = NLMSG_ALIGN((uint32_t)s_payload_size);
	if(s_buffer_size < ((size_t)NLMSG_LENGTH(s_aligned_payload_size))) {
		return((size_t)0u);
	}
	
	s_nlmsghdr = (struct nlmsghdr *)memset((void *)s_buffer, 0, sizeof(struct nlmsghdr));
	s_nlmsghdr->nlmsg_len = (uint32_t)NLMSG_LENGTH(s_aligned_payload_size);
	s_nlmsghdr->nlmsg_type = (uint16_t)s_nlmsg_type;
	s_nlmsghdr->nlmsg_flags = (uint16_t)s_nlmsg_flags;
	s_nlmsghdr->nlmsg_seq = (uint32_t)s_nlmsg_seq;
	s_nlmsghdr->nlmsg_pid = (uint32_t)s_nlmsg_pid;

	s_payload_ptr = (void *)NLMSG_DATA(s_nlmsghdr);
	if(s_payload == ((const void *)0)) {
		(void)memset(s_payload_ptr, 0, s_payload_size);
	}
	else if(((const void *)s_payload_ptr) != s_payload) {
		(void)memcpy(s_payload_ptr, s_payload, s_payload_size);
	}

	return((size_t)s_nlmsghdr->nlmsg_len);
}

static size_t hwport_append_netlink_attr(void *s_buffer, size_t s_buffer_size, unsigned int s_nla_type, const void *s_attr, size_t s_attr_size)
{
	struct nlmsghdr *s_nlmsghdr;
	struct nlattr *s_nlattr;
	void *s_attr_ptr;

	s_nlmsghdr = (struct nlmsghdr *)s_buffer;
	if(s_buffer_size < ((size_t)(s_nlmsghdr->nlmsg_len + NLA_HDRLEN + NLA_ALIGN(s_attr_size)))) {
		return((size_t)0u);
	}

	s_nlattr = hwport_peek_f(struct nlattr *, s_buffer, s_nlmsghdr->nlmsg_len);
	s_nlattr->nla_len = (uint16_t)NLA_HDRLEN + s_attr_size;
	s_nlattr->nla_type = (uint16_t)s_nla_type;

	s_attr_ptr = hwport_peek(s_nlattr, NLA_HDRLEN);
	if(s_attr == ((const void *)0)) {
		(void)memset((void *)s_attr_ptr, 0, s_attr_size);
	}
	else if(((const void *)s_attr_ptr) != s_attr) {
		(void)memcpy((void *)s_attr_ptr, s_attr, s_attr_size);
	}

	s_nlmsghdr->nlmsg_len += (uint32_t)NLA_ALIGN(s_nlattr->nla_len);

	return((size_t)s_nlmsghdr->nlmsg_len);
}

int main(int s_argc, char **s_argv)
{
	pid_t s_mypid;

	__u32 s_nl_groups;
	int s_socket;
	struct sockaddr_nl s_sockaddr_nl; 

	size_t s_buffer_size;
	void *s_buffer;

	ssize_t s_send_bytes;

	uint16_t s_family_id;

	(void)s_argc;
	(void)s_argv;

	(void)fprintf(stdout, "NETLINK IOTOP MONITOR\n\n");

	s_mypid = getpid();

	s_nl_groups = (__u32)0u;

	s_socket = socket(PF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
	if(s_socket == (-1)) {
		perror("socket");
		return(EXIT_FAILURE);
	}

	(void)memset((void *)(&s_sockaddr_nl), 0, sizeof(s_sockaddr_nl));
	s_sockaddr_nl.nl_family = AF_NETLINK;
	s_sockaddr_nl.nl_pad = (unsigned short)0u;
	s_sockaddr_nl.nl_pid = (pid_t)0;
	s_sockaddr_nl.nl_groups = s_nl_groups; /* Multicast groups mask */

	if(bind(s_socket, (const struct sockaddr *)(&s_sockaddr_nl), (socklen_t)sizeof(s_sockaddr_nl)) == (-1)) {
		perror("bind");
		return(EXIT_FAILURE);
	}
	(void)fprintf(stdout, "listening... (mypid is %ld)\n", (long)s_mypid);   

	s_buffer_size = (size_t)(512 << 10);
	s_buffer = malloc(s_buffer_size);
	if(s_buffer == ((void *)0)) {
		(void)fprintf(stderr, "not enough memory !\n");
		close(s_socket);
		return(EXIT_FAILURE);
	}

	s_family_id = (uint16_t)0u;

	do { /* get family id */
		static const char cg_name[] = {TASKSTATS_GENL_NAME};

		struct genlmsghdr *s_genlmsghdr;
		size_t s_message_size;

		s_message_size = hwport_generate_netlink_message(
			s_buffer,
		       	s_buffer_size,
			GENL_ID_CTRL,
			NLM_F_REQUEST /* | NLM_F_ACK */,
			0u /* seq */,
			(unsigned int)s_mypid,
			(const void *)0,
			sizeof(struct genlmsghdr)
		);
		s_genlmsghdr = (struct genlmsghdr *)NLMSG_DATA(s_buffer);
		s_genlmsghdr->cmd = (uint8_t)CTRL_CMD_GETFAMILY;
		s_genlmsghdr->version = (uint8_t)TASKSTATS_GENL_VERSION;

		s_message_size = hwport_append_netlink_attr(
			s_buffer,
		       	s_buffer_size,
			CTRL_ATTR_FAMILY_NAME,
			(const void *)(&cg_name[0]),
			sizeof(cg_name)
		);

		s_send_bytes = send(s_socket, (const void *)s_buffer, s_message_size, MSG_NOSIGNAL);
		if(s_send_bytes == ((ssize_t)(-1))) {
			perror("send nlmsg");
		}
		else {
			struct nlmsghdr *s_nlmsghdr;
			size_t s_offset;
			struct nlattr *s_nlattr;

			(void)recv(s_socket, memset(s_buffer, 0, s_buffer_size), s_buffer_size, MSG_NOSIGNAL);

			s_nlmsghdr = (struct nlmsghdr *)s_buffer;
			s_genlmsghdr = (struct genlmsghdr *)NLMSG_DATA(s_buffer);

			for(s_offset = (size_t)(NLMSG_HDRLEN + NLMSG_ALIGN(sizeof(struct genlmsghdr)));s_offset < s_nlmsghdr->nlmsg_len;) {
				if((s_offset + ((size_t)NLA_HDRLEN)) > ((size_t)s_nlmsghdr->nlmsg_len)) {
					/* attibute header 만큼이 남지 않았음. */
					break;
				}
				s_nlattr = hwport_peek_f(struct nlattr *, s_buffer, s_offset);
				if(s_nlattr->nla_len < ((size_t)NLA_HDRLEN)) {
					/* attribute length 가 최소 크기를 만족하지 못함 */
					break;
				}
				if((s_offset + ((size_t)NLA_ALIGN(s_nlattr->nla_len))) > ((size_t)s_nlmsghdr->nlmsg_len)) {
					/* attribute 공간이 nlmsg를 넘어섬 */
					break;
				}

				switch(s_nlattr->nla_type) {
					case CTRL_ATTR_UNSPEC:
#if 0L
						(void)fprintf(stdout, "CTRL_ATTR_UNSPEC: nla type=%u, nla len=%u\n", s_nlattr->nla_type, s_nlattr->nla_len);
#endif
						break;
					case CTRL_ATTR_FAMILY_ID:
						s_family_id = hwport_peek_const_type(uint16_t, s_nlattr, NLA_HDRLEN);
#if 0L
						(void)fprintf(stdout, "CTRL_ATTR_FAMILY_ID: nla type=%u, nla len=%u\n", s_nlattr->nla_type, s_nlattr->nla_len);
						(void)fprintf(stdout, "  - family-id is %u (%02XH)\n", (unsigned int)s_family_id, (unsigned int)s_family_id);
#endif
						break;
					case CTRL_ATTR_FAMILY_NAME:
#if 0L
						(void)fprintf(stdout, "CTRL_ATTR_FAMILY_NAME: nla type=%u, nla len=%u\n", s_nlattr->nla_type, s_nlattr->nla_len);
						(void)fprintf(stdout, "  - family-name is \"%s\"\n", hwport_peek_f(char *, s_nlattr, NLA_HDRLEN));
#endif
						break;
					case CTRL_ATTR_VERSION:
#if 0L
						(void)fprintf(stdout, "CTRL_ATTR_VERSION: nla type=%u, nla len=%u\n", s_nlattr->nla_type, s_nlattr->nla_len);
						(void)fprintf(stdout, "  - version is %lXH\n", (unsigned long)hwport_peek_const_type(uint32_t, s_nlattr, NLA_HDRLEN));
#endif
						break;
					case CTRL_ATTR_HDRSIZE:
#if 0L
						(void)fprintf(stdout, "CTRL_ATTR_HDRSIZE: nla type=%u, nla len=%u\n", s_nlattr->nla_type, s_nlattr->nla_len);
						(void)fprintf(stdout, "  - hdrsize is %lXH\n", (unsigned long)hwport_peek_const_type(uint32_t, s_nlattr, NLA_HDRLEN));
#endif
						break;
					case CTRL_ATTR_MAXATTR:
#if 0L
						(void)fprintf(stdout, "CTRL_ATTR_MAXATTR: nla type=%u, nla len=%u\n", s_nlattr->nla_type, s_nlattr->nla_len);
						(void)fprintf(stdout, "  - maxattr is %lXH\n", (unsigned long)hwport_peek_const_type(uint32_t, s_nlattr, NLA_HDRLEN));
#endif
						break;
					case CTRL_ATTR_OPS:
#if 0L
						(void)fprintf(stdout, "CTRL_ATTR_OPS: nla type=%u, nla len=%u\n", s_nlattr->nla_type, s_nlattr->nla_len);
#endif
						break;
					case CTRL_ATTR_MCAST_GROUPS:
#if 0L
						(void)fprintf(stdout, "CTRL_ATTR_MCAST_GROUPS: nla type=%u, nla len=%u\n", s_nlattr->nla_type, s_nlattr->nla_len);
#endif
						break;
					default:
						(void)fprintf(stdout, "CTRL_ATTR_UNKNOWN???: nla type=%u, nla len=%u\n", s_nlattr->nla_type, s_nlattr->nla_len);
						break;
				}

				s_offset += (size_t)NLA_ALIGN(s_nlattr->nla_len);
			}
		}
	}while(0);

	if(s_family_id == ((uint16_t)0u)) {
		(void)fprintf(stderr, "unknown family id !\n");
		
		free(s_buffer);
		(void)close(s_socket);

		return(EXIT_FAILURE);
	}

	do {
		DIR *s_dir;

		s_dir = opendir("/proc");
		if(s_dir != ((DIR *)0)) {
			struct dirent *s_dirent;
			int s_id;

			for(;;) {
				s_dirent = readdir(s_dir);
				if(s_dirent == ((struct dirent *)0)) {
					break;
				}

				s_id = atoi(s_dirent->d_name);
				if(s_id <= 0) {
					continue;
				}

				do {
					uint32_t s_tid;

					struct genlmsghdr *s_genlmsghdr;
					size_t s_message_size;

					s_message_size = hwport_generate_netlink_message(
							s_buffer,
							s_buffer_size,
							s_family_id /* type */,
							NLM_F_REQUEST /* | NLM_F_ACK */,
							0u /* seq */,
							(unsigned int)s_mypid,
							(const void *)0,
							sizeof(struct genlmsghdr)
							);
					s_genlmsghdr = (struct genlmsghdr *)NLMSG_DATA(s_buffer);
					s_genlmsghdr->cmd = (uint8_t)TASKSTATS_CMD_GET;
					s_genlmsghdr->version = (uint8_t)TASKSTATS_GENL_VERSION;

					s_tid = (uint32_t)s_id;
					s_message_size = hwport_append_netlink_attr(
							s_buffer,
							s_buffer_size,
							TASKSTATS_CMD_ATTR_PID,
							(const void *)(&s_tid),
							sizeof(s_tid)
							);

					s_send_bytes = send(s_socket, (const void *)s_buffer, (size_t)s_message_size, MSG_NOSIGNAL);
					if(s_send_bytes == ((ssize_t)(-1))) {
						perror("send nlmsg");
					}
					else {
						struct nlmsghdr *s_nlmsghdr;
						size_t s_offset;
						struct nlattr *s_nlattr;

						(void)recv(s_socket, memset(s_buffer, 0, s_buffer_size), s_buffer_size, MSG_NOSIGNAL);

						s_nlmsghdr = (struct nlmsghdr *)s_buffer;
						s_genlmsghdr = (struct genlmsghdr *)NLMSG_DATA(s_buffer);
#if 0L
						(void)fprintf(stdout, "gen cmd=%lXH\n", (unsigned long)s_genlmsghdr->cmd);
						(void)fprintf(stdout, "gen version=%lXH\n", (unsigned long)s_genlmsghdr->version);
#endif

						for(s_offset = (size_t)(NLMSG_HDRLEN + NLMSG_ALIGN(sizeof(struct genlmsghdr)));s_offset < s_nlmsghdr->nlmsg_len;) {
							if((s_offset + ((size_t)NLA_HDRLEN)) > ((size_t)s_nlmsghdr->nlmsg_len)) {
								/* attibute header 만큼이 남지 않았음. */
								break;
							}
							s_nlattr = hwport_peek_f(struct nlattr *, s_buffer, s_offset);
							if(s_nlattr->nla_len < ((size_t)NLA_HDRLEN)) {
								/* attribute length 가 최소 크기를 만족하지 못함 */
								break;
							}
							if((s_offset + ((size_t)NLA_ALIGN(s_nlattr->nla_len))) > ((size_t)s_nlmsghdr->nlmsg_len)) {
								/* attribute 공간이 nlmsg를 넘어섬 */
								break;
							}

							switch(s_nlattr->nla_type) {
								case TASKSTATS_TYPE_AGGR_PID:
#if 0L
									(void)fprintf(stdout, "TASKSTATS_TYPE_AGGR_PID: nla type=%u, nla len=%u\n", s_nlattr->nla_type, s_nlattr->nla_len);
#endif
									do {
										size_t s_offset2;
										struct nlattr *s_nlattr2;
										struct taskstats *s_taskstats;

										for(s_offset2 = (size_t)NLA_HDRLEN;s_offset2 < s_nlattr->nla_len;) {
											if((s_offset2 + ((size_t)NLA_HDRLEN)) > ((size_t)s_nlattr->nla_len)) {
												/* attibute header 만큼이 남지 않았음. */
												break;
											}
											s_nlattr2 = hwport_peek_f(struct nlattr *, s_nlattr, s_offset2);
											if(s_nlattr2->nla_len < ((size_t)NLA_HDRLEN)) {
												/* attribute length 가 최소 크기를 만족하지 못함 */
												break;
											}
											if((s_offset2 + ((size_t)NLA_ALIGN(s_nlattr2->nla_len))) > ((size_t)s_nlattr->nla_len)) {
												/* attribute 공간이 nlmsg를 넘어섬 */
												break;
											}

											switch(s_nlattr2->nla_type) {
												case TASKSTATS_TYPE_PID:
#if 0L
													(void)fprintf(stdout, "TASKSTATS_TYPE_PID is %d\n", hwport_peek_const_type(int, s_nlattr2, NLA_HDRLEN));
#endif
													break;
												case TASKSTATS_TYPE_TGID:
#if 0L
													(void)fprintf(stdout, "TASKSTATS_TYPE_TGID is %d\n", hwport_peek_const_type(int, s_nlattr2, NLA_HDRLEN));
#endif
													break;
												case TASKSTATS_TYPE_STATS:
													s_taskstats = hwport_peek_f(struct taskstats *, s_nlattr2, NLA_HDRLEN);
													(void)fprintf(
														stdout,
													       	"TASKSTATS_TYPE_STATS: pid=%5lu, CPU=%08lXH, R=%8llu, W=%8llu, CW=%8llu, comm=\"%s\"\n",
														(unsigned long)s_tid,
														(unsigned long)s_taskstats->cpu_count,
														(unsigned long long)s_taskstats->read_bytes,
														(unsigned long long)s_taskstats->write_bytes,
														(unsigned long long)s_taskstats->cancelled_write_bytes,
														(char *)(&s_taskstats->ac_comm[0])
														);
													break;
												default:
													(void)fprintf(stdout, "UNKNOWN ATTRIBUTE???: nla2 type=%u, nla2 len=%u\n", s_nlattr2->nla_type, s_nlattr2->nla_len);
													break;
											}

											s_offset2 += (size_t)NLA_ALIGN(s_nlattr2->nla_len);
										}
									}while(0);

									break;
								default:
									(void)fprintf(stdout, "UNKNOWN ATTRIBUTE???: nla type=%u, nla len=%u\n", s_nlattr->nla_type, s_nlattr->nla_len);
									break;
							}

							s_offset += (size_t)NLA_ALIGN(s_nlattr->nla_len);
						}
					}
				}while(0);
			}

			closedir(s_dir);
		}
	}while(0);


	free(s_buffer);
	(void)close(s_socket);

	(void)fprintf(stdout, "\nEnd of %s\n", basename(s_argv[0]));

	return(EXIT_SUCCESS);
}

/* vim:set noexpandtab tabstop=4 shiftwidth=4 softtabstop=4 autoindent cindent smarttab fileencoding=utf8: */
/* End of source */
