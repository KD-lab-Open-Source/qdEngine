// Copyright David Abrahams 2001. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#ifndef CLASS_DWA20011214_HPP
# define CLASS_DWA20011214_HPP

# include <boost/python/detail/prefix.hpp>
# include <boost/utility.hpp>
# include <boost/python/object_core.hpp>
# include <boost/python/type_id.hpp>
# include <cstddef>

namespace boost { namespace python {

namespace objects { 

struct BOOST_PYTHON_DECL class_base : python::api::object
{
    // constructor
    class_base(
        char const* name              // The name of the class
        
        , std::size_t num_types         // A list of class_ids. The first is the type
        , type_info const*const types    // this is wrapping. The rest are the types of
                                        // any bases.
        
        , char const* doc = 0           // Docstring, if any.
        );


    // Implementation detail. Hiding this in the private section would
    // require use of template friend declarations.
    void enable_pickling_(bool getstate_manages_dict);

 protected:
    void add_property(char const* name, object const& fget);
    void add_property(char const* name, object const& fget, object const& fset);

    void add_static_property(char const* name, object const& fget);
    void add_static_property(char const* name, object const& fget, object const& fset);
    
    // Retrieve the underlying object
    void setattr(char const* name, object const&);

    // Set a special attribute in the class which tells Boost.Python
    // to allocate extra bytes for embedded C++ objects in Python
    // instances.
    void set_instance_size(std::size_t bytes);

    // Set an __init__ function which throws an appropriate exception
    // for abstract classes.
    void def_no_init();

    // Effects:
    //  setattr(self, staticmethod(getattr(self, method_name)))
    void make_method_static(const char *method_name);
};

BOOST_PYTHON_DECL void copy_class_object(type_info const& src, type_info const& dst);

}}} // namespace boost::python::objects

#endif // CLASS_DWA20011214_HPP
