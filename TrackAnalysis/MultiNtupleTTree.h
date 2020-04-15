#ifndef __H__MULTINTUPLETTREE__
#define __H__MULTINTUPLETTREE__

#include <map>
#include <string>

#include "NTupleTTree.h"

class MultiNtupleTTree : public NTupleTTree {
 public:
   virtual void activate_branches(TTree & tree) const =0;
   virtual void attach_for_writing(TTree & tree) const =0;
   virtual void attach_for_writing(TTree & tree) const=0;
   void add(std::string const &name,NTupleTTree *ntuple);
   void remove(std::string const &name,NTupleTTree *ntuple);
 protected:
   std::map<std:string,NTupleTTree *> m_trees;
};
