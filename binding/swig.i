#define NPK_API 
#define NPK_DEV_API 
#define NPK_CSTR const char*
#define NPK_STR char*

%newobject npk_package_open;
%typemap(in) NPK_TEAKEY teakey[4] (NPK_TEAKEY temp[4]) {
    int i;
    if (!PySequence_Check($input)) {
        PyErr_SetString(PyExc_ValueError,"Expected a sequence");
        return NULL;
    }
    if (PySequence_Length($input) != 4) {
        PyErr_SetString(PyExc_ValueError,"Size mismatch. Expected 4 elements");
        return NULL;
    }
    for (i = 0; i < 4; ++i) {
        PyObject *o = PySequence_GetItem($input,i);
        if (PyNumber_Check(o)) {
            temp[i] = (int) PyInt_AsLong(o);
        } else {
            PyErr_SetString(PyExc_ValueError,"Sequence elements must be numbers");      
            return NULL;
        }
    }
    $1 = temp;
}
