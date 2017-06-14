/******************************************************************************
 * Copyright (C) 2017 by Yifan Jiang                                          *
 * jiangyi@student.ethz.com                                                   *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 ******************************************************************************/

/*
* Control process master class
*
* Contains the main state machine of CPU1, handling control processes
*/

#ifndef CONTROL_PROCESS_MASTER_H
#define CONTROL_PROCESS_MASTER_H

#include "stdint.h"

class ControlProcessMaster{

  public:

    enum ControlProcessMaster_STATES {
      STATE_IDEL,
      STATE_ENABLE,
      STATE_CLSW,
      STATE_PLSW,
      STATE_POLARITY
    };

    ControlProcessMaster():
      hehe(0)
      {}

    ~ControlProcessMaster(){}

    void Update(void);

  private:
    enum ControlProcessMaster_STATES m_state;
    uint16_t hehe;
};

void CreateControProcessMasterInstance(void);

#endif
