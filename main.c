#include <linux/if_ether.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool
sniff_setup(int sd, const char* iface)
{
    int ifidx = if_nametoindex(iface);
    if (! ifidx) {
        fprintf(stderr, "interface %s has no index: %s\n", iface, strerror(errno));
        return false;
    }
    {
        struct sockaddr_ll addr = {
            .sll_family = AF_PACKET,
            .sll_protocol = 0,
            .sll_ifindex = ifidx,
        };
        if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            fprintf(stderr, "could not bind socket to interface: %s\n", strerror(errno));
            return false;
        }
    }
    {
        struct packet_mreq mreq = {
            .mr_ifindex = ifidx,
            .mr_type = PACKET_MR_PROMISC,
        };
        if (setsockopt(sd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
            fprintf(stderr, "could not enter promiscuous mode: %s\n", strerror(errno));
            return false;
        }
    }
    return true;
}

static bool
sniff_perform(int sd)
{
    while (true) {
        uint8_t buf[1500];
        ssize_t size = recvfrom(sd, buf, sizeof(buf), MSG_TRUNC, NULL, NULL);
        printf("[%ld]", size);
        printf("\n");
    }
    return true;
}

static bool
sniff(const char *iface)
{
    int sd = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
    if (sd < 0) {
        fprintf(stderr, "socket() failed: %s\n", strerror(errno));
        return false;
    }
    bool result = sniff_setup(sd, iface);
    result = result && sniff_perform(sd);
    close(sd);
    return result;
}

int
main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s interface\n", argv[0]);
        return 1;
    }
    return sniff(argv[1]) ? 0 : 1;
}
