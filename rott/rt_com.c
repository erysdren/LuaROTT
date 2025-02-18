/*
Copyright (C) 1994-1995 Apogee Software, Ltd.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

#include "rt_def.h"
#include "_rt_com.h"
#include "rt_com.h"
#include "rt_util.h"
#include "rt_in.h"
#include "rt_crc.h"
#include "rt_playr.h"
#include "isr.h"
#include "rt_msg.h"
#include "rottnet.h"
#include "rt_main.h"
#include "rt_net.h"
#include "rt_draw.h"
//#include "rt_ser.h"
#include "ipxnet.h"

#include <SDL3_net/SDL_net.h>

// GLOBAL VARIABLES

// Same as in real mode
rottcom_t   *	rottcom;
int badpacket;
int consoleplayer;
byte ROTTpacket[MAXCOMBUFFERSIZE];
int controlsynctime;

// LOCAL VARIABLES

static int    ComStarted=false;
static int    transittimes[MAXPLAYERS];

void SyncTime( int client );
void SetTransitTime( int client, int time );

SDLNet_Server *tcp_server = NULL;
SDLNet_StreamSocket *tcp_client = NULL;
SDLNet_Address *tcp_client_addr = NULL;
SDLNet_Address *tcp_server_addr = NULL;
static const char *hostname = NULL;
static Uint16 port = 34858;

static void ReadUDPPacket()
{
	int i;

	rottcom->remotenode = -1;

	if (IsServer)
	{
		Sint64 oldesttime = SDL_GetTicks();
		int oldestnode = -1;

		for (i = 0; i < rottcom->numplayers; i++)
		{
			if (nodeadr[i].sock != NULL && nodeadr[i].time < oldesttime)
			{
				oldesttime = nodeadr[i].time;
				oldestnode = i;
			}
		}

		// no packets
		if (oldestnode == -1)
			return;

		// read from socket
		rottcom->datalength = SDLNet_ReadFromStreamSocket(nodeadr[oldestnode].sock, rottcom->data, MAXPACKETSIZE);
		if (rottcom->datalength == -1)
			Error("SERVER: Failed: %s", SDL_GetError());
		if (rottcom->datalength == 0)
			return;

		nodeadr[oldestnode].time = SDL_GetTicks();

		// set remotenode to the sender of the packet
		rottcom->remotenode = oldestnode+1;
	}
	else
	{
		rottcom->datalength = SDLNet_ReadFromStreamSocket(tcp_client, rottcom->data, MAXPACKETSIZE);
		if (rottcom->datalength == -1)
			Error("CLIENT: Failed: %s", SDL_GetError());
		if (rottcom->datalength == 0)
			return;

		// set remotenode to the sender of the packet
		for (i=0 ; i<=rottcom->numplayers ; i++)
			if (nodeadr[i].sock == tcp_client)
				rottcom->remotenode = i;
	}
}

static void WriteUDPPacket()
{
	if (rottcom->remotenode == MAXNETNODES)
	{
		/* broadcast packet */
		for (int i = 0; i < rottcom->numplayers; i++)
		{
			nodeadr_t *node = &nodeadr[i];
			if (!node->sock)
				continue;
			if (!SDLNet_WriteToStreamSocket(node->sock, rottcom->data, rottcom->datalength))
				Error("Failed to send packet: %s\n", SDL_GetError());
		}
	}
	else
	{
		nodeadr_t *node = &nodeadr[rottcom->remotenode];
		if (!node->sock)
			Error("Invalid destination %d\n", rottcom->remotenode);
		if (!SDLNet_WriteToStreamSocket(node->sock, rottcom->data, rottcom->datalength))
			Error("Failed to send packet: %s\n", SDL_GetError());
	}

#if 0
	nodeadr_t *node = &nodeadr[rottcom->remotenode];
	if (!node->sock)
		Error("Invalid destination %d\n", rottcom->remotenode);
	if (!SDLNet_WriteToStreamSocket(node->sock, rottcom->data, rottcom->datalength))
		Error("Failed to send packet: %s\n", SDL_GetError());
	// SDLNet_WaitUntilStreamSocketDrained(node->sock, 50);
#endif
}

/*
===============
=
= InitROTTNET
=
===============
*/

