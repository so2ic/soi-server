#ifndef PACKET_H
#define PACKET_H

/* LIST OF META TYPE
 * 0x01 -> game start
 * 0x02 -> ask to play
 * 0x03 -> send card
 * 0x04 -> send card name
 * 0x05 -> send hp
 * 0x06 -> send mana
 * 0x07 -> send power
 * 0x08 -> send mastery
 * 0x09 -> send card id
 * 0x0A -> remove card
 * 0x0B -> end turn
 * 0x0C -> increase mastery
 * 0X0D -> deal damage
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
