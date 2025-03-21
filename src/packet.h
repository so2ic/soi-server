#ifndef PACKET_H
#define PACKET_H

/* LIST OF META TYPE
 * 0x01 -> game start
 * 0x02 -> ask to play
 * 0x03 -> send card
 * 0x04 -> send card name
 *
 * Received on callback
 * 0xFF -> ACK
*/
typedef struct
{
    uint8_t type;
    size_t size; 
} meta_t;

#endif // PACKET_H
