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


#ifndef __DISTANCE_HPP
#define __DISTANCE_HPP

#include "contingency.hpp"
#include "basstat.hpp"

class TExample;
WRAPPER(ExamplesDistance);
WRAPPER(ExamplesDistanceConstructor);
WRAPPER(ExampleGenerator);


class TExamplesDistance : public TOrange {
public:
  __REGISTER_ABSTRACT_CLASS
  virtual float operator()(const TExample &, const TExample &) const=0;
};


class TExamplesDistanceConstructor : public TOrange {
public:
  __REGISTER_ABSTRACT_CLASS
  bool ignoreClass; //P if true (default), class value is ignored when computing distances

  TExamplesDistanceConstructor(const bool & = true);
  virtual PExamplesDistance operator()(PExampleGenerator, const int & = 0, PDomainDistributions = PDomainDistributions(), PDomainBasicAttrStat = PDomainBasicAttrStat()) const=0;
};



/* Hamiltonian distance: 
      the number of different (incompatible) attribute values.
*/
class TExamplesDistance_Hamiltonian : public TExamplesDistance {
public:
  __REGISTER_CLASS

  bool ignoreClass; //P if true (default), class value is ignored when computing distances

  TExamplesDistance_Hamiltonian(const bool & = true);
  virtual float operator()(const TExample &, const TExample &) const;
};


class TExamplesDistanceConstructor_Hamiltonian : public TExamplesDistanceConstructor {
public:
  __REGISTER_CLASS

  virtual PExamplesDistance operator()(PExampleGenerator, const int & = 0, PDomainDistributions = PDomainDistributions(), PDomainBasicAttrStat = PDomainBasicAttrStat()) const;
};



/* An abstract functional objects which returns 'normalized' distance between two examples.
   'ranges' stores
      1/attribute_range for continuous attributes
      1/number_of_values for ordinal attributes
      -1.0 for nominal attribute
      0 if attribute is to be ignored (this can happen for various reasons,
          such as continuous attribute with no known values)
   When computing "difs", it returns a vector that contains
      abs(ex1[i]-ex2[i]) * ranges[i] for continuous and ordinal attributes
      0 or 1 for nominal attributes
   Distance between two values can be greater than 1!
*/
class TExamplesDistance_Normalized : public TExamplesDistance {
public:
  __REGISTER_ABSTRACT_CLASS

  PFloatList normalizers; //P normalizing factors for attributes
  int domainVersion; //P version of domain on which the ranges were computed

  TExamplesDistance_Normalized();
  TExamplesDistance_Normalized(const bool &, PExampleGenerator, PDomainDistributions = PDomainDistributions(), PDomainBasicAttrStat = PDomainBasicAttrStat());

  void getDifs(const TExample &ex1, const TExample &ex2, vector<float> &difs) const;
};


/* Maximal distance: 
      the largest distance between two corresponding attribute values
   Be careful about nominal attributes - they will often prevail since
   the distance between them is too easily 1
*/
class TExamplesDistance_Maximal : public TExamplesDistance_Normalized {
public:
  __REGISTER_CLASS

  TExamplesDistance_Maximal();
  TExamplesDistance_Maximal(const bool &, PExampleGenerator, PDomainDistributions = PDomainDistributions(), PDomainBasicAttrStat = PDomainBasicAttrStat());
  virtual float operator()(const TExample &, const TExample &) const;
};


class TExamplesDistanceConstructor_Maximal : public TExamplesDistanceConstructor {
public:
  __REGISTER_CLASS

  virtual PExamplesDistance operator()(PExampleGenerator, const int & = 0, PDomainDistributions = PDomainDistributions(), PDomainBasicAttrStat = PDomainBasicAttrStat()) const;
};



/* Manhattan distance:
      a sum of absolute differences between pairs attribute values
*/
class TExamplesDistance_Manhattan : public TExamplesDistance_Normalized {
public:
  __REGISTER_CLASS

  TExamplesDistance_Manhattan();
  TExamplesDistance_Manhattan(const bool &, PExampleGenerator, PDomainDistributions = PDomainDistributions(), PDomainBasicAttrStat = PDomainBasicAttrStat());
  virtual float operator()(const TExample &, const TExample &) const;
};


class TExamplesDistanceConstructor_Manhattan : public TExamplesDistanceConstructor {
public:
  __REGISTER_CLASS

  TExamplesDistanceConstructor_Manhattan();
  virtual PExamplesDistance operator()(PExampleGenerator, const int & = 0, PDomainDistributions = PDomainDistributions(), PDomainBasicAttrStat = PDomainBasicAttrStat()) const;
};


/* Euclidean distance:
     square root of sum of squared distances between corresponding attribute values
*/
class TExamplesDistance_Euclidean : public TExamplesDistance_Normalized {
public:
  __REGISTER_CLASS

  TExamplesDistance_Euclidean();
  TExamplesDistance_Euclidean(const bool &, PExampleGenerator, PDomainDistributions = PDomainDistributions(), PDomainBasicAttrStat = PDomainBasicAttrStat());
  virtual float operator()(const TExample &, const TExample &) const;
};


class TExamplesDistanceConstructor_Euclidean : public TExamplesDistanceConstructor {
public:
  __REGISTER_CLASS

  TExamplesDistanceConstructor_Euclidean();
  TExamplesDistanceConstructor_Euclidean(PExampleGenerator);
  virtual PExamplesDistance operator()(PExampleGenerator, const int & = 0, PDomainDistributions = PDomainDistributions(), PDomainBasicAttrStat = PDomainBasicAttrStat()) const;
};


/* Relief distance */

class TExamplesDistance_Relief : public TExamplesDistance {
public:
  __REGISTER_CLASS

  PDomainDistributions distributions; //P distributions of attributes' values
  PFloatList averages; //P average values of attributes
  PFloatList normalizations; //P ranges of attributes' values
  PFloatList bothSpecial; //P distance if both values of both attributes are undefined

  virtual float operator()(const TExample &, const TExample &) const;
  virtual float operator()(const int &attrNo, const TValue &v1, const TValue &v2) const;
};


class TExamplesDistanceConstructor_Relief : public TExamplesDistanceConstructor {
public:
  __REGISTER_CLASS

  TExamplesDistanceConstructor_Relief();
  virtual PExamplesDistance operator()(PExampleGenerator, const int & = 0, PDomainDistributions = PDomainDistributions(), PDomainBasicAttrStat = PDomainBasicAttrStat()) const;
};

WRAPPER(ExamplesDistance_Relief);

#endif

