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


#include <limits>
#include <list>
#include <math.h>
#include "errors.hpp"

#include "random.hpp"
#include "vars.hpp"
#include "domain.hpp"
#include "examples.hpp"
#include "examplegen.hpp"
#include "table.hpp"

#include "distvars.hpp"
#include "distance.hpp"
#include "nearest.hpp"
#include "meta.hpp"

#include "knn.ppp"


TkNNLearner::TkNNLearner(const float &ak, PExamplesDistanceConstructor edc)
: k(ak),
  distanceConstructor(edc)
{}


PClassifier TkNNLearner::operator()(PExampleGenerator gen, const int &weight)
{ if (!gen->domain->classVar)
    raiseError("class-less domain");

  PFindNearest findNearest = TFindNearestConstructor_BruteForce(distanceConstructor ? distanceConstructor : mlnew TExamplesDistanceConstructor_Euclidean(), true)
                               (gen, weight, getMetaID());

  return mlnew TkNNClassifier(gen->domain, weight, k, findNearest, rankWeight);
}


TkNNClassifier::TkNNClassifier(PDomain dom, const int &wei, const float &ak, PFindNearest fdist, const bool &rw)
: TClassifierFD(dom, true),
  findNearest(fdist),
  k(ak),
  rankWeight(rw),
  weightID(wei)
{}


PDistribution TkNNClassifier::classDistribution(const TExample &oexam)
{ checkProperty(findNearest);

  TExample exam(domain, oexam);

  PExampleGenerator neighbours = findNearest->call(exam, k);
  PDistribution classDist = TDistribution::create(classVar);

  if (neighbours->numberOfExamples()==1)
    classDist->add((*neighbours->begin()).getClass());

  else {
    if (rankWeight) {
      const float &sigma2 = k*k / -log(0.001);
      int rank2 = 1, rankp=1; // rank2 is rank^2, rankp = rank^2 - (rank-1)^2; and, voila, we don't need rank :)
      PEITERATE(ei, neighbours)
        classDist->add((*ei).getClass(), WEIGHT(*ei) * exp(-(rank2 += (rankp+=2))/sigma2));
    }
    else {
      const int &distanceID = findNearest->distanceID;

      // This is not really elegant, but there's not other way to reach the last example...
      const TExample *last = NULL;
      { PEITERATE(ei, neighbours)
          last = &*ei;
      }

      float lastwei = WEIGHT2(*last, distanceID);

      const float &sigma2 = lastwei*lastwei / -log(0.001);
      PEITERATE(ei, neighbours) {
        const float &wei = WEIGHT2(*ei, distanceID);
        classDist->add((*ei).getClass(), WEIGHT(*ei) * exp(-wei*wei/sigma2));
      }
    }
  }

  classDist->normalize();
  return classDist;
}
