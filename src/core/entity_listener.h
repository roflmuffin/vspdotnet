#pragma once

#include "core/globals.h"
#include "core/global_listener.h"

#include <public/eiface.h>
#include <game/shared/shareddefs.h>
#include <game/shared/entitylist_base.h>

// This stub is required to get the correct vtables from the HL2SDK.
// Trying to include the correct class involves like 15 header files
// in a circular dependency hell so I'm just gonna stub them instead.
class IEntityListener
{
public:
    virtual void OnEntityCreated( CBaseEntity *pEntity ) {};
    virtual void OnEntitySpawned( CBaseEntity *pEntity ) {};
    virtual void OnEntityDeleted( CBaseEntity *pEntity ) {};
};

class CGlobalEntityList : public CBaseEntityList
{
public:
private:
    int m_iHighestEnt; // the topmost used array index
    int m_iNumEnts;
    int m_iNumEdicts;

    bool m_bClearingEntities;
    CUtlVector<IEntityListener *>   m_entityListeners;

public:
    IServerNetworkable* GetServerNetworkable( CBaseHandle hEnt ) const;
    CBaseNetworkable* GetBaseNetworkable( CBaseHandle hEnt ) const;
    CBaseEntity* GetBaseEntity( CBaseHandle hEnt ) const;
    edict_t* GetEdict( CBaseHandle hEnt ) const;

    int NumberOfEntities( void );
    int NumberOfEdicts( void ) {return m_iNumEdicts;}

    // mark an entity as deleted
    void AddToDeleteList( IServerNetworkable *ent );
    // call this before and after each frame to delete all of the marked entities.
    void CleanupDeleteList( void );
    int ResetDeleteList( void );

    // frees all entities in the game
    void Clear( void );

    // Returns true while in the Clear() call.
    bool    IsClearingEntities()    {return m_bClearingEntities;}

    // add a class that gets notified of entity events
    void AddListenerEntity( IEntityListener *pListener ){
        if ( m_entityListeners.Find( pListener ) >= 0 )
        {
            AssertMsg( 0, "Can't add listeners multiple times\n" );
            return;
        }
        m_entityListeners.AddToTail( pListener );
    }
    void RemoveListenerEntity( IEntityListener *pListener ) {
        m_entityListeners.FindAndRemove( pListener );
    }
};

namespace vspdotnet {

    class ScriptCallback;

    class EntityListener : public IEntityListener, public GlobalClass {
    public:
        EntityListener() {};
        ~EntityListener(){};
        virtual void OnEntityCreated(CBaseEntity* pEntity) override;
        void HandleEntityCreated(CBaseEntity* pEntity, int index);
        void HandleEntitySpawned(CBaseEntity* pEntity, int index);
        void HandleEntityDeleted(CBaseEntity* pEntity, int index);
        virtual void OnEntitySpawned(CBaseEntity* pEntity) override;
        virtual void OnEntityDeleted(CBaseEntity* pEntity) override;
        virtual void OnAllInitialized();
        void Setup();
      private:
        ScriptCallback* m_on_entity_created_callback_;
        ScriptCallback* m_on_entity_spawned_callback_;
        ScriptCallback* m_on_entity_deleted_callback_;
    };
   
    }
