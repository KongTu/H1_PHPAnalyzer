// Variables to be saved in a TTree

#ifdef VARIABLES

#define FLOAT_VAR(x) Float_t x;
#define FLOAT_ARRAY(x,n,index) Float_t x[n];
#define INT_VAR(x) Int_t x;
#define INT_ARRAY(x,n,index) Int_t x[n];
#define HEXINT_ARRAY(x,n,index) Int_t x[n];

VARIABLES()

#undef FLOAT_VAR
#undef FLOAT_ARRAY
#undef INT_VAR
#undef INT_ARRAY
#undef HEXINT_ARRAY

public:

#define FLOAT_VAR(x) tree->SetBranchAddress(#x,&x);
#define FLOAT_ARRAY(x,n,index) tree->SetBranchAddress(#x,x);
#define INT_VAR(x) FLOAT_VAR(x)
#define INT_ARRAY(x,n,index) FLOAT_ARRAY(x,n,index)
#define HEXINT_ARRAY(x,n,index) FLOAT_ARRAY(x,n,index)

virtual void setBranchAddresses(TTree *tree) {
  VARIABLES()
}

#undef FLOAT_VAR
#undef FLOAT_ARRAY
#undef INT_VAR
#undef INT_ARRAY
#undef HEXINT_ARRAY

#define FLOAT_VAR(x) tree->Branch(#x,&x,#x "/F");
#define FLOAT_ARRAY(x,n,index) tree->Branch(#x,x,#x "[" #index "]/F");
#define INT_VAR(x) tree->Branch(#x,&x,#x "/I");
#define INT_ARRAY(x,n,index) tree->Branch(#x,x,#x "[" #index "]/I");
#define HEXINT_ARRAY(x,n,index) INT_ARRAY(x,n,index)
virtual void createBranches(TTree *tree) {
  VARIABLES()
}
#undef FLOAT_VAR
#undef FLOAT_ARRAY
#undef INT_VAR
#undef INT_ARRAY
#undef HEXINT_ARRAY

#define FLOAT_VAR(x) std::cout<<#x "="<<x<<"\n";
#define FLOAT_ARRAY(x,n,index) std::cout<<#x<<"["<<index<<"]={"; for(int i=0;i<index;i++) { if(i) std::cout<<","; std::cout<<x[i]; } std::cout<<"}\n";
#define INT_VAR(x) std::cout<<#x "="<<x<<" (0x"<<std::setbase(16)<<x<<std::setbase(10)<<")\n";
#define INT_ARRAY(x,n,index) FLOAT_ARRAY(x,n,index)
#define HEXINT_ARRAY(x,n,index) std::cout<<#x<<"["<<index<<"]={"<<std::setbase(16); for(int i=0;i<index;i++) { if(i) std::cout<<","; std::cout<<"0x"<<x[i]; } std::cout<<std::setbase(10)<<"}\n";

virtual void print(void) const {
  VARIABLES()
}
#undef FLOAT_VAR
#undef FLOAT_ARRAY
#undef INT_VAR
#undef INT_ARRAY
#undef HEXINT_ARRAY

#undef VARIABLES
#endif
