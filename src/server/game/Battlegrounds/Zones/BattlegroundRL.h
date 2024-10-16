/*
* This file is part of Project SkyFire https://www.projectskyfire.org.
* See LICENSE.md file for Copyright information
*/

#ifndef SF_BATTLEGROUNDRL_H
#define SF_BATTLEGROUNDRL_H

#include "Battleground.h"

enum BattlegroundRLObjectTypes
{
    BG_RL_OBJECT_DOOR_1 = 0,
    BG_RL_OBJECT_DOOR_2 = 1,
    BG_RL_OBJECT_BUFF_1 = 2,
    BG_RL_OBJECT_BUFF_2 = 3,
    BG_RL_OBJECT_MAX = 4
};

enum BattlegroundRLObjects
{
    BG_RL_OBJECT_TYPE_DOOR_1 = 185918,
    BG_RL_OBJECT_TYPE_DOOR_2 = 185917,
    BG_RL_OBJECT_TYPE_BUFF_1 = 184663,
    BG_RL_OBJECT_TYPE_BUFF_2 = 184664
};

class BattlegroundRL : public Battleground
{
public:
    BattlegroundRL();
    ~BattlegroundRL() { }

    /* inherited from BattlegroundClass */
    void AddPlayer(Player* player) OVERRIDE;
    void Reset() OVERRIDE;
    void FillInitialWorldStates(WorldStateBuilder& builder) OVERRIDE;
    void StartingEventCloseDoors() OVERRIDE;
    void StartingEventOpenDoors() OVERRIDE;

    void RemovePlayer(Player* player, uint64 guid, uint32 team) OVERRIDE;
    void HandleAreaTrigger(Player* Source, uint32 Trigger) OVERRIDE;
    bool SetupBattleground() OVERRIDE;
    void HandleKillPlayer(Player* player, Player* killer) OVERRIDE;
};
#endif
