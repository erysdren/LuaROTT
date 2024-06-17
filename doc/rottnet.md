
# Unofficial ROTT Network Protocol Spec

Here is some basic documentation for the protocol that ROTT uses to communicate
between the client and server for COMM-BAT. This information is sourced from
the GPLv2 source code provided by Apogee Software on December 20th 2002. All
values shown are assumed to be in little-endian byte order.

-- erysdren (it/she/they) 2024.

## Simple Types

- [U8] - Unsigned 8-bit integer (unsigned char)
- [U16] - Unsigned 16-bit integer (unsigned short)
- [I16] - Signed 16-bit integer (signed short)
- [U32] - Unsigned 32-bit integer (unsigned int)
- [I32] - Signed 32-bit integer (signed int)
- [STR] - String with ASCII encoding (char)

## Complex Types

### DESC

| Data Type | Description |
|-----------|-------------|
| U8 | Character |
| U8 | Uniform Color |
| STR | Codename (9 bytes) |

### SPECIALS

| Data Type | Description |
|-----------|-------------|
| I32 | God Mode Time |
| I32 | Dog Mode Time |
| I32 | Shrooms Mode Time |
| I32 | Elasto Mode Time |
| I32 | Asbestos Vest Time |
| I32 | Bullet Proof Vest Time |
| I32 | Gas Mask Time |
| I32 | Mercury Mode Time |
| I32 | God Mode Respawn Time |
| I32 | Dog Mode Respawn Time |
| I32 | Shrooms Mode Respawn Time |
| I32 | Elasto Mode Respawn Time |
| I32 | Asbestos Vest Respawn Time |
| I32 | Bullet Proof Vest Respawn Time |
| I32 | Gas Mask Respawn Time |
| I32 | Mercury Mode Respawn Time |

### BATTLETYPE

| Data Type | Description |
|-----------|-------------|
| U32 | Gravity |
| U32 | Speed |
| U32 | Ammo |
| U32 | Hit Points |
| U32 | Spawn Dangers |
| U32 | Spawn Health |
| U32 | Spawn Weapons |
| U32 | Spawn Mines |
| U32 | Respawn Items |
| U32 | Weapon Persistence |
| U32 | Random Weapons |
| U32 | Friendly Fire |
| U32 | Light Level |
| I32 | Kills |
| I32 | Danger Damage |
| U32 | Time Limit |
| U32 | Respawn Time |

## Packet Structure

Packets always start with a U8 value defining its type. This allows for 256
possible packet types. Packets have a maximum size of 2048 bytes.

## Packet types

## 0x01 | Movement Queue

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |
| I16 | X Velocity |
| I16 | Y Velocity |
| U16 | Z Height (absolute?) |
| U16 | Player Button State |

## 0x02 | Request Packets

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |
| U8 | Number of Packets |

## 0x03 | Fixup Packets

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |
| U8 | Number of Packets |
| * | Packet Data |

## 0x04 | Text

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |
| U8 | Destination Player |
| STR | Text (33 bytes) |

## 0x05 | Pause Game

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |

## 0x06 | Quit Game

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |

## 0x07 | Sync Time

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Sync Time |

## 0x08 | Remote Ridicule

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |
| U8 | Source Player |
| U8 | Unknown |
| U8 | Destination Player |

## 0x0A | Respawn

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |

## 0x0B | Unpause Game

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |

## 0x0C | Server Data

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |
| U8 | Number of Packets |
| * | Packet Data |

## 0x0F | Game Description

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| U8 | Player ID |
| U8 | Violence Level |
| U8 | Product ID |
| U32 | Game Version |
| DESC | Player Description |

## 0x10 | Game Play

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| * | Dummy Data (20 bytes) |

## 0x11 | Game Master

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| U8 | Level Number |
| U16 | Map CRC |
| U8 | Violence Level |
| U8 | Product ID |
| U8 | Mode |
| U32 | Game Version |
| U8 | Teamplay |
| SPECIALS | Specials Times |
| BATTLETYPE | Battle Mode Options |
| STR | Level Filename (20 bytes) |
| I32 | Random Seed |
| U8 | Ludicrous Gibs |
| DESC[] | Array of Player Descriptions (11 if registered, 5 if shareware). |

## 0x12 | Game Acknowledgement

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| U8 | Player ID |

## 0x13 | Game End

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |

## 0x13 | Sync Time

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Sync Time |

## 0x16 | Movement Queue and Sound

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |
| I16 | X Velocity |
| I16 | Y Velocity |
| U16 | Z Height (absolute?) |
| U16 | Player Button State |
| U8 | Sound Packet Type |
| * | Sound Data (256 bytes) |

## 0x17 | Exit Game

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |

## 0x18 | Game End

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |

## 0x19 | Null Movement Queue

| Data Type | Description |
|-----------|-------------|
| U8 | Packet Type |
| I32 | Packet Time |
