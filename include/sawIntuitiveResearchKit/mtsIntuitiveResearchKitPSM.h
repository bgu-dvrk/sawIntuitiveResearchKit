/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2013-05-15

  (C) Copyright 2013 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsIntuitiveResearchKitPSM_h
#define _mtsIntuitiveResearchKitPSM_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstParameterTypes/prmPositionJointSet.h>
#include <cisstParameterTypes/prmPositionJointGet.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstRobot/robManipulator.h>

class mtsIntuitiveResearchKitPSM: public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

public:
    mtsIntuitiveResearchKitPSM(const std::string & componentName, const double periodInSeconds);
    mtsIntuitiveResearchKitPSM(const mtsTaskPeriodicConstructorArg & arg);
    ~mtsIntuitiveResearchKitPSM(){}

    void Configure(const std::string & filename = "");
    void Startup(void);
    void Run(void);
    void Cleanup(void);

protected:

    void Init(void);

    struct {
        mtsFunctionRead GetPositionJoint;
        mtsFunctionWrite SetPositionJoint;
    } PID;

    prmPositionCartesianGet CartesianCurrent;
    vctFrm4x4 CartesianPrevious;
    prmPositionJointGet JointCurrent;
    prmPositionJointSet JointDesired;
    robManipulator Manipulator;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsIntuitiveResearchKitPSM);

#endif // _mtsIntuitiveResearchKitPSM_h
