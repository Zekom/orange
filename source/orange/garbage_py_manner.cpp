/*
    This file is part of Orange.

    Orange is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Orange is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Orange; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Authors: Janez Demsar, Blaz Zupan, 1996--2002
    Contact: janez.demsar@fri.uni-lj.si
*/


#include "garbage.hpp"
#include <string>
#include <vector>
#include <map>
using namespace std;

void floatfloat_mapdestructor(void *x) { mldelete (map<float, float> *) x; }
TGCCounterNML<map<float, float> >::TDestructor GCPtrNML<map<float, float> >::destructor = floatfloat_mapdestructor;

typedef TGCCounterNML<int> TPyNotOrange;

void NotOrange_dealloc(TPyNotOrange *self)
{ if (!self->is_reference)
    self->destructor((void *)self->ptr);
}

PyTypeObject PyNotOrOrange_Type =  {
  PyObject_HEAD_INIT((_typeobject *)&PyType_Type)
  0,
  "Not Orange",
  sizeof(TPyNotOrange), 0,
  (destructor)NotOrange_dealloc,                     /* tp_dealloc */
  0, 0, 0, 0,
  0,                             /* tp_repr */
  0,                                 /* tp_as_number */
  0, 0, 0, 0,
  0,                              /* tp_str */
  0,                      /* tp_getattro */
  0,                      /* tp_setattro */
  0,
  Py_TPFLAGS_HAVE_CLASS | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_SEQUENCE_IN, /* tp_flags */
  0, 0, 0, 0, 0, 0, 0,
  0,                                    /* tp_methods */
  0, 0, 0, 0, 0, 0,
  offsetof(TPyNotOrange, notorange_dict),                  /* tp_dictoffset */
};


