#include "utils-client.h"

// Found on https://www.linuxquestions.org/questions/programming-9/how-to-change-mac-addres-via-c-code-801613/

int set_mac(char * iface, uint8_t * newMac)
{
	struct ifreq ifr;
	int s;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	assert(s != -1);

	strcpy(ifr.ifr_name, iface);
	ifr.ifr_hwaddr.sa_data[0] = newMac[0];
	ifr.ifr_hwaddr.sa_data[1] = newMac[1];
	ifr.ifr_hwaddr.sa_data[2] = newMac[2];
	ifr.ifr_hwaddr.sa_data[3] = newMac[3];
	ifr.ifr_hwaddr.sa_data[4] = newMac[4];
	ifr.ifr_hwaddr.sa_data[5] = newMac[5];
	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
	if(ioctl(s, SIOCSIFHWADDR, &ifr) == -1){
		perror("IOCTL");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void set_arp_cache(char * ip, uint8_t _new_mac)
{
	char cmd[64];
	sprintf(cmd,"arp -s %s %x:%x:%x:%x:%x:%x",ip,_new_mac[0],_new_mac[1],_new_mac[2],_new_mac[3],_new_mac[4],_new_mac[5]);
	printf("%s",cmd);
	system(cmd);
}

int send_garp(uint8_t * mac)
{
	typedef struct _arp_hdr arp_hdr;
struct _arp_hdr {
  uint16_t htype;
  uint16_t ptype;
  uint8_t hlen;
  uint8_t plen;
  uint16_t opcode;
  uint8_t sender_mac[6];
  uint8_t sender_ip[4];
  uint8_t target_mac[6];
  uint8_t target_ip[4];
};

// Define some constants.

  char *interface;
  int i, frame_length, sd, bytes;
  arp_hdr arphdr;
  uint8_t src_ip[4], src_mac[6], dst_mac[6], ether_frame[IP_MAXPACKET];
  struct sockaddr_in *ipv4;
  struct sockaddr_ll device;
  struct ifreq ifr;

  if (argc != 2) {
    printf ("Usage: %s INTERFACE\n", argv[0]);
    exit (EXIT_FAILURE);
  }

  // Interface to send packet through.
  interface = argv[1];

  // Submit request for a socket descriptor to look up interface.
  if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
    perror ("socket() failed to get socket descriptor for using ioctl()");
    exit (EXIT_FAILURE);
  }

  // Use ioctl() to look up interface name and get its IPv4 address.
  memset (&ifr, 0, sizeof (ifr));
  snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
  if (ioctl (sd, SIOCGIFADDR, &ifr) < 0) {
    perror ("ioctl() failed to get source IP address");
    return (EXIT_FAILURE);
  }

  // Copy source IP address.
  ipv4 = (struct sockaddr_in *)&ifr.ifr_addr;
  memcpy (src_ip, &ipv4->sin_addr, 4 * sizeof (uint8_t));

  // Use ioctl() to look up interface name and get its MAC address.
  memset (&ifr, 0, sizeof (ifr));
  snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
  if (ioctl (sd, SIOCGIFHWADDR, &ifr) < 0) {
    perror ("ioctl() failed to get source MAC address");
    return (EXIT_FAILURE);
  }

  close (sd);

  // Copy source MAC address.
  memcpy (src_mac, ifr.ifr_hwaddr.sa_data, 6 * sizeof (uint8_t));

  // Report source MAC address to stdout.
  printf ("MAC address for interface %s is", interface);
  for (i=0; i<5; i++) {
    printf ("%02x:", src_mac[i]);
  }
  printf ("%02x\n", src_mac[5]);

  // Find interface index from interface name and store index in
  // struct sockaddr_ll device, which will be used as an argument of sendto().
  if ((device.sll_ifindex = if_nametoindex (interface)) == 0) {
    perror ("if_nametoindex() failed to obtain interface index");
    exit (EXIT_FAILURE);
  }
  printf ("Index for interface %s is %i\n", interface, device.sll_ifindex);

  // Set destination MAC address: broadcast address
  memset (dst_mac, 0xff, 6 * sizeof (uint8_t));

  memcpy (&arphdr.sender_ip, src_ip, 4 * sizeof (uint8_t));
  memcpy (&arphdr.target_ip, src_ip, 4 * sizeof (uint8_t));

  // Fill out sockaddr_ll.
  device.sll_family = AF_PACKET;
  memcpy (device.sll_addr, src_mac, 6 * sizeof (uint8_t));
  device.sll_halen = htons (6);

  // ARP header

  // Hardware type (16 bits): 1 for ethernet
  arphdr.htype = htons (1);

  // Protocol type (16 bits): 2048 for IP
  arphdr.ptype = htons (ETH_P_IP);

  // Hardware address length (8 bits): 6 bytes for MAC address
  arphdr.hlen = 6;

  // Protocol address length (8 bits): 4 bytes for IPv4 address
  arphdr.plen = 4;

  // OpCode: 1 for ARP request
  arphdr.opcode = htons (ARPOP_REQUEST);

  // Sender hardware address (48 bits): MAC address
  memcpy (&arphdr.sender_mac, src_mac, 6 * sizeof (uint8_t));

  // Target hardware address (48 bits): zero
  memset (&arphdr.target_mac, 0, 6 * sizeof (uint8_t));

  // Fill out ethernet frame header.

  // Ethernet frame length = ethernet header (MAC + MAC + ethernet type) + ethernet data (ARP header)
  frame_length = 6 + 6 + 2 + ARP_HDRLEN;

  // Destination and Source MAC addresses
  memcpy (ether_frame, dst_mac, 6 * sizeof (uint8_t));
  memcpy (ether_frame + 6, src_mac, 6 * sizeof (uint8_t));

  // Next is ethernet type code (ETH_P_ARP for ARP).
  // http://www.iana.org/assignments/ethernet-numbers
  ether_frame[12] = ETH_P_ARP / 256;
  ether_frame[13] = ETH_P_ARP % 256;

  // Next is ethernet frame data (ARP header).

  // ARP header
  memcpy (ether_frame + ETH_HDRLEN, &arphdr, ARP_HDRLEN * sizeof (uint8_t));

  // Submit request for a raw socket descriptor.
  if ((sd = socket (PF_PACKET, SOCK_RAW, htons (ETH_P_ALL))) < 0) {
    perror ("socket() failed");
    exit (EXIT_FAILURE);
  }

  // Send ethernet frame to socket.
  if ((bytes = sendto (sd, ether_frame, frame_length, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) {
    perror ("sendto() failed");
    exit (EXIT_FAILURE);
  }

  // Close socket descriptor.
  close (sd);

  return (EXIT_SUCCESS);
}