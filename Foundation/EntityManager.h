// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_Foundation_EntityManager_h
#define incl_Foundation_EntityManager_h

#include "EntityInterface.h"

namespace Foundation
{
    class Framework;

    ////! Acts as a generic scenegraph for all entities in the world
    ///*!
    //    Contains all entities in the world in a generic fashion.
    //    Acts as a factory for all entities.
    //*/
    //class EntityManager
    //{
    //public:
    //    EntityManager(Framework *framework) : mFramework(framework) {}
    //    ~EntityManager() {}

    //    //! Creates new entity that contains the specified components
    //    /*!
    //        \param components list of component names the entity will use
    //    */
    //    EntityPtr createEntity(const Core::StringVector &components);

    //    //! Creates an empty entity
    //    EntityPtr createEntity();
    //    
    //    //! Returns entity with the specified id
    //    EntityPtr getEntity(Core::entity_id_t id);   
    //
    //private:
    //    typedef std::map<Core::entity_id_t, EntityPtr> EntityMap;

    //    EntityMap mEntities;
    //    Framework *mFramework;
    //};
}

#endif 