void InitROTTNET(void)
{
	int netarg;

	if (ComStarted==true)
		return;

	ComStarted=true;

	/*
	server-specific options:
	-net: enables netplay
	-server: run as rott server (default port 34858)
	-port: select a non-default port for server
	-host: select a non-default ip address to bind to
	-standalone: run as standalone server
	-remoteridicule: enable remote ridicule
	-players: number of players to expect

	client-specific options:
	-master: request to have control
	-net: specifies the host to connect to
	-port: select a non-default port to connect to
	*/

	if (!SDL_WasInit(SDL_INIT_EVENTS))
		SDL_Init(SDL_INIT_EVENTS);

	if (!SDLNet_Init())
		Error("SDLNet: %s", SDL_GetError());

	rottcom = (rottcom_t *) malloc (sizeof(rottcom_t));
	memset(rottcom, 0, sizeof(rottcom_t));
        
	rottcom->ticstep = 1;
	rottcom->gametype = 1;
	rottcom->remotenode = -1;

	/* get port */
	netarg = CheckParm("port");
	if (netarg && netarg < _argc-1)
		port = atoi(_argv[netarg+1]);

	if (CheckParm("server")) {
		IsServer = true;
		if (CheckParm("standalone")) {
			rottcom->consoleplayer = 0;
			standalone = true;
		} else {
			rottcom->consoleplayer = 1;
		}

		if (CheckParm("remoteridicule")) {
			rottcom->remoteridicule = 1;
		} else {
			rottcom->remoteridicule = 0;
		}

		netarg = CheckParm("players");
		if (netarg && netarg < _argc-1) {
			rottcom->numplayers = atoi(_argv[netarg+1]);
		} else {
			rottcom->numplayers = 2;
		}

		/* create server */
		tcp_server = SDLNet_CreateServer(NULL, port);
		if (!tcp_server)
			Error("SERVER: Failed: %s", SDL_GetError());

		SDL_Log("Listening on port %d", port);

		rottcom->client = 0;
	} else {
		IsServer = false;
		rottcom->client = 1;

		netarg = CheckParm("net");
		if (netarg && netarg < _argc-1)
			hostname = _argv[netarg+1];

		if (!hostname)
			Error("No address specified with -net\n");

		tcp_server_addr = SDLNet_ResolveHostname(hostname);
		if (tcp_server_addr)
		{
			if (SDLNet_WaitUntilResolved(tcp_server_addr, -1) == -1)
			{
				SDLNet_UnrefAddress(tcp_server_addr);
				tcp_server_addr = NULL;
			}
		}

		if (!tcp_server_addr)
			Error("CLIENT: Failed: %s\n", SDL_GetError());

		/* create client */
		tcp_client = SDLNet_CreateClient(tcp_server_addr, port);
		if (!tcp_client)
			Error("CLIENT: Failed: %s", SDL_GetError());

		// wait for connection
		SDL_Log("CLIENT: Connecting to %s port %d", SDLNet_GetAddressString(tcp_server_addr), port);
		if (SDLNet_WaitUntilConnected(tcp_client, -1) == -1)
			Error("CLIENT: Failed: %s", SDL_GetError());

		/* consoleplayer will be initialized after connecting */
		/* numplayers will be initialized after connecting */
		/* remoteridicule will be initialized after connecting */
	}

	LookForNodes();

        /* client-server negotiation protocol, as inspired by ipxsetup.c */
        /*
          Best case:
          client sends a HereIAm packet.
          server replies with a YouAre packet, and broadcasts an Info packet
            to the rest, indicating that the new player has joined.
          client replies with an IAm packet
          until all players have joined, the server broadcasts an Info packet
            to the rest, indicating that another player is needed.
          once server has enough players, it broadcasts an AllDone packet.
          
          In detail:
          Client state: HereIAm (initial connection)
          Client sends HereIAm packet, waits for YouAre from server.
          At timeout, Client resends HereIam
          When client receives YouAre, client switches to IAm state
          
          Client state: IAm
          Client sends IAm packet, waits for IAmAck from server.
          At timeout, Client resends IAm
          When client receives IAmAck, client switches to WaitForDone state.
          
          Client state: WaitForDone
          Client waits for AllDone packet.
          When client receives AllDone, it sends an AllDoneAck.
         */

	remoteridicule = rottcom->remoteridicule;

	if (rottcom->ticstep != 1)
		remoteridicule = false;

	if (!quiet)
	{
		if (remoteridicule == true)
			printf("ROTTNET: LIVE Remote Ridicule Enabled\n");

		printf("ROTTNET: consoleplayer=%ld\n",(long)rottcom->consoleplayer);
	}
}

