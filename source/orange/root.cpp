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


#include "values.hpp"
#include "errors.hpp"
#include "root.ppp"

TOrange::TWarningFunction *TOrange::warningFunction = NULL;

TPropertyDescription _no_properties[] = { {NULL} };

size_t const _no_components[] = { 0 };

bool castableTo(const TClassDescription *objecttype, const TClassDescription *basetype)
{ do
    if (objecttype==basetype)
      return true;
    else
      objecttype = objecttype->base;
  while (objecttype);

  return false;
}



TOrange::TOrange()
: name(""),
  shortDescription(""),
  description(""),
  myWrapper(NULL)
{}


TOrange::TOrange(const TOrange &orb) 
: name(orb.name),
  shortDescription(orb.shortDescription),
  description(orb.description), 
  myWrapper(NULL)
{}


TOrange::~TOrange()
{}


void TOrange::afterSet(const char *)
{}


const type_info &TOrange::propertyType(const char *name) const
{ return *propertyDescription(name)->type; }


const TPropertyDescription *TOrange::propertyDescription(const char *name, bool noException) const
{ for (const TPropertyDescription *pd = classDescription()->properties; pd->name; pd++)
    if (!strcmp(pd->name, name))
      return pd;

  if (!noException)
    raiseError("there is no property '%s'", name);

  return NULL;
}


#define MEMBER(ofs) (((char *)(this)) + ofs)
#define CONST_MEMBER(ofs) (((char const *)(this)) + ofs)

#define SIMPLE_GETSET_PROPERTY(_TYPE)                           \
void TOrange::setProperty(const char *name, const _TYPE &b)     \
{ const TPropertyDescription *pd = propertyDescription(name);   \
  if (pd->readOnly)                                             \
    raiseError("'%s.%s' is read-only", typeid(*this).name()+7, name);             \
  if (*pd->type != typeid(_TYPE))                               \
    raiseError("type mismatch, unable to set '%s.%s'", typeid(*this).name()+7, name);           \
  *(_TYPE *)MEMBER(pd->offset) = b;                             \
  afterSet(name);                                               \
}                                                               \
                                                                \
                                                                \
void TOrange::getProperty(const char *name, _TYPE &b) const     \
{ const TPropertyDescription *pd = propertyDescription(name);   \
  if (*pd->type != typeid(_TYPE))                               \
    raiseError("type mismatch, unable to read '%s.%s'", typeid(*this).name()+7, name);         \
  b = *(_TYPE const *)CONST_MEMBER(pd->offset);                 \
}


SIMPLE_GETSET_PROPERTY(bool)
SIMPLE_GETSET_PROPERTY(int)
SIMPLE_GETSET_PROPERTY(float)
SIMPLE_GETSET_PROPERTY(string)
SIMPLE_GETSET_PROPERTY(TValue)

#undef SIMPLE_GETSET_PROPERTY


void TOrange::wr_setProperty(const char *name, const POrange &b)
{ const TPropertyDescription *pd = propertyDescription(name);
  if (pd->readOnly)
    raiseError("'%s.%s' is read-only", typeid(*this).name()+7, name);
  if (b && !castableTo(b->classDescription(), pd->classDescription))
    raiseError("type mismatch, unable to set '%s.%s' (expected %s, got %s).", typeid(*this).name()+7, name, pd->name, b->name.c_str());
  *(POrange *)MEMBER(pd->offset) = b;
  afterSet(name);
}

  
void TOrange::wr_getProperty(const char *name, POrange &b) const
{ const TPropertyDescription *pd = propertyDescription(name);
  // Warning: no type checking here. We cannot guess what the user wants the pointer b to point to.
  b = *(POrange const *)CONST_MEMBER(pd->offset);
}


int TOrange::traverse(visitproc visit, void *arg) const
{ for(size_t const *ci = classDescription()->components; *ci; ci++) {
    // shouldn't take POrange's -- don't want to create any additional references
    TGCCounter<TOrange> *ptr = ((POrange const *)CONST_MEMBER(*ci))->counter;
    if (ptr)
      VISIT(ptr)
  }
  return 0;
}


int TOrange::dropReferences()
{ for(size_t const *ci = classDescription()->components; *ci; ci++)
    // Strictly speaking, this is wrong -- we assign POrange to a field of type, say, PVariable.
    // However, POrange(NULL) doesn't binary differ from PVariable(NULL)
    // And even if it were not, the state of the object after dropReference doesn't matter;
    // what matters is getting rid of all members. 
    *(POrange *)MEMBER(*ci) = POrange();
  return 0;
}



extern char excbuf[256], excbuf2[256]; // defined in errors.cpp

void TOrange::raiseError(const char *anerr, ...) const
{ va_list vargs;
  #ifdef HAVE_STDARG_PROTOTYPES
    va_start(vargs, anerr);
  #else
    va_start(vargs);
  #endif

  snprintf(excbuf, 512, "'orange.%s': %s", typeid(*this).name()+7, anerr);
  vsnprintf(excbuf2, 512, excbuf, vargs);
  throw mlexception(excbuf2);
}


void TOrange::raiseErrorWho(const char *who, const char *anerr, ...) const
{ va_list vargs;
  #ifdef HAVE_STDARG_PROTOTYPES
    va_start(vargs, anerr);
  #else
    va_start(vargs);
  #endif

  snprintf(excbuf, 512, "'orange.%s.%s': %s", typeid(*this).name()+7, who, anerr);
  vsnprintf(excbuf2, 512, excbuf, vargs);
  throw mlexception(excbuf2);
}


void TOrange::raiseWarning(const char *anerr, ...) const
{ 
  if (warningFunction) {
    va_list vargs;
    #ifdef HAVE_STDARG_PROTOTYPES
      va_start(vargs, anerr);
    #else
      va_start(vargs);
    #endif

    snprintf(excbuf, 512, "'%s': %s", typeid(*this).name()+7, anerr);
    vsnprintf(excbuf2, 512, excbuf, vargs);
    warningFunction(excbuf2);
  }
}

void TOrange::raiseWarningWho(const char *who, const char *anerr, ...) const
{ 
  if (warningFunction) {
    va_list vargs;
    #ifdef HAVE_STDARG_PROTOTYPES
      va_start(vargs, anerr);
    #else
      va_start(vargs);
    #endif

    snprintf(excbuf, 512, "'orange.%s.%s': %s", typeid(*this).name()+7, who, anerr);
    vsnprintf(excbuf2, 512, excbuf, vargs);
    warningFunction(excbuf2);
  }
}
