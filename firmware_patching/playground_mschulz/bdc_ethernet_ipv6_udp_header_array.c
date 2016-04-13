unsigned char bdc_ethernet_ipv6_udp_header_array[] = {
  0x20, 0x00, 0x00, 0x00,		/* BDC Header */
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,	/* ETHERNET: Destination MAC Address */
  'N', 'E', 'X', 'M', 'O', 'N',		/* ETHERNET: Source MAC Address */
  0x86, 0xDD,				/* ETHERNET: Type */
  0x60, 0x00, 0x00, 0x00,		/* IPv6: Version / Traffic Class / Flow Label */
  0x00, 0x08,				/* IPv6: Payload Length */
  0x88,					/* IPv6: Next Header = UDPLite */
  0x01,					/* IPv6: Hop Limit */
  0xFF, 0x02, 0x00, 0x00,		/* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x00,		/* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x00,		/* IPv6: Source IP */
  0x00, 0x00, 0x00, 0x01,		/* IPv6: Source IP */
  0xFF, 0x02, 0x00, 0x00,		/* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x00,		/* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x00,		/* IPv6: Destination IP */
  0x00, 0x00, 0x00, 0x01,		/* IPv6: Destination IP */
  0xD6, 0xD8,				/* UDPLITE: Source Port = 55000 */
  0xD6, 0xD8,				/* UDPLITE: Destination Port = 55000 */
  0x00, 0x08,				/* UDPLITE: Checksum Coverage */
  0x52, 0x46,				/* UDPLITE: Checksum only over UDPLITE header*/
};

unsigned int bdc_ethernet_ipv6_udp_header_length = sizeof(bdc_ethernet_ipv6_udp_header_array) / 
        sizeof(bdc_ethernet_ipv6_udp_header_array[0]);