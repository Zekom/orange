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


/* 

Here are three kinds of classes, one being obsolete.

Discretizers are derived from TTransformValue.
They take a continuous value and discretize it.
There are only two discretizers, one uses intervals
of equal widths (TEquiDistDiscretizer) and the other
uses a set of cut-off points (TIntervalDiscretizer).

Discretizations construct discretizers. There's an
abstract TDiscretization class, from which we derive
TEquiDistDiscretization (the only one that constructs
TEquiDistDiscretizer), and TEquiNDiscretization,
TEntropyDiscretization and TBiModalDiscretization.

DiscretizedDomains are obsolete. These are different
classes derived from Domain. Their constructors get
examples and the constructed domain has continuous
attributes replaced by discretized.

DiscretizedDomains are succeeded by DomainDiscretization,
a class that is given a Discretization as a property,
while it's call operator gets an example generator and
returns a discretized domain.

*/

#ifndef __DISCRETIZE_HPP
#define __DISCRETIZE_HPP

#include <vector>
using namespace std;

#include "values.hpp"
#include "transval.hpp"
#include "domain.hpp"
#include "distvars.hpp"

WRAPPER(BasicAttrStat)

WRAPPER(Discretization)
WRAPPER(EquiDistDiscretizer)
WRAPPER(IntervalDiscretizer)


class TDiscretization : public TOrange {
public:
  __REGISTER_ABSTRACT_CLASS

  virtual PVariable operator()(PExampleGenerator, PVariable, const long &weightID=0)=0;
  void setTransformer(PVariable var, PVariable evar);
};



class TDomainDiscretization : public TOrange {
public:
  __REGISTER_CLASS

  PDiscretization discretization; //P discretization

  TDomainDiscretization(PDiscretization = PDiscretization());
  virtual PDomain operator()(PExampleGenerator, const long &weightID=0);

protected:
  PDomain equiDistDomain(PExampleGenerator gen);
  PDomain equiNDomain(PExampleGenerator gen, const long &weightID=0);
  PDomain otherDomain(PExampleGenerator gen, const long &weightID=0);
};



class TEquiDistDiscretizer : public TTransformValue {
public:
  __REGISTER_CLASS

  int   numberOfIntervals; //P number of intervals
  float firstVal; //P the lowest possible value
  float step; //P step (width of interval)

  TEquiDistDiscretizer(const int=-1, const float=-1.0, const float=-1.0);

  virtual void transform(TValue &);

  /* This is static since it requires a wrapped EquiDiscDiscretizer;
     (it will be stored in constructed Variable!)
     'this' pointer would not suffice - there's a danger of it being rewrapped */
  static  PVariable constructVar(PVariable, PEquiDistDiscretizer);
};


class TThresholdDiscretizer : public TTransformValue {
public:
  __REGISTER_CLASS

  float threshold; //P threshold

  TThresholdDiscretizer(const float &threshold = 0.0);
  virtual void transform(TValue &);
};


class TIntervalDiscretizer : public TTransformValue  {
public:
  __REGISTER_CLASS

  PFloatList points; //P cut-off points

  TIntervalDiscretizer();
  TIntervalDiscretizer(PFloatList apoints);
  TIntervalDiscretizer(const string &boundaries);

  virtual void      transform(TValue &);
  static PVariable constructVar(PVariable var, PIntervalDiscretizer);
};




class TEquiDistDiscretization : public TDiscretization {
public:
  __REGISTER_CLASS

  int numberOfIntervals; //P number of intervals

  TEquiDistDiscretization(const int anumber=4);
  virtual PVariable operator()(PExampleGenerator, PVariable, const long &weightID=0);
  virtual PVariable operator()(PBasicAttrStat, PVariable) const;
};





class TFixedDiscretization : public TDiscretization {
public:
  __REGISTER_CLASS

  PFloatList points; //P cut-off points

  TFixedDiscretization(TFloatList &apoints);
  TFixedDiscretization(const string &boundaries);

  virtual PVariable operator()(PExampleGenerator, PVariable, const long &weightID=0);
};



class TEquiNDiscretization : public TDiscretization {
public:
  __REGISTER_CLASS

  int numberOfIntervals; //P number of intervals
  bool recursiveDivision; //P find cut-off points by recursive division (default = true)

  TEquiNDiscretization(int anumber =4);
  virtual PVariable operator()(const TContDistribution &, PVariable var) const;
  virtual PVariable operator()(PExampleGenerator, PVariable, const long &weightID=0);

  void cutoffsByCounting(PIntervalDiscretizer, const TContDistribution &) const;
  void cutoffsByDivision(PIntervalDiscretizer, const TContDistribution &) const;
  void cutoffsByDivision(const int &noInt, TFloatList &points, 
                        map<float, float>::const_iterator fbeg, map<float, float>::const_iterator fend,
                        const float &N) const;
};



class TEntropyDiscretization : public TDiscretization {
public:
  __REGISTER_CLASS

  int maxNumberOfIntervals; //P maximal number of intervals; default = 0 (no limits)

  TEntropyDiscretization();
  typedef map<float, TDiscDistribution> TS;

  virtual PVariable operator()(PExampleGenerator, PVariable, const long &weightID = 0);
  virtual PVariable operator()(const TS &, const TDiscDistribution &, PVariable, const long &weightID = 0) const;

protected:
  void divide(const TS::const_iterator &, const TS::const_iterator &, const TDiscDistribution &, float entropy, int k, vector<pair<float, float> > &) const;
};



class TBiModalDiscretization : public TDiscretization {
public:
  __REGISTER_CLASS
  virtual PVariable operator()(PExampleGenerator, PVariable, const long &weightID=0);
};



/* OBSOLETE */

class TDiscretizedDomain : public TDomain {
public:
  __REGISTER_CLASS

  PDiscretization defaultDiscretization; //P default discretization
  int defaultInt; //P default number of intervals
  int weight; //P id of attribute with weight

  TDiscretizedDomain(const int aweight=0, const int adInt=4, PDiscretization defaultdisc=PDiscretization());
  TDiscretizedDomain(PExampleGenerator, const int aweight=0, const int adInt=4, PDiscretization defaultdisc=PDiscretization());
  TDiscretizedDomain(PExampleGenerator, const vector<int> &discretizeId, const int aweight=0, const int adInt=4, PDiscretization defaultdisc=PDiscretization());

  void learn(PExampleGenerator gen);
  void learn(PExampleGenerator gen, const vector<int> &discretizeId);
};


#endif
