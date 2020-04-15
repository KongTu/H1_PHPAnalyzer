////////////////////////////////////////////////////////////////
// Class UserFiller
//
// Author: Ursula Berthon
// Last update: $Date: 2002/01/25 14:02:23 $
//          by: $Author: flucke $
// 
// Description: Defines properties of user defined event filling
//               
////////////////////////////////////////////////////////////////

#ifndef __USERFILLER_H
#define __USERFILLER_H

#include "H1Skeleton/H1EventFiller.h"


class H1Event;

class UserFiller: public H1EventFiller {
  public:
  virtual void   Fill(H1Event* event);
  
  ClassDef(UserFiller,1)  // class for filling data of UserEvent
};

#endif
