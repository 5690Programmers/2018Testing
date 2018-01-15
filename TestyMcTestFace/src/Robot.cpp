#include <iostream>
#include <memory>
#include <string>

#include <AHRS.h>

#include <Joystick.h>
#include <XboxController.h>
#include <SampleRobot.h>
#include <SmartDashboard/SendableChooser.h>
#include <SmartDashboard/SmartDashboard.h>
#include <Drive/DifferentialDrive.h>
#include <Timer.h>
#include <VictorSP.h>
#include <SpeedControllerGroup.h>
/**
 * This is a demo program+++++-+ showing the use of the RobotDrive class.
 * The SampleRobot class is the base of a robot application that will
 * automatically call your Autonomous and OperatorControl methods at the right
 * time as controlled by the switches on the driver station or the field
 * controls.
 *
 * WARNING: While it may look like a good choice to use for your code if you're
 * inexperienced, don't. Unless you know what you are doing, complex code will
 * be much more difficult under this system. Use IterativeRobot or Command-Based
 * instead if you're new.
 */
class Robot: public frc::SampleRobot {
	//"The PWM values where wrong :/" - logan ("i know" -camden ('-'))
	//left side
	frc::VictorSP left {3};
	frc::VictorSP left2 {2};
	//right side
	frc::VictorSP right {1};
	frc::VictorSP right2 {0};

	//climber
	frc::VictorSP Climber {4};
	//Intake
	frc::VictorSP Intake { 5 };
	frc::DoubleSolenoid IntakeArm { 0, 1 };

	frc::SpeedControllerGroup GroupL{ left, left2};
	frc::SpeedControllerGroup GroupR{ right, right2};

	frc::DifferentialDrive myRobot {GroupR, GroupL};
	//frc::Joystick stick { 0 };
	frc::XboxController Xbox { 0 };
	frc::SendableChooser<std::string> chooser;

	AHRS *ahrs;

	const std::string autoNameDefault = "Default";
	const std::string autoNameCustom = "My Auto";

public:
	Robot() {
		//Note SmartDashboard is not initialized here, wait until RobotInit to make SmartDashboard calls
		myRobot.SetExpiration(0.1);

		ahrs = new AHRS(SPI::Port::kMXP);
	}

	void RobotInit() {
		chooser.AddDefault(autoNameDefault, autoNameDefault);
		chooser.AddObject(autoNameCustom, autoNameCustom);
		frc::SmartDashboard::PutData("Auto Modes", &chooser);
	}


	/*
	 * This autonomous (along with the chooser code above) shows how to select
	 * between different autonomous modes using the dashboard. The sendable
	 * chooser code works with the Java SmartDashboard. If you prefer the
	 * LabVIEW Dashboard, remove all of the chooser code and uncomment the
	 * GetString line to get the auto name from the text box below the Gyro.
	 *
	 * You can add additional auto modes by adding additional comparisons to the
	 * if-else structure below with additional strings. If using the
	 * SendableChooser make sure to add them to the chooser code above as well.
	 */
	void Autonomous() {
		auto autoSelected = chooser.GetSelected();
		// std::string autoSelected = frc::SmartDashboard::GetString("Auto Selector", autoNameDefault);
		std::cout << "Auto selected: " << autoSelected << std::endl;
		std::string gameData;
				gameData=frc::DriverStation::GetInstance().GetGameSpecificMessage();
				if(gameData[0]=='L')
				{

					//put code for switch on left
				}else{
					//Put code for the switch on the right here
				}
		if (autoSelected == autoNameCustom)
		{
			// Custom Auto goes here
			std::cout << "Running custom Autonomous" << std::endl;
			myRobot.SetSafetyEnabled(false);
			myRobot.CurvatureDrive(0.5, 0.0, false); //go straight for 1 second
			frc::Wait(1.0);                // for 2 seconds
			myRobot.CurvatureDrive(0.5,-1, true);//turn for 0.5 seconds
			frc::Wait(0.5);
			myRobot.CurvatureDrive(0.5, 0.0, false); //go straight for 1 second
			frc::Wait(1.0);
			myRobot.CurvatureDrive(0.0, 0.0, false);  // stop robot
			frc::Wait(1.0);

		} else {
			// Default Auto goes here
			std::cout << "Running default Autonomous" << std::endl;
			myRobot.SetSafetyEnabled(false);
			myRobot.CurvatureDrive(-0.5, 0.0, false); // drive forwards half speed
			frc::Wait(2.0);                // for 2 seconds
			myRobot.CurvatureDrive(0.0, 0.0, false);  // stop robot
		}
	}

	/*
	 * Runs the motors with arcade steering.
	 *
	 */
	void OperatorControl() override {
		myRobot.SetSafetyEnabled(true);
		double deadzone = 0.2;
		double XboxY;
		double XboxX;
	//	Climber.setInverted(true);
		while (IsOperatorControl() && IsEnabled()) {
			// drive with arcade style (use right stick)
			//myRobot.ArcadeDrive(-stick.GetY(), stick.GetX(), false);
			/*myRobot.ArcadeDrive(Xbox.GetY(XboxController::JoystickHand(0)),
				                Xbox.GetX(XboxController::JoystickHand(0)),
								true);
			*/
			//Tank with adjustments
			if(Xbox.GetX(XboxController::JoystickHand(0)) > deadzone || Xbox.GetX(XboxController::JoystickHand(0)) < -deadzone)
				XboxX = Xbox.GetX(XboxController::JoystickHand(0));

			else
				XboxX = 0;

			if(Xbox.GetY(XboxController::JoystickHand(0)) > deadzone || Xbox.GetY(XboxController::JoystickHand(0)) < -deadzone)
				XboxY = Xbox.GetY(XboxController::JoystickHand(0));

			else
				XboxY = 0;

			myRobot.ArcadeDrive( XboxY, XboxX/1.25, true);


			//Climber Trigger
			if (Xbox.GetTriggerAxis(XboxController::JoystickHand(1)))
			{
				Climber.Set(-Xbox.GetTriggerAxis(XboxController::JoystickHand(1)));
			}else
			{
				Climber.Set(0);
			}


			//Intake
			if (Xbox.GetYButton())
				Intake.Set(0.5);

			else if(Xbox.GetAButton())
				Intake.Set(-0.25);

			else
				Intake.Set(0);

			//Placing
			if (Xbox.GetBButton()){
				myRobot.ArcadeDrive(-.25,0, false);
				Intake.Set(-0.35);
				IntakeArm.Set(DoubleSolenoid::Value(2));
				Wait(0.005);
				myRobot.ArcadeDrive(0,0, false);
				Intake.Set(0);
			}

			// wait for a motor update time
			frc::Wait(0.005);
		}
	}

	/*
	 * Runs during test mode
	 */
	void Test() override {

	}
};

START_ROBOT_CLASS(Robot)
