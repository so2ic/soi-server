#ifndef PACKET_H
#define PACKET_H

/* LIST OF META TYPE
 * 0x01 -> game start
*/
typedef struct
{
    uint8_t type;
    size_t size; 
} meta_t;

#endif // PACKET_H
