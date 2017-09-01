#pragma once
#ifndef _OBJECT_
#define _OBJECT_ 1
#include "obj_type.hpp"

namespace obj {
	/*returns the runtime std::type::info for the templated class type*/
	template<class _obj>
	inline type::info class_info() { return type::info(typeid(_obj).hash_code(), typeid(_obj).name(), false); }
	/*returns the runtime std::type::info for the templated protocol type*/
	template<class _obj>
	inline type::info proto_info() { return type::info(typeid(_obj).hash_code(), typeid(_obj).name(), true); }


	/*the root base class of all typed classes*/
	class object {
#pragma region instance variables
	private:
		/*the std::type of the host runtime class*/
		std::shared_ptr<type> _type;
#pragma endregion

#pragma region constructors
	public:
		object() { __reg_type<object>(this); }
		virtual ~object() {}
#pragma endregion

#pragma region protected methods
	protected:
		/*registers a single type as the host runtime class*/
		template<class _obj>
		inline static void __reg_type(object *obj) {
			obj->_type.reset(new type(class_info<_obj>(), _type));
		}
		/*registers a single protocol that is implemented by the host runtime class*/
		inline static void __reg_proto(object *obj, const type::info &info) {
			obj->_type->_protocols.push_back(info);
		}
		/*registers a single protocol that is implemented by the host runtime class*/
		template<class _obj>
		inline static void __reg_proto(object *obj) {
			__reg_proto(obj, proto_info<_obj>());
		}
		/*registers a list of protocol std::type::info that are implemented by this runtime class*/
		inline static void __reg_protos(object *obj, const std::vector<type::info> &infos) {
			size_t size = infos.size();
			if (size == 1) __reg_proto(obj, infos[0]);
			for (size_t c = 0; c < size; c++) {
				obj->_type->_protocols.push_back(infos[c]);
			}
		}
#pragma endregion

#pragma region public methods
	public:
		/*returns true if this class is an instance of the provided object*/
		inline bool instanceof(const object *obj) const {
			return instanceof(obj->get_type().get_info());
		}
		/*returns true if this class inherits from the class or proto described
		provided by the std::type::info*/
		inline bool instanceof(const type::info &obj_type) const {
			return (obj_type._is_protocol && _type->has_protocol(obj_type)) || _type->has_class(obj_type);
		}
		const type & get_type() const { return *_type; }
		virtual const char * to_string() const { return _type->_info._name; }
#pragma endregion
	};
}

#endif