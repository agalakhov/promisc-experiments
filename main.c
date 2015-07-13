#include <net/if.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <unistd.h>

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool
sniff_setup(int sd, const char* iface)
{
    (void) sd;
    {
        int ifidx = if_nametoindex(iface);
        if (! ifidx) {
            fprintf(stderr, "interface %s has no index: %s\n", iface, strerror(errno));
            return false;
        }
        
    }
    return true;
}

static bool
sniff_perform(int sd)
{
    (void) sd;
    return true;
}

static bool
sniff(const char *iface)
{
    int sd = socket(AF_PACKET, SOCK_DGRAM, IPPROTO_UDP);
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
