#ifndef __FILEGEN_HPP
#define __FILEGEN_HPP

#include <string>
#include <list>

#include "examplegen.hpp"

bool skipNonEmptyLine(FILE *file, const char *filename, const char &commentChar);

const char *getExtension(const char *filename);
char *replaceExtension(const char *filename, const char *extension, const char *oldExtension);

class ORANGE_API TFileExampleIteratorData {
public:
  FILE *file;
  const string &filename;
  int line;

  TFileExampleIteratorData(const string &name, const int &startDataPos = 0, const int &startDataline = 0);
  TFileExampleIteratorData(FILE *, const string &name, const int &line);
  TFileExampleIteratorData(const TFileExampleIteratorData &);
  ~TFileExampleIteratorData();
};


/*  A generator which retrieves examples from the file. It has an abstract
    method for reading a TExample; by defining it, descendants of
    TFileExampleGenerator can read different file formats. */
class ORANGE_API TFileExampleGenerator : public TExampleGenerator {
public:
  __REGISTER_ABSTRACT_CLASS

  string filename; //P filename
  int startDataPos; //P starting position of the data in file
  int startDataLine; //P line in the file where the data starts

  TFileExampleGenerator(const string &, PDomain);

  virtual TExampleIterator begin();
  virtual TExampleIterator begin(TExampleIterator &);
  virtual bool randomExample(TExample &);

  virtual int numberOfExamples();


protected:
  virtual void     increaseIterator(TExampleIterator &);
  virtual bool     sameIterators(const TExampleIterator &, const TExampleIterator &);
  virtual void     deleteIterator(TExampleIterator &source);
  virtual void     copyIterator(const TExampleIterator &source, TExampleIterator &dest);

  // An abstract method for reading examples. Derived classes must provide this method.
  virtual bool readExample (TFileExampleIteratorData &, TExample &)=0;
};

#endif
