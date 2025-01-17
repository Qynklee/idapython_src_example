{
    "SwigDirector_IDP_Hooks::SwigDirector_IDP_Hooks" : [
        ("maybe_collect_director_fixed_method_set",
         [
             ("auto_queue_empty", "ev_auto_queue_empty"),
         ]),
    ],
    "SwigDirector_IDB_Hooks::SwigDirector_IDB_Hooks" : [
        ("maybe_collect_director_fixed_method_set", None),
    ],
    "SwigDirector_IDP_Hooks::ev_get_bg_color" : [

        ("director_method_call_arity_cap", (
            False, # add GIL lock
            "ev_get_bg_color",
            "(method , __argcnt == 2 ? (PyObject *) obj1 : (PyObject *) obj0, __argcnt == 2 ? (PyObject *) NULL : (PyObject *) obj1, NULL)",
            "(swig_get_self(), (PyObject *) swig_method_name , __argcnt == 2 ? (PyObject *) obj1 : (PyObject *) obj0, __argcnt == 2 ? (PyObject *) NULL : (PyObject *) obj1, NULL)")
        ),

        ("insert_before_text", ("int swig_val;",
"""
if ( __argcnt == 2 )
{
  if ( result == Py_None )
  {
    result = PyInt_FromLong(0);
  }
  else if ( IDAPyInt_Check(result) )
  {
    *color = bgcolor_t(IDAPyInt_AsLong(result));
    result = IDAPyInt_FromLong(1);
  }
}
""")),
        ],

    "SwigDirector_IDP_Hooks::ev_set_idp_options" : [
        (
            "repl_text",
            (
                "obj2 = SWIG_NewPointerObj(SWIG_as_voidptr(value), SWIGTYPE_p_void,  0 );",
                (
                    "obj2 = new_PyObject_from_idpopt_value(value_type, value);",
                    "  if ( ((PyObject *) obj2) == nullptr )",
                    "    Swig::DirectorException::raise(\"Unknown 'value_type'\");",
                ),
            ),
        ),
    ],

    "SwigDirector_IDB_Hooks::renamed" : [
        ("director_method_call_arity_cap", (
            False, # add GIL lock
            "renamed",
            "(method ,(PyObject *)obj0,(PyObject *)obj1,(PyObject *)obj2,(__argcnt < 5 ? NULL : (PyObject *)obj3), NULL)",
            "(swig_get_self(), (PyObject *) swig_method_name ,(PyObject *)obj0,(PyObject *)obj1,(PyObject *)obj2,(__argcnt < 5 ? NULL : (PyObject *)obj3), NULL)")
        ),
    ],

    "SwigDirector_IDB_Hooks::compiler_changed" : [
        ("director_method_call_arity_cap", (
            False, # add GIL lock
            "compiler_changed",
            "(method ,(__argcnt == 1 ? NULL : (PyObject *)obj0), NULL)",
            "(swig_get_self(), (PyObject *) swig_method_name ,(__argcnt == 1 ? NULL : (PyObject *)obj0), NULL)")
        ),
    ],

    "SwigDirector_IDB_Hooks::bookmark_changed" : [
        ("director_method_call_arity_cap", (
            False, # add GIL lock
            "bookmark_changed",
            "(method ,(PyObject *)obj0,(PyObject *)obj1,(PyObject *)obj2,(__argcnt < 5 ? NULL : (PyObject *)obj3), NULL)",
            "(swig_get_self(), (PyObject *) swig_method_name ,(PyObject *)obj0,(PyObject *)obj1,(PyObject *)obj2,(__argcnt < 5 ? NULL : (PyObject *)obj3), NULL)")
        ),
    ],

    "SwigDirector_IDB_Hooks::segm_deleted" : [
        ("director_method_call_arity_cap", (
            False, # add GIL lock
            "segm_deleted",
            "(method ,(PyObject *)obj0,(PyObject *)obj1,(__argcnt < 4 ? NULL : (PyObject *)obj2), NULL)",
            "(swig_get_self(), (PyObject *) swig_method_name ,(PyObject *)obj0,(PyObject *)obj1,(__argcnt < 4 ? NULL : (PyObject *)obj2), NULL)")
        ),
    ],
}
