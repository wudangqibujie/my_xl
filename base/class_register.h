//
// Created by jay on 2023/1/2.
//

#ifndef MY_XL_CLASS_REGISTER_H
#define MY_XL_CLASS_REGISTER_H

#include <map>
#include <string>

//------------------------------------------------------------------------------
// The first parameter, register_name, should be unique globally.
// Another approach for this is to define a template for base class. It would
// make the code more readable, but the only issue of using template is that
// each base class could have only one register then. It doesn't sound very
// likely that a user wants to have multiple registers for one base class,
// but we keep it as a possibility.
// We would switch to using template class if necessary.
//------------------------------------------------------------------------------
#define CLASS_REGISTER_DEFINE_REGISTRY(register_name, base_class_name)       \
  class ObjectCreatorRegistry_##register_name {                              \
   public:                                                                   \
   typedef base_class_name* (*Creator)();                                    \
                                                                             \
   ObjectCreatorRegistry_##register_name()                                   \
   : m_default_creator(nullptr) {}                                           \
   ~ObjectCreatorRegistry_##register_name() {}                               \
                                                                             \
   void SetDefaultCreator(Creator creator) {                                 \
     m_default_creator = creator;                                            \
   }                                                                         \
                                                                             \
   void AddCreator(std::string entry_name, Creator creator) {                \
     m_creator_registry[entry_name] = creator;                               \
   }                                                                         \
                                                                             \
   base_class_name* CreateObject(const std::string& entry_name);             \
                                                                             \
   private:                                                                  \
   typedef std::map<std::string, Creator> CreatorRegistry;                   \
   Creator m_default_creator;                                                \
   CreatorRegistry m_creator_registry;                                       \
  };                                                                         \
                                                                             \
  inline ObjectCreatorRegistry_##register_name&                              \
  GetRegistry_##register_name() {                                            \
    static ObjectCreatorRegistry_##register_name registry;                   \
    return registry;                                                         \
  }                                                                          \
                                                                             \
  class DefaultObjectCreatorRegister_##register_name {                       \
   public:                                                                   \
   DefaultObjectCreatorRegister_##register_name(                             \
       ObjectCreatorRegistry_##register_name::Creator creator) {             \
     GetRegistry_##register_name().SetDefaultCreator(creator);               \
   }                                                                         \
   ~DefaultObjectCreatorRegister_##register_name() {}                        \
  };                                                                         \
                                                                             \
  class ObjectCreatorRegister_##register_name {                              \
   public:                                                                   \
   ObjectCreatorRegister_##register_name(                                    \
       const std::string& entry_name,                                        \
       ObjectCreatorRegistry_##register_name::Creator creator) {             \
     GetRegistry_##register_name().AddCreator(entry_name,                    \
                                              creator);                      \
   }                                                                         \
   ~ObjectCreatorRegister_##register_name() {}                               \
  }

#define CLASS_REGISTER_IMPLEMENT_REGISTRY(register_name, base_class_name)  \
  base_class_name* ObjectCreatorRegistry_##register_name::CreateObject(      \
      const std::string& entry_name) {                                       \
    Creator creator = m_default_creator;                                     \
    CreatorRegistry::const_iterator it =                                     \
        m_creator_registry.find(entry_name);                                 \
    if (it != m_creator_registry.end()) {                                    \
      creator = it->second;                                                  \
    }                                                                        \
                                                                             \
    if (creator != nullptr) {                                                \
      return (*creator)();                                                   \
     } else {                                                                \
      return nullptr;                                                        \
    }                                                                        \
  }

#define CLASS_REGISTER_DEFAULT_OBJECT_CREATOR(register_name,                 \
                                              base_class_name,               \
                                              class_name)                    \
  base_class_name* DefaultObjectCreator_##register_name##class_name() {      \
    return new class_name;                                                   \
  }                                                                          \
  DefaultObjectCreatorRegister_##register_name                               \
  g_default_object_creator_register_##register_name##class_name(             \
      DefaultObjectCreator_##register_name##class_name)

#define CLASS_REGISTER_OBJECT_CREATOR(register_name,                         \
                                      base_class_name,                       \
                                      entry_name_as_string,                  \
                                      class_name)                            \
  base_class_name* ObjectCreator_##register_name##class_name() {             \
    return new class_name;                                                   \
  }                                                                          \
  ObjectCreatorRegister_##register_name                                      \
  g_object_creator_register_##register_name##class_name(                     \
      entry_name_as_string,                                                  \
      ObjectCreator_##register_name##class_name)

#define CLASS_REGISTER_CREATE_OBJECT(register_name, entry_name_as_string)    \
  GetRegistry_##register_name().CreateObject(entry_name_as_string)

#endif //MY_XL_CLASS_REGISTER_H
