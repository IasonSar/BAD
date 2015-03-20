#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <getopt.h>
#include <sys/mman.h>
#include <native/task.h>
#include <native/pipe.h>
#include <rtdm/rtcan.h>

extern int optind, opterr, optopt;
static int s = -1, verbose = 0, print = 1;
static nanosecs_rel_t timeout = 0, with_timestamp = 0, timestamp_rel = 0;
RT_TASK rt_task_desc;
#define BUF_SIZ 255
#define MAX_FILTER 16
struct sockaddr_can recv_addr;
struct can_filter recv_filter[MAX_FILTER];
static int filter_count = 0;



void rt_task(void)
{
	int i, ret, count = 0;
	struct can_frame frame;
	struct sockaddr_can addr;
	socklen_t addrlen = sizeof(addr);
	struct msghdr msg;
	struct iovec iov;
	nanosecs_abs_t timestamp, timestamp_prev = 0;
	if (with_timestamp) {
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		msg.msg_name = (void *)&addr;
		msg.msg_namelen = sizeof(struct sockaddr_can);
		msg.msg_control = (void *)&timestamp;
		msg.msg_controllen = sizeof(nanosecs_abs_t);
	}
	if (with_timestamp) {
		iov.iov_base = (void *)&frame;
		iov.iov_len = sizeof(can_frame_t);
		ret = rt_dev_recvmsg(s, &msg, 0);
	} else
		ret = rt_dev_recvfrom(s, (void *)&frame, sizeof(can_frame_t), 0,
				(struct sockaddr *)&addr, &addrlen);
	if (ret < 0) {
		switch (ret) {
			case -ETIMEDOUT:
				if (verbose)
					printf("rt_dev_recv: timed out");
				continue;
			case -EBADF:
				if (verbose)
					printf("rt_dev_recv: aborted because socket was closed");
				break;
			default:
				fprintf(stderr, "rt_dev_recv: %s\n", strerror(-ret));
		}
		break;
	}
	if (print && (count % print) == 0) {
		printf("#%d: (%d) ", count, addr.can_ifindex);
		if (with_timestamp && msg.msg_controllen) {
			if (timestamp_rel) {
				printf("%lldns ", (long long)(timestamp - timestamp_prev));
				timestamp_prev = timestamp;
			} else
				printf("%lldns ", (long long)timestamp);
		}
		if (frame.can_id & CAN_ERR_FLAG)
			printf("!0x%08x!", frame.can_id & CAN_ERR_MASK);
		else if (frame.can_id & CAN_EFF_FLAG)
			printf("<0x%08x>", frame.can_id & CAN_EFF_MASK);
		else
			printf("<0x%03x>", frame.can_id & CAN_SFF_MASK);
		printf(" [%d]", frame.can_dlc);
		if (!(frame.can_id & CAN_RTR_FLAG))
			for (i = 0; i < frame.can_dlc; i++) {
				printf(" %02x", frame.data[i]);
			}
		if (frame.can_id & CAN_ERR_FLAG) {
			printf(" ERROR ");
			if (frame.can_id & CAN_ERR_BUSOFF)
				printf("bus-off");
			if (frame.can_id & CAN_ERR_CRTL)
				printf("controller problem");
		} else if (frame.can_id & CAN_RTR_FLAG)
			printf(" remote request");
		printf("\n");
	}
	count++;
}
