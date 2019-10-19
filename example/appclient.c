#include <stdio.h>
#include <stdlib.h>
#include <enet/enet.h>

int
main (int argc, char ** argv)
{
    ENetHost *client;
    ENetAddress address;
    ENetEvent event;
    ENetPeer *peer;

    if (enet_initialize () != 0) {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }

    /* Create host  */
    client = enet_host_create (NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                2 /* allow up 2 channels to be used, 0 and 1 */,
                0 /* assume any amount of incoming bandwidth */,
                0 /* assume any amount of outgoing bandwidth */);
    if (client == NULL)
    {
        fprintf (stderr, 
                 "An error occurred while trying to create an ENet client host.\n");
        exit (EXIT_FAILURE);
    }

    /* Connect to some.server.net:12340. */
    enet_address_set_host_ip(& address, "127.0.0.1");
    address.port = 12340;

    /* Initiate the connection, allocating the two channels 0 and 1. */
    peer = enet_host_connect (client, & address, 2, 0);    
    if (peer == NULL) {
       fprintf (stderr, 
                "No available peers for initiating an ENet connection.\n");
       exit (EXIT_FAILURE);
    }
 
    puts ("Connecting");
    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service (client, & event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        puts ("Connection to 127.0.0.1:12340 succeeded.");
    } else {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset (peer);
    }

    sleep(10);
    puts ("send data");
    ENetPacket * packet = enet_packet_create ("packet", 
                                              strlen ("packet") + 1, 
                                              ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send (peer, 0, packet);
    enet_host_flush (client);

    sleep(5);

    enet_host_destroy(client);

    atexit (enet_deinitialize);
}
