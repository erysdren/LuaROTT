
# Unofficial ROTT Network Protocol Spec

Here is some basic documentation for the protocol that ROTT uses to communicate
between the client and server, both for COMM-BAT and demos. This information is
sourced from the GPLv2 source code provided by Apogee Software on December 20th
2002. All values shown are assumed to be in little-endian byte order.

-- erysdren (it/she/they).

## Types

- [U8] - Unsigned 8-bit integer (unsigned char)
- [U16] - Unsigned 16-bit integer (unsigned short)
- [I16] - Signed 16-bit integer (signed short)
- [U32] - Unsigned 32-bit integer (unsigned int)
- [I32] - Signed 32-bit integer (signed int)
- [STR] - String with ASCII encoding (char)
- [STRZ] - Null-terminated string with ASCII encoding (char)

## Packet Structure

Packets always start with a U8 value defining its type. This allows for 256 possible packet types.

## 0x00 | No-op

No data.

## 0x01 | Movement Queue

| Data Type | Description |
|-----------|-------------|
| U8 | Packet type |
| I32 | Packet time |
| I16 | X axis velocity |
| I16 | Y axis velocity |
| U16 | Unknown |
| U16 | Player button state |
| * | Remote Ridicule sound data |

## 0x02 | Request Packets

| Data Type | Description |
|-----------|-------------|
| U8 | Packet type |
| I32 | Packet time |
| U8 | Number of packets |

## 0x03 | Fixup Packets

| Data Type | Description |
|-----------|-------------|
| U8 | Packet type |
| I32 | Packet time |
| U8 | Number of packets |
| * | Packet data |

#define COM_FIXUP 3
#define COM_TEXT 4
#define COM_PAUSE 5
#define COM_QUIT 6
#define COM_SYNC 7
#define COM_REMRID 8
#define COM_RESPAWN 10
#define COM_UNPAUSE 11
#define COM_SERVER 12
#define COM_START 13
#define COM_GAMEDESC 15
#define COM_GAMEPLAY 16
#define COM_GAMEMASTER 17
#define COM_GAMEACK 18
#define COM_ENDGAME 19
#define COM_SYNCTIME 20
#define COM_SOUNDANDDELTA 22
#define COM_EXIT 23
#define COM_GAMEEND 24
#define COM_DELTANULL 25
