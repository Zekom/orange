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


#include "domain.hpp"
#include "distvars.hpp"
#include "contingency.hpp"
#include "examplegen.hpp"

#include "learn.ppp"


TLearner::TLearner(const int &aneeds)
: needs(aneeds)
{}


PClassifier TLearner::operator()(PVariable)
{ if (needs==NeedsNothing)
    raiseError("invalid value of 'needs'");
  else
    raiseError("no examples"); 
  return PClassifier();
}


PClassifier TLearner::operator()(PDistribution dist)
{ switch (needs) {
    case NeedsNothing:
      return operator()(dist->variable);
    case NeedsClassDistribution:
      raiseError("invalid value of 'needs'");
    default:
      raiseError("cannot learn from class distribution only"); 
  };
  return PClassifier();
}


PClassifier TLearner::operator()(PDomainDistributions ddist)
{ switch (needs) {
    case NeedsNothing:
      return operator()(ddist->back()->variable);
    case NeedsClassDistribution:
      return operator()(ddist->back());
    case NeedsDomainDistribution:
      raiseError("invalid value of 'needs'");
    default:
      raiseError("cannot learn from distributions only");
  }
  return PClassifier();
}


PClassifier TLearner::operator()(PDomainContingency dcont)
{ switch (needs) {
    case NeedsNothing:
      return operator()(dcont->classes->variable);
    case NeedsClassDistribution:
      return operator()(dcont->classes);
    case NeedsDomainDistribution:
      return operator()(dcont->getDistributions());
    case NeedsDomainContingency:
      raiseError("invalid value of 'needs'");
    default:
      raiseError("cannot learn from contingencies only");
  }
  return PClassifier();
}



PClassifier TLearner::operator()(PExampleGenerator gen, const int &weight)
{ 
  if (!gen || !gen->domain)
    raiseError("TLearner: no examples or invalid example generator");
  if (!gen->domain->classVar)
    raiseError("class-less domain");

  switch (needs) {
    case NeedsNothing:
      return operator()(gen->domain->classVar);
    case NeedsClassDistribution:
      return operator()(getClassDistribution(gen, weight));
    case NeedsDomainDistribution:
      return operator()(PDomainDistributions(mlnew TDomainDistributions(gen, weight)));
    case NeedsDomainContingency:
      return operator()(PDomainContingency(mlnew TDomainContingency(gen, weight)));
    default:
      raiseError("invalid value of 'needs'");
  }
  return PClassifier();
}



PClassifier TLearner::smartLearn(PExampleGenerator gen, const int &weight,
                                 PDomainContingency dcont,
                                 PDomainDistributions ddist,
                                 PDistribution dist)
{ 
  switch (needs) {

    case NeedsNothing:
      if (!gen || !gen->domain)
        raiseError("TLearner: no examples or invalid example generator");
      if (!gen->domain->classVar)
        raiseError("class-less domain");
      return operator()(gen->domain->classVar);

    case NeedsClassDistribution:
      if (dist)
        return operator()(dist);
      else if (ddist)
        return operator()(ddist->back());
      else if (dcont)
        return operator()(dcont->classes);
      else
        return operator()(getClassDistribution(gen, weight));

    case NeedsDomainDistribution:
      if (ddist)
        return operator()(ddist);
      else if (dcont)
        return operator()(dcont->getDistributions());
      else
        return operator()(PDomainDistributions(mlnew TDomainDistributions(gen, weight)));

    case NeedsDomainContingency:
      if (dcont)
        return operator()(dcont);
      else
        return operator()(PDomainContingency(mlnew TDomainContingency(gen, weight)));

    case NeedsExampleGenerator:
      return operator()(gen, weight);

    default:
      raiseError("invalid value of 'needs'");
  }

  return PClassifier();
}

  
TLearnerFD::TLearnerFD()
: TLearner()
{}


TLearnerFD::TLearnerFD(PDomain ad)
: domain(ad)
{}