/*
===============
=
= QuitROTTNET
=
===============
*/

void QuitROTTNET(void)
{
	if (tcp_server)
		SDLNet_DestroyServer(tcp_server);
	if (tcp_client)
		SDLNet_DestroyStreamSocket(tcp_client);
	if (tcp_server_addr)
		SDLNet_UnrefAddress(tcp_server_addr);
	SDLNet_Quit();
}

/*
================
=
= ReadPacket
=
================
*/

boolean ReadPacket (void)
{
   unsigned short   crc;
   unsigned short   sentcrc;

   // Set command (Get Packet)
	rottcom->command=CMD_GET;

   badpacket = 0;

   // Check to see if a packet is ready

	ReadUDPPacket();

   // Is it ready?

   if (rottcom->remotenode!=-1)
      {
      // calculate crc on packet
      crc=CalculateCRC ((byte *)&rottcom->data[0], rottcom->datalength-sizeof(unsigned short));

      // get crc inside packet
      sentcrc=*((unsigned short *)(&rottcom->data[rottcom->datalength-sizeof(unsigned short)]));

      // are the crcs the same?
      if (crc!=sentcrc)
         {
         badpacket=1;
         SDL_Log("BADPKT at %d\n",GetTicCount());
         }
      if (networkgame==false)
         {
         rottcom->remotenode=server;
         }
      else
         {
         if ((IsServer==true) && (rottcom->remotenode>0))
            rottcom->remotenode--;
         }
      memcpy(&ROTTpacket[0], &rottcom->data[0], rottcom->datalength);

      SDL_Log( "ReadPacket: time=%ld size=%ld src=%ld type=%d\n",GetTicCount(), rottcom->datalength,rottcom->remotenode,rottcom->data[0]);

      return true;
      }
   else // Not ready yet....
      return false;
}


/*
=============
=
= WritePacket
=
=============
*/

void WritePacket (void * buffer, int len, int destination)
{
   unsigned short      crc;

    if (!rottcom)
        return;

   // set send command
	rottcom->command=CMD_SEND;

   // set destination
   rottcom->remotenode=destination;

   if (len>(int)(MAXCOMBUFFERSIZE-sizeof(unsigned short)))
      {
      Error("WritePacket: Overflowed buffer (%d bytes)\n", len);
      }

   // copy local buffer into realmode buffer
   memcpy((byte *)&(rottcom->data[0]),(byte *)buffer,len);

   // calculate CRC
   crc=CalculateCRC (buffer, len);

   // put CRC into realmode buffer packet
   *((unsigned short *)&rottcom->data[len])=crc;

   // set size of realmode packet including crc
   rottcom->datalength=len+sizeof(unsigned short);

   if (*((byte *)buffer)==0)
       Error("Packet type = 0\n");

   if (networkgame==true)
      {
      if (IsServer==true)
         rottcom->remotenode++; // server fix-up
      }

     SDL_Log( "WritePacket: time=%ld size=%ld src=%ld type=%d\n",GetTicCount(),rottcom->datalength,rottcom->remotenode,rottcom->data[0]);
   // Send It !
	WriteUDPPacket();
}




/*
=============
=
= ValidSyncPacket
=
=============
*/
boolean ValidSyncPacket ( synctype * sync )
{
   if (ReadPacket() && (badpacket==0))
      {
      if (((syncpackettype *)&(ROTTpacket[0]))->type==COM_SYNC)
         {
         memcpy(&(sync->pkt),&(ROTTpacket[0]),sizeof(sync->pkt));
         return true;
         }
      }
   return false;
}

/*
=============
=
= SendSyncPacket
=
=============
*/
void SendSyncPacket ( synctype * sync, int dest)
{
   sync->pkt.type=COM_SYNC;
   sync->sendtime=GetTicCount();
   WritePacket( &(sync->pkt.type) , sizeof(syncpackettype) , dest );
}


/*
=============
=
= SlavePhaseHandler
=
=============
*/

