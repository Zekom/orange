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


#ifndef __TDIDT_HPP
#define __TDIDT_HPP

#include "classify.hpp"
#include "learn.hpp"

#include "contingency.hpp"

WRAPPER(PruneTree);
WRAPPER(TreeSplitConstructor);
WRAPPER(TreeStopCriteria);
WRAPPER(Distribution)
WRAPPER(ComputeDomainContingency)
WRAPPER(TreeExampleSplitter)

WRAPPER(TreeNode)
#define TTreeNodeList TOrangeVector<PTreeNode>
VWRAPPER(TreeNodeList)


class TTreeNode : public TOrange {
public:
  __REGISTER_CLASS

  PClassifier nodeClassifier; //P classifies an example

  PDistribution distribution; //P distribution of classes in the node
  PDomainContingency contingency; //P domain contingency for examples in the node
  PExampleGenerator examples; //P learning examples (if stored)
  int weightID; //P weightID used to construct this node

  PClassifier branchSelector; //P classifier that select a branch for an example
  PTreeNodeList branches; //P classifiers presenting the branches
  PStringList branchDescriptions; //P descriptions of branches
  PDiscDistribution branchSizes; //P numbers of examples in branches

  int treeSize() const;
  void removeStoredInfo();
};

WRAPPER(TreeNode)
WRAPPER(TreeDescender)


class TTreeLearner : public TLearner {
public:
  __REGISTER_CLASS

  PTreeSplitConstructor split; //P split criterion
  PTreeStopCriteria stop; //P stop criterion
  PComputeDomainContingency contingencyComputer; //P computes contingency matrix
  PLearner nodeLearner; //P node learner
  PTreeExampleSplitter exampleSplitter; //P splits examples to branches

  bool storeExamples; //P if true (default: false), learning examples in nodes are stored
  bool storeDistributions; //P if true (default), class distributions of learning examples in nodes are stored
  bool storeContingencies; //P if true (default), contingency matrices for examples are stored
  bool storeNodeClassifier; //P if true (default), the internal nodes have classifiers; needed for pruning

  PTreeDescender descender; //P descends down the tree

  TTreeLearner();
  virtual PClassifier operator()(PExampleGenerator gen, const int &weight =0);
  virtual PTreeNode operator()(PExampleGenerator gen, const int &ppWeight, PDistribution apriorClass, vector<bool> &candidates);

  PDiscDistribution branchSizesFromSubsets(PExampleGeneratorList subsets, const int &weightID, const vector<int> &weights) const;

};

WRAPPER(TreeLearner)


class TTreeDescender: public TOrange {
public:
  __REGISTER_ABSTRACT_CLASS

  virtual PTreeNode operator()(PTreeNode node, const TExample &, PDiscDistribution &) =0;
};

WRAPPER(TreeDescender)

// (the comment below prevent pyprops from matching the lines)
#define DEFINEDESCENDER(name) \
class TTreeDescender_##name: public TTreeDescender { \
public: virtual PTreeNode operator()(PTreeNode node, const TExample &, PDiscDistribution &); \
/**/  __REGISTER_CLASS \
};

DEFINEDESCENDER(UnknownToNode)
DEFINEDESCENDER(UnknownToBranch)
DEFINEDESCENDER(UnknownToCommonBranch)
DEFINEDESCENDER(UnknownToCommonSelector)
DEFINEDESCENDER(UnknownMergeAsBranchSizes)
DEFINEDESCENDER(UnknownMergeAsSelector)


class TTreeClassifier : public TClassifierFD {
public:
  __REGISTER_CLASS

  PTreeNode tree; //P decision tree
  PTreeDescender descender; //P object that descends down the tree

  TTreeClassifier(PDomain = PDomain(), PTreeNode = PTreeNode(), PTreeDescender = PTreeDescender());

  virtual TValue operator ()(const TExample &);
  virtual PDistribution classDistribution(const TExample &);
  virtual void predictionAndDistribution(const TExample &, TValue &, PDistribution &);

  virtual PDistribution classDistribution(PTreeNode, const TExample &);
  virtual PDistribution vote(PTreeNode, const TExample &, PDiscDistribution branchWeights);
};

WRAPPER(TreeClassifier)


class TTreePruner : public TOrange {
public:
  __REGISTER_ABSTRACT_CLASS

  virtual PTreeNode operator()(PTreeNode) =0; 
};

class TTreePruner_SameMajority : public TTreePruner {
public:
  __REGISTER_CLASS

  virtual PTreeNode operator()(PTreeNode); 
  virtual PTreeNode operator()(PTreeNode node, vector<bool> &bestValues);
};

class TTreePruner_m : public TTreePruner {
public:
  __REGISTER_CLASS

  float m; //P m for m-estimate

  TTreePruner_m(const float & =0);
  virtual PTreeNode operator()(PTreeNode); 

  template<class T>
  float operator()(PTreeNode node, const T &m_by_p, PTreeNode &newNode) const;

private:
  float estimateError(const PTreeNode &, const vector<float> &m_by_p) const;
  float estimateError(const PTreeNode &, const float &m_by_se) const;
};

#endif
