/*
* This file is part of Project SkyFire https://www.projectskyfire.org.
* See LICENSE.md file for Copyright information
*/

#ifndef SF_PLAYER_DUMP_H
#define SF_PLAYER_DUMP_H

#include <map>
#include <set>
#include <string>

enum DumpTableType
{
    DTT_CHARACTER,      //                                  // characters

    DTT_CHAR_TABLE,     //                                  // character_achievement, character_achievement_progress,
    // character_action, character_aura, character_homebind,
    // character_queststatus, character_queststatus_rewarded, character_reputation,
    // character_spell, character_spell_cooldown, character_ticket, character_talent.
    // character_cuf_profiles, character_currency

    DTT_EQSET_TABLE,    // <- guid                          // character_equipmentsets

    DTT_INVENTORY,      //    -> item guids collection      // character_inventory

    DTT_MAIL,           //    -> mail ids collection        // mail
    //    -> item_text

    DTT_MAIL_ITEM,      // <- mail ids                      // mail_items
    //    -> item guids collection

    DTT_ITEM,           // <- item guids                    // item_instance
    //    -> item_text

    DTT_ITEM_GIFT,      // <- item guids                    // character_gifts

    DTT_PET,            //    -> pet guids collection       // character_pet
    DTT_PET_TABLE       // <- pet guids                     // pet_aura, pet_spell, pet_spell_cooldown
};

enum DumpReturn
{
    DUMP_SUCCESS,
    DUMP_FILE_OPEN_ERROR,
    DUMP_TOO_MANY_CHARS,
    DUMP_UNEXPECTED_END,
    DUMP_FILE_BROKEN,
    DUMP_CHARACTER_DELETED
};

class PlayerDump
{
protected:
    PlayerDump() { }
};

class PlayerDumpWriter : public PlayerDump
{
public:
    PlayerDumpWriter() { }

    bool GetDump(uint32 guid, std::string& dump);
    DumpReturn WriteDump(std::string const& file, uint32 guid);
private:
    typedef std::set<uint32> GUIDs;

    bool DumpTable(std::string& dump, uint32 guid, char const* tableFrom, char const* tableTo, DumpTableType type);
    std::string GenerateWhereStr(char const* field, GUIDs const& guids, GUIDs::const_iterator& itr);
    std::string GenerateWhereStr(char const* field, uint32 guid);

    GUIDs pets;
    GUIDs mails;
    GUIDs items;
};

class PlayerDumpReader : public PlayerDump
{
public:
    PlayerDumpReader() { }

    DumpReturn LoadDump(std::string const& file, uint32 account, std::string name, uint32 guid);
};

#endif
