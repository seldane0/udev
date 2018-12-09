#include <linux/netlink.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> /* for fcntl() */
#include <stdio.h>

#define BUF_SZ 8192
char    buf[BUF_SZ];
int     l;

int
setup_rcvbuf(int fd)
{
    int ret, sz, rbuf;
    socklen_t len;

    /* Let's get the default RCVBUF. */
    len = sizeof(rbuf);
    ret = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rbuf, &len);
    if (ret < 0) {
        perror("getsockopt() failed.\n");
        return 1;
    }
    printf("Default rcvbuf=%d\n", rbuf);

    /* Now let's set it to what we want. */
    sz = BUF_SZ;
    ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &sz, sizeof(sz));
    if (ret < 0) {
        perror("getsockopt() failed.\n");
        return 1;
    }

    /* Let's get it again. */
    len = sizeof(rbuf);
    ret = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rbuf, &len);
    if (ret < 0) {
        perror("getsockopt() failed.\n");
        return 1;
    }
    printf("New rcvbuf=%d\n", rbuf);
    return 0; 
}

int
make_nonblock(int fd)
{
    int flag;

    flag = fcntl(fd, F_GETFL);
    if (fcntl(fd, F_SETFL, flag | O_NONBLOCK) < 0) {
        perror("fcntl() call failed.\n");
        return 1;
    }

    return 0;
}

int
main(int argc, char *argv[])
{
    int fd, ret, i;
    struct sockaddr_nl sa;

    fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (fd == -1) {
        perror("socket() call failed.\n");
        return 1;
    }
    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_pid = getpid();
    sa.nl_groups = -1;

    /* Set the recv buffer size. */
    if (setup_rcvbuf(fd)) {
        return 1;
    }

    /* Make socket non-blocking. */

    ret = bind(fd, (struct sockaddr *)&sa, sizeof(sa));
    if (ret == -1) {
        perror("bind() failed.");
        return 2;
    }

    while (1) {
        l = recv(fd, buf, BUF_SZ, 0);
        printf("*** Read %d bytes\n", l);
        for (i=0; i < l; i++) {
            if (buf[i] == 0) {
                printf("[0x00]\n");
            } else if ((buf[i] < 33) || (buf[i] > 126)) {
                printf("[0x%02hhx]", buf[i]);
            } else {
                printf("%c", buf[i]);
            }
        }
        printf("*** <end>\n");
    }

    close(fd);
    return 0;
}
