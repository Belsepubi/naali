// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_RexLogic_EC_OpenSimPresence_h
#define incl_RexLogic_EC_OpenSimPresence_h

#include "ComponentInterface.h"
//#include "Foundation.h"
#include "RexUUID.h"

namespace RexLogic
{
    //! This component is present on all agents when connected to an OpenSim world. 
    class EC_OpenSimPresence : public Foundation::ComponentInterface
    {
        DECLARE_EC(EC_OpenSimPresence);
       
    public:
        virtual ~EC_OpenSimPresence();

        //! set first name
        void SetFirstName(const std::string &name);    
        //! get first name
        std::string GetFirstName();  
        //! set last name
        void SetLastName(const std::string &name);    
        //! get last name
        std::string GetLastName(); 
        //! get full name
        std::string GetFullName(); 
     
        // !ID related
        uint64_t RegionHandle;
        uint32_t LocalId;
        RexTypes::RexUUID FullId;
        uint32_t ParentId;
                
    private:
        //! first name of avatar
        std::string first_name_;
        //! last name of avatar
        std::string last_name_;

        EC_OpenSimPresence(Foundation::ModuleInterface* module);
    };
}

#endif