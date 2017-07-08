/******************************************************************************
 * Copyright (C) 2017 by Yifan Jiang                                          *
 * jiangyi@student.ethz.com                                                   *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 ******************************************************************************/

/*
* ObjectDictionary class
*
* implements object dictionary, which is compatible with CANOpen standard (CiA301)
* but is also extendable to make use of higher-BW communication methods
*/

#ifndef OBJECT_DICTIONARY_H
#define OBJECT_DICTIONARY_H

#include "stdint.h"
#include "F28x_Project.h"

#include "CommutationMaster.h"
#include "CiATypeDef.h"
#include "ObjectDictionaryEntry.h"
#include "ObjectDictionaryEntryBase.h"
#include "ControlProcessData.h"
#include "CurrentLoopController.h"
#include "CurrentControlProcess.h"
#include "CurrentLoopSweepSine.h"
#include "PositionControlProcess.h"

#define MAX_NO_OF_ENTRY 100

class ObjectDictionary{

public:

  ObjectDictionary(ControlProcessData * ControlProcessDataPtr,
                   CommutationMaster * CommutationMasterPtr,
                   CurrentLoopController * CurrentLoopControllerPtr)
  {
    _CommutationMaster = CommutationMasterPtr;

    uint16_t i;
    for(i=0; i<MAX_NO_OF_ENTRY; i++){
      _IdxArray[i] = i*5;
      _InstanceArray[i] = NULL;
      _AccessFunctionArray[i] = NULL;
    }

    _InstanceArray[1] = static_cast<ObjectDictionaryEntryBase*>(ControlProcessDataPtr);
    _AccessFunctionArray[1] = static_cast<void (ObjectDictionaryEntryBase::*)(ObdAccessHandle*)> (&ControlProcessData::AccessParameter);

    _InstanceArray[2] = static_cast<ObjectDictionaryEntryBase*>(CurrentLoopControllerPtr);
    _AccessFunctionArray[2] = static_cast<void (ObjectDictionaryEntryBase::*)(ObdAccessHandle*)> (&CurrentLoopController::AccessControlGains);

    _InstanceArray[5] = static_cast<ObjectDictionaryEntryBase*>(CommutationMasterPtr);
    _AccessFunctionArray[5] = static_cast<void (ObjectDictionaryEntryBase::*)(ObdAccessHandle*)> (&CommutationMaster::AccessParameter);
  }

  ~ObjectDictionary(){}

  void AccessEntry(CiA_Message * msg_in, CiA_Message * msg_out){
    ObdAccessHandle handle;
    handle.AccessType = msg_in->Sdo.SdoCtrl_ccs;
    handle.Data.DataInt16[0] = msg_in->Sdo.Data[0];
    handle.Data.DataInt16[1] = msg_in->Sdo.Data[1];
    int16_t pos = SearchEntry(msg_in->Sdo.SdoIdx, msg_in->Sdo.SdoSubIdx);
    if(pos==-1){
      msg_out->Sdo.SdoAccessResult = OBD_ACCESS_ERR_IDX_NONEXIST;
    } else {
      if(_InstanceArray[pos]!=NULL){
        (_InstanceArray[pos]->*(_AccessFunctionArray[pos]))(&handle);
        msg_out->Sdo.SdoAccessResult = handle.AccessResult;
        msg_out->Sdo.Data[0] = handle.Data.DataInt16[0];
        msg_out->Sdo.Data[1] = handle.Data.DataInt16[1];
      } else {
        msg_out->Sdo.SdoAccessResult = OBD_ACCESS_ERR_IDX_NONEXIST;
      }
    }

    msg_out->Sdo.Length = 10;
  }

private:

  int16_t SearchEntry(uint16_t Idx, uint16_t SubIdx);

  CommutationMaster * _CommutationMaster;

  uint32_t _IdxArray[ MAX_NO_OF_ENTRY ];
  ObjectDictionaryEntryBase * _InstanceArray[ MAX_NO_OF_ENTRY ];
  void (ObjectDictionaryEntryBase::*_AccessFunctionArray[ MAX_NO_OF_ENTRY ])(ObdAccessHandle*);

};


#endif
