/** COPYRIGHT NOTICE
 *
 *  Ossium Engine
 *  Copyright (c) 2018-2020 Tim Lane
 *
 *  This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
 *
 *  2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
 *
 *  3. This notice may not be removed or altered from any source distribution.
 *
**/
#ifndef TYPEFACTORY_H
#define TYPEFACTORY_H

#include <functional>

#include "logging.h"

namespace Ossium
{

    inline namespace TypeSystem
    {

        ///
        /// TypeRegistry
        ///

        /// Used for statically registering types for checking types at runtime.
        template<class BaseType>
        class OSSIUM_EDL TypeRegistry
        {
        protected:
            static Uint32 nextTypeIdent;
            Uint32 typeIdent;

        public:
            TypeRegistry()
            {
                typeIdent = nextTypeIdent;
                nextTypeIdent++;
            }

            const Uint32 GetType()
            {
                return typeIdent;
            }

            static Uint32 GetTotalTypes()
            {
                return (Uint32)nextTypeIdent;
            }

            static bool IsValidType(Uint32 id)
            {
                return id < GetTotalTypes();
            }

        };

        template<class BaseType>
        Uint32 TypeRegistry<BaseType>::nextTypeIdent = 0;

        ///
        /// TypeFactory
        ///

        /// Similar to the type registry, but also allows instantiation of specific types and registers type names.
        template<class CoreType, class IdType>
        class OSSIUM_EDL TypeFactory : public TypeRegistry<CoreType>
        {
        private:
            typedef std::function<CoreType*(void*)> FactoryFunc;

            /// Construct-on-first-use idiom to solve static initialisation order fiasco
            static std::unordered_map<Uint32, FactoryFunc>& gen_map()
            {
                static std::unordered_map<Uint32, FactoryFunc>* sifmap = new std::unordered_map<Uint32, FactoryFunc>();
                return *sifmap;
            }
            static std::unordered_map<std::string, Uint32>& type_name_map()
            {
                static std::unordered_map<std::string, Uint32>* sifmap = new std::unordered_map<std::string, Uint32>();
                return *sifmap;
            }
            static std::unordered_map<Uint32, const char*>& type_id_map()
            {
                static std::unordered_map<Uint32, const char*>* sifmap = new std::unordered_map<Uint32, const char*>();
                return *sifmap;
            }
            static std::unordered_map<Uint32, bool>& type_abstract_map()
            {
                static std::unordered_map<Uint32, bool>* sifmap = new std::unordered_map<Uint32, bool>();
                return *sifmap;
            }
            /// Map of base names to the names of all derivative types.
            static std::unordered_map<std::string, std::vector<IdType>>& derived_type_name_map()
            {
                static std::unordered_map<std::string, std::vector<IdType>>* sifmap = new std::unordered_map<std::string, std::vector<IdType>>();
                return *sifmap;
            }
            /// Map of base type ids to the type ids of all derivative types.
            /// Note that this must be MANUALLY initialised from within the main() function.
            static std::unordered_map<IdType, std::vector<IdType>>& derived_type_id_map()
            {
                static std::unordered_map<IdType, std::vector<IdType>>* sifmap = new std::unordered_map<IdType, std::vector<IdType>>();
                return *sifmap;
            }

            const char* key;

            inline static bool initialised = false;

        public:
            TypeFactory() = delete;

            TypeFactory(const char* name, FactoryFunc factory, std::string baseName, bool abstract_type = false)
            {
                Log.Info("Type factory instantiated for type \"{0}\" [{1}].", name, TypeRegistry<CoreType>::typeIdent);
                gen_map()[TypeRegistry<CoreType>::typeIdent] = factory;
                type_name_map()[name] = TypeRegistry<CoreType>::typeIdent;
                type_id_map()[TypeRegistry<CoreType>::typeIdent] = name;
                type_abstract_map()[TypeRegistry<CoreType>::typeIdent] = abstract_type;
                derived_type_name_map()[baseName].push_back(TypeRegistry<CoreType>::typeIdent);
                key = name;
            }

            TypeFactory(const char* name, FactoryFunc factory, bool abstract_type = false)
            {
                Log.Info("Type factory instantiated for type \"{0}\" [{1}].", name, TypeRegistry<CoreType>::typeIdent);
                gen_map()[TypeRegistry<CoreType>::typeIdent] = factory;
                type_name_map()[name] = TypeRegistry<CoreType>::typeIdent;
                type_id_map()[TypeRegistry<CoreType>::typeIdent] = name;
                type_abstract_map()[TypeRegistry<CoreType>::typeIdent] = abstract_type;
                key = name;
            }

            /// Initialises the derived type id map
            static int Init()
            {
                if (!initialised)
                {
                    /// Get all base types
                    for (auto itr : derived_type_name_map())
                    {
                        auto id_itr = type_name_map().find(itr.first);
                        if (id_itr != type_name_map().end())
                        {
                            derived_type_id_map()[id_itr->second] = itr.second;
                        }
                        else
                        {
                            Log.Warning("Failed to locate type '{0}' during type factory initialisation!", itr.first);
                        }
                    }
                    initialised = true;
                }
                return 0;
            }

            static CoreType* Create(IdType typeId, void* args)
            {
                auto itr = gen_map().find(typeId);
                if (itr != gen_map().end())
                {
                    return itr->second(args);
                }
                Log.Error("Failed to create target [type id '{0}'] with TypeFactory instance!", typeId);
                return nullptr;
            }

            static CoreType* Create(std::string targetType, void* args)
            {
                auto itr = type_name_map().find(targetType);
                if (itr != type_name_map().end())
                {
                    return Create(itr->second, args);
                }
                Log.Error("Failed to create target type '{0}' with TypeFactory instance!", targetType);
                return nullptr;
            }

            static std::string GetName(IdType ident)
            {
                auto itr = type_id_map().find(ident);
                if (itr != type_id_map().end())
                {
                    return itr->second;
                }
                return "";
            }

            static IdType GetId(std::string name)
            {
                auto itr = type_name_map().find(name);
                if (itr != type_name_map().end())
                {
                    return itr->second;
                }
                return TypeRegistry<CoreType>::GetTotalTypes();
            }

            static bool IsAbstract(IdType ident)
            {
                auto itr = type_abstract_map().find(ident);
                if (itr != type_abstract_map().end())
                {
                    return itr->second;
                }
                return false;
            }

            std::vector<IdType>& GetDerivedTypes()
            {
                return derived_type_id_map()[TypeRegistry<CoreType>::typeIdent];
            }

            std::string GetName()
            {
                return key;
            }

        };

    }

}

#endif // TYPEFACTORY_H
