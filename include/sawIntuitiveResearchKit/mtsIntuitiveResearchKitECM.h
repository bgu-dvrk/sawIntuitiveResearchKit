/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  Author(s):  Anton Deguet
  Created on: 2013-05-15

  (C) Copyright 2013-2014 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsIntuitiveResearchKitECM_h
#define _mtsIntuitiveResearchKitECM_h

#include <cisstMultiTask/mtsTaskPeriodic.h>
#include <cisstParameterTypes/prmPositionJointSet.h>
#include <cisstParameterTypes/prmPositionJointGet.h>
#include <cisstParameterTypes/prmPositionCartesianGet.h>
#include <cisstParameterTypes/prmPositionCartesianSet.h>
#include <cisstRobot/robManipulator.h>
#include <cisstRobot/robLSPB.h>

// temporary
#include <cisstOSAbstraction/osaStopwatch.h>

class mtsIntuitiveResearchKitECM: public mtsTaskPeriodic
{
    CMN_DECLARE_SERVICES(CMN_DYNAMIC_CREATION_ONEARG, CMN_LOG_ALLOW_DEFAULT);

public:
    static const size_t NumberOfJoints = 4;
    static const size_t NumberOfBrakes = 3;

    mtsIntuitiveResearchKitECM(const std::string & componentName, const double periodInSeconds);
    mtsIntuitiveResearchKitECM(const mtsTaskPeriodicConstructorArg & arg);
    inline ~mtsIntuitiveResearchKitECM() {}

    void Configure(const std::string & filename);
    void Startup(void);
    void Run(void);
    void Cleanup(void);

protected:

    enum RobotStateType {
        ECM_UNINITIALIZED, /*! State when constructed */
        ECM_HOMING_POWERING, /*! Turn power on, calibrate encoders and current */
        ECM_HOMING_CALIBRATING_ARM, /*! Calibrate using pots and move to zero position for all joints except last one */
        ECM_READY,
        ECM_POSITION_CARTESIAN, /**< Go to command cartesian position */
        ECM_CONSTRAINT_CONTROLLER_CARTESIAN,
        ECM_MANUAL /**< User manually move robot */
    };

    void Init(void);

    /*! Get data from the PID level based on current state. */
    void GetRobotData(void);

    /*! Verify that the state transition is possible, initialize global
      variables for the desired state and finally set the state. */
    void SetState(const RobotStateType & newState);

    /*! Homing procedure, will check the homing state and call the required method. */
    void RunHoming(void);

    /*! Homing procedure, power the robot and initial current and encoder calibration. */
    void RunHomingPower(void);

    /*! Homing procedure, home all joints except last one using potentiometers as reference. */
    void RunHomingCalibrateArm(void);

    /*! Cartesian state. */
    void RunPositionCartesian(void);

    /*! Wrapper to convert vector of 7 values to prmPositionJointSet and send to PID */
    void SetPositionJointLocal(const vctDoubleVec & newPosition);

    void EventHandlerTrackingError(void);
    void EventHandlerManipClutch(const prmEventButton & button);
    void EventHandlerSUJClutch(const prmEventButton & button);

    void SetPositionCartesian(const prmPositionCartesianSet & newPosition);
    void SetRobotControlState(const std::string & state);

    struct {
        mtsFunctionWrite Enable;
        mtsFunctionRead GetPositionJoint;
        mtsFunctionRead GetPositionJointDesired;
        mtsFunctionWrite SetPositionJoint;
        mtsFunctionWrite SetCheckJointLimit;
        mtsFunctionWrite EnableTrackingError;
        mtsFunctionWrite SetTrackingErrorTolerance;
    } PID;

    // Required interface
    struct {
        //! Enable Robot Power
        mtsFunctionVoid EnablePower;
        mtsFunctionVoid DisablePower;
        mtsFunctionRead GetActuatorAmpStatus;
        mtsFunctionRead GetBrakeAmpStatus;
        mtsFunctionVoid BiasEncoder;
        mtsFunctionWrite SetActuatorCurrent;
        mtsFunctionWrite UsePotsForSafetyCheck;
        mtsFunctionWrite SetPotsToEncodersTolerance;
        mtsFunctionVoid BrakeRelease;
        mtsFunctionVoid BrakeEngage;
    } RobotIO;

    struct {
        mtsFunctionRead GetButton;
        bool IsPressed;
    } ManipClutch;

    struct {
        mtsFunctionRead GetButton;
        bool IsPressed;
    } SUJClutch;

    // Functions for events
    struct {
        mtsFunctionWrite RobotStatusMsg;
        mtsFunctionWrite RobotErrorMsg;
        mtsFunctionWrite ManipClutch;
        RobotStateType ManipClutchPreviousState;
        mtsFunctionWrite SUJClutch;
    } EventTriggers;

    // ZC: cache Cartesian Goal position
    prmPositionCartesianSet CartesianGoalSet;
    bool IsCartesianGoalSet;

    prmPositionCartesianGet CartesianCurrentParam;
    vctFrm4x4 CartesianCurrent;
    prmPositionJointGet JointCurrentParam;
    vctDoubleVec JointCurrent;
    prmPositionJointSet JointDesiredParam;
    vctDoubleVec JointDesired;
    robManipulator Manipulator;

    vctFrm4x4 CartesianPositionFrm;
    RobotStateType RobotState;

    struct {
        robLSPB LSPB;
        vctDoubleVec Velocity;
        vctDoubleVec Acceleration;
        vctDoubleVec Start;
        vctDoubleVec Goal;
        vctDoubleVec GoalError;
        vctDoubleVec GoalTolerance;
    } JointTrajectory;

    // Home Action
    double HomingTimer;
    bool HomingPowerRequested;
    bool HomingCalibrateArmStarted;

    int Counter;
};

CMN_DECLARE_SERVICES_INSTANTIATION(mtsIntuitiveResearchKitECM);

#endif // _mtsIntuitiveResearchKitECM_h