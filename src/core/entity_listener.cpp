#include "entity_listener.h"

#include "log.h"

namespace vspdotnet {
    EntityListener Listener;

    void EntityListener::OnEntityCreated(CBaseEntity *pEntity) {
        VSPDN_CORE_TRACE("Entity Created: {0}", (void*)pEntity);
    }

    void EntityListener::OnEntitySpawned(CBaseEntity *pEntity) {
        VSPDN_CORE_TRACE("Entity Spawned: {0}", (void*)pEntity);
    }

    void EntityListener::OnEntityDeleted(CBaseEntity *pEntity) {
        VSPDN_CORE_TRACE("Entity Deleted: {0}", (void*)pEntity);
    }
}
