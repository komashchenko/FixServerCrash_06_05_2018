/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"

FSC g_FSC;		/**< Global singleton for extension's main interface */
SMEXT_LINK(&g_FSC);


bool FSC::SDK_OnLoad(char *error, size_t err_max, bool late)
{
	playerhelpers->AddClientListener(&g_FSC);

	return true;
}

void FSC::SDK_OnUnload()
{
	if(m_FDeniedHookID != 0) SH_REMOVE_HOOK_ID(m_FDeniedHookID);
	else playerhelpers->RemoveClientListener(&g_FSC);
}
 
bool FSC::InterceptClientConnect(int client, char *error, size_t maxlength)
{
	INetChannel *pNetChan = (INetChannel *)engine->GetPlayerNetInfo(client);
	if (pNetChan != NULL)
	{
		m_FDeniedHookID = SH_ADD_VPHOOK(INetChannelHandler, FileDenied, pNetChan->GetMsgHandler(), SH_MEMBER(this, &FSC::FileDenied), false);
		
		playerhelpers->RemoveClientListener(&g_FSC);
	}
	
	return true;
}

void FSC::FileDenied(const char *fileName, unsigned int transferID, bool isReplayDemoFile)
{
	if(!fileName || strlen(fileName) < 3)
	{
		IGamePlayer *player = playerhelpers->GetGamePlayer(META_IFACEPTR(IClient)->GetPlayerSlot() + 1);
		
		if(!player->IsInKickQueue())
		{
			smutils->LogMessage(myself, "%s (%s | %s) was kicked for 'Fix Server Crash 06.05.2018'.", player->GetName(), player->GetAuthString(false), player->GetIPAddress());
			
			gamehelpers->AddDelayedKick(player->GetIndex(), player->GetUserId(), "Downloading file failed.");
		}
		
		RETURN_META(MRES_SUPERCEDE);
	}
	RETURN_META(MRES_IGNORED);
}