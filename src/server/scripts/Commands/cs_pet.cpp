/*
* This file is part of Project SkyFire https://www.projectskyfire.org.
* See LICENSE.md file for Copyright information
*/

#include "Chat.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "Pet.h"
#include "Player.h"
#include "ScriptMgr.h"

class pet_commandscript : public CommandScript
{
public:
    pet_commandscript() : CommandScript("pet_commandscript") { }

    std::vector<ChatCommand> GetCommands() const OVERRIDE
    {
        static std::vector<ChatCommand> petCommandTable =
        {
            { "create",  rbac::RBAC_PERM_COMMAND_PET_CREATE,  false, &HandlePetCreateCommand,  "", },
            { "learn",   rbac::RBAC_PERM_COMMAND_PET_LEARN,   false, &HandlePetLearnCommand,   "", },
            { "unlearn", rbac::RBAC_PERM_COMMAND_PET_UNLEARN, false, &HandlePetUnlearnCommand, "", },
        };

        static std::vector<ChatCommand> commandTable =
        {
            { "pet", rbac::RBAC_PERM_COMMAND_PET, false, NULL, "", petCommandTable },
        };
        return commandTable;
    }
    static bool HandlePetCreateCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* player = handler->GetSession()->GetPlayer();
        Creature* creatureTarget = handler->getSelectedCreature();

        if (!creatureTarget || creatureTarget->IsPet() || creatureTarget->GetTypeId() == TypeID::TYPEID_PLAYER)
        {
            handler->PSendSysMessage(LANG_SELECT_CREATURE);
            handler->SetSentErrorMessage(true);
            return false;
        }

        CreatureTemplate const* creatrueTemplate = creatureTarget->GetCreatureTemplate();
        // Creatures with family 0 crashes the server
        if (!creatrueTemplate->family)
        {
            handler->PSendSysMessage("This creature cannot be tamed. (family id: 0).");
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (player->GetPetGUID())
        {
            handler->PSendSysMessage("You already have a pet");
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Everything looks OK, create new pet
        Pet* pet = new Pet(player, PetType::HUNTER_PET);
        if (!pet->CreateBaseAtCreature(creatureTarget))
        {
            delete pet;
            handler->PSendSysMessage("Error 1");
            return false;
        }

        creatureTarget->setDeathState(DeathState::JUST_DIED);
        creatureTarget->RemoveCorpse();
        creatureTarget->SetHealth(0); // just for nice GM-mode view

        pet->SetUInt64Value(UNIT_FIELD_CREATED_BY, player->GetGUID());
        pet->SetUInt32Value(UNIT_FIELD_FACTION_TEMPLATE, player->getFaction());

        if (!pet->InitStatsForLevel(creatureTarget->getLevel()))
        {
            SF_LOG_ERROR("misc", "InitStatsForLevel() in EffectTameCreature failed! Pet deleted.");
            handler->PSendSysMessage("Error 2");
            delete pet;
            return false;
        }

        // prepare visual effect for levelup
        pet->SetUInt32Value(UNIT_FIELD_LEVEL, creatureTarget->getLevel() - 1);

        pet->GetCharmInfo()->SetPetNumber(sObjectMgr->GeneratePetNumber(), true);
        // this enables pet details window (Shift+P)
        pet->InitPetCreateSpells();
        pet->SetFullHealth();

        pet->GetMap()->AddToMap(pet->ToCreature());

        // visual effect for levelup
        pet->SetUInt32Value(UNIT_FIELD_LEVEL, creatureTarget->getLevel());

        player->SetMinion(pet, true);
        pet->SavePetToDB(PET_SAVE_AS_CURRENT);
        player->PetSpellInitialize();

        return true;
    }

    static bool HandlePetLearnCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();
        Pet* pet = player->GetPet();

        if (!pet)
        {
            handler->PSendSysMessage("You have no pet");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 spellId = handler->extractSpellIdFromLink((char*)args);

        if (!spellId || !sSpellMgr->GetSpellInfo(spellId))
            return false;

        // Check if pet already has it
        if (pet->HasSpell(spellId))
        {
            handler->PSendSysMessage("Pet already has spell: %u", spellId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        // Check if spell is valid
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
        if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo))
        {
            handler->PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spellId);
            handler->SetSentErrorMessage(true);
            return false;
        }

        pet->learnSpell(spellId);

        handler->PSendSysMessage("Pet has learned spell %u", spellId);
        return true;
    }

    static bool HandlePetUnlearnCommand(ChatHandler* handler, char const* args)
    {
        if (!*args)
            return false;

        Player* player = handler->GetSession()->GetPlayer();
        Pet* pet = player->GetPet();
        if (!pet)
        {
            handler->PSendSysMessage("You have no pet");
            handler->SetSentErrorMessage(true);
            return false;
        }

        uint32 spellId = handler->extractSpellIdFromLink((char*)args);

        if (pet->HasSpell(spellId))
            pet->removeSpell(spellId, false);
        else
            handler->PSendSysMessage("Pet doesn't have that spell");

        return true;
    }
};

void AddSC_pet_commandscript()
{
    new pet_commandscript();
}