boolean SlavePhaseHandler( synctype * sync )
{
   boolean done;

   done=false;

   switch (sync->pkt.phase)
      {
      case SYNC_PHASE1:
         break;
      case SYNC_PHASE2:
         ISR_SetTime(sync->pkt.clocktime);
         break;
      case SYNC_PHASE3:
         sync->pkt.clocktime=GetTicCount();
         break;
      case SYNC_PHASE4:
         ISR_SetTime(GetTicCount()-sync->pkt.delta);
         sync->pkt.clocktime=GetTicCount();
         break;
      case SYNC_PHASE5:
         ISR_SetTime(GetTicCount()-sync->pkt.delta);
         sync->sendtime=sync->pkt.clocktime;
         done=true;
         break;
      }
   return done;
}



/*
=============
=
= MasterPhaseHandler
=
=============
*/

boolean MasterPhaseHandler( synctype * sync )
{
   boolean done;

   done=false;

   switch (sync->pkt.phase)
      {
      case SYNC_PHASE1:
         sync->pkt.phase=SYNC_PHASE2;
         sync->pkt.clocktime=GetTicCount()+(sync->deltatime>>1);
         break;
      case SYNC_PHASE2:
         sync->pkt.phase=SYNC_PHASE3;
         break;
      case SYNC_PHASE3:
         sync->pkt.delta=sync->pkt.clocktime-GetTicCount()+(sync->deltatime>>1);
         sync->pkt.phase=SYNC_PHASE4;
         break;
      case SYNC_PHASE4:
         sync->pkt.phase=SYNC_PHASE5;
         sync->pkt.delta=sync->pkt.clocktime-GetTicCount()+(sync->deltatime>>1);
         sync->sendtime=GetTicCount()+SYNCTIME;
         sync->pkt.clocktime=sync->sendtime;
         done=true;
         break;
      }
   return done;
}


/*
=============
=
= ComSetTime
=
=============
*/

void ComSetTime ( void )
{
   int i;
   syncpackettype * syncpacket;
   boolean done=false;

   syncpacket=(syncpackettype *)SafeMalloc(sizeof(syncpackettype));


   // Sync clocks

   if (networkgame==true)
      {
      if (IsServer==true)
         {
         for (i=0;i<numplayers;i++)
            {
            if (PlayerInGame(i)==false)
               continue;
            if (standalone==true)
               SyncTime(i);
            else if (i!=consoleplayer)
               SyncTime(i);
            if (standalone==true)
               printf("ComSetTime: player#%ld\n",(long)i);
            }
         }
      else
         {
         SyncTime(0);
         }
      }
   else // Modem 2-player game
      {
      if (consoleplayer==0)
         SyncTime(server);
      else
         SyncTime(server);
      }

   if ( ( (networkgame==true) && (IsServer==true) ) ||
        ( (networkgame==false) && (consoleplayer==0) )
      ) // Master/Server
      {
      int nump;
      int time;

      syncpacket->type=COM_START;
      syncpacket->clocktime=GetTicCount();
      controlsynctime=syncpacket->clocktime;
      if (networkgame==true)
         nump=numplayers;
      else
         nump=1;

      time = GetTicCount();

      for (i=0;i<nump;i++)
         {
         WritePacket( &(syncpacket->type) , sizeof(syncpackettype) , i );
         }

      while (GetTicCount()<time+(VBLCOUNTER/4)) ;

      for (i=0;i<nump;i++)
         {
         WritePacket( &(syncpacket->type) , sizeof(syncpackettype) , i );
         }

      if (standalone==true)
         printf("ComSetTime: Start packets sent\n");
      }
   else // Slave/Client
      {
      while (done==false)
         {
         AbortCheck("ComSetTime aborted as client");

         if (ReadPacket() && (badpacket==0))
            {
            memcpy(syncpacket,&(ROTTpacket[0]),sizeof(syncpackettype));
            if (syncpacket->type==COM_START)
               {
               controlsynctime=syncpacket->clocktime;
               done=true;
               }
            }
         }
      }
   if (standalone==false)
      {
      AddMessage("All players synched.",MSG_SYSTEM);
      ThreeDRefresh();
      }
   SafeFree(syncpacket);

//
// flush out any extras
//
   while (GetTicCount()<controlsynctime+VBLCOUNTER)
      {
      ReadPacket ();
      }
}

