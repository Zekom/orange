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


#ifndef __ROOT_HPP
#define __ROOT_HPP

#ifdef _MSC_VER
  #pragma warning (disable : 4786 4114 4018 4267 4244 4702 4710 4290)
#endif

#include "garbage.hpp"
class TValue;

#include <string>
using namespace std;

#ifndef offsetof
  #define offsetof(_TYPE, _MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#define __REGISTER_ABSTRACT_CLASS \
  static TClassDescription st_classDescription; \
  virtual TClassDescription const *classDescription() const;

#define __REGISTER_CLASS \
  static TClassDescription st_classDescription; \
  virtual TClassDescription const *classDescription() const; \
  virtual TOrange *clone() const;


/* traverse vs. visit 

'traverse' should call 'visit' on all wrapped objects it (directly) contains.
Visiting wrapped objects that are registered as components is taken care of
by TOrange. Thus, only unregistered objects, usually those that are included
in lists need to be called.

'traverse' should call 'traverse' on all owned unwrapped objects (by inclusion
or by unwrapped pointers (which are not referenced by any other object! -- use
of unwrapped pointer is very strongly dissuaded). This is needed so that the
references to those object are 'explained'.
*/


#define PVISIT(obj) if ((obj).counter) VISIT((obj).counter);
#define TRAVERSE(proc) { int res=proc(visit, arg); if (res) return res; }
#define DROPREFERENCES(proc) { int res=proc(); if (res) return res; }
#define CLONE(type,x) (x) ? dynamic_cast<type *>((x)->clone()) : (type *)(NULL)
#define call operator()


struct _tclassdescription;

typedef struct {
  const char *name;
  const char *description;
  const type_info *type;
  const struct _tclassdescription *classDescription;

  size_t offset;

  bool readOnly;
  bool obsolete;
} TPropertyDescription;


typedef struct _tclassdescription {
  const char *name;
  const type_info *type;
  const struct _tclassdescription *base;

  TPropertyDescription const *properties;
  size_t const *components;
} TClassDescription;


extern TPropertyDescription _no_properties[];
extern size_t const _no_components[];

bool castableTo(const TClassDescription *objecttype, const TClassDescription *basetype);


WRAPPER(Orange)


class TOrange {
public:
  __REGISTER_CLASS
  typedef void TWarningFunction(const char *);
  static TWarningFunction *warningFunction;

  string name;             //P name of the object
  string shortDescription; //P short description of the object
  string description;      //P long description of the object

  TGCCounter<TOrange> *myWrapper;

  TOrange();
  TOrange(const TOrange &orb);
  virtual ~TOrange();

  virtual void afterSet(const char *name);

  void    setProperty(const char *name, const bool &b);    void    getProperty(const char *name, bool &b) const;
  void    setProperty(const char *name, const int &b);     void    getProperty(const char *name, int &b) const;
  void    setProperty(const char *name, const float &b);   void    getProperty(const char *name, float &b) const;
  void    setProperty(const char *name, const string &b);  void    getProperty(const char *name, string &b) const;
  void    setProperty(const char *name, const TValue &b);  void    getProperty(const char *name, TValue &b) const;
  void wr_setProperty(const char *name, const POrange &b); void wr_getProperty(const char *name, POrange &b) const;
  
  const TPropertyDescription *propertyDescription(const char *name, bool noException = false) const;
  const type_info &propertyType(const char *name) const;

  int traverse(visitproc visit, void *arg) const;
  int dropReferences();

  void raiseError(const char *anerr, ...) const;
  void raiseWarning(const char *anerr, ...) const;
  void raiseErrorWho(const char *who, const char *anerr, ...) const;
  void raiseWarningWho(const char *who, const char *anerr, ...) const;

  #define checkProperty(name) if (!name) raiseError("'"#name"' not set");
};


extern TPropertyDescription TOrange_properties[];
extern size_t const TOrange_components[];

#endif
