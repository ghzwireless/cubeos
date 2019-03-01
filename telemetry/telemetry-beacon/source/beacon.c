#ifdef YOTTA_CFG_TELEMETRY

#include <telemetry/telemetry.h>
#include "telemetry-beacon/beacon.h"
#include <csp/csp.h>

static uint8_t data_count = 0;

void telemetry_update_beacon(telem_data data)
{
    /* We check the data type here currently but maybe in the
     * future we could sync up telemetry ports with the flags
     * and only send the data that is needed
     */
    if (data.source.dest_flag & TELEMETRY_BEACON_FLAG)
    {
        /* A dirty hack to fill up the whole array with data */
        beacon_data[data_count++ % TELEMETRY_NUM_BEACON] = data;
    }
}

/**
 * Thread for receiving data packets from telemetry service.
 * This function could potentially be autogenerated or stored in a generic
 * telemetry lib. The only special thing about it is a) the port 
 * and b) the function called when data is received
 */
CSP_DEFINE_TASK(beacon_rx_thread)
{
    int running = 1;
    csp_socket_t *socket = csp_socket(CSP_SO_NONE);
    csp_conn_t *conn;
    csp_packet_t *packet;

    csp_bind(socket, TELEMETRY_BEACON_PORT);
    csp_listen(socket, 5);
    telem_data data;

    while(running) {
        if( (conn = csp_accept(socket, 10000)) == NULL ) {
            continue;
        }

        while( (packet = csp_read(conn, 100)) != NULL ) {
            switch( csp_conn_dport(conn) ) {
                case TELEMETRY_BEACON_PORT:
                    data = *((telem_data*)packet->data);
                    csp_buffer_free(packet);
                    telemetry_update_beacon(data);
                    break;
                default:
                    csp_service_handler(conn, packet);
                    break;
            }
        }

        csp_close(conn);
    }
}

/**
 * Thread for "transmitting" beacon data to an external destination (comms/ground)
 */
CSP_DEFINE_TASK(beacon_thread)
{
    telem_data data;
    int i;
    while(1)
    {
        csp_sleep_ms(YOTTA_CFG_TELEMETRY_BEACON_INTERVAL);
        for (i = 0; i < TELEMETRY_NUM_BEACON; i++)
        {
            data = beacon_data[i];
            printf("BEACON:%d:%d:%d\r\n", data.source.source_id, data.timestamp, data.data);
            /** Transmit data to comms here */
        }        
    }
}

#endif