/*
=============
=
= InitialMasterSync
=
=============
*/
void InitialMasterSync ( synctype * sync, int client )
{
   boolean done=false;
   int i;

   if (networkgame==true)
      {
      for (i=0;i<numplayers;i++)
         {
         if (i<=client)
            continue;
         sync->pkt.type=COM_SYNC;
         sync->pkt.phase=SYNC_MEMO;
         sync->pkt.clocktime=client;
         SendSyncPacket(sync,i);
         }
      }

   // Initialize send time so as soon as we enter the loop, we send

   sync->sendtime=GetTicCount()-SYNCTIME;

   while (done==false)
      {
      sync->pkt.phase=SYNC_PHASE0;

      AbortCheck("Initial sync aborted as master");
	   if ((sync->sendtime+SYNCTIME) <= GetTicCount())
         SendSyncPacket(sync,client);
      if (ValidSyncPacket(sync)==true)
         {
         if (sync->pkt.phase==SYNC_PHASE0)
            {
            int time=GetTicCount();

            while (time+SYNCTIME>GetTicCount())
               {
               ReadPacket();
               }
            time=GetTicCount();
            while (time+SYNCTIME>GetTicCount()) {}
            done=true;
            }
         }
      }
}

/*
=============
=
= InitialSlaveSync
=
=============
*/
void InitialSlaveSync ( synctype * sync )
{
   boolean done=false;

   while (done==false)
      {
      AbortCheck("Initial sync aborted as slave");
      if (ValidSyncPacket(sync)==true)
         {
         if (sync->pkt.phase==SYNC_MEMO)
            {
            char str[50]="Server is synchronizing player ";
            char str2[10];

            strcat(str,itoa(sync->pkt.clocktime+1,str2,10));
            AddMessage(str,MSG_SYSTEM);
            ThreeDRefresh();
            }
         if (sync->pkt.phase==SYNC_PHASE0)
            {
            int time=GetTicCount();

            SendSyncPacket(sync,server);
            while (time+SYNCTIME>GetTicCount())
               {
               ReadPacket();
               }
            done=true;
            }
         }
      }
   AddMessage("Server is synchronizing your system",MSG_SYSTEM);
   ThreeDRefresh();
}


/*
=============
=
= SyncTime
=
=============
*/

void SyncTime( int client )
{
   int dtime[NUMSYNCPHASES];
   boolean done;
   int i;
   synctype * sync;

   sync=(synctype *)SafeMalloc(sizeof(synctype));

   if ( ((networkgame==true) && (IsServer==true)) ||
         ((networkgame==false) && (consoleplayer==0)) )
      {
      // Master

      InitialMasterSync ( sync, client );

      done=false;

      // Initial setup for Master
      // Initialize send time so as soon as we enter the loop, we send

      sync->pkt.phase=SYNC_PHASE1;
      sync->sendtime=GetTicCount()-SYNCTIME;

      while (done==false)
         {
         // Master

         AbortCheck("SyncTime aborted as master");

		   if ((sync->sendtime+SYNCTIME) <= GetTicCount())
            SendSyncPacket(sync,client);

         while (ValidSyncPacket(sync)==true)
            {

            // find average delta

            sync->deltatime=0;

            // calculate last delta

            dtime[sync->pkt.phase]=GetTicCount()-sync->sendtime;

            for (i=0;i<=sync->pkt.phase;i++)
               sync->deltatime+=dtime[i];
            if (i!=0)
               sync->deltatime/=i;
            else
               Error("SyncTime: this should not happen\n");

            done = MasterPhaseHandler( sync );

            SendSyncPacket(sync,client);

            }
         }
      }
   else
      {
      // Slave

      InitialSlaveSync ( sync );

      done=false;

      while (done==false)
         {
         // Slave

         AbortCheck("SyncTime aborted as slave");

         while (ValidSyncPacket(sync)==true)
            {
            done = SlavePhaseHandler( sync );

            if (done==false)
               SendSyncPacket(sync,server);

            }
         }
      }

   while (sync->sendtime > GetTicCount())
      {
      while (ReadPacket()) {}
      }
   while ((sync->sendtime+SYNCTIME) > GetTicCount())
      {
      }

   if ( ((networkgame==true) && (IsServer==true)) ||
         ((networkgame==false) && (consoleplayer==0)) )
      SetTransitTime( client, (sync->deltatime>>1));

   SafeFree(sync);
}

/*
=============
=
= SetTransitTime
=
=============
*/

void SetTransitTime( int client, int time )
{
   transittimes[client]=time;
}

/*
=============
=
= GetTransitTime
=
=============
*/

int GetTransitTime( int client )
{
   return transittimes[client];
}

