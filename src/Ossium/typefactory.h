#ifndef TYPEFACTORY_H
#define TYPEFACTORY_H

#include <functional>

using namespace std;

namespace Ossium
{

    inline namespace TypeSystem
    {

        ///
        /// TypeRegistry
        ///

        /// Used for statically registering types for checking types at runtime.
        template<class BaseType>
        class TypeRegistry
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
        class TypeFactory : public TypeRegistry<CoreType>
        {
        private:
            typedef function<CoreType*(void*)> FactoryFunc;

            /// Construct-on-first-use idiom to solve static initialisation order fiasco
            static unordered_map<Uint32, FactoryFunc>& gen_map()
            {
                static unordered_map<Uint32, FactoryFunc>* sifmap = new unordered_map<Uint32, FactoryFunc>();
                return *sifmap;
            }
            static unordered_map<string, Uint32>& type_name_map()
            {
                static unordered_map<string, Uint32>* sifmap = new unordered_map<string, Uint32>();
                return *sifmap;
            }
            static unordered_map<Uint32, const char*>& type_id_map()
            {
                static unordered_map<Uint32, const char*>* sifmap = new unordered_map<Uint32, const char*>();
                return *sifmap;
            }

            const char* key;

        public:
            TypeFactory(const char* name, FactoryFunc factory)
            {
                //SDL_Log("Type factory instantiated for type \"%s\" [%d].", name, TypeRegistry<CoreType>::typeIdent);
                gen_map()[TypeRegistry<CoreType>::typeIdent] = factory;
                type_name_map()[name] = TypeRegistry<CoreType>::typeIdent;
                type_id_map()[TypeRegistry<CoreType>::typeIdent] = name;
                key = name;
            }

            static CoreType* Create(IdType typeId, void* args)
            {
                auto itr = gen_map().find(typeId);
                if (itr != gen_map().end())
                {
                    return itr->second(args);
                }
                SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Failed to create target [type id '%d'] with TypeFactory instance!", typeId);
                return nullptr;
            }

            static CoreType* Create(string targetType, void* args)
            {
                auto itr = type_name_map().find(targetType);
                if (itr != type_name_map().end())
                {
                    return Create(itr->second, args);
                }
                SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "Failed to create target type '%s' with TypeFactory instance!", targetType.c_str());
                return nullptr;
            }

            static string GetName(IdType ident)
            {
                auto itr = type_id_map().find(ident);
                if (itr != type_id_map().end())
                {
                    return itr->second;
                }
                return "";
            }

            static IdType GetId(string name)
            {
                auto itr = type_name_map().find(name);
                if (itr != type_name_map().end())
                {
                    return itr->second;
                }
                return TypeRegistry<CoreType>::GetTotalTypes();
            }

            string GetName()
            {
                return key;
            }

        };

    }

}

#endif // TYPEFACTORY_H